/**************************************************************************************************************
 * altobeam RTOS
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/
#include "atbm_hal.h"
#include "../../include/svn_version.h"
#define DPLL_CLOCK 40

extern void frame_hexdump(char *prefix, atbm_uint8 *data, atbm_uint8 len);
extern int wsm_recovery(struct atbmwifi_common *hw_priv);
int atbm_hmac_receive_data(struct sbus_priv *self,unsigned int addr,atbm_void  *dst, int count);
int atbm_hmac_xmit_data(struct sbus_priv *self);
atbm_void atbm_process_rx_packet_v0(struct atbmwifi_common *hw_priv, struct atbm_buff *skb);
atbm_void atbm_process_rx_packet_v1(struct atbmwifi_common *hw_priv, struct atbm_buff *skb);
static void atbm_build_wsm_header_v1(struct atbmwifi_common *hw_priv, struct wsm_hdr_tx *wsm_tx, int *tx_len);

struct atbm_txrx_ops{
	int align_size;
	ATBM_BOOL hdr_rollback;
	void (*process_rx)(struct atbmwifi_common *hw_priv, struct atbm_buff *skb);
	void (*build_hdr)(struct atbmwifi_common *hw_priv, struct wsm_hdr_tx *wsm_tx, int *tx_len);;
};

static struct atbm_txrx_ops atbm_ops[ATBM_TXRX_OPS_MAX] = {
	{
		.hdr_rollback = ATBM_HDR_ROLLBACK,
		.build_hdr = atbm_build_wsm_header_v0,
		.process_rx = atbm_process_rx_packet_v0,
		.align_size = ATBM_PACKET_ALIGN_SIZE_V0,
	},
	{
		.hdr_rollback = ATBM_FALSE,
		.build_hdr = atbm_build_wsm_header_v1,
		.process_rx = atbm_process_rx_packet_v1,
		.align_size = ATBM_PACKET_ALIGN_SIZE_V1,
	}
};

static struct atbm_txrx_ops *current_ops;

#if ATBM_HIF_DEBUG
struct{
	int tx_skb_cnt;
	int tx_pkt_cnt;
	int rx_skb_cnt;
	int rx_pkt_cnt;
	atbm_uint32 last_print_time;
}hif_status;

static void add_tx_skb_cnt(){
	hif_status.tx_skb_cnt++;
}
static void add_tx_pkt_cnt(){
	hif_status.tx_pkt_cnt++;
}
static void add_rx_skb_cnt(){
	hif_status.rx_skb_cnt++;
}
static void add_rx_pkt_cnt(){
	hif_status.rx_pkt_cnt++;
}
static dump_hif_status(){
	if(atbm_TimeAfter(hif_status.last_print_time, 5*ATBM_HZ)){
		return;
	}
	wifi_printk(WIFI_ALWAYS, "tx %d %d rx %d %d\n", hif_status.tx_skb_cnt,
				hif_status.tx_pkt_cnt, hif_status.rx_skb_cnt, hif_status.rx_pkt_cnt);
	hif_status.tx_skb_cnt = 0;
	hif_status.tx_pkt_cnt = 0;
	hif_status.rx_skb_cnt = 0;
	hif_status.rx_pkt_cnt = 0;
	hif_status.last_print_time = atbm_GetOsTime();
}
#else
static void add_tx_skb_cnt(){}
static void add_tx_pkt_cnt(){}
static void add_rx_skb_cnt(){}
static void add_rx_pkt_cnt(){}
static void dump_hif_status(){}
#endif

static void atbm_process_decript_fifo(struct wsm_hdr_rx *hw_hdr, struct wsm_rx *rx)
{
	atbm_uint8 enc_type;

	if(atbm_unlikely(!(hw_hdr->enc_hdr.EncWriteBack & ENC_WB_PROC_SUCCESS))){
		if(hw_hdr->enc_hdr.EncWriteBack & (ENC_WB_MIC_VERIFY_ERR
						|ENC_WB_WEP_CRC_ERR|ENC_WB_BIP_MIC_FAIL)){
			rx->status = WSM_STATUS_MICFAILURE;
		}else{
			rx->status = WSM_STATUS_DECRYPTFAILURE;
		}
		wifi_printk(WIFI_DBG_ERROR, "addr:%p %p wb:%x\n", hw_hdr, rx, hw_hdr->enc_hdr.EncWriteBack);
	}
#ifdef CHKSUM_HW_SUPPORT
	else if(!(hw_hdr->enc_hdr.EncWriteBack & ENC_WB_CHECKSUM_SUCCESS)){
		rx->flags |= WSM_RX_STATUS_CHKSUM_ERROR;
	}
#endif
	enc_type = hw_hdr->enc_hdr.EncWriteBack >> ENC_WB_KEYTYPE_OFFSET;
	switch(enc_type){
		case WSM_KEY_TYPE_WEP_DEFAULT:
		case WSM_KEY_TYPE_WEP_PAIRWISE:
			rx->flags |= WSM_RX_STATUS_WEP;
			break;
		case WSM_KEY_TYPE_TKIP_GROUP:
		case WSM_KEY_TYPE_TKIP_PAIRWISE:
			rx->flags |= WSM_RX_STATUS_TKIP;
			break;
		case WSM_KEY_TYPE_AES_GROUP:
		case WSM_KEY_TYPE_AES_PAIRWISE:
			rx->flags |= WSM_RX_STATUS_AES;
			break;
		case WSM_KEY_TYPE_WAPI_GROUP:
		case WSM_KEY_TYPE_WAPI_PAIRWISE:
		case WSM_KEY_TYPE_IGTK_GROUP:
		case WSM_KEY_TYPE_NONE:
		case WSM_KEY_TYPE_UNKNOWN:
			break;
		default:
			wifi_printk(WIFI_DBG_ERROR, "1addr:%p %p wb:%x\n", hw_hdr, rx, hw_hdr->enc_hdr.EncWriteBack);
			ATBM_WARN_ON(1);
			break;
	}
}

/**************************************************************************************************************
 * 函数名: atbm_rx_bh_cb
 *
 * 功能描述:
 *   RX Bottom Half 回调函数，处理从硬件接收到的 802.11 帧。
 *   这是 RX 流程中的关键处理阶段，负责解析 WSM 头部、验证序列号、
 *   并将数据包分发到相应的处理函数（数据帧/管理帧/控制帧）。
 *
 * 在 RX 流程中的位置:
 *   阶段 4: WSM 解析 → 802.11 帧处理
 *   在 atbm_process_rx_packet_v0/v1() 中调用，处理解聚合后的单个帧
 *
 * 参数说明:
 *   @hw_priv:  硬件私有数据指针，包含驱动的全局状态
 *   @skb:      接收到的数据包（Socket Buffer），包含 WSM 头部和 802.11 帧
 *   @checkseq: 是否检查 WSM 序列号
 *              - ATBM_TRUE: 检查序列号（单帧接收时）
 *              - ATBM_FALSE: 不检查序列号（多帧聚合时，序列号已在外层检查）
 *
 * 返回值:
 *   0: 成功处理
 *
 * 处理流程:
 *   1. 解析 WSM 头部：获取消息长度和 ID
 *   2. 异常处理：如果是异常消息（0x0800），调用 wsm_handle_exception() 处理
 *   3. 序列号检查：如果 checkseq 为 TRUE，验证 WSM 序列号
 *   4. 消息分发：调用 wsm_handle_rx() 将消息分发到相应的处理函数
 *   5. 资源释放：处理完成后释放 skb
 *
 * 注意事项:
 *   - 序列号用于检测数据包丢失和乱序
 *   - 序列号错误时会触发恢复流程（wsm_recovery）
 *   - 函数负责释放 skb，调用者不需要释放
 *
 * 相关函数:
 *   - atbm_process_rx_packet_v0/v1(): 调用此函数
 *   - wsm_handle_rx(): 消息分发处理
 *   - wsm_handle_exception(): 异常消息处理
 *   - wsm_recovery(): 错误恢复
 *
 **************************************************************************************************************/
int atbm_rx_bh_cb(struct atbmwifi_common *hw_priv,struct atbm_buff *skb, ATBM_BOOL checkseq)
{
	struct wsm_hdr *wsm;
	atbm_uint32 wsm_len = 0;
	int wsm_id;
	atbm_uint8 wsm_seq;

	wsm = (struct wsm_hdr *)ATBM_OS_SKB_DATA(skb);

	wsm_len = __atbm_le32_to_cpu(wsm->len);

	wsm_id	= __atbm_le32_to_cpu(wsm->id) & 0xFFF;

	//ATBM_BUG_ON(wsm_len > 4096);
	atbm_skb_trim(skb,0);
	atbm_skb_put(skb,wsm_len);
	add_rx_skb_cnt();

	if (atbm_unlikely(wsm_id == 0x0800)) {
		wsm_handle_exception(hw_priv,
			 &ATBM_OS_SKB_DATA(skb)[sizeof(*wsm)],
			wsm_len - sizeof(*wsm));
		if(wsm_recovery(hw_priv)== RECOVERY_ERR){
			ATBM_BUG_ON(1);
		}
		goto __free;
	}
	if(checkseq){
		wsm_seq = (__atbm_le32_to_cpu(wsm->id) >> 13) & 7;	
		if (ATBM_WARN_ON(wsm_seq != hw_priv->wsm_rx_seq)) {	
			wifi_printk(WIFI_DBG_ERROR,"rx wsm_seq error %d %d \n",wsm_seq,hw_priv->wsm_rx_seq);
			if(wsm_recovery(hw_priv)== RECOVERY_ERR){
				ATBM_BUG_ON(1);
			}
			goto __free;
		}
		hw_priv->wsm_rx_seq = (wsm_seq + 1) & 7;
	}

	/* atbm_wsm_rx takes care on SKB livetime */
	if (ATBM_WARN_ON(wsm_handle_rx(hw_priv, wsm_id, wsm,&skb))){
		wifi_printk(WIFI_ALWAYS,"%s %d \n",__FUNCTION__,__LINE__);
		if(wsm_recovery(hw_priv)== RECOVERY_ERR){
			ATBM_BUG_ON(1);
		}
		goto __free;
	}

__free:
	//wifi_printk(WIFI_DBG_ERROR, "%s %d\n",__func__,__LINE__);
	if(skb != ATBM_NULL){
		atbm_dev_kfree_skb(skb);
	}

	return 0;
}

/**************************************************************************************************************
 * 函数名: atbm_tx_task
 *
 * 功能描述:
 *   TX 任务处理函数，负责从发送队列中取出数据包并通过总线发送到硬件。
 *   该函数在 BH 任务中被调用，是 TX 流程中从软件到硬件的关键桥梁。
 *
 * 在 TX 流程中的位置:
 *   阶段 4: 队列管理 → 硬件发送
 *   在 atbm_hmac_bh() 中调用，当 bh_tx 标志被设置时执行
 *
 * 参数说明:
 *   @hw_priv: 硬件私有数据指针，包含 WiFi 硬件的通用信息和状态
 *
 * 返回值:
 *   无
 *
 * 处理流程:
 *   1. 进入循环处理模式
 *   2. 调用 atbm_hmac_xmit_data() 从队列取出数据包并发送到硬件
 *   3. 循环直到队列为空（status <= 0）
 *
 * 调用时机:
 *   - 在 BH 任务中，当 bh_tx 标志被设置时调用
 *   - bh_tx 标志由 atbm_bh_schedule_tx() 设置
 *   - 通常在数据包加入队列后触发
 *
 * 注意事项:
 *   - 函数在 BH 任务上下文中执行，优先级较低
 *   - 循环处理直到队列为空，避免频繁唤醒
 *   - 实际的数据发送由 atbm_hmac_xmit_data() 完成
 *
 * 相关函数:
 *   - atbm_hmac_bh(): BH 任务主函数，调用此函数
 *   - atbm_hmac_xmit_data(): 实际的数据发送函数
 *   - atbm_bh_schedule_tx(): 唤醒 BH 任务进行 TX 处理
 *
 **************************************************************************************************************/
atbm_void atbm_tx_task(struct atbmwifi_common *hw_priv)
{
	int status =0;
	do {
		//if(hw_priv->bh_term)|| (hw_priv->bh_error))
		//{
		//	wifi_printk(WIFI_IF,"atbm_tx_task term(%d),err(%d)\n",atbm_atomic_read(&hw_priv->bh_term),hw_priv->bh_error);
		//	return;
		//}
		/*atbm transmit packet to device*/
		//hw_priv->sbus_ops->lock(hw_priv->sbus_priv);
		atbm_hmac_xmit_data(hw_priv->sbus_priv);
		//hw_priv->sbus_ops->unlock(hw_priv->sbus_priv);
	}while(status > 0);
}

/**************************************************************************************************************
 * 函数名: atbm_process_rx_packet_v0
 *
 * 功能描述:
 *   V0 版本的 RX 数据包处理函数，支持多帧聚合解析。
 *   负责解析 WSM 头部，检测是否为多帧聚合，并将数据包分发到 atbm_rx_bh_cb() 处理。
 *
 * 在 RX 流程中的位置:
 *   阶段 3: WSM 解析与多帧聚合处理
 *   在 atbm_rx_task() 中通过 current_ops->process_rx() 调用
 *
 * 参数说明:
 *   @hw_priv: 硬件私有数据指针，包含驱动的全局状态
 *   @skb:     接收到的数据包（Socket Buffer），可能包含单帧或多帧聚合
 *
 * 返回值:
 *   无
 *
 * 处理流程:
 *   1. 解析 WSM 头部：获取消息长度和 ID
 *   2. 检测多帧聚合：
 *      - WSM_MULTI_RECEIVE_INDICATION_ID (USB)
 *      - WSM_SINGLE_CHANNEL_MULTI_RECEIVE_INDICATION_ID (SDIO)
 *   3. 序列号检查：对于多帧聚合，在外层检查序列号
 *   4. 多帧解聚合：
 *      a. 解析 wsm_multi_rx 结构体，获取帧数量
 *      b. 循环提取每一帧
 *      c. 为每一帧分配新的 skb（或重用原 skb）
 *      d. 调用 atbm_rx_bh_cb() 处理每一帧
 *   5. 单帧处理：直接调用 atbm_rx_bh_cb() 处理
 *   6. 重新提交接收请求：调用 atbm_hmac_receive_data() 保持接收链
 *
 * 多帧聚合格式:
 *   ┌─────────────────────────────────────────┐
 *   │ WSM_MULTI_RECEIVE_INDICATION_ID         │
 *   │ + RxFrameNum (帧数量)                    │
 *   ├─────────────────────────────────────────┤
 *   │ Frame 1: WSM_RECEIVE_INDICATION_ID     │
 *   │ + 数据                                   │
 *   ├─────────────────────────────────────────┤
 *   │ Frame 2: ...                            │
 *   └─────────────────────────────────────────┘
 *
 * 注意事项:
 *   - 多帧聚合时，序列号在外层检查，内层不再检查
 *   - 支持零拷贝模式（ATBM_RX_REUSE_PACKET）重用原始 skb
 *   - 函数负责释放原始 skb（如果不重用）
 *   - 处理完成后需要重新提交接收请求
 *
 * 相关函数:
 *   - atbm_rx_task(): 调用此函数
 *   - atbm_rx_bh_cb(): 处理单个帧
 *   - atbm_hmac_receive_data(): 重新提交接收请求
 *
 **************************************************************************************************************/
atbm_void atbm_process_rx_packet_v0(struct atbmwifi_common *hw_priv, struct atbm_buff *skb){
#define RX_ALLOC_BUFF_OFFLOAD (  (56+16)/*RX_DESC_OVERHEAD*/+4/*FCS_LEN*/ -16 /*WSM_HI_RX_IND*/)
#if HI_RX_MUTIL_FRAME
	struct wsm_hdr *wsm;
	atbm_uint32 wsm_len, wsm_id, data_len;
	struct atbm_buff *skb_copy;
	ATBM_BOOL checkseq = ATBM_TRUE;
	ATBM_BOOL multirx = ATBM_FALSE;

	wsm = (struct wsm_hdr *)ATBM_OS_SKB_DATA(skb);
	wsm_len = __atbm_le32_to_cpu(wsm->len);
	wsm_id	= __atbm_le32_to_cpu(wsm->id) & 0xFFF;
	atbm_skb_trim(skb, wsm_len);
	//wifi_printk(WIFI_DBG_ERROR, "%s rxdata %x\n",__func__,wsm_id);
#if ATBM_USB_BUS
	if(wsm_id == WSM_MULTI_RECEIVE_INDICATION_ID){
		multirx = ATBM_TRUE; 
	}else if(wsm_id == WSM_SINGLE_CHANNEL_MULTI_RECEIVE_INDICATION_ID)
#endif
#if ATBM_SDIO_BUS
	if(wsm_id == WSM_MULTI_RECEIVE_INDICATION_ID || wsm_id == WSM_SINGLE_CHANNEL_MULTI_RECEIVE_INDICATION_ID)
#endif
	{
		atbm_uint8 wsm_seq;

		multirx = ATBM_TRUE;
		wsm_seq = (__atbm_le32_to_cpu(wsm->id) >> 13) & 7;		
		if (ATBM_WARN_ON(wsm_seq != hw_priv->wsm_rx_seq)) {		
			wifi_printk(WIFI_DBG_ERROR,"rx wsm_seq error %d %d \n",wsm_seq,hw_priv->wsm_rx_seq);
			if(wsm_recovery(hw_priv)== RECOVERY_ERR){
				ATBM_BUG_ON(1);
			}
			atbm_dev_kfree_skb(skb);
			/*atbm transmit packet to device*/
			atbm_hmac_receive_data(hw_priv->sbus_priv,0x2,ATBM_NULL,RX_BUFFER_SIZE);
			return;
		}
		hw_priv->wsm_rx_seq = (wsm_seq + 1) & 7;
		checkseq = ATBM_FALSE;
	}

	if(multirx){
		struct wsm_multi_rx *  multi_rx = (struct wsm_multi_rx *)ATBM_OS_SKB_DATA(skb);			
		int RxFrameNum = multi_rx->RxFrameNum;
		data_len = wsm_len ;
		data_len -= sizeof(struct wsm_multi_rx);
		wsm = (struct wsm_hdr *)(multi_rx+1);
		wsm_len = __atbm_le32_to_cpu(wsm->len);
		wsm_id	= __atbm_le32_to_cpu(wsm->id) & 0xFFF;
		do {
			//wifi_printk(WIFI_DBG_ERROR, "wsm_len:%02x wsm_id:%02x\n", wsm_len, wsm_id);
			if(data_len < wsm_len){
				wifi_printk(WIFI_DBG_ERROR,"skb->len %x,wsm_len %x\n",ATBM_OS_SKB_LEN(skb),wsm_len);
				break;
			}
			ATBM_BUG_ON((wsm_id  & ~WSM_TX_LINK_ID(WSM_TX_LINK_ID_MAX)) !=  WSM_RECEIVE_INDICATION_ID);
#if ATBM_RX_REUSE_PACKET
			skb_copy = atbm_dev_reuse_skb(skb, (atbm_uint8*)wsm, wsm_len);
#else
			skb_copy = atbm_dev_alloc_skb(wsm_len + 16);
			/* In AP mode RXed SKB can be looped back as a broadcast.
			 * Here we reserve enough space for headers. */
			atbm_skb_reserve(skb_copy,  (8 - (((unsigned long)ATBM_OS_SKB_DATA(skb_copy))&7))/*ATBM_ALIGN 8*/);
			
			atbm_memmove(ATBM_OS_SKB_DATA(skb_copy), wsm, wsm_len);
			atbm_skb_put(skb_copy,wsm_len);
#endif
			atbm_rx_bh_cb(hw_priv,skb_copy,checkseq);
			data_len -= ATBM_ALIGN(wsm_len + RX_ALLOC_BUFF_OFFLOAD,4);
			RxFrameNum--;

#if (ATBM_RX_REUSE_PACKET == 0)
			if(RxFrameNum == 0){
				atbm_dev_kfree_skb(skb);
				/*atbm transmit packet to device*/
				atbm_hmac_receive_data(hw_priv->sbus_priv,0x2,ATBM_NULL,RX_BUFFER_SIZE);
				return;
			}
#endif
			wsm = (struct wsm_hdr *)((atbm_uint8 *)wsm +ATBM_ALIGN(( wsm_len + RX_ALLOC_BUFF_OFFLOAD),4));
			wsm_len = __atbm_le32_to_cpu(wsm->len);
			wsm_id	= __atbm_le32_to_cpu(wsm->id) & 0xFFF;
#if ATBM_RX_REUSE_PACKET
			if(RxFrameNum == 1){
				atbm_skb_pull(skb, ATBM_OS_SKB_LEN(skb) - wsm_len);
				atbm_rx_bh_cb(hw_priv,skb,checkseq);
				/*atbm transmit packet to device*/
				atbm_hmac_receive_data(hw_priv->sbus_priv,0x2,ATBM_NULL,RX_BUFFER_SIZE);
				return;
			}
#endif
		}while(1);
	}
	else
#endif
	{
		atbm_rx_bh_cb(hw_priv,skb,checkseq);
		/*atbm transmit packet to device*/
		atbm_hmac_receive_data(hw_priv->sbus_priv,0x2,ATBM_NULL,RX_BUFFER_SIZE);
	}	
}

/**************************************************************************************************************
 * 函数名: atbm_process_rx_packet_v1
 *
 * 功能描述:
 *   V1 版本的 RX 数据包处理函数，使用新的 WSM 格式（wsm_hdr_rx）。
 *   负责解析 WSM 头部，处理聚合数据包，并将数据包分发到 atbm_rx_bh_cb() 处理。
 *
 * 在 RX 流程中的位置:
 *   阶段 3: WSM 解析与多帧聚合处理
 *   在 atbm_rx_task() 中通过 current_ops->process_rx() 调用
 *
 * 参数说明:
 *   @hw_priv: 硬件私有数据指针，包含驱动的全局状态
 *   @skb:     接收到的数据包（Socket Buffer），可能包含单帧或多帧聚合
 *
 * 返回值:
 *   无
 *
 * 处理流程:
 *   1. 解析 wsm_hdr_rx 头部：获取总长度
 *   2. 循环处理聚合的数据包：
 *      a. 检查是否为最后一个包（通过长度或结束标记判断）
 *      b. 如果是最后一个包，重用原始 skb
 *      c. 如果不是最后一个包，分配新的 skb 并复制数据
 *      d. 处理解密 FIFO（如果 WSM ID 为 0x804）
 *      e. 调用 atbm_rx_bh_cb() 处理每一帧
 *      f. 移动到下一帧
 *   3. 重新提交接收请求：调用 atbm_hmac_receive_data() 保持接收链
 *
 * V1 格式特点:
 *   - 使用 wsm_hdr_rx 结构体，包含额外的 total_len 字段
 *   - 支持硬件解密 FIFO 处理
 *   - 聚合包通过 total_len 字段判断边界
 *
 * 注意事项:
 *   - V1 格式与 V0 格式不同，使用不同的头部结构
 *   - 支持零拷贝模式（ATBM_RX_REUSE_PACKET）重用原始 skb
 *   - 函数负责释放原始 skb（如果不重用）
 *   - 处理完成后需要重新提交接收请求
 *
 * 相关函数:
 *   - atbm_rx_task(): 调用此函数
 *   - atbm_rx_bh_cb(): 处理单个帧
 *   - atbm_process_decript_fifo(): 处理解密 FIFO
 *   - atbm_hmac_receive_data(): 重新提交接收请求
 *
 **************************************************************************************************************/
atbm_void atbm_process_rx_packet_v1(struct atbmwifi_common *hw_priv, struct atbm_buff *skb){
	struct wsm_hdr_rx *wsm;
	atbm_uint32 wsm_len;
	struct atbm_buff *skb_copy;

	wsm = (struct wsm_hdr_rx *)ATBM_OS_SKB_DATA(skb);
	wsm_len = __atbm_le32_to_cpu(wsm->total_len);
	do {
#if ATBM_RX_REUSE_PACKET
		if(WSM_GET_RX_ID(wsm->wsm.id) == 0x804){
			atbm_process_decript_fifo(wsm, (struct wsm_rx *)(wsm + 1));
		}
		/*If the last aggregated pkt, reuse the skb*/
		if((ATBM_OS_SKB_LEN(skb) <= (sizeof(struct wsm_hdr_rx) + wsm_len))
				|| (*((atbm_uint32*)(ATBM_OS_SKB_DATA(skb) + ATBM_ALIGN(wsm_len,4))) == 0)){
			atbm_skb_pull(skb, offsetof(struct wsm_hdr_rx, wsm));
			atbm_rx_bh_cb(hw_priv,skb,ATBM_TRUE);
			/*atbm transmit packet to device*/
			atbm_hmac_receive_data(hw_priv->sbus_priv,0x2,ATBM_NULL,RX_BUFFER_SIZE);
			break;
		}
		skb_copy = atbm_dev_reuse_skb(skb, (atbm_uint8*)&wsm->wsm, wsm->wsm.len);
#else
		if(ATBM_OS_SKB_LEN(skb) < sizeof(struct wsm_hdr_rx) || wsm_len == 0){
			atbm_dev_kfree_skb(skb);
			/*atbm transmit packet to device*/
			atbm_hmac_receive_data(hw_priv->sbus_priv,0x2,ATBM_NULL,RX_BUFFER_SIZE);
			break;
		}
		if(WSM_GET_RX_ID(wsm->wsm.id) == 0x804){
			atbm_process_decript_fifo(wsm, (struct wsm_rx *)(wsm + 1));
		}
		skb_copy = atbm_dev_alloc_skb(wsm->wsm.len + 16);
		/* In AP mode RXed SKB can be looped back as a broadcast.
		 * Here we reserve enough space for headers. */
		atbm_skb_reserve(skb_copy,  (8 - (((unsigned long)ATBM_OS_SKB_DATA(skb_copy))&7))/*ATBM_ALIGN 8*/);

		atbm_memmove(ATBM_OS_SKB_DATA(skb_copy), &wsm->wsm, wsm->wsm.len);
		atbm_skb_put(skb_copy, wsm->wsm.len);
#endif
		atbm_rx_bh_cb(hw_priv,skb_copy,ATBM_TRUE);
		atbm_skb_pull(skb, ATBM_ALIGN(wsm_len,4));
		wsm = (struct wsm_hdr_rx *)ATBM_OS_SKB_DATA(skb);
		wsm_len = __atbm_le32_to_cpu(wsm->total_len);
	}while(1);
}

/**************************************************************************************************************
 * 函数名: atbm_rx_task
 *
 * 功能描述:
 *   RX 任务处理函数，从 rx_frame_queue 队列中取出数据包并调用处理函数。
 *   这是 BH 任务中的 RX 处理入口，负责从队列中取出所有待处理的数据包。
 *
 * 在 RX 流程中的位置:
 *   阶段 3: BH 任务处理
 *   在 atbm_hmac_bh() 中调用，当 bh_rx 标志被设置时执行
 *
 * 参数说明:
 *   @hw_priv: 硬件私有数据指针，包含 rx_frame_queue 队列和驱动状态
 *
 * 返回值:
 *   无
 *
 * 处理流程:
 *   1. 循环从 rx_frame_queue 队列中取出数据包
 *   2. 检查任务终止标志（bh_term）和错误标志（bh_error）
 *   3. 调用 current_ops->process_rx() 处理数据包（v0 或 v1 版本）
 *   4. 继续处理直到队列为空
 *
 * 队列说明:
 *   - rx_frame_queue: 存储从硬件接收到的原始数据包（WSM 格式）
 *   - 入队位置: atbm_hmac_receive_data_complete()
 *   - 出队位置: 本函数
 *
 * 注意事项:
 *   - 函数会处理队列中的所有数据包，直到队列为空
 *   - 如果检测到终止或错误标志，会提前退出
 *   - process_rx 函数负责释放 skb
 *
 * 相关函数:
 *   - atbm_hmac_bh(): 调用此函数
 *   - atbm_process_rx_packet_v0(): V0 版本的数据包处理
 *   - atbm_process_rx_packet_v1(): V1 版本的数据包处理
 *   - atbm_hmac_receive_data_complete(): 将数据包加入队列
 *
 **************************************************************************************************************/
atbm_void atbm_rx_task(struct atbmwifi_common *hw_priv)
{
	struct atbm_buff *skb ;

	while ((skb = atbm_skb_dequeue(&hw_priv->rx_frame_queue)) != ATBM_NULL) {
		if(hw_priv->bh_term|| hw_priv->bh_error)
			break;
		current_ops->process_rx(hw_priv, skb);
	}
}

void atbm_hmac_coml(struct atbmwifi_common *hw_priv){
	unsigned long flags=0;
    int ret = 0;
	int i;
	if(atbm_bh_is_term(hw_priv))
	{
		wifi_printk(WIFI_ALWAYS,"atbm_urb_coml Error\n");
		return;
	}
	atbm_spin_lock_irqsave(&hw_priv->tx_com_lock,&flags);
	while (!atbm_list_empty(&hw_priv->tx_urb_cmp)) {
	    struct sbus_desc *desc_cmp = atbm_list_first_entry(&hw_priv->tx_urb_cmp, struct sbus_desc, list);
		atbm_list_del(&desc_cmp->list);
		atbm_spin_unlock_irqrestore(&hw_priv->tx_com_lock,flags);
		struct sbus_priv *self = desc_cmp->obj;
		struct atbmwifi_common	*hw_priv	= self->core;

		for(i = 0; i < desc_cmp->frame_cnt; i++){
			if(desc_cmp->data[i] != ATBM_NULL){
				struct wsm_tx *tx = (struct wsm_tx *)desc_cmp->data[i];
				atbm_uint8 queue_id;
				struct atbmwifi_queue *queue=ATBM_NULL;

				if(!(tx->htTxParameters & atbm_cpu_to_le32(WSM_HT_TX_NEED_CONFIRM))){
					queue_id = atbmwifi_queue_get_queue_id(tx->packetID);
					queue = &hw_priv->tx_queue[queue_id];
					if(queue_id>=4){
						frame_hexdump("Error",(atbm_uint8*)tx,sizeof(*tx));
					}
					ATBM_BUG_ON(queue_id>=4);
					ret=atbmwifi_queue_remove(queue,tx->packetID);
					if(ret){
						wifi_printk(WIFI_DBG_ERROR,">>>>RET=%x\n",ret);
					}
				}
			}
		}
		ATBM_TX_DESC_PUT(self,self->drvobj,desc_cmp->desc_id);
		desc_cmp->link = 0;
		atbm_spin_lock_irqsave(&hw_priv->tx_com_lock,&flags);
	}
	atbm_spin_unlock_irqrestore(&hw_priv->tx_com_lock,flags);
	atbm_bh_schedule_tx(hw_priv);
}

/**************************************************************************************************************
 * 函数名: atbm_hmac_bh
 *
 * 功能描述:
 *   BH（Bottom Half）任务主处理函数，负责处理 RX 接收、TX 发送和 URB 完成事件。
 *   这是驱动的核心任务，在独立的低优先级线程中运行，处理所有的数据收发。
 *
 * 在 RX 流程中的位置:
 *   阶段 2: BH 任务调度
 *   由 RTOS 调度器调度执行，当 bh_rx/bh_tx 标志被设置时唤醒
 *
 * 参数说明:
 *   @arg:        硬件私有数据指针（struct atbmwifi_common *）
 *   @wake_state: 唤醒状态（由 RTOS 传入）
 *
 * 返回值:
 *   无
 *
 * 处理流程:
 *   1. 原子交换获取标志：获取 bh_rx、bh_tx、urb_comp 标志并清零
 *   2. 检查终止条件：如果 bh_term 或 bh_error 被设置，直接返回
 *   3. URB 完成处理：如果 urb_compl 标志被设置，调用 atbm_hmac_coml() 处理
 *   4. RX 处理：如果 rx 标志被设置，调用 atbm_rx_task() 处理接收
 *   5. TX 处理：如果 tx 标志被设置，调用 atbm_tx_task() 处理发送
 *   6. SDIO 队列处理：如果未启用独立 TX 写入任务，处理 SDIO 发送队列
 *
 * 任务创建:
 *   - 在 atbm_register_bh() 中创建
 *   - 任务名称: "atbm_bh"
 *   - 任务优先级: BH_TASK_PRIO
 *   - 任务超时: 100*ATBM_HZ（100 秒）
 *
 * 唤醒机制:
 *   - RX 唤醒: atbm_hmac_receive_data_complete() 中设置 bh_rx 并唤醒
 *   - TX 唤醒: atbm_bh_schedule_tx() 中设置 bh_tx 并唤醒
 *   - URB 完成唤醒: atbm_hmac_xmit_data_complete() 中设置 urb_comp 并唤醒
 *
 * 注意事项:
 *   - 使用原子操作获取标志，确保并发安全
 *   - 任务优先级较低，避免影响实时任务
 *   - 处理完成后任务会阻塞等待下次唤醒
 *
 * 相关函数:
 *   - atbm_rx_task(): RX 处理
 *   - atbm_tx_task(): TX 处理
 *   - atbm_hmac_coml(): URB 完成处理
 *   - atbm_register_bh(): 创建 BH 任务
 *
 **************************************************************************************************************/
atbm_void atbm_hmac_bh(atbm_void *arg, int wake_state)
{
	struct atbmwifi_common *hw_priv = (struct atbmwifi_common *)arg;
	int rx, tx,urb_compl;

	rx = atbm_atomic_xchg(&hw_priv->bh_rx, 0);
	tx = atbm_atomic_xchg(&hw_priv->bh_tx, 0);
#if (ATBM_TX_CMPL_TASK == 0)
	urb_compl = atbm_atomic_xchg(&hw_priv->urb_comp, 0);
#endif
	
	if ( hw_priv->bh_term || hw_priv->bh_error){
		wifi_printk(WIFI_DBG_ERROR,"%s BH thread break %d %d\n",__FUNCTION__,hw_priv->bh_term,hw_priv->bh_error);
		return;
	}
#if (ATBM_TX_CMPL_TASK == 0)
	if(urb_compl){
		//wifi_printk(WIFI_ALWAYS," urb_compl ----->.\n");
		atbm_hmac_coml(hw_priv);
	}
#endif
	if (rx){
		atbm_rx_task(hw_priv);	
	}
	if (tx){
		atbm_tx_task(hw_priv);
	}
#if (ATBM_TX_WRITE_TASK == 0)
	atbm_sdio_tx_queue_process(hw_priv,0,ATBM_TRUE);
#endif //#if (ATBM_TX_WRITE_TASK)
	dump_skb_status();
	dump_hif_status();
}

#if ATBM_TX_CMPL_TASK
atbm_void atbm_hmac_cmpl_bh(void *arg)
{
	struct atbmwifi_common *hw_priv = (struct atbmwifi_common *)arg;
	int urb_compl;

	urb_compl = atbm_atomic_xchg(&hw_priv->urb_comp, 0);

	if(urb_compl){
		//wifi_printk(WIFI_ALWAYS," urb_compl ----->.\n");
		atbm_hmac_coml(hw_priv);
	}
 
	wifi_printk(WIFI_BH,"atbm_hmac_tx_bh while--\n");
}
#endif

static atbm_void atbm_hmac_xmit_data_complete(ATBM_INF_DESC *atbm_desc)
{
	struct sbus_desc *desc_cmp=(struct sbus_desc*)(atbm_desc->context);
	struct sbus_priv *self = desc_cmp->obj;
	struct atbmwifi_common	*hw_priv = self->core;
	unsigned long flags;
	int ret = 0;
	int i;

	switch(atbm_desc->status){
		case 0:
			break;
		default:
			wifi_printk(WIFI_ALWAYS,"WARNING> status %d\n",atbm_desc->status);
			break;
	}

#if CONFIG_WIFI_BT_COMB
	if(desc_cmp->cnf == 0){
		atbm_spin_lock(&hw_priv->wsm_cmd.lock);
		hw_priv->wsm_cmd.ret = 0;
		hw_priv->wsm_cmd.done = 1;
		hw_priv->wsm_cmd.cmd = 0xFFFF;
		atbm_spin_unlock(&hw_priv->wsm_cmd.lock);
		atbm_os_wakeup_event(&hw_priv->wsm_cmd_wq);
	}
#endif

#if ATBM_DIRECT_TX
	for(i = 0; i < desc_cmp->frame_cnt; i++){
		if(desc_cmp->data[i] != ATBM_NULL){
			struct wsm_tx *tx = (struct wsm_tx *)desc_cmp->data[i];
			atbm_uint8 queue_id;
			struct atbmwifi_queue *queue=ATBM_NULL;
			if(!(tx->htTxParameters & atbm_cpu_to_le32(WSM_HT_TX_NEED_CONFIRM))){
				queue_id = atbmwifi_queue_get_queue_id(tx->packetID);
				queue = &hw_priv->tx_queue[queue_id];
				ATBM_BUG_ON(queue_id>=4);
				if(atbmwifi_queue_remove(queue,tx->packetID)){
					wifi_printk(WIFI_DBG_ERROR,">>>>error\n");
				}
			}
		}
	}
	ATBM_TX_DESC_PUT(self,self->drvobj,desc_cmp->desc_id);
	desc_cmp->link = 0;
	atbm_bh_schedule_tx(hw_priv);
#else
	/*Add tx urb to list tail*/
	atbm_spin_lock_irqsave(&hw_priv->tx_com_lock,&flags);
	atbm_list_add_tail(&desc_cmp->list,&hw_priv->tx_urb_cmp);
	atbm_spin_unlock_irqrestore(&hw_priv->tx_com_lock,flags);

	atbm_atomic_add_return(1, &hw_priv->urb_comp);
#if ATBM_TX_CMPL_TASK
	atbm_wakeupThreadInternal(hw_priv->cmpl_bh_thread);
#else
	atbm_wakeupThreadInternal(hw_priv->bh_thread);
#endif
#endif
	return ;
}

void atbm_hmac_free_err_cmd(struct sbus_priv *self)
{
	struct atbmwifi_common	*hw_priv = self->core;

	atbm_spin_lock_bh(&hw_priv->wsm_cmd.lock);
	hw_priv->wsm_cmd.ret = -1;
	hw_priv->wsm_cmd.done = 1;
	hw_priv->wsm_cmd.cmd = 0xFFFF;
	atbm_spin_unlock_bh(&hw_priv->wsm_cmd.lock);
	wifi_printk(WIFI_DBG_ERROR, "%s:release wsm_cmd.lock\n",__func__);
	atbm_os_wakeup_event(&hw_priv->wsm_cmd_wq);		
}


void atbm_hmac_free_err_data(struct sbus_priv *self,struct sbus_desc *tx_desc)
{
	struct atbmwifi_common	*hw_priv = self->core;
	struct wsm_tx *wsm = (struct wsm_tx *)tx_desc->data;	
	struct atbmwifi_queue *queue;
	atbm_uint8 queue_id;
	struct atbm_buff *skb;
	const struct atbmwifi_txpriv *txpriv;
	
	wifi_printk(WIFI_DBG_ERROR, "%s:release tx pakage\n",__func__);
	ATBM_BUG_ON(wsm == NULL);
	queue_id = atbmwifi_queue_get_queue_id(wsm->packetID);

	ATBM_BUG_ON(queue_id >= 4);
	queue = &hw_priv->tx_queue[queue_id];
	ATBM_BUG_ON(queue == NULL);

	if(!ATBM_WARN_ON(atbmwifi_queue_get_skb(queue, wsm->packetID, &skb, &txpriv))) {
		struct atbmwifi_ieee80211_tx_info *tx = ATBM_IEEE80211_SKB_TXCB(skb);
		tx->flags |= ATBM_IEEE80211_TX_STAT_ACK;
		atbmwifi_queue_remove(queue, wsm->packetID);
	}
}

static void atbm_build_wsm_header_v1(struct atbmwifi_common *hw_priv, struct wsm_hdr_tx *wsm_tx, int *tx_len){
	wsm_tx->u.common.mark = atbm_cpu_to_le16(0xe569);
	*tx_len = ATBM_ALIGN(*tx_len, 4);
#if ATBM_QUEUE_8023_FRAMES
	wsm_tx->enc_hdr.EncFlags &= ~atbm_cpu_to_le32(0xff << 24);	
#endif
	wsm_tx->enc_hdr.EncFlags |= atbm_cpu_to_le32(atbm_checksum_u8((atbm_uint8*)wsm_tx, 19) << 24);	
}

/**************************************************************************************************************
 * TX 聚合实现说明：
 * 
 * atbm_hmac_xmit_data() 函数有两个实现版本，通过 CONFIG_INF_AGGR_TX 宏控制：
 * 
 * 1. 第一个实现（第 940 行）：#if CONFIG_INF_AGGR_TX
 *    - 使用聚合缓冲区（Aggregation Buffer）机制
 *    - 预先为每个描述符分配固定大小的聚合缓冲区（BUFF_ALLOC_LEN）
 *    - 使用 do-while 循环，动态决定聚合数量
 *    - 将数据包复制到聚合缓冲区中
 *    - 最后统一提交一个描述符（ATBM_TX_DESC_FILL(0, ...)）
 * 
 * 2. 第二个实现（第 1167 行）：#else
 *    - 不使用聚合缓冲区
 *    - 使用 for 循环，固定 max_agg_num 次迭代
 *    - 直接使用原始数据指针（不复制数据）
 *    - 在循环中逐个填充描述符（ATBM_TX_DESC_FILL(i, ...)）
 *    - 最后统一提交一个描述符
 * 
 * 主要区别对比：
 * 
 * | 特性 | 第一个实现 (CONFIG_INF_AGGR_TX) | 第二个实现 (#else) |
 * |------|--------------------------------|-------------------|
 * | **缓冲区管理** | 预先分配聚合缓冲区（每个描述符 BUFF_ALLOC_LEN） | 不使用聚合缓冲区 |
 * | **循环方式** | do-while（动态，可提前退出） | for（固定 max_agg_num 次） |
 * | **数据存储** | 复制到聚合缓冲区（atbm_memcpy） | 直接使用原始数据指针 |
 * | **描述符填充** | 最后统一填充（ATBM_TX_DESC_FILL(0,...)） | 循环中逐个填充（ATBM_TX_DESC_FILL(i,...)） |
 * | **聚合限制** | max_agg_num（考虑 INF_BUF_SG_NUM 和固件限制） | max_agg_num（考虑 INF_BUF_SG_NUM 和固件限制） |
 * | **内存占用** | 较大（每个描述符 BUFF_ALLOC_LEN 字节） | 较小（无额外缓冲区） |
 * | **性能** | 较高（批量复制，减少总线开销） | 较低（逐个处理，无批量优化） |
 * | **WSM 序列号** | 每个包递增（hw_priv->wsm_tx_seq++） | 最后统一递增一次 |
 * | **错误处理** | 遍历所有包清理（for 循环） | 简化错误处理 |
 * 
 * 选择建议：
 *   - 需要高性能、减少总线开销：使用第一个实现（CONFIG_INF_AGGR_TX）
 *   - 内存受限、简单实现：使用第二个实现（#else）
 * 
 * 参考文档: doc/TX聚合宏定义关系说明.md, doc/TX发包流程文档.md
 **************************************************************************************************************/
#if CONFIG_INF_AGGR_TX
static void atbm_init_tx_aggr_buf(struct sbus_desc *tx_desc, int size, int len){
	int i;
	
	for(i = 0; i < size; i++){
		tx_desc[i].pallocated_buf = atbm_kmalloc(len, GFP_KERNEL);
		ATBM_BUG_ON(!tx_desc[i].pallocated_buf);
		tx_desc[i].pallocated_buf_len = len;
	}
}

static void atbm_free_tx_aggr_buf(struct sbus_desc *tx_desc, int size){
	int i;
	
	for(i = 0; i < size; i++){
		if(!ATBM_WARN_ON(tx_desc[i].pallocated_buf == ATBM_NULL))
			atbm_kfree(tx_desc[i].pallocated_buf);
	}
}

int atbm_hmac_free_tx_wsm(struct sbus_priv *self,struct sbus_desc *tx_desc)
{
	struct wsm_tx *wsm = NULL;

	wsm = tx_desc->data[0]; 
	if((wsm) && (!(wsm->htTxParameters&atbm_cpu_to_le32(WSM_HT_TX_NEED_CONFIRM)))){
		
		struct atbmwifi_queue *queue;
		atbm_uint8 queue_id;
		struct atbmwifi_common	*hw_priv = self->core;
		struct atbm_buff *skb;
		const struct atbmwifi_txpriv *txpriv;

		queue_id = atbmwifi_queue_get_queue_id(wsm->packetID);

		ATBM_BUG_ON(queue_id >= 4);

		queue = &hw_priv->tx_queue[queue_id];
		ATBM_BUG_ON(queue == NULL);

		if(!ATBM_WARN_ON(atbmwifi_queue_get_skb(queue, wsm->packetID, &skb, &txpriv))) {	
			struct atbmwifi_ieee80211_tx_info *tx = ATBM_IEEE80211_SKB_TXCB(skb);
			atbmwifi_queue_remove(queue, wsm->packetID);
		}

		tx_desc->data[0] = ATBM_NULL;
		return 1;
	}
	return 0;
}

/**************************************************************************************************************
 * 函数名: atbm_hmac_xmit_data
 *
 * 功能描述:
 *   底层数据发送函数，从发送队列取出数据包，构建 WSM 消息，并通过总线（USB/SDIO）发送到硬件。
 *   这是 TX 流程中的最后一个软件处理阶段，负责将数据包实际发送到硬件。函数支持数据包聚合，
 *   可以将多个数据包聚合到一个描述符中发送，提高传输效率。
 *
 * 在 TX 流程中的位置:
 *   阶段 4: 队列管理 → 硬件发送
 *   在 BH 任务中调用（atbm_tx_task()），从队列取出数据包并发送到硬件
 *
 * 参数说明:
 *   @self: 总线私有数据指针，包含总线相关的配置和状态信息（USB/SDIO）
 *
 * 返回值:
 *   > 0: 成功，返回发送的数据包数量（tx_burst）
 *   0: 成功，但未发送数据包（队列为空）
 *   < 0: 失败（-2: 总线挂起, -4: 描述符获取失败, -7: 缓冲区分配失败, -1: 提交失败）
 *
 * 处理流程:
 *   1. 检查总线状态：如果总线挂起，直接返回
 *   2. 获取发送描述符：从描述符池获取一个可用的描述符
 *   3. 初始化描述符：清零描述符的缓冲区和帧计数
 *   4. 循环处理数据包：
 *      a. 从队列获取数据包（wsm_get_tx）
 *      b. 复制数据到聚合缓冲区
 *      c. 构建 WSM 头部（根据接口类型：USB/SDIO）
 *      d. 设置 WSM 序列号
 *      e. 更新描述符信息
 *   5. 填充描述符：设置描述符的缓冲区、长度、完成回调等
 *   6. 提交描述符：通过总线接口提交到硬件（USB: URB, SDIO: 消息队列）
 *   7. 错误处理：如果提交失败，清理资源并返回错误
 *
 * 数据包聚合:
 *   - 支持将多个数据包聚合到一个描述符中发送
 *   - 最大聚合数量：INF_BUF_SG_NUM
 *   - 聚合可以提高传输效率，减少总线开销
 *
 * 注意事项:
 *   - 函数使用自旋锁保护，确保并发安全
 *   - 描述符从池中获取，避免频繁的内存分配
 *   - WSM 序列号用于硬件和驱动的同步
 *   - 不同接口（USB/SDIO）有不同的头部构建方式
 *   - 提交失败时需要清理所有已聚合的数据包
 *
 * 相关函数:
 *   - wsm_get_tx(): 从队列获取数据包
 *   - atbm_build_wsm_header_v0/v1(): 构建 WSM 头部（USB/SDIO）
 *   - ATBM_TX_SUBMIT_DESC(): 提交描述符到硬件（USB: usb_submit_urb, SDIO: sdio_tx_submit）
 *   - atbm_hmac_xmit_data_complete(): 发送完成回调
 *
 **************************************************************************************************************/
int atbm_hmac_xmit_data(struct sbus_priv *self)
{
	unsigned int pipe;
	int status=0;
	int tx_burst=0;                    /* 发送的数据包数量（聚合数量） */
	int vif_selected;                  /* 选中的虚拟接口 ID */
	struct wsm_hdr_tx *wsm;            /* WSM 头部指针 */
	struct atbmwifi_common *hw_priv=self->core;
	atbm_uint8 *data =ATBM_NULL;       /* 数据包数据指针 */
	int tx_len=0;                      /* 数据包长度 */
	int ret = 0;
	int desc_id =-1;                   /* 描述符 ID */
	struct sbus_desc *tx_desc = ATBM_NULL;  /* 发送描述符指针 */
	unsigned long flags;
	atbm_uint8 *txdmabuff = ATBM_NULL;
	atbm_uint8 *usb_aggr_buff = ATBM_NULL;    /* USB 聚合缓冲区指针 */
	/* 计算最大聚合数量：取驱动层限制（INF_BUF_SG_NUM）和固件层限制（numInpChBufs-2）的最小值 */
	/* 注意：必须确保不超过 tx_desc->data 数组大小（INF_BUF_SG_NUM），防止越界 */
	int max_agg_num = INF_BUF_SG_NUM;	
	int fw_limit = hw_priv->wsm_caps.numInpChBufs - 2;

	if(fw_limit > 0){
		/* 固件层限制：numInpChBufs-2（保留2个缓冲区用于其他用途） */
		/* 取驱动层限制和固件层限制的最小值，确保不超过数组大小 */
		max_agg_num = atbm_min(fw_limit, INF_BUF_SG_NUM);
	}
	else {
		/* numInpChBufs <= 2 时，最多聚合1个数据包 */
		max_agg_num = 1;
	}
	
	/* 步骤 1: 检查总线状态 */
	/* 如果 USB 正在断开连接，停止发送数据包 */
	//If Usb disconnecting, stop tx usb packet to host
	if(self->suspend){
		return -2;
	}

	wifi_printk(WIFI_IF, "atbm_hmac_xmit_data++\n");
	
	/* 步骤 2: 锁定发送完成锁 */
	/* 使用自旋锁保护发送操作，确保并发安全 */
	atbm_spin_lock_irqsave(&hw_priv->tx_com_lock, &flags);
	
	/* 步骤 3: 获取发送描述符 */
	/* 从描述符池获取一个可用的发送描述符 */
	desc_id = ATBM_TX_DESC_GET(self,self->drvobj,TX_DESC_NUM);
	if(desc_id<0){
		/* 描述符池已满，无法获取描述符 */
		wifi_printk(WIFI_DBG_MSG, "atbm_usb_xmit_data:desc_id<0\n");
		status=-4;
		goto error;
	}

	/* 步骤 4: 初始化描述符 */
	tx_desc = &self->tx_desc[desc_id];
	tx_desc->pallocated_buf_len = 0;  /* 清零聚合缓冲区长度 */
	tx_desc->frame_cnt = 0;            /* 清零帧计数 */
#if CONFIG_WIFI_BT_COMB
	tx_desc->cnf = 1;                  /* WiFi/BT 共存确认标志 */
#endif

	/* 步骤 5: 检查描述符缓冲区 */
	if(tx_desc->pallocated_buf == NULL)
	{
		/* 缓冲区未分配，恢复描述符并返回错误 */
		ATBM_TX_DESC_RESTORE(self,self->drvobj,desc_id);
		status=-7;
		goto error;
	}
	usb_aggr_buff = tx_desc->pallocated_buf;
	
	/* 步骤 6: 循环处理数据包（聚合） */
	do{
		/* 步骤 6.1: 从队列获取数据包 */
		/* 从 WMM AC 队列中按优先级获取数据包 */
		ret = wsm_get_tx(hw_priv, &data, &tx_len, &tx_burst,
					&vif_selected);
		if (ret <= 0) {
			/* 队列为空，没有更多数据包，退出循环 */
			wifi_printk(WIFI_IF,"tx:atbm_usb_urb_put ATBM_NULL %d\n",desc_id);
			break;
		}

		/* 步骤 6.2: 复制数据到聚合缓冲区 */
		/* 将数据包复制到描述符的聚合缓冲区中 */
		atbm_memcpy(usb_aggr_buff, data, tx_len);
		wsm = (struct wsm_hdr_tx *)usb_aggr_buff;
		/* 保存原始数据指针，用于错误处理 */
		tx_desc->data[tx_desc->frame_cnt] = data;

		/* 步骤 6.3: 构建 WSM 头部 */
		/* 根据接口类型（USB/SDIO）构建相应的 WSM 头部 */
		current_ops->build_hdr(hw_priv, wsm, &tx_len);

		add_tx_skb_cnt();

		/* 步骤 6.4: 设置 WSM 序列号 */
		/* 清除旧的序列号 */
		wsm->id &= atbm_cpu_to_le32(~WSM_TX_SEQ(WSM_TX_SEQ_MAX));
		/* 设置新的序列号 */
		wsm->id |= atbm_cpu_to_le32(WSM_TX_SEQ(hw_priv->wsm_tx_seq));
		self->tx_vif_selected = vif_selected;

		/* 增加序列号，用于下一个数据包 */
		hw_priv->wsm_tx_seq++;
		
		/* 步骤 6.5: 更新描述符信息 */
		/* 累加聚合缓冲区长度（按对齐大小） */
		tx_desc->pallocated_buf_len += ATBM_ALIGN(tx_len, current_ops->align_size);
		/*
		 *if the data is cmd ,keep it last in the aggr buff
		 */
		wifi_printk(WIFI_IF, "wsm->len:%d:wsm->id %d seq %d desc_id %d\n",wsm->len, wsm->id,hw_priv->wsm_tx_seq,desc_id);
		
		/* 步骤 6.6: 判断数据包类型 */
		if(wsm_txed(hw_priv, data)==0){
			/* 数据帧 */
			struct wsm_tx *tx = (struct wsm_tx *)data;
			hw_priv->wsm_txframe_num++;
			/* 如果不需要确认，清空数据指针（不跟踪） */
			if(tx->htTxParameters & atbm_cpu_to_le32(WSM_HT_TX_NEED_CONFIRM)){
				tx_desc->data[tx_desc->frame_cnt] = ATBM_NULL;
			}
		}
		else {
			/* 命令帧，清空数据指针 */
			tx_desc->data[tx_desc->frame_cnt] = ATBM_NULL;
#if CONFIG_WIFI_BT_COMB
			/* WiFi/BT 共存：设置确认标志 */
			atbm_spin_lock(&hw_priv->wsm_cmd.lock);
			tx_desc->cnf = hw_priv->wsm_cmd.cnf;
			atbm_spin_unlock(&hw_priv->wsm_cmd.lock);
#endif
		}

		/* 步骤 6.7: 增加帧计数 */
		tx_desc->frame_cnt++;
		/* 如果达到最大聚合数量，退出循环 */
		/* 注意：max_agg_num 已经考虑了驱动层限制（INF_BUF_SG_NUM）和固件层限制（numInpChBufs-2） */
		if(tx_desc->frame_cnt >= max_agg_num){
			break;
		}
		
		/* 额外的安全检查：确保 frame_cnt 不超过数组大小（防止异常情况） */
		if(tx_desc->frame_cnt >= INF_BUF_SG_NUM){
			wifi_printk(WIFI_DBG_ERROR, "Warning: frame_cnt(%d) >= INF_BUF_SG_NUM(%d), break to prevent overflow\n", 
			           tx_desc->frame_cnt, INF_BUF_SG_NUM);
			break;
		}
		
		/* 移动聚合缓冲区指针到下一个位置 */
		usb_aggr_buff += ATBM_ALIGN(tx_len, current_ops->align_size);
	}while(1);

	/* 步骤 7: 验证描述符 */
	if(tx_desc->pallocated_buf_len == 0){
		/* 没有数据包，恢复描述符并返回 */
		ATBM_TX_DESC_RESTORE(self,self->drvobj,desc_id);
		goto error;
	}

	if(tx_desc->frame_cnt == 0){
		/* 帧计数为 0，警告 */
		ATBM_WARN_ON(1);
	}

	/* 步骤 8: 填充描述符 */
	add_tx_pkt_cnt();
	/* 填充描述符的缓冲区、长度、完成回调等信息 */
	ATBM_TX_DESC_FILL(0,tx_desc->test_desc,tx_desc->pallocated_buf,tx_desc->pallocated_buf_len,
				self->drvobj,atbm_hmac_xmit_data_complete,tx_desc);
	tx_desc->link = 1;
	
	/* 步骤 9: 提交描述符到硬件 */
	/* USB: 提交 URB, SDIO: 发送到消息队列 */
	status = ATBM_TX_SUBMIT_DESC(tx_desc->test_desc, GFP_ATOMIC);

	/* 步骤 10: 错误处理 */
	if (status) {
		/* 提交失败，清理所有已聚合的数据包 */
		atbm_uint8 i = 0;
		int wsm_id;		
		struct wsm_tx *wsm_txd = NULL;				

		tx_desc->test_desc->status = 0;
		/* 遍历所有已聚合的数据包，清理资源 */
		for(i = 0;i<tx_desc->frame_cnt;i++){			
			wsm_txd = (struct wsm_tx *)tx_desc->data[i];
			if(!wsm_txd){
				continue;
			}			
			wsm_id = atbm_le16_to_cpu(wsm_txd->hdr.id) & 0x3F;			
			wifi_printk(WIFI_DBG_ERROR, "%s:wsm_id(%x)\n",__func__,wsm_id);			
			hw_priv->wsm_tx_seq++;			
//			if(wsm_id == WSM_FIRMWARE_CHECK_ID){
//				continue;			
//			}			
			if(wsm_id == WSM_TRANSMIT_REQ_MSG_ID){
				/* 数据帧：释放数据包资源 */
				if(!(wsm_txd->htTxParameters&atbm_cpu_to_le32(WSM_HT_TX_NEED_CONFIRM))){
					continue;
				}							
				atbm_hmac_free_err_data(self,tx_desc);			
			}else {
				/* 命令帧：释放命令资源 */
				atbm_hmac_free_err_cmd(self);			
			}						
		}		
		/* 恢复描述符到池中 */
		ATBM_TX_DESC_RESTORE(self,self->drvobj,desc_id);			
		wifi_printk(WIFI_ALWAYS, "release all data finished\n");
		status = -1;
		goto error;
	}

	/* 步骤 11: 返回发送的数据包数量 */
	if(status==0){
		status = tx_burst;
	}
error:
	/* 解锁发送完成锁 */
	atbm_spin_unlock_irqrestore(&hw_priv->tx_com_lock, flags);
	//atbm_atomic_set(&self->tx_lock, 0);
	return status;
}
#else
/**************************************************************************************************************
 * 函数名: atbm_hmac_xmit_data (第二个实现版本 - 非聚合缓冲区模式)
 * 
 * 条件编译: #else (当 CONFIG_INF_AGGR_TX 未定义时)
 * 
 * 功能描述:
 *   不使用聚合缓冲区的 TX 数据发送函数。
 *   该实现直接在循环中逐个填充描述符，不使用额外的聚合缓冲区，内存占用更小。
 * 
 * 特点:
 *   1. 不使用聚合缓冲区，直接使用原始数据指针
 *   2. 使用 for 循环，固定 max_agg_num 次迭代
 *   3. 在循环中逐个填充描述符（ATBM_TX_DESC_FILL(i, ...)）
 *   4. WSM 序列号在最后统一递增一次（而非每个包递增）
 *   5. 错误处理更简化
 * 
 * 与第一个实现的区别:
 *   - ❌ 不使用聚合缓冲区，不复制数据（节省内存）
 *   - ✅ 内存占用更小（无 BUFF_ALLOC_LEN 额外开销）
 *   - ✅ 实现更简单
 *   - ⚠️ 性能可能较低（逐个处理，无批量复制优化）
 *   - ⚠️ 总线开销可能较大（无批量传输优化）
 * 
 * 处理流程:
 *   1. 获取描述符
 *   2. for 循环（最多 max_agg_num 次）：
 *      - 从队列获取数据包
 *      - 直接使用数据包指针（tx_desc->data[i] = data）
 *      - 在循环中填充描述符（ATBM_TX_DESC_FILL(i, ...)）
 *      - 如果遇到命令帧或需要确认的帧，退出循环
 *   3. 统一提交描述符
 *   4. WSM 序列号统一递增一次
 * 
 * 参考: doc/TX聚合宏定义关系说明.md, doc/TX发包流程文档.md
 **************************************************************************************************************/
int atbm_hmac_xmit_data(struct sbus_priv *self)
{
	unsigned int pipe;
	int status = 0;
	int tx_burst = 0;
	int vif_selected;
	struct wsm_hdr_tx *wsm;
	struct atbmwifi_common *hw_priv=self->core;
	atbm_void *txdata =ATBM_NULL;
	atbm_uint8 *data =ATBM_NULL;
	int tx_len=0;
	int ret = 0;
	int desc_id =-1;
	int i;
	struct sbus_desc *tx_desc = ATBM_NULL;
	unsigned long flags;
	/* 计算最大聚合数量：取驱动层限制（INF_BUF_SG_NUM）和固件层限制（numInpChBufs-2）的最小值 */
	/* 注意：必须确保不超过 tx_desc->data 数组大小（INF_BUF_SG_NUM），防止越界 */
	int max_agg_num = INF_BUF_SG_NUM;	
	/* 固件层限制：numInpChBufs-2（保留2个缓冲区用于其他用途） */
	int fw_limit = hw_priv->wsm_caps.numInpChBufs - 2;

	/* 安全检查：确保 max_agg_num 不超过数组大小，防止越界 */
	if(fw_limit > 0){
		max_agg_num = MIN(fw_limit, INF_BUF_SG_NUM);
	}
	else {
		max_agg_num = 1;
	}
	
	//If Usb disconnecting, stop tx usb packet to host
	if(self->suspend){
		return -2;
	}
	wifi_printk(WIFI_IF, "atbm_usb_xmit_data++\n");
	atbm_spin_lock_irqsave(&hw_priv->tx_com_lock,&flags);
	desc_id = ATBM_TX_DESC_GET(self,self->drvobj,TX_DESC_NUM);
	if(desc_id<0){
		wifi_printk(WIFI_DBG_MSG, "atbm_usb_xmit_data:desc_id<0\n");
		status=-4;
		goto error;
	}

	tx_desc = &self->tx_desc[desc_id];
#if CONFIG_WIFI_BT_COMB
	tx_desc->cnf = 1;
#endif
	for(i = 0; i < max_agg_num; i++){
		/* 安全检查：确保 i 不超过数组大小，防止越界 */
		if (i >= INF_BUF_SG_NUM) {
			wifi_printk(WIFI_DBG_ERROR, "Warning: i(%d) >= INF_BUF_SG_NUM(%d), break to prevent overflow\n", 
			           i, INF_BUF_SG_NUM);
			break;
		}
		
		ret = wsm_get_tx(hw_priv, &data, &tx_len, &tx_burst,
					&vif_selected);
		if (ret <= 0) {
			  wifi_printk(WIFI_IF,"tx:atbm_usb_urb_put ATBM_NULL %d\n",desc_id);
			  break;
		} else {
			wsm = (struct wsm_hdr_tx *)data;

			tx_desc->data[i] = data;
			current_ops->build_hdr(hw_priv, wsm, &tx_len);

			add_tx_skb_cnt();
			self->tx_vif_selected =vif_selected;
			wsm->id &= atbm_cpu_to_le32(~WSM_TX_SEQ(WSM_TX_SEQ_MAX));
			wsm->id |= atbm_cpu_to_le32(WSM_TX_SEQ(hw_priv->wsm_tx_seq));
			txdata =(atbm_void*)wsm;

			//wifi_printk(WIFI_ALWAYS, "wsm->id:wsm->id %d seq %d desc_id %d\n",wsm->id,hw_priv->wsm_tx_seq,desc_id);
			ATBM_TX_DESC_FILL(i, tx_desc->test_desc,txdata,tx_len,self->drvobj,atbm_hmac_xmit_data_complete,tx_desc);
			if(wsm_txed(hw_priv, data)==0){
	            struct wsm_tx *tx = (struct wsm_tx *)data;
				hw_priv->wsm_txframe_num++;
				//add by wp, 
				//this is to fix bug , because in sometime atbm_urb_coml is call later than txconfirm
				//when need txconfirm frame , txconfirm will free skb, but atbm_urb_coml need skb
	            if(tx->htTxParameters & atbm_cpu_to_le32(WSM_HT_TX_NEED_CONFIRM)){
	                tx_desc->data[i] = ATBM_NULL;
	            }
			}else {
				tx_desc->data[i] = ATBM_NULL;
				atbm_spin_lock(&hw_priv->wsm_cmd.lock);
				tx_desc->cnf = hw_priv->wsm_cmd.cnf;
				atbm_spin_unlock(&hw_priv->wsm_cmd.lock);
				i += 1;
				break;
			}
		}
	}

	if(i > 0){
		tx_desc->frame_cnt = i;
		tx_desc->link =1;
		add_tx_pkt_cnt();
		status = ATBM_TX_SUBMIT_DESC(tx_desc->test_desc, GFP_ATOMIC);
		if (status) {
			status = -5;
			if (vif_selected != -1) {
				atbm_hmac_free_err_data(self,tx_desc);
			}else {
				atbm_hmac_free_err_cmd(self);
			}
			ATBM_TX_DESC_RESTORE(self,self->drvobj,desc_id);
			wifi_printk(WIFI_DBG_ANY," <ERROR>tx:atbm_usb_urb_put %d\n",desc_id);
			//msleep(1000);
			goto error;
		}
		self->tx_seqnum++;
		wifi_printk(WIFI_IF, "tx_seq %d\n",self->tx_seqnum);
		hw_priv->wsm_tx_seq++;
	}else{
		ATBM_TX_DESC_RESTORE(self,self->drvobj,desc_id);
		status = -3;
	}
error:
	atbm_spin_unlock_irqrestore(&hw_priv->tx_com_lock,flags);
	//atbm_atomic_set(&self->tx_lock, 0);
	return status;
}
#endif

/**************************************************************************************************************
 * 函数名: atbm_hmac_receive_data_complete
 *
 * 功能描述:
 *   接收完成回调函数，当硬件接收数据完成时被调用（USB URB 完成或 SDIO 读取完成）。
 *   负责验证接收状态、处理数据包、将数据包加入队列并唤醒 BH 任务。
 *
 * 在 RX 流程中的位置:
 *   阶段 1: 硬件接收完成 → 队列入队
 *   USB: URB 完成回调中调用
 *   SDIO: atbm_sdio_process_rx() 中调用
 *
 * 参数说明:
 *   @atbm_desc: 接口描述符指针，包含接收状态、实际长度、上下文等信息
 *              - status: 接收状态（0 表示成功）
 *              - actual_length: 实际接收的数据长度
 *              - context: 指向 sbus_desc 的指针
 *
 * 返回值:
 *   无
 *
 * 处理流程:
 *   1. 获取上下文：从描述符中获取 rx_desc、sbus_priv、skb、hw_priv
 *   2. 状态检查：检查接收状态，非 0 表示错误
 *   3. USB 挂起处理：如果启用 USB_SUSPEND_SUPPORT，处理挂起状态下的数据重组
 *   4. 长度验证：验证接收长度是否合法（>= 4 字节，<= RX_BUFFER_SIZE）
 *   5. 队列入队：
 *      a. 如果启用 ATBM_IMMD_RX，直接处理数据包
 *      b. 否则，将 skb 加入 rx_frame_queue 队列
 *   6. 唤醒 BH 任务：设置 bh_rx 标志并唤醒 bh_thread
 *   7. 错误处理：如果发生错误，释放资源或重新提交接收请求
 *
 * 队列操作:
 *   - 入队: atbm_skb_queue_tail(&hw_priv->rx_frame_queue, skb)
 *   - 唤醒: atbm_wakeupThreadInternal(hw_priv->bh_thread)
 *
 * 注意事项:
 *   - 此函数在中断上下文中执行（USB）或任务上下文中执行（SDIO）
 *   - 需要快速处理，避免长时间阻塞
 *   - 错误时需要正确释放资源
 *   - USB 挂起状态下需要处理数据重组
 *
 * 相关函数:
 *   - atbm_hmac_receive_data(): 提交接收请求
 *   - atbm_rx_task(): 从队列取出数据包处理
 *   - atbm_hmac_bh(): BH 任务主函数
 *
 **************************************************************************************************************/
atbm_void  atbm_hmac_receive_data_complete( ATBM_INF_DESC *atbm_desc)
{
	struct sbus_desc *rx_desc = (struct sbus_desc*)atbm_desc->context;
	struct sbus_priv *self = rx_desc->obj;
	struct atbm_buff *skb = rx_desc->test_skb;
	struct atbmwifi_common *hw_priv = self->core;
	int RecvLength = atbm_desc->actual_length;
	struct wsm_hdr *wsm;
	unsigned long flags;

	wifi_printk(WIFI_IF, "rxendd  Len %d desc_id %d\n",RecvLength,rx_desc->desc_id);

	if(!hw_priv)
		goto __free;

	rx_desc->link =0;

	switch(atbm_desc->status){
		case 0:
			break;
		default:
			wifi_printk(WIFI_DBG_ANY, "atbm_usb_rx_complete2 error status=%d len %d\n",atbm_desc->status,RecvLength);
			goto __free;
	}
#if USB_SUSPEND_SUPPORT
	if((self->drvobj->suspend_skb_len != 0)&&(self->drvobj->suspend_skb != ATBM_NULL))
	{
		if(atbm_skb_tailroom(self->drvobj->suspend_skb)<self->drvobj->suspend_skb_len+RecvLength)
		{
			struct atbm_buff * long_suspend_skb = ATBM_NULL;
			ATBM_BUG_ON(self->drvobj->suspend_skb_len+RecvLength>RX_BUFFER_SIZE);
			long_suspend_skb = atbm_dev_alloc_skb(RX_BUFFER_SIZE+64);
			ATBM_BUG_ON(!long_suspend_skb);
			atbm_skb_reserve(long_suspend_skb, 64);
			atbm_memcpy((atbm_uint8 *)ATBM_OS_SKB_DATA(long_suspend_skb),ATBM_OS_SKB_DATA(self->drvobj->suspend_skb),self->drvobj->suspend_skb_len);
			atbm_dev_kfree_skb(self->drvobj->suspend_skb);
			self->drvobj->suspend_skb = long_suspend_skb;
		}
		atbm_memcpy((atbm_uint8 *) ATBM_OS_SKB_DATA(self->drvobj->suspend_skb) + self->drvobj->suspend_skb_len,ATBM_OS_SKB_DATA(skb),RecvLength);
		RecvLength += self->drvobj->suspend_skb_len;
		atbm_memcpy(ATBM_OS_SKB_DATA(skb),(atbm_uint8 *)ATBM_OS_SKB_DATA(self->drvobj->suspend_skb),RecvLength);
		self->drvobj->suspend_skb_len = 0;					
	}
	
	wsm = (struct wsm_hdr *)ATBM_OS_SKB_DATA(skb);
	wifi_printk(WIFI_DBG_MSG,"atbm: atbm_usb_receive_data_complete()--len=%d, id=%d\n",wsm->len, wsm->id);
	if (wsm->len != RecvLength){
		if(((wsm->len  % 512)==0) && ((wsm->len+1) == RecvLength)){
			//this correct , lmac output len = (wsm len +1 ) ,inorder to let hmac usb callback
		}
		else {
			wifi_printk(WIFI_IF,"rx rebulid usbsuspend  id %d wsm->len %d,RecvLength %d\n",wsm->id,wsm->len,RecvLength);

			if(self->drvobj->suspend_skb_len== 0){ 
				if(wsm->len > RX_BUFFER_SIZE){
					wifi_printk(WIFI_DBG_ERROR," %s %d id %d wsm->len %d,RecvLength %d\n",__FUNCTION__,__LINE__,wsm->id,wsm->len,RecvLength);
					goto resubmit;
				}
				wifi_printk(WIFI_DBG_ERROR, "rx rebulid usbsuspend0	\n");
				/*
				* alloc 4K buff for suspend_skb is save
				*/
				ATBM_BUG_ON(self->drvobj->suspend_skb == ATBM_NULL);
				atbm_memcpy((atbm_uint8 *)ATBM_OS_SKB_DATA(self->drvobj->suspend_skb),ATBM_OS_SKB_DATA(skb),RecvLength);
				self->drvobj->suspend_skb_len = RecvLength;
				goto resubmit;
			}
		}
	}
	
	ATBM_WARN_ON_FUNC(self->drvobj->suspend_skb_len != 0);
#endif  //USB_SUSPEND_SUPPORT

	if (ATBM_WARN_ON(4 > RecvLength)){
		wsm = (struct wsm_hdr *)ATBM_OS_SKB_DATA(skb);
		wifi_printk(WIFI_DBG_ERROR,"%s %d id %d wsm->len %d,RecvLength %d\n",__FUNCTION__,__LINE__,wsm->id,wsm->len,RecvLength);
		//frame_hexdump("atbm_usb_receive_data_complete",(atbm_uint8 *)wsm,32);
		goto resubmit;
	}

	ATBM_BUG_ON(RecvLength > RX_BUFFER_SIZE);
	self->rx_seqnum++;

	add_rx_pkt_cnt();
	rx_desc->test_skb = ATBM_NULL;
	ATBM_OS_SKB_LEN(skb) = RecvLength;
	//atbm_spin_lock_irqsave(&self->lock, &flags);
#if ATBM_IMMD_RX
	{
		current_ops->process_rx(hw_priv, skb);
		ATBM_RX_DESC_PUT(self,self->drvobj,rx_desc->desc_id);
		return;
	}
#else
	
	ATBM_RX_DESC_PUT(self,self->drvobj,rx_desc->desc_id);
	atbm_skb_queue_tail(&hw_priv->rx_frame_queue, skb);

	atbm_atomic_set(&hw_priv->bh_rx, 1);
	wifi_printk(WIFI_BH,"atbm_bh_wakeup rxend\n");
	atbm_wakeupThreadInternal(hw_priv->bh_thread);

	wifi_printk(WIFI_DBG_MSG,"atbm: atbm_usb_receive_data_complete() 5.\n");

	if(!hw_priv->init_done){
		wifi_printk(WIFI_DBG_ERROR, "[BH] irq. init_done =0 drop\n");
		goto __free;
	}
	if (/* ATBM_WARN_ON */(hw_priv->bh_error))
		goto __free;

	wifi_printk(WIFI_DBG_MSG,"atbm: atbm_usb_receive_data_complete() 6.\n");
	return;
#endif
__free:
	if(self->drvobj->suspend_skb_len != 0){
		wifi_printk(WIFI_DBG_ERROR,"rx rebulid usbsuspend3	rx drop\n");
	}
	//atbm_usb_urb_put(self,self->drvobj->rx_urb_map,rx_urb->desc_id);
	wifi_printk(WIFI_DBG_ERROR, "[WARNING] atbm_hmac_receive_data drop\n");

	return;

resubmit:
	if(!hw_priv->init_done){
		wifi_printk(WIFI_DBG_ERROR, "[BH] irq. init_done =0 drop\n");
		goto __free;
	}
	if (/* ATBM_WARN_ON */(hw_priv->bh_error))
		goto __free;

	ATBM_RX_DESC_PUT(self,self->drvobj,rx_desc->desc_id);
	atbm_hmac_receive_data(hw_priv->sbus_priv,0x2,ATBM_NULL,RX_BUFFER_SIZE);
	wifi_printk(WIFI_DBG_ANY, "atbm_hmac_receive_data resubmit\n");
	return;
}

/**************************************************************************************************************
 * 函数名: atbm_hmac_receive_data
 *
 * 功能描述:
 *   提交接收请求到硬件，准备接收下一个数据包。
 *   这是 RX 流程的起点，负责从描述符池获取描述符并提交到硬件。
 *
 * 在 RX 流程中的位置:
 *   阶段 0: 提交接收请求
 *   - 初始化时调用，启动接收链
 *   - 接收完成后调用，保持接收链
 *
 * 参数说明:
 *   @self:  总线私有数据指针，包含描述符池和总线状态
 *   @addr:  地址（通常为 0x2，表示数据端点）
 *   @dst:   目标缓冲区（通常为 NULL，由描述符管理）
 *   @count: 接收长度（通常为 RX_BUFFER_SIZE）
 *
 * 返回值:
 *   0:  成功
 *   -2: 总线挂起或提交失败
 *   -4: 描述符获取失败（描述符池已空）
 *
 * 处理流程:
 *   1. 检查总线状态：如果总线挂起，直接返回 -2
 *   2. 获取描述符：从描述符池获取一个可用的接收描述符
 *   3. 填充描述符：设置描述符的缓冲区、长度、完成回调等
 *   4. 提交描述符：
 *      - USB: 提交 URB 到 USB 硬件
 *      - SDIO: 提交到 SDIO 消息队列
 *   5. 错误处理：如果提交失败，恢复描述符到池中
 *
 * 描述符管理:
 *   - 获取: ATBM_RX_DESC_GET()
 *   - 填充: ATBM_RX_DESC_FILL()
 *   - 提交: ATBM_RX_SUBMIT_DESC()
 *   - 恢复: ATBM_RX_DESC_PUT()
 *
 * 注意事项:
 *   - 描述符从池中获取，避免频繁的内存分配
 *   - 提交后，硬件会异步接收数据
 *   - 接收完成后，atbm_hmac_receive_data_complete() 会被调用
 *   - 需要保持接收链，确保始终有描述符等待接收
 *
 * 相关函数:
 *   - atbm_hmac_receive_data_complete(): 接收完成回调
 *   - atbm_process_rx_packet_v0/v1(): 处理完成后调用此函数重新提交
 *
 **************************************************************************************************************/
int atbm_hmac_receive_data(struct sbus_priv *self,unsigned int addr,atbm_void  *dst, int count)
{
	unsigned int pipe;
	int status = 0;
	struct sbus_desc *rx_desc;
	int desc_id;
	struct atbmwifi_common *hw_priv=self->core;

	//If Usb disconnecting, stop rx usb packet from host
	if(self->suspend){
		return -2;
	}
	desc_id = ATBM_RX_DESC_GET(self, self->drvobj, RX_DESC_NUM);
	if(desc_id<0){
		status=-4;
		goto __err_rx;
	}
	rx_desc = &self->rx_desc[desc_id];

	ATBM_RX_DESC_FILL(rx_desc->test_desc,rx_desc->test_skb,count,self->drvobj,atbm_hmac_receive_data_complete,rx_desc);
	rx_desc->link = 1;
	status = ATBM_RX_SUBMIT_DESC(rx_desc->test_desc, 0);
	wifi_printk(WIFI_IF, "ATBM_RX_SUBMIT_DESC rx desc_id %d\n",desc_id);
	if (status) {
		status = -2;
		wifi_printk(WIFI_DBG_ERROR,"receive_data ATBM_RX_SUBMIT_DESC ++ ERR %d\n",status);
		goto __err_skb;
	}
__err_skb:
	if(status < 0){
		ATBM_RX_DESC_PUT(self,self->drvobj,desc_id);
	}
__err_rx:
	return status;
}

/**************************************************************************************************************
 * 函数名: atbm_register_bh
 *
 * 功能描述:
 *   注册并创建 BH（Bottom Half）任务，初始化 RX/TX 处理所需的资源。
 *   这是驱动初始化的关键步骤，创建数据收发的核心任务。
 *
 * 在 RX 流程中的位置:
 *   初始化阶段: 创建 BH 任务
 *   在驱动初始化时调用（atbm_core_init() 或类似函数）
 *
 * 参数说明:
 *   @hw_priv: 硬件私有数据指针，包含驱动的全局状态
 *
 * 返回值:
 *   0:  成功
 *   -1: 任务创建失败
 *
 * 处理流程:
 *   1. 选择操作集：根据 HIF 版本选择 V0 或 V1 操作集
 *   2. 初始化原子变量：bh_rx、bh_tx 清零
 *   3. 初始化等待事件：wsm_cmd_wq
 *   4. 初始化终止标志：bh_term、bh_error 清零
 *   5. 初始化 TX 聚合缓冲区（如果启用 CONFIG_INF_AGGR_TX）
 *   6. 初始化 SDIO 队列（如果是 SDIO 接口）
 *   7. 创建 URB 完成任务（如果启用 ATBM_TX_CMPL_TASK）
 *   8. 创建 BH 任务：
 *      - 任务名称: "atbm_bh"
 *      - 回调函数: atbm_hmac_bh
 *      - 优先级: BH_TASK_PRIO
 *      - 超时: 100*ATBM_HZ
 *
 * 创建的任务:
 *   - atbm_bh: 主 BH 任务，处理 RX/TX
 *   - atbm_bh_cmpl: URB 完成任务（可选）
 *
 * 注意事项:
 *   - 必须在驱动初始化时调用
 *   - 任务创建失败会导致驱动无法正常工作
 *   - 需要在 atbm_unregister_bh() 中释放资源
 *
 * 相关函数:
 *   - atbm_unregister_bh(): 注销 BH 任务
 *   - atbm_hmac_bh(): BH 任务主函数
 *   - atbm_createThreadInternal(): 创建任务
 *
 **************************************************************************************************************/
int atbm_register_bh(struct atbmwifi_common *hw_priv)
{
	atbm_thread_internal_t *bh_thread;
#if ATBM_TX_CMPL_TASK
	atbm_thread_internal_t *cmpl_bh_thread;
#endif
	int status =0;

	current_ops = &atbm_ops[hw_priv->chip_priv->hif_version];
	hw_priv->chip_priv->hdr_rollback = current_ops->hdr_rollback;
	atbm_atomic_set(&hw_priv->bh_rx, 0);
	atbm_atomic_set(&hw_priv->bh_tx, 0);
	atbm_os_init_waitevent(&hw_priv->wsm_cmd_wq);
	hw_priv->bh_term=0;
	hw_priv->bh_error=0;
	wifi_printk(WIFI_DBG_ERROR,"atbm_register_bh\n");
#if CONFIG_INF_AGGR_TX
	atbm_init_tx_aggr_buf(hw_priv->sbus_priv->tx_desc,TX_DESC_NUM, BUFF_ALLOC_LEN);
#endif

#if ATBM_SDIO_BUS
	atbm_sdio_queue_init(hw_priv);
#endif
#if ATBM_TX_CMPL_TASK
	/*Create RxData Task*/
	cmpl_bh_thread=atbm_createThreadInternal("atbm_bh_cmpl",
									(thread_callback)atbm_hmac_cmpl_bh,
									(atbm_void*)hw_priv,
									CMPL_BH_TASK_PRIO,
									10*ATBM_HZ);
	if (!cmpl_bh_thread){
		wifi_printk(WIFI_DBG_ERROR,"cmpl_bh_thread Failed\n");
		return -1;
	}
	hw_priv->cmpl_bh_thread = cmpl_bh_thread;
#endif

	/*Create RxData Task*/
	bh_thread=atbm_createThreadInternal("atbm_bh",
									(thread_callback)atbm_hmac_bh,
									(atbm_void*)hw_priv,
									BH_TASK_PRIO,
									100*ATBM_HZ);
	if (!bh_thread){
		wifi_printk(WIFI_DBG_ERROR,"bh_thread Failed\n");
		return -1;
	}
	hw_priv->bh_thread = bh_thread;

	return status;
}

atbm_void atbm_unregister_bh(struct atbmwifi_common *hw_priv)
{
	//kill createThread
	hw_priv->bh_term=1;
	atbm_stopThreadInternal(hw_priv->bh_thread);

	atbm_os_delete_waitevent(&hw_priv->wsm_cmd_wq);
	atbm_os_DeleteMutex(&hw_priv->wsm_cmd_mux);
#if ATBM_SDIO_BUS
	atbm_sdio_queue_exit(hw_priv);
#endif
#if CONFIG_INF_AGGR_TX
	atbm_free_tx_aggr_buf(hw_priv->sbus_priv->tx_desc, TX_DESC_NUM);
#endif
#if ATBM_TX_CMPL_TASK
	atbm_stopThreadInternal(hw_priv->cmpl_bh_thread);
#endif
}


