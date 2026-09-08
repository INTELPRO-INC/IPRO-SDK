/**************************************************************************************************************
 * altobeam RTOS
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/
#ifdef ATBM_DHCP
#include <udp.h>
#include <ip.h>
#endif

#include "atbm_hal.h"
#include "atbm_proto.h"
#include "atbm_ratectrl.h"

extern struct atbmwifi_common g_hw_prv;
extern 	atbm_int32 globle_rate ;
int tx_rate_down = 0;
int tx_rate_static = 1;
/* TX policy cache implementation					*/
#define IS_BOOTP_PORT(src_port,des_port) ((((src_port) == 67)&&((des_port) == 68)) || \
										   (((src_port) == 68)&&((des_port) == 67)))
#define ATBM_APOLLO_INVALID_RATE_ID (0xFF)
#define RATE_INDEX_N_6_5M 14
#define __hweight8(w)		\
      (	(!!((w) & (1ULL << 0))) +	\
	(!!((w) & (1ULL << 1))) +	\
	(!!((w) & (1ULL << 2))) +	\
	(!!((w) & (1ULL << 3))) +	\
	(!!((w) & (1ULL << 4))) +	\
	(!!((w) & (1ULL << 5))) +	\
	(!!((w) & (1ULL << 6))) +	\
	(!!((w) & (1ULL << 7)))	)

#ifndef LINUX_OS
#define hweight16(w) ((__hweight8(w))+((__hweight8(w))>>8))
#endif

/* This is a version of the rx handler that can be called from hard irq
 * context. Post the skb on the queue and schedule the tasklet */

int atbmwifi_rx_filter_retry(struct atbmwifi_vif *priv,atbm_uint8 link_id,struct atbm_buff *skb);
int atbmwifi_rx_filter_mac(struct atbmwifi_vif *priv, struct atbm_buff *skb);
int atbmwifi_ieee80211_rx(struct atbmwifi_vif *priv,struct atbm_buff *skb);
int __atbmwifi_ieee80211_rx(struct atbmwifi_vif *priv,struct atbm_buff *skb);
extern atbm_uint8 atbm_wmm_status_get(atbm_void);
extern atbm_void hostapd_eap_wsc_init(struct atbmwifi_vif *priv,atbm_uint8 *da);
extern atbm_void atbmwifi_set_rssi(atbm_int8 rssi);
extern atbm_void atbmwifi_set_SNR(atbm_int8 rssi);

void atbm_sdio_suspend_rx(struct atbmwifi_common *hw_priv);
void atbm_sdio_resume_rx(struct atbmwifi_common *hw_priv);




struct atbm_icmp_echo_hdr {
    unsigned char  type;   /* ���衧��D��a */
    unsigned char  code;   /* ??��2?? */
    unsigned short chksum; /* D?��?��|o��a */
    unsigned short id;     /* ?����o��o??��? */
    unsigned short seqno;  /* D���㡧��Do? */
};

struct atbm_iphdr {
    unsigned int ihl:4;        // IP ��a?��2?3?��?����
    unsigned int version:4;    // IP ????��?
    unsigned char tos;          // ?��t??���衧��D��a
    unsigned short tot_len;     // ?����13?��?���� (??��1����?�졧a?��2?o��a��oy?Y)
    unsigned short id;          // ?����o��o??��?
    unsigned short frag_off;    // ?��?????����?
    unsigned char ttl;          // ��|��2???��o?��??
    unsigned char protocol;     // ��|?2?D-���㡧| (����? TCP, UDP)
    unsigned short check;       // D?��?��|o��a
    unsigned int saddr;         // ??? IP |��???��
    unsigned int daddr;         // ??|��? IP |��???��
};
/*
#define IP_PROTO_ICMP    1
#define IP_PROTO_IGMP    2
#define IP_PROTO_UDP     17
#define IP_PROTO_UDPLITE 136
#define IP_PROTO_TCP     6

*/
int ieee80211_is_rx_icmp_frame(struct atbm_buff *skb,int in)
{

	//const struct iphdr *ip;	
	unsigned short ether_type;
	struct atbmwifi_ieee80211_hdr *hdr = (struct atbmwifi_ieee80211_hdr *)skb->abuf;

	unsigned char  mac_hdr_len=atbmwifi_ieee80211_hdrlen(hdr->frame_control);
	
	unsigned char *ip = NULL;
	struct atbm_iphdr *skbiphdr = NULL;
	//int iphdr_index = 0;
	//struct ieee80211_hdr *frame = (struct ieee80211_hdr *)skb->data;
	//iphdr_index = ieee80211_hdrlen(frame->frame_control)+8;
	//skbiphdr = (struct iphdr *)(skb->data+iphdr_index);
	struct atbmwifi_ieee80211_mgmt *mgmt = (struct atbmwifi_ieee80211_mgmt *)skb->abuf;

	if(!atbmwifi_ieee80211_is_data(hdr->frame_control))
		return 0;

	if (hdr->frame_control&atbm_cpu_to_le32(ATBM_IEEE80211_FCTL_PROTECTED))
	{
		mac_hdr_len+=8;
	}
	
	skbiphdr =(struct atbm_iphdr *)((unsigned char *)skb->abuf+mac_hdr_len +6+2);
	ether_type=*(unsigned short*)((unsigned char *)skb->abuf+mac_hdr_len+6);
	//wifi_printk(WIFI_ALWAYS,"%s:%s , ether_type=%d , %d\n",__func__,in?"rx":"tx",ether_type,htons(ATBM_ETH_P_IP));
	if (ether_type == atbm_htons(ATBM_ETH_P_IP))
	{
	//	wifi_printk(WIFI_ALWAYS,"%s:protocol=%d , %d\n",__func__,skbiphdr->protocol,IP_PROTO_TCP);
		if(skbiphdr->protocol == 1/*IP_PROTO_ICMP*/)
		{
			
			struct atbm_icmp_echo_hdr *icmp_packet = (struct atbm_icmp_echo_hdr *)((unsigned char *)skbiphdr+(skbiphdr->ihl<<2));
			if(in){
				wifi_printk(WIFI_ALWAYS,"rx icmp: %d\n",icmp_packet->type);
			}else{
				wifi_printk(WIFI_ALWAYS,"tx icmp: %d\n",icmp_packet->type);
			}
			if(icmp_packet->type == 0 || icmp_packet->type == 8){
				wifi_printk(WIFI_ALWAYS,"ieee80211 seq:%d \n",mgmt->seq_ctrl>>4);
				ip	= (char *)&(skbiphdr->saddr);
				wifi_printk(WIFI_ALWAYS,"saddr:%d.%d.%d.%d \n",ip[0],ip[1],ip[2],ip[3]);
				ip	= (char *)&(skbiphdr->daddr);
				wifi_printk(WIFI_ALWAYS,"daddr:%d.%d.%d.%d \n",ip[0],ip[1],ip[2],ip[3]);
				wifi_printk(WIFI_ALWAYS,"icmp type:%d %s \n",icmp_packet->type,icmp_packet->type == 0?"reply":"echo");
				wifi_printk(WIFI_ALWAYS,"icmp id:%d \n",atbm_ntohs(icmp_packet->id));
				wifi_printk(WIFI_ALWAYS,"icmp seq:%d \n",atbm_ntohs(icmp_packet->seqno));
				return 1;
			}
		}
	}
	return 0;
}





#define ATBM_MAX_RX_QUEUE_NUM 100

/**************************************************************************************************************
 * 函数名: atbm_netrx_task
 *
 * 功能描述:
 *   网络 RX 任务处理函数，从 rx_task_skb_list 队列中取出数据包并进行 802.11 到 802.3 转换，
 *   然后提交到协议栈。这是异步 RX 处理模式（ATBM_RX_TASK_QUEUE == 1）的核心处理函数。
 *
 * 在 RX 流程中的位置:
 *   阶段 6: 802.11 层处理 → 协议栈提交
 *   当启用 ATBM_RX_TASK_QUEUE 时，在独立的 RX 任务中调用
 *
 * 参数说明:
 *   @priv: 虚拟接口私有数据指针，包含 rx_task_skb_list 队列
 *
 * 返回值:
 *   无
 *
 * 处理流程:
 *   1. 循环从 rx_task_skb_list 队列中取出数据包
 *   2. AMSDU 解聚合：调用 atbmwifi_ieee80211_rx_h_amsdu() 处理 AMSDU
 *   3. 格式转换：调用 atbmwifi_ieee80211_data_to_8023() 将 802.11 转换为 802.3
 *   4. 协议栈提交：调用 atbmwifi_ieee80211_deliver_skb() 提交到协议栈
 *   5. 资源释放：如果需要，释放 skb
 *   6. 继续处理直到队列为空
 *
 * 队列说明:
 *   - rx_task_skb_list: 存储需要延迟处理的数据包
 *   - 入队位置: __atbmwifi_ieee80211_rx()
 *   - 出队位置: 本函数
 *
 * 注意事项:
 *   - 仅在 ATBM_RX_TASK_QUEUE == 1 时使用
 *   - AMSDU 解聚合后会产生多个子帧，需要分别处理
 *   - 格式转换失败时需要释放 skb
 *
 * 相关函数:
 *   - atbm_rx_task_work(): 工作队列回调，调用此函数
 *   - __atbmwifi_ieee80211_rx(): 将数据包加入队列
 *   - atbmwifi_ieee80211_rx_h_amsdu(): AMSDU 解聚合
 *   - atbmwifi_ieee80211_data_to_8023(): 802.11 到 802.3 转换
 *   - atbmwifi_ieee80211_deliver_skb(): 提交到协议栈
 *
 **************************************************************************************************************/
atbm_void atbm_netrx_task(struct atbmwifi_vif *priv)
{
	struct atbm_buff *skb = ATBM_NULL;
	atbm_uint16 needfree = 1;
	int ret;
	do{
		skb = atbm_skb_dequeue(&priv->rx_task_skb_list);
		if(skb == ATBM_NULL){
			break;
		}
#if (ATBM_RX_TASK_QUEUE == 2)
		atbmwifi_tx_start(skb, priv);
#else
		ret = atbmwifi_ieee80211_rx_h_amsdu(priv,skb);
		if(ret){
			needfree = 0;
			goto __free_return;
		}
		ret = atbmwifi_ieee80211_data_to_8023(skb, priv->mac_addr,priv->iftype);
		if(ret){
			//needfree = 1;
			wifi_printk(WIFI_RX,"atbmwifi_ieee80211_data_to_8023 err\n");
			goto __free_return;
		}

		atbmwifi_ieee80211_deliver_skb(priv,skb,&needfree);
__free_return:
		if(needfree){
			atbm_dev_kfree_skb(skb);
		}
#endif
	}while(1);
}

void atbm_rx_task_work(struct atbm_work_struct *work)
{
	struct atbmwifi_vif *priv = (struct atbmwifi_vif *)work;
	atbm_netrx_task(priv);
}
/**************************************************************************************************************
 * 函数名: atbmwifi_ieee80211_rx_irqsafe
 *
 * 功能描述:
 *   802.11 帧接收入口函数（中断安全版本），负责过滤和分发接收到的数据帧。
 *   这是从 WSM 层到 802.11 层的接口函数，处理数据帧的过滤和分发。
 *
 * 在 RX 流程中的位置:
 *   阶段 5: WSM 处理 → 802.11 帧处理
 *   在 wsm_handle_rx() 中调用，处理 WSM_RECEIVE_INDICATION_ID 消息
 *
 * 参数说明:
 *   @priv: 虚拟接口私有数据指针
 *   @skb:  接收到的数据包（802.11 帧格式）
 *
 * 返回值:
 *   0:  成功处理（skb 已被处理或释放）
 *   -1: 过滤失败（重试帧或 MAC 地址过滤）
 *
 * 处理流程:
 *   1. 重试帧过滤：调用 atbmwifi_rx_filter_retry() 过滤重复帧
 *   2. MAC 地址过滤：调用 atbmwifi_rx_filter_mac() 过滤非目标帧
 *   3. 分发处理：调用 __atbmwifi_ieee80211_rx() 进行帧类型分发
 *   4. 资源释放：如果 __atbmwifi_ieee80211_rx() 返回需要释放，释放 skb
 *
 * 过滤机制:
 *   - 重试帧过滤：检测并丢弃重复的重传帧
 *   - MAC 地址过滤：检查目标地址是否匹配
 *
 * 注意事项:
 *   - 函数名包含 "irqsafe"，但实际上可能在任务上下文中调用
 *   - 过滤失败时返回 -1，调用者需要释放 skb
 *   - 成功处理时返回 0，skb 已被处理或释放
 *
 * 相关函数:
 *   - atbmwifi_rx_filter_retry(): 重试帧过滤
 *   - atbmwifi_rx_filter_mac(): MAC 地址过滤
 *   - __atbmwifi_ieee80211_rx(): 帧类型分发处理
 *
 **************************************************************************************************************/
 int atbmwifi_ieee80211_rx_irqsafe(struct atbmwifi_vif *priv,struct atbm_buff *skb)
{
	struct atbmwifi_ieee80211_rx_status *hdr = (struct atbmwifi_ieee80211_rx_status *)ATBM_IEEE80211_SKB_RXCB(skb);
	
	if(atbmwifi_rx_filter_retry(priv,hdr->link_id,skb) <0){		
		wifi_printk(WIFI_RX, "[rx_retry] filter\n");
		return -1;
	}

	if(atbmwifi_rx_filter_mac(priv,skb) < 0){
		wifi_printk(WIFI_RX, "[rx_mac] filter\n");
		return -1;
	}
	
	#ifdef ATBM_RX_STATUS_USE_QUEUE
	skb->Type = RX_SKB__RX_PACKAGE;
	#endif

	if(__atbmwifi_ieee80211_rx(priv,skb)){
		atbm_dev_kfree_skb(skb);
	}

	return 0;
} 

#ifdef ATBM_RX_STATUS_USE_QUEUE
 int atbmwifi_ieee80211_tx_status_irqsafe(struct atbmwifi_vif *priv,struct atbm_buff *skb)
{
	skb->Type = RX_SKB__TX_STATUS;
	atbm_skb_queue_tail(&priv->rx_task_skb_list,skb);
	/*Do rx task schedule,change task excute text*/
	
	///TODO atbm_net_RxTask
	//atbm_netrx_task();
	atbm_queue_work(priv->hw_priv, priv->rx_task_work);
}
#endif

#if ATBM_PKG_REORDER

#define TID_IS_SAFE(tid_index,action)	if((tid_index)>ATBM_RX_DATA_QUEUES)	action
#define SEQ_IS_SAFE(seq_index,action) if((seq_index)>=BUFF_STORED_LEN)	action
#define reorder_debug(debug_en,...)  if(debug_en)	iot_printf(ATBM_KERN_DEBUG __VA_ARGS__)
#define REORDER_DEBUG		(0)
#define REORDER_ERROR		(1)
#define THE_RETRY_PKG_INEDX	(0x800)
#define BUFF_INDEX_IS_SAFE(index)	((index)&(BUFF_STORED_LEN-1))
#define DEUG_SPINLOCK		(0)
#if DEUG_SPINLOCK
#define spinlock_debug(type)		wifi_printk(WIFI_ALWAYS,"%s:tid_params_%s\n",__func__,#type)
#else
#define spinlock_debug(type)
#endif
#define tid_params_spin_lock(lock,type) \
	do									\
	{									\
		spinlock_debug(type);			\
		type(lock,0);						\
	}while(0)
#define tid_params_spin_unlock(lock,type)		\
	do										\
	{										\
		spinlock_debug(type);				\
		type(lock);							\
	}while(0)


static struct atbm_ba_tid_params *atbm_get_tid_params(struct atbm_reorder_queue_comm * atbm_reorder,atbm_uint8 tid)
{
	struct atbm_ba_tid_params *tid_params = ATBM_NULL;
	
	if(tid>=ATBM_RX_DATA_QUEUES)
	{
		return ATBM_NULL;
	}

//	atbm_os_mutexLock(&atbm_reorder->reorder_mutex,0);
	tid_params = &atbm_reorder->atbm_rx_tid[tid];
//	atbm_os_mutexUnLock(&atbm_reorder->reorder_mutex);

	return tid_params;
}
static atbm_void atbm_skb_buff_queue(struct atbm_ba_tid_params *tid_params,atbm_uint8 index,struct atbm_buff *skb)
{
	atbm_uint8 start_index = BUFF_INDEX_IS_SAFE(tid_params->start_seq);
	tid_params->skb_reorder_buff[index] = skb;

	//set time to the oldest time
	while(tid_params->frame_rx_time[index]==0){
		tid_params->frame_rx_time[index] = atbm_GetOsTimeMs();
		//reorder_debug(REORDER_DEBUG,"enqueue index %x\n",index);
		if(start_index == index)
			break;
		index =BUFF_INDEX_IS_SAFE(index-1);
	}

	tid_params->skb_buffed++; 
	ATBM_WARN_ON_FUNC(tid_params->wind_size<tid_params->skb_buffed);
 	//__atbm_skb_queue_tail(&tid_params->header, skb);
}
static atbm_void atbm_skb_buff_dequeue(struct atbmwifi_vif *priv,struct atbm_ba_tid_params *tid_params,atbm_uint8 index,int uplayer)
{
	struct atbm_buff *skb;
	 
	skb = tid_params->skb_reorder_buff[index];
	//reorder_debug(REORDER_DEBUG,"dequeue index %x\n",index);
	if(skb == ATBM_NULL){
		tid_params->frame_rx_time[index] = 0;
	}
	else {
		ATBM_WARN_ON_FUNC(tid_params->skb_buffed ==0);
		ATBM_WARN_ON_FUNC(tid_params->wind_size<tid_params->skb_buffed);
		tid_params->skb_buffed--;
		tid_params->frame_rx_time[index] = 0;
		tid_params->skb_reorder_buff[index] = ATBM_NULL; 
		
		if(uplayer==1){
			if(atbmwifi_ieee80211_rx_irqsafe(priv, skb) != 0)
				atbm_dev_kfree_skb(skb);
		}
		else {			
			atbm_dev_kfree_skb(skb);
		}
	}
	//__atbm_skb_dequeue(&tid_params->header)
}

int atbm_reorder_skb_forcedrop(struct atbmwifi_vif *priv,struct atbm_ba_tid_params *tid_params,int need_free)
{
	atbm_uint8 i = 0;
	atbm_uint8 seq = 0;	

	//reorder_debug(REORDER_DEBUG,"%s:has_buffed(%d)\n",__func__,tid_params->skb_buffed);
	seq = BUFF_INDEX_IS_SAFE(tid_params->start_seq);
	for(i=0;i<need_free;i++)
	{
		atbm_skb_buff_dequeue(priv,tid_params,seq,0);
		tid_params->start_seq = (tid_params->start_seq+1)&SEQ_NUM_MASKER;
		seq = BUFF_INDEX_IS_SAFE(tid_params->start_seq);
	}		

	return 0;
}
int atbm_reorder_skb_forcefree(struct atbmwifi_vif *priv,struct atbm_ba_tid_params *tid_params,int need_free)
{
	atbm_uint8 i = 0;
	atbm_uint8 seq = 0;	

	//reorder_debug(REORDER_DEBUG,"%s:has_buffed(%d)\n",__func__,tid_params->skb_buffed);
	seq = BUFF_INDEX_IS_SAFE(tid_params->start_seq);
	for(i=0;i<need_free;i++)
	{
		atbm_skb_buff_dequeue(priv,tid_params,seq,1);
		tid_params->start_seq = (tid_params->start_seq+1)&SEQ_NUM_MASKER;
		seq = BUFF_INDEX_IS_SAFE(tid_params->start_seq);
	}		

	return 0;
}

int atbm_reorder_skb_uplayer(struct atbmwifi_vif *priv,struct atbm_ba_tid_params *tid_params)
{
	atbm_uint16 seq = BUFF_INDEX_IS_SAFE(tid_params->start_seq);

	while(tid_params->skb_buffed)
	{
		if(tid_params->skb_reorder_buff[seq] == ATBM_NULL)
		{
			//reorder_debug(REORDER_DEBUG,"%s,index(%d)\n",__func__,seq);
			break;
		}
		atbm_skb_buff_dequeue(priv,tid_params,seq,1);
		tid_params->start_seq = (tid_params->start_seq+1)&SEQ_NUM_MASKER;
		seq = BUFF_INDEX_IS_SAFE(tid_params->start_seq);
	}

	return 0;
}

static atbm_void atbm_reorder_pkg_timeout(atbm_void *data1,atbm_void *data2)
{
	struct atbm_ba_tid_params *tid_params= (struct atbm_ba_tid_params *)data1;
	struct atbmwifi_vif *priv = (struct atbmwifi_vif *)tid_params->reorder_priv;
	//struct atbm_buff_head frames;
	atbm_uint8 i = 0;
	atbm_uint16 index = 0;
	if(!atbm_test_bit(BAR_TID_EN,&tid_params->tid_en))
	{
		return;
	}
	tid_params_spin_lock(&tid_params->skb_reorder_spinlock,atbm_os_mutexLock);
	if(!tid_params->skb_buffed)
		goto exit;
	index = BUFF_INDEX_IS_SAFE(tid_params->start_seq);
	reorder_debug(REORDER_DEBUG,"atbm_reorder_pkg_timeout end_time(%d),end_tick(%d),start_index(%d),start_seq(%x)\n",atbm_GetOsTimeMs(),atbm_GetOsTime(),index,tid_params->start_seq);
	while(i<tid_params->wind_size) 
	{
		if(tid_params->frame_rx_time[index] ==0){
			reorder_debug(REORDER_DEBUG,"atbm_reorder_pkg_timeout frame_rx_time(%d),start_index(%d)\n",tid_params->frame_rx_time[index],index);
			break;
		}
		if(!atbm_TimeAfter(tid_params->frame_rx_time[index], (tid_params->timeout/2))){
			reorder_debug(REORDER_ERROR,"ReOrder:timeout %x>%x sn %x\n",tid_params->frame_rx_time[index],atbm_GetOsTimeMs(),tid_params->start_seq);
			atbm_skb_buff_dequeue(priv,tid_params,index,1);
		}
		else {
			break;
		}
		tid_params->start_seq = (tid_params->start_seq+1)&SEQ_NUM_MASKER;
		index = BUFF_INDEX_IS_SAFE(tid_params->start_seq);
		i++;			
	}
	atbm_reorder_skb_uplayer(priv,tid_params);
	index = BUFF_INDEX_IS_SAFE(tid_params->start_seq);
 	if(tid_params->skb_buffed){		
		wifi_printk(WIFI_RX,"atbm: atbm_reorder_pkg_timeout(), start_time(%d),start_index(%d),start_seq(%x)\n",atbm_GetOsTimeMs(),index,tid_params->start_seq);		
		wifi_printk(WIFI_RX,"atbm: atbm_reorder_pkg_timeout(), warning-->timeout(%d)\n",tid_params->timeout);
		atbmwifi_eloop_register_timeout(0,tid_params->timeout,atbm_reorder_pkg_timeout,(atbm_void *)tid_params,ATBM_NULL);
 	}
	else {
	 	atbm_clear_bit(REORDER_TIMER_RUNING,&tid_params->timer_running);
	}
exit:
	tid_params_spin_unlock(&tid_params->skb_reorder_spinlock,atbm_os_mutexUnLock);
}

int atbm_reorder_skb_queue(struct atbmwifi_vif *priv,struct atbm_buff *skb,atbm_uint8 link_id)
{
	atbm_uint16 index = 0;
	int res = 0;
	struct atbm_reorder_queue_comm * atbm_reorder = ATBM_NULL;
	struct atbm_ba_tid_params *tid_params;
	struct atbmwifi_ieee80211_hdr *frame = (struct atbmwifi_ieee80211_hdr *)ATBM_OS_SKB_DATA(skb);
	atbm_uint8 frag =  frame->seq_ctrl & ATBM_IEEE80211_SCTL_FRAG;
	atbm_uint8 more = atbmwifi_ieee80211_has_morefrags(frame->frame_control);
	atbm_uint8 *qc = atbmwifi_ieee80211_get_qos_ctl(frame);
	int tid = 0;
	atbm_uint16 frame_seq = 0;
	atbm_uint16 start_seq = 0;
	atbm_uint16 now_frame_id = 0;
	
	if(link_id>ATBMWIFI__MAX_STA_IN_AP_MODE)
	{
		ATBM_WARN_ON_FUNC(1);
		return 0;
	}
	/*
	*fragment frame cant not been buffedn in 
	*ampdu queue;
	*/
	if(frag|more)
	{
		reorder_debug(REORDER_DEBUG,"is a fregment frames\n");
		return 0;
	}
	tid = *qc & /*IEEE80211_QOS_CTL_TID_MASK(*/7;
	/*
	*if it is a fragment frame,we add it to the buff.
	*/
	atbm_reorder = &priv->atbm_reorder_link_id[link_id];
	tid_params =atbm_get_tid_params(atbm_reorder,tid);
	if(tid_params == ATBM_NULL)
	{
		return 0;
	}
	if(!atbm_test_bit(BAR_TID_EN,&tid_params->tid_en))
	{
		return 0;
	}
	if(atbmwifi_ieee80211_is_qos_nullfunc(frame->frame_control))
	{
		return 0;
	}
#if ATBM_DRIVER_PROCESS_BA
	/* reset session timer */
	if (tid_params->session_timeout){
		atbmwifi_eloop_cancel_timeout(sta_rx_agg_session_timer_expired, &tid_params->sta_priv->timer_to_tid[tid], ATBM_NULL);
		atbmwifi_eloop_register_timeout(0, tid_params->session_timeout, sta_rx_agg_session_timer_expired,
			  &tid_params->sta_priv->timer_to_tid[tid], ATBM_NULL);
	}
#endif

	start_seq = tid_params->start_seq;
	frame_seq = (frame->seq_ctrl>>4)&SEQ_NUM_MASKER;
	now_frame_id = BUFF_INDEX_IS_SAFE(frame_seq);
	
	tid_params_spin_lock(&tid_params->skb_reorder_spinlock,atbm_os_mutexLock);
	index = ((frame_seq)-(tid_params->start_seq))&SEQ_NUM_MASKER;
	if (
		//only hope that frame come,so directly queue
		((index == 0)&&(tid_params->skb_buffed == 0))	
	    )
	{
		tid_params->start_seq =(frame_seq+1)&SEQ_NUM_MASKER;
		res = 0;
		goto exit_reorder;
	}
	/*
	*the frame maybe has been lost,so
	*send to the mac80211.
	*/
	if(index>=THE_RETRY_PKG_INEDX)
	{
		res = -1;
		atbm_dev_kfree_skb(skb);
		reorder_debug(REORDER_DEBUG,"<WARNNING>rx a prev frame,seq(%x),start(%x)\n",(frame->seq_ctrl&(ATBM_IEEE80211_SCTL_SEQ))>>4,start_seq);
		goto exit_reorder;
	}
	/*
	*skb buff has been overflowed,maybe same frames has bee buffed a long time,
	*so send some prev frames;
	*/
	if(index >= tid_params->wind_size)
	{
		int need_free = index-tid_params->wind_size+1;
		reorder_debug(REORDER_ERROR,"overflow:need_free(%d),sn(%x),ssn(%x)\n",need_free,frame_seq,tid_params->start_seq);
		if(need_free>=tid_params->wind_size)
		{
			reorder_debug(REORDER_ERROR,"<WARNNING> seq unexception,just to uplayer %d\n",need_free);
			need_free = tid_params->wind_size;
			atbm_reorder_skb_forcefree(priv,tid_params,need_free);
			tid_params->start_seq =(frame_seq+1)&SEQ_NUM_MASKER;
			res = 0;
			goto exit_reorder;;
		}
		atbm_reorder_skb_forcefree(priv,tid_params,need_free);	
	}
	else {
		/*
		*retry frames or fragment frames 
		*/
		if(tid_params->skb_reorder_buff[now_frame_id] != ATBM_NULL)
		{
			/*
			*if it is a retry frame,we discare it;
			*/
			res = -1;
			atbm_dev_kfree_skb(skb);
			reorder_debug(REORDER_DEBUG,"%s:ieee80211_has_retry\n",__func__);
			goto exit_reorder;
		}
		else
		{
			//reorder_debug(REORDER_DEBUG,"seq(%x),start(%x),index(%d)\n",frame_seq,start_seq,index);
		}
	}
	atbm_skb_buff_queue(tid_params,now_frame_id,skb);
	
//maybe_send_order:
	/*
	*dequeue the sequential frames after the the "index" frame;
	*/
	atbm_reorder_skb_uplayer(priv,tid_params);
	
//		reorder_debug(REORDER_DEBUG,"header(%d),ampdu reordered,buffed(%d)---2\n",tid_params->index_hread,tid_params->skb_buffed);
	if((!atbm_test_bit(REORDER_TIMER_RUNING,&tid_params->timer_running)||(start_seq!= tid_params->start_seq))&&
		(tid_params->skb_buffed)&&
		(tid_params->timeout))
	{
		unsigned int time;
		index= BUFF_INDEX_IS_SAFE(tid_params->start_seq);

		atbm_set_bit(REORDER_TIMER_RUNING,&tid_params->timer_running);	
		reorder_debug(REORDER_DEBUG,"mod_timer,skb_buffed(%d),start_seq(%x),frame_seq(%x),timeout(%d),start_time(%d),start_tick(%d)\n",tid_params->skb_buffed,tid_params->start_seq,frame_seq,tid_params->timeout,atbm_GetOsTimeMs(),atbm_GetOsTime());

		if(tid_params->frame_rx_time[index] == 0){
			int i =0;
			
			wifi_printk(WIFI_ALWAYS,"now_frame_id =%d=%d %d\n ",index,now_frame_id,start_seq );
			while(i < 64){
				wifi_printk(WIFI_ALWAYS,"tid_params->frame_rx_time[%d]=%d\n ",index,tid_params->frame_rx_time[index] );
				index++;
				index = BUFF_INDEX_IS_SAFE(index);
				i++;
			}
		}
		ATBM_WARN_ON_FUNC(tid_params->frame_rx_time[index] == 0);
		wifi_printk(WIFI_RX,"atbm_reorder(), warning-->timeout(%d)\n",tid_params->timeout);

		time = atbm_GetOsTimeMs() - tid_params->frame_rx_time[index];
		atbmwifi_eloop_cancel_timeout(atbm_reorder_pkg_timeout, (atbm_void *)tid_params, ATBM_NULL);
		atbmwifi_eloop_register_timeout(0,(tid_params->timeout > time) ? (tid_params->timeout - time) : 0,atbm_reorder_pkg_timeout,(atbm_void *)tid_params,ATBM_NULL);
	}
	else if(tid_params->skb_buffed==0)
	{
		atbm_clear_bit(REORDER_TIMER_RUNING,&tid_params->timer_running);
		atbmwifi_eloop_cancel_timeout(atbm_reorder_pkg_timeout, (atbm_void *)tid_params, ATBM_NULL);
	}
	res = -1;
exit_reorder:
	tid_params_spin_unlock(&tid_params->skb_reorder_spinlock,atbm_os_mutexUnLock);
	return res;
}
atbm_void atbm_reorder_func_init(struct atbmwifi_vif *priv)
{
	
	atbm_uint8 i, k;
	struct atbm_reorder_queue_comm * atbm_reorder;
	struct atbm_ba_tid_params *tid_params;
	reorder_debug(REORDER_DEBUG,"%s\n",__func__);
	for(k=0;k<ATBMWIFI__MAX_STA_IN_AP_MODE;k++)
	{
		atbm_reorder = &priv->atbm_reorder_link_id[k];
		for(i=0;i<ATBM_RX_DATA_QUEUES;i++)
		{
			tid_params = &atbm_reorder->atbm_rx_tid[i];
			atbm_clear_bit(BAR_TID_EN,&tid_params->tid_en);
			atbm_os_mutexLockInit(&tid_params->skb_reorder_spinlock);
			//tid_params->overtime_timer.data = (unsigned long)tid_params;
			//tid_params->overtime_timer.function = atbm_reorder_pkg_timeout;
			//init_timer(&tid_params->overtime_timer);
			//atbm_InitTimer(&tid_params->overtime_timer,atbm_reorder_pkg_timeout,(atbm_void*)tid_params);
			//atbm_reorder->atbm_rx_tid[i].skb_reorder_buff = NULL;
			//atbm_reorder->atbm_rx_tid[i].frame_rx_time = NULL;
		}
		//atbm_os_mutexLockInit(&atbm_reorder->reorder_mutex);
		atbm_reorder->link_id=k;
	}
}

atbm_void atbm_reorder_func_deinit(struct atbmwifi_vif *priv)
{
	atbm_uint8 i, k;
	struct atbm_reorder_queue_comm * atbm_reorder;
	struct atbm_ba_tid_params *tid_params;
	reorder_debug(REORDER_DEBUG,"%s\n",__func__);
	for(k=0;k<ATBMWIFI__MAX_STA_IN_AP_MODE;k++)
	{
		atbm_reorder = &priv->atbm_reorder_link_id[k];
		for(i=0;i<ATBM_RX_DATA_QUEUES;i++)
		{
			tid_params = &atbm_reorder->atbm_rx_tid[i];
			atbm_os_DeleteMutex(&tid_params->skb_reorder_spinlock);
		}
	}
}

atbm_void atbm_reorder_tid_buffed_clear(struct atbmwifi_vif *priv,struct atbm_ba_tid_params *tid_params)
{
	//atbm_uint8 header= 0;
	if((priv == ATBM_NULL)||(tid_params==ATBM_NULL))
	{
		return;
	}
	if(!tid_params->tid_en)
	{
		reorder_debug(REORDER_DEBUG,"!tid_params->tid_en\n");
		return;
	}
	
	if(!tid_params->skb_buffed)
		return;

	atbm_reorder_skb_forcefree(priv,tid_params,tid_params->wind_size);
	atbmwifi_eloop_cancel_timeout(atbm_reorder_pkg_timeout, (atbm_void *)tid_params, ATBM_NULL);
	atbm_clear_bit(REORDER_TIMER_RUNING,&tid_params->timer_running);
}

atbm_void atbm_reorder_tid_reset(struct atbmwifi_vif *priv,struct atbm_ba_tid_params *tid_params)
{
	//struct atbm_buff *clear_skb;
	if((priv == ATBM_NULL)||(tid_params==ATBM_NULL))
	{
		return;
	}
	if(!atbm_test_bit(BAR_TID_EN,&tid_params->tid_en))
	{
		reorder_debug(REORDER_DEBUG,"!tid_params->tid_en\n");
		return;
	}
	
	tid_params_spin_lock(&tid_params->skb_reorder_spinlock,atbm_os_mutexLock);
	if(!tid_params->skb_buffed)
		goto exit_tid_reset;
	atbm_reorder_skb_forcedrop(priv,tid_params,tid_params->wind_size);

	tid_params->start_seq = 0;
	atbmwifi_eloop_cancel_timeout(atbm_reorder_pkg_timeout, (atbm_void *)tid_params, ATBM_NULL);
	atbm_clear_bit(REORDER_TIMER_RUNING,&tid_params->timer_running);
exit_tid_reset:
	tid_params_spin_unlock(&tid_params->skb_reorder_spinlock,atbm_os_mutexUnLock);

}
atbm_void atbm_reorder_func_reset(struct atbmwifi_vif *priv,atbm_uint8 link_id)
{
	atbm_uint8 link_start, link_end,tid;
	struct atbm_reorder_queue_comm * atbm_reorder;
	if(link_id == 0xff)
	{
		link_start = 0;
		link_end = ATBMWIFI__MAX_STA_IN_AP_MODE;
	}
	else
	{
		if(link_id>=ATBMWIFI__MAX_STA_IN_AP_MODE)
		{
			return;
		}
		link_start = link_id;
		link_end = link_start+1;
	}
	for(;link_start<link_end;link_start++)
	{	
		atbm_reorder =  &priv->atbm_reorder_link_id[link_start];
//		atbm_os_mutexLock(&atbm_reorder->reorder_mutex);
		for(tid=0;tid<ATBM_RX_DATA_QUEUES;tid++)
		{
			if(!atbm_test_bit(BAR_TID_EN,&atbm_reorder->atbm_rx_tid[tid].tid_en))
			{
				continue;
			}
			atbm_reorder_tid_reset(priv,&atbm_reorder->atbm_rx_tid[tid]);
			tid_params_spin_lock(&atbm_reorder->atbm_rx_tid[tid].skb_reorder_spinlock,atbm_os_mutexLock);
			atbm_clear_bit(BAR_TID_EN,&atbm_reorder->atbm_rx_tid[tid].tid_en);
			atbm_reorder->atbm_rx_tid[tid].ssn = 0;
			atbm_reorder->atbm_rx_tid[tid].wind_size = 0;
			atbm_reorder->atbm_rx_tid[tid].start_seq = 0;
			tid_params_spin_unlock(&atbm_reorder->atbm_rx_tid[tid].skb_reorder_spinlock,atbm_os_mutexUnLock);
		}
//		atbm_os_mutexUnLock(&atbm_reorder->reorder_mutex);
	}
}
atbm_void atbm_updata_ba_tid_params(struct atbmwifi_vif *priv,struct atbm_ba_params *ba_params)
{
	atbm_uint8 tid;
	atbm_uint8 link_id;
	struct atbm_reorder_queue_comm * atbm_reorder =ATBM_NULL;
	struct atbm_ba_tid_params *tid_params = ATBM_NULL;
	atbm_uint8 action = ba_params->action;
	int i=0;

	if(priv == ATBM_NULL)
	{
		reorder_debug(REORDER_DEBUG,"err:%s:priv == ATBM_NULL\n",__func__);
		return;
	}
	if(priv->join_status < ATBMWIFI__JOIN_STATUS_MONITOR)
	{
		reorder_debug(REORDER_DEBUG,"interface type(%d) err\n",priv->join_status);
		return;
	}
	if((priv->join_status == ATBMWIFI__JOIN_STATUS_AP)&&
		(ba_params->link_id >= ATBMWIFI__MAX_STA_IN_AP_MODE))
	{
		reorder_debug(REORDER_DEBUG,"%s link_id(%d) err\n",__FUNCTION__,ba_params->link_id );
		return;
	}
	if (priv->join_status == ATBMWIFI__JOIN_STATUS_STA)
	{
		ba_params->link_id = 1;
	}
	tid = ba_params->tid;
	link_id = ba_params->link_id;
	atbm_reorder = &priv->atbm_reorder_link_id[link_id-1];
	tid_params = &atbm_reorder->atbm_rx_tid[tid];
	switch(action)
	{
		case ATBM_BA__ACTION_RX_ADDBR:
		{
			reorder_debug(REORDER_DEBUG,"WSM_BA_FLAGS__RX_ADDBA_RE\n");
//			atbm_os_mutexLock(&atbm_reorder->reorder_mutex);
			if(atbm_test_bit(BAR_TID_EN,&tid_params->tid_en))
			{
				wifi_printk(WIFI_ALWAYS,"tid_params->tid_en\n");	
				goto exit_action_rx_addba;
			}
			tid_params_spin_lock(&tid_params->skb_reorder_spinlock,atbm_os_mutexLock);
			tid_params->ssn = ba_params->ssn;
			tid_params->wind_size = ba_params->win_size;
			tid_params->timeout = ba_params->timeout;
			if(tid_params->timeout == 0)
				tid_params->timeout = AMPDU_REORDER_TIME_INTERVAL;
			else if(tid_params->timeout >= AMPDU_REORDER_TIME_INTERVAL*2)
				tid_params->timeout = AMPDU_REORDER_TIME_INTERVAL*2;
			else if(tid_params->timeout < AMPDU_REORDER_TIME_INTERVAL/2)
				tid_params->timeout = AMPDU_REORDER_TIME_INTERVAL/2;
			//tid_params->timeout = 100;
			tid_params->start_seq = (ba_params->ssn)&SEQ_NUM_MASKER;
			tid_params->index_tail = tid_params->wind_size-1;
			for(i=0;i<BUFF_STORED_LEN;i++){
				tid_params->skb_reorder_buff[i]=ATBM_NULL;
				tid_params->frame_rx_time[i]=0;
			}
			tid_params->session_timeout = ba_params->timeout;
			//__skb_queue_head_init(&tid_params->header);
			tid_params->reorder_priv = priv;
			tid_params->skb_buffed = 0;
			tid_params->sta_priv = ba_params->sta_priv;
			atbm_set_bit(BAR_TID_EN,&tid_params->tid_en);
			tid_params_spin_unlock(&tid_params->skb_reorder_spinlock,atbm_os_mutexUnLock);
exit_action_rx_addba:
//			atbm_os_mutexUnLock(&atbm_reorder->reorder_mutex);
			break;
		}
		case ATBM_BA__ACTION_RX_DELBA:
		{
			
			if(!atbm_test_bit(BAR_TID_EN,&tid_params->tid_en))
				goto exit_action_rx_delba;
			reorder_debug(REORDER_DEBUG,"WSM_BA_FLAGS__RX_DELBA_RE,link_id(%d)\n",link_id);
			atbm_reorder_tid_reset(priv,tid_params);
//			atbm_os_mutexLock(&atbm_reorder->reorder_mutex);
			tid_params_spin_lock(&tid_params->skb_reorder_spinlock,atbm_os_mutexLock);
			atbm_clear_bit(BAR_TID_EN,&atbm_reorder->atbm_rx_tid[tid].tid_en);
			tid_params->ssn = 0;
			tid_params->wind_size = 0;
			tid_params->start_seq = 0;
			tid_params_spin_unlock(&tid_params->skb_reorder_spinlock,atbm_os_mutexUnLock);
exit_action_rx_delba:
//			atbm_os_mutexUnLock(&atbm_reorder->reorder_mutex);
			break;			
		}
		case ATBM_BA__ACTION_RX_BAR:
		{
			atbm_uint16 pre_start_seq = 0;
			 
//			atbm_os_mutexLock(&atbm_reorder->reorder_mutex);
			if(!atbm_test_bit(BAR_TID_EN,&tid_params->tid_en))
				goto exit_actin_bar;
			tid_params_spin_lock(&tid_params->skb_reorder_spinlock,atbm_os_mutexLock);
			pre_start_seq = tid_params->start_seq;
			//reorder_debug(REORDER_DEBUG,"RX_BAR,BARsn(%x),Ssn(%x)\n",ba_params->ssn,pre_start_seq);
			if(((ba_params->ssn-pre_start_seq)&SEQ_NUM_MASKER)>THE_RETRY_PKG_INEDX)
				goto exit_actin_bar_spin_unlock;
			if((ba_params->ssn-pre_start_seq)==0){
				goto exit_actin_bar_spin_unlock;
			}
			reorder_debug(REORDER_DEBUG,"RX_BAR,BARsn(%x),Ssn(%x)\n",ba_params->ssn,pre_start_seq);
			/*
			*if pre_start_seq+1 <ba_params->ssn maybe we have missed
			*same package so clear the buff queue
			*/
			if((((ba_params->ssn-pre_start_seq)&SEQ_NUM_MASKER) <= tid_params->wind_size)
				&&tid_params->skb_buffed)
			{
				atbm_uint8 need_free = ((ba_params->ssn - pre_start_seq)&SEQ_NUM_MASKER);
				
				reorder_debug(REORDER_DEBUG,"RX_BAR,BARsn(%x),Ssn(%x)\n",ba_params->ssn,pre_start_seq);
				reorder_debug(REORDER_DEBUG,"_RX_BAR case 3,ssn(%x),buffed(%d)need_free %d\n",tid_params->start_seq,tid_params->skb_buffed,need_free);
				
				/*
				*1. a)if need_free >tid_params->wind_size, we dequeue the skb_queue ,then tid_params->skb_buffed
				*	  must be equal to zero;
				*    b)if need_free <tid_params->wind_size,we dequeue the skb_queue,then pre_start_seq must be 
				*	 equal to ba_params->ssn;
				*/
				atbm_reorder_skb_forcefree(priv,tid_params,need_free);
				/*
				*2. if need_free<tid_params->wind_size,then pre_start_seq must equal to ba_params->ssn
				*/
				if(tid_params->skb_buffed&&(tid_params->start_seq != ba_params->ssn))
				{
					reorder_debug(REORDER_DEBUG,"WSM_BA_FLAGS__RX_BAR err\n");
				}
				/*
				*3.from the index_head ,the skb_buff has some buff that is not equal to NULL,we should dequeue it.
				*but must MAKE SURE THAT :pre_start_seq == ba_params->ssn
				*/				
				atbm_reorder_skb_uplayer(priv,tid_params);
				
			}
			else {
				if(tid_params->skb_buffed){
					reorder_debug(REORDER_DEBUG,"_RX_BAR free ALL case 4,(%x)=ssn(%x)-start_seq(%x),buffed(%d)\n",(ba_params->ssn-pre_start_seq),
					ba_params->ssn,tid_params->start_seq,tid_params->skb_buffed);
					atbm_reorder_skb_forcefree(priv,tid_params,tid_params->wind_size);
				}
				tid_params->start_seq = ba_params->ssn;
			}

			if((!atbm_test_bit(REORDER_TIMER_RUNING,&tid_params->timer_running)||(pre_start_seq!= tid_params->start_seq))&&
				(tid_params->skb_buffed)&&
				(tid_params->timeout))
			{		
				atbm_uint16 index= BUFF_INDEX_IS_SAFE(tid_params->start_seq);
				atbm_set_bit(REORDER_TIMER_RUNING,&tid_params->timer_running);
				ATBM_WARN_ON_FUNC(tid_params->frame_rx_time[index] == 0);
				wifi_printk(WIFI_RX,"atbm: atbm_updata_ba_tid_params(), warning-->timeout(%d)\n",tid_params->timeout);
				atbmwifi_eloop_register_timeout(0,tid_params->timeout,atbm_reorder_pkg_timeout,(atbm_void *)tid_params,ATBM_NULL);
			}
			else if(tid_params->skb_buffed==0)
			{
				atbm_clear_bit(REORDER_TIMER_RUNING,&tid_params->timer_running);
				atbmwifi_eloop_cancel_timeout(atbm_reorder_pkg_timeout, (atbm_void *)tid_params, ATBM_NULL);
			}
			tid_params->ssn = ba_params->ssn;
exit_actin_bar_spin_unlock:
			tid_params_spin_unlock(&tid_params->skb_reorder_spinlock,atbm_os_mutexUnLock);
exit_actin_bar:
			break;
		}
		default:
		{
			wifi_printk(WIFI_ALWAYS,"%s:action err\n",__func__);
			break;
		}
	}
}
#endif
#if NEW_SUPPORT_PS
static atbm_void atbmwifi_ieee80211_rx_h_sta_process(struct atbmwifi_vif *priv,struct atbmwifi_ieee80211_hdr *hdr) 
{
	int tid;
	struct atbmwifi_sta_priv * sta_priv = atbmwifi_sta_find(priv,hdr->addr2);
	unsigned long flags;

	if(sta_priv) {		
		/*
		 * Change STA power saving mode only at the end of a frame
		 * exchange sequence.
		 */
		if(ATBM_BIT(sta_priv->link_id) & priv->sta_asleep_mask) {
			/*
			 * Ignore doze->wake transitions that are
			 * indicated by non-data frames, the standard
			 * is unclear here, but for example going to
			 * PS mode and then scanning would cause a
			 * doze->wake transition for the probe request,
			 * and that is clearly undesirable.
			 */
			if (atbmwifi_ieee80211_is_data(hdr->frame_control) &&
			           !atbmwifi_ieee80211_has_pm(hdr->frame_control)){
			    atbm_spin_lock_irqsave(&priv->ps_state_lock, &flags);
			    atbmwifi_set_tim(priv, sta_priv->link_id, 0);
			    //atbm_clear_bit(sta_priv->link_id,(atbm_uint32 *)priv->tim_vbitmap);
				atbm_ps_notify(priv,sta_priv->link_id,0);
				atbm_spin_unlock_irqrestore(&priv->ps_state_lock, flags);
			}
		} else {
			if (atbmwifi_ieee80211_has_pm(hdr->frame_control)){
				atbm_spin_lock_irqsave(&priv->ps_state_lock, &flags);
				atbm_ps_notify(priv,sta_priv->link_id,1);
				if(sta_priv->link_id > 0 && sta_priv->link_id <= ATBMWIFI__MAX_STA_IN_AP_MODE){
					for(tid = 0; tid < ATBMWIFI__MAX_TID; tid++){
						if(priv->link_id_db[sta_priv->link_id - 1].buffered[tid] > 0){
							atbmwifi_sta_set_buffered(sta_priv, tid, ATBM_TRUE);
							priv->buffered_set_mask |= ATBM_BIT(sta_priv->link_id);
							break;
						}
					}
				}
				atbm_spin_unlock_irqrestore(&priv->ps_state_lock, flags);
				atbm_bh_wakeup(priv->hw_priv);
			}
		}
	}

}
static int atbmwifi_rx_uapsd_and_pspoll(struct atbmwifi_vif *priv,struct atbmwifi_ieee80211_hdr * hdr)
{
	int ret;
	atbm_uint16 fc;
	int link_id;
	int tid;
	//struct atbmwifi_ieee80211_hdr *hdr=ATBM_NULL;
	//hdr = (struct atbmwifi_ieee80211_hdr *)ATBM_OS_SKB_DATA(skb);
	fc = hdr->frame_control;
	link_id = atbmwifi_find_hard_link_id(priv,&hdr->addr2[0]);
	tid = *atbmwifi_ieee80211_get_qos_ctl(hdr) & ATBM_IEEE80211_QOS_CTL_TID_MASK;
	wifi_printk(WIFI_PS,"atbmwifi_rx_uapsd_and_pspoll-->link_id=%d fc %x tid =%d\n",link_id,fc,tid);
	/*If no station is sleep,todo continue*/
	if(((priv->sta_asleep_mask &ATBM_BIT(link_id))==0)){
		return RX_CONTINUE;
	}else{
		wifi_printk(WIFI_PS,"atbmwifi_rx_uapsd_and_pspoll-->link_id=%d,sta_asleep_mask=%x\n",link_id,priv->sta_asleep_mask);
		/* The device handles station powersave, so don't do anything about
		 * uAPSD and PS-Poll frames */	
		if (atbmwifi_ieee80211_is_pspoll(fc)){
			wifi_printk(WIFI_PS,"do ps poll prcoess\n");
			/* if we are in ps , do nothing */
			if(priv->link_id_db[link_id-1].sta_priv.flags & WLAN_STA_PS/*atbm_test_bit(WLAN_STA_PS,&priv->link_id_db[link_id-1].sta_priv.flags)*/){
				return RX_CONTINUE;
			}
			atbmwifi_deliver_poll_response(priv,hdr,link_id);
			/*It's tiger Frame,FrameCtrl|=PM bit 0|qosData|NullData*/
		}else if(atbmwifi_ieee80211_has_pm(fc) &&
						(atbmwifi_ieee80211_is_qos_nullfunc(fc)||
						atbmwifi_ieee80211_is_data_qos(fc))/*||
						atbmwifi_ieee80211_is_nullfunc(fc)*/){ /*do u-apsd process*/
			/* if we are in a service period, do nothing */
			if(priv->link_id_db[link_id-1].sta_priv.flags & WLAN_STA_SP/*atbm_test_bit(WLAN_STA_SP,&priv->link_id_db[link_id-1].sta_priv.flags)*/){
				return RX_CONTINUE;
			}
			wifi_printk(WIFI_PS,"do uapsd prcoess\n");
			ret=atbmwifi_deliver_uapsd_response(priv,hdr,link_id,tid);
			if(ret<0){
				wifi_printk(WIFI_PS,"Error!!!No queue support uapsd\n");
			}
		}
	}	
	return RX_CONTINUE;
}
#endif

/**************************************************************************************************************
 * 函数名: __atbmwifi_ieee80211_rx
 *
 * 功能描述:
 *   802.11 帧类型分发处理函数，根据帧类型（数据/管理/控制）分发到不同的处理路径。
 *   这是 802.11 层的核心处理函数，负责帧类型判断和分发。
 *
 * 在 RX 流程中的位置:
 *   阶段 5: 802.11 帧类型分发
 *   在 atbmwifi_ieee80211_rx_irqsafe() 中调用
 *
 * 参数说明:
 *   @priv: 虚拟接口私有数据指针
 *   @skb:  接收到的数据包（802.11 帧格式）
 *
 * 返回值:
 *   0: 不需要释放 skb（已被处理或加入队列）
 *   1: 需要释放 skb
 *
 * 处理流程:
 *   1. 解析帧控制字段：获取帧类型
 *   2. 电源管理处理：更新 STA 的电源状态（AP 模式）
 *   3. 帧类型分发：
 *      a. 控制帧（非 BAR）：直接释放
 *      b. Null 帧/QoS Null 帧：直接释放
 *      c. 数据帧：
 *         - 解析 QoS 字段
 *         - 更新 RSSI
 *         - 如果启用 ATBM_RX_TASK_QUEUE，加入 rx_task_skb_list 队列
 *         - 否则，直接处理 AMSDU、格式转换、提交协议栈
 *      d. 管理帧：加入 WPA 事件队列处理
 *
 * 数据帧处理路径:
 *   同步模式（ATBM_RX_TASK_QUEUE == 0）:
 *     → atbmwifi_ieee80211_rx_h_amsdu()
 *     → atbmwifi_ieee80211_data_to_8023()
 *     → atbmwifi_ieee80211_deliver_skb()
 *   异步模式（ATBM_RX_TASK_QUEUE == 1）:
 *     → atbm_skb_queue_tail(&priv->rx_task_skb_list, skb)
 *     → atbm_queue_work() 唤醒 RX 任务
 *
 * 注意事项:
 *   - 返回值指示调用者是否需要释放 skb
 *   - 管理帧通过 WPA 事件队列异步处理
 *   - 数据帧可以同步或异步处理，取决于配置
 *
 * 相关函数:
 *   - atbmwifi_ieee80211_rx_irqsafe(): 调用此函数
 *   - atbmwifi_ieee80211_rx_h_amsdu(): AMSDU 解聚合
 *   - atbmwifi_ieee80211_data_to_8023(): 802.11 到 802.3 转换
 *   - atbmwifi_ieee80211_deliver_skb(): 提交到协议栈
 *   - atbmwifi_wpa_event_queue(): 管理帧事件队列
 *
 **************************************************************************************************************/
 int __atbmwifi_ieee80211_rx(struct atbmwifi_vif *priv,struct atbm_buff *skb)
{
	struct atbmwifi_ieee80211_hdr *hdr=ATBM_NULL;
	atbm_uint16 fc;
	atbm_uint8 ret=0;
	atbm_uint16 needfree = 0;
	struct atbmwifi_ieee80211_rx_status *hwhdr = (struct atbmwifi_ieee80211_rx_status *)ATBM_IEEE80211_SKB_RXCB(skb);
	hdr = (struct atbmwifi_ieee80211_hdr *)ATBM_OS_SKB_DATA(skb);
	fc = hdr->frame_control;
#if NEW_SUPPORT_PS
	atbmwifi_rx_uapsd_and_pspoll(priv,hdr);
	atbmwifi_ieee80211_rx_h_sta_process(priv,hdr); 
#endif
	if(atbmwifi_ieee80211_is_ctl(fc) && !atbmwifi_ieee80211_is_back_req(fc)){
		needfree = 1;
	}
	else if(atbmwifi_ieee80211_is_nullfunc(fc)){
		needfree = 1;
	}else if(atbmwifi_ieee80211_is_qos_nullfunc(fc)){
		needfree = 1;
	}
	else if (atbmwifi_ieee80211_is_data_present(fc)) {
		struct atbmwifi_sta_priv * sta_priv =ATBM_NULL;
		needfree = 1;
		//amsdu
		atbmwifi_ieee80211_parse_qos(priv,skb);

		//update rssi
		priv->bss.rssi = hwhdr->signal;
		sta_priv = atbmwifi_sta_find(priv, hdr->addr2);
		if(sta_priv){
			rate_control_rx_status(sta_priv, hwhdr);
			sta_priv->rssi = hwhdr->signal;
		}

#if (ATBM_RX_TASK_QUEUE==1)
		atbm_skb_queue_tail(&priv->rx_task_skb_list,skb);
		/*Do rx task schedule,change task excute text*/
		///TODO atbm_net_RxTask
		atbm_queue_work(priv->hw_priv, priv->rx_task_work);
		needfree = 0;
#else //ATBM_RX_TASK_QUEUE
		ret = atbmwifi_ieee80211_rx_h_amsdu(priv,skb);
		if(ret){
			needfree = 0;
			goto __free_return;
		}
		ret = atbmwifi_ieee80211_data_to_8023(skb, priv->mac_addr,priv->iftype);
		if(ret){
			//needfree = 1;
			wifi_printk(WIFI_RX,"atbmwifi_ieee80211_data_to_8023 err\n");
			goto __free_return;
		}

		atbmwifi_ieee80211_deliver_skb(priv,skb,&needfree);
#endif
	}
	else {
		atbmwifi_wpa_event_queue((atbm_wpa_event_data)priv,(atbm_wpa_event_data)skb,(atbm_wpa_event_data)ATBM_NULL,WPA_EVENT__RX_PKG,ATBM_WPA_EVENT_NOACK);
		needfree = 0;
	}
	
__free_return:
	return (int)needfree;

}

/**************************************************************************************************************
 * 函数名: atbmwifi_ieee80211_rx
 *
 * 功能描述:
 *   802.11 管理帧和控制帧处理函数，处理非数据帧（管理帧、BAR 等）。
 *   这是管理帧的入口处理函数，负责分发到 AP 或 STA 模式的管理帧处理函数。
 *
 * 在 RX 流程中的位置:
 *   阶段 5: 802.11 管理帧处理
 *   在 WPA 事件处理中调用，处理管理帧
 *
 * 参数说明:
 *   @priv: 虚拟接口私有数据指针
 *   @skb:  接收到的数据包（802.11 帧格式）
 *
 * 返回值:
 *   1: 需要释放 skb
 *   其他: 不需要释放 skb
 *
 * 处理流程:
 *   1. 解析帧控制字段：获取帧类型
 *   2. BAR 帧处理：如果是 Block Ack Request，更新 BA 参数
 *   3. 管理帧处理：
 *      a. PMF 检查：如果启用 802.11w，检查加密管理帧
 *      b. Action 帧：调用 atbmwifi_rx_actionFrame() 处理
 *      c. 其他管理帧：
 *         - AP 模式：调用 atbmwifi_rx_ap_mgmtframe()
 *         - STA 模式：调用 atbmwifi_rx_sta_mgmtframe()
 *
 * 管理帧类型:
 *   - Beacon: 信标帧
 *   - Probe Request/Response: 探测请求/响应
 *   - Authentication: 认证帧
 *   - Association Request/Response: 关联请求/响应
 *   - Deauthentication: 取消认证
 *   - Disassociation: 取消关联
 *   - Action: 动作帧
 *
 * 注意事项:
 *   - 管理帧通常需要释放 skb
 *   - BAR 帧用于包重排序功能
 *   - PMF（Protected Management Frames）需要特殊处理
 *
 * 相关函数:
 *   - atbmwifi_rx_actionFrame(): Action 帧处理
 *   - atbmwifi_rx_ap_mgmtframe(): AP 模式管理帧处理
 *   - atbmwifi_rx_sta_mgmtframe(): STA 模式管理帧处理
 *   - atbm_updata_ba_tid_params(): 更新 BA 参数
 *
 **************************************************************************************************************/
 int atbmwifi_ieee80211_rx(struct atbmwifi_vif *priv,struct atbm_buff *skb)
{
	struct atbmwifi_ieee80211_hdr *hdr=ATBM_NULL;
	atbm_uint16 fc;

	int needfree = 1;

	hdr = (struct atbmwifi_ieee80211_hdr *)ATBM_OS_SKB_DATA(skb);
	fc = hdr->frame_control;

#if (ATBM_RX_REUSE_PACKET == 0)
	ATBM_BUG_ON(skb->ref != 1);
#endif

#if ATBM_PKG_REORDER
	if(atbmwifi_ieee80211_is_back_req(hdr->frame_control))
	{
		struct atbm_ba_params ba_params;
		struct atbmwifi_ieee80211_bar * bar_data = (struct atbmwifi_ieee80211_bar *)ATBM_OS_SKB_DATA(skb);
		struct atbmwifi_ieee80211_rx_status *hdr = ATBM_IEEE80211_SKB_RXCB(skb);

		ba_params.tid = atbm_le16_to_cpu(bar_data->control) >> 12;
		ba_params.ssn =  atbm_le16_to_cpu(bar_data->start_seq_num) >> 4;
		ba_params.action= ATBM_BA__ACTION_RX_BAR;
		ba_params.link_id = hdr->link_id;
		wifi_printk(WIFI_ALWAYS,"rx BAR:ssn(%x),tid(%d),link_id(%d)\n",ba_params.ssn,ba_params.tid,ba_params.link_id);
		atbm_updata_ba_tid_params(priv,&ba_params);
		goto exit;
	}
#endif //ATBM_PKG_REORDER

	if(atbmwifi_ieee80211_is_mgmt(fc)) {		
#if CONFIG_IEEE80211W
		if(atbmwifi_ieee80211_drop_unencrypted_mgmt(priv,skb)){
			return needfree;
		}
#endif
		if (atbmwifi_ieee80211_is_action(fc)){
			atbmwifi_rx_actionFrame(priv,skb);
		}else {
			if(atbmwifi_is_ap_mode(priv->iftype)){
				atbmwifi_rx_ap_mgmtframe(priv,skb);
			}
			else{
				atbmwifi_rx_sta_mgmtframe(priv,skb);
			}
		}
    }
exit:
	return needfree;
}

int atbmwifi_ieee80211_tx_status(struct atbmwifi_vif *priv,struct atbm_buff *skb, struct wsm_tx_confirm *arg)
{
	struct atbmwifi_ieee80211_tx_info *tx = (struct atbmwifi_ieee80211_tx_info*)ATBM_IEEE80211_SKB_TXCB(skb);
	struct atbmwifi_cfg *config = atbmwifi_get_config(priv);

	struct atbmwifi_ieee80211_hdr *hdr;
	atbm_uint16 fc;
	atbm_uint16 stype;

	hdr=(struct atbmwifi_ieee80211_hdr *)ATBM_OS_SKB_DATA(skb);
	fc = hdr->frame_control; 
	if(atbmwifi_ieee80211_is_mgmt(fc)){
		stype = fc & atbm_cpu_to_le16(ATBM_IEEE80211_FCTL_STYPE);
		if(((stype==ATBM_IEEE80211_STYPE_ASSOC_RESP)||
		  	(stype==ATBM_IEEE80211_STYPE_REASSOC_RESP)) &&
		  	atbmwifi_is_ap_mode(priv->iftype)){
			struct atbmwifi_ieee80211_mgmt *mgmt = (struct atbmwifi_ieee80211_mgmt *) ATBM_OS_SKB_DATA(skb);
				//TO DO 4way-handshake
#if CONFIG_WPS
			if(!mgmt->u.assoc_resp.status_code && ((priv->pbc) || (priv->pin)))
			{
				hostapd_eap_wsc_init(priv, mgmt->da);
			}else
#endif
			if(!mgmt->u.assoc_resp.status_code && config->wpa)
			{
				atbmwifi_event_uplayer(priv,ATBM_WIFI_ASSOCRSP_TXOK_EVENT,mgmt->da);
			}
		}
#if CONFIG_P2P
		else if(stype == ATBM_IEEE80211_STYPE_ACTION && ((priv->iftype == ATBM_NL80211_IFTYPE_P2P_GO) || (priv->iftype == ATBM_NL80211_IFTYPE_P2P_CLIENT))){
			struct atbmwifi_ieee80211_mgmt *mgmt = (struct atbmwifi_ieee80211_mgmt *)ATBM_OS_SKB_DATA(skb);
			atbm_p2p_tx_action_ack(priv, mgmt, arg->status);
		}
#endif
	}else if(tx->b_eapol){
		if(atbmwifi_is_sta_mode(priv->iftype)){
			wpa_supplicant_eapol_notice_ack(priv);
		}
#if CONFIG_WPS
		else{
			struct hostapd_data *hostapd = (struct hostapd_data *)(priv->appdata);
			if(hostapd && hostapd->wpsdata && hostapd->wpsdata->state == RECV_DONE){
				wpa_supplicant_eapol_notice_ack(priv);
			}
		}
#endif
	}
	return 0;
}

static void atbm_tx_rate_status(struct atbmwifi_vif *priv,struct wsm_tx_confirm *arg, int link_id)
 {
	int i = 0;
	atbm_uint16 *rate = (atbm_uint16*)(&arg->RateTable);    
	struct atbmwifi_sta_priv  *sta_priv = ATBM_NULL;

	//wifi_printk(WIFI_ALWAYS, "link_id:%d\n", link_id);
	if((priv->iftype == ATBM_NL80211_IFTYPE_AP)||(priv->iftype ==ATBM_NL80211_IFTYPE_P2P_GO)){
		if((link_id >0) && (link_id <= ATBMWIFI__MAX_STA_IN_AP_MODE )&&(priv->link_id_db[link_id-1].sta_priv.sta_rc_priv!=ATBM_NULL)){
			sta_priv = &priv->link_id_db[link_id-1].sta_priv;
		}
    }else {
		sta_priv = &priv->bss.sta_priv;
	}
    
     if(atbm_unlikely((sta_priv == ATBM_NULL)||(sta_priv->sta_rc_priv == ATBM_NULL))){
         return;
     }
     if(atbm_unlikely(sta_priv->rate.txs_retrys == NULL)){
         return;
     }
     if(atbm_unlikely(arg->NumOfTxRate != sta_priv->rate.n_rates)){
         return;
     }

     for(i = 0;i < (arg->NumOfTxRate * 2 + 4); i++){
         sta_priv->rate.txs_retrys[i] = rate[i];
     }
	//dump_mem(sta_priv->rate.txs_retrys, 144);
	rate_control_tx_hmac(sta_priv);	
  
 }

 

extern atbm_void sta_tx_test_end(int status);
 atbm_void atbmwifi_tx_confirm_cb(struct atbmwifi_common *hw_priv,
			  struct wsm_tx_confirm *arg,int if_id,	int link_id)
{
	atbm_uint8 queue_id = atbmwifi_queue_get_queue_id(arg->packetID);
	struct atbmwifi_queue *queue = &hw_priv->tx_queue[queue_id];
	struct atbm_buff *skb;
	const struct atbmwifi_txpriv *txpriv;
	struct atbmwifi_vif *priv;
	/*tx count is the times of number the frame was transmited*/
	priv = _atbmwifi_hwpriv_to_vifpriv(hw_priv, if_id);
	if (atbm_unlikely(!priv))
		return;

	if (arg->status)
		wifi_printk(WIFI_TX, "TX failed: %d.\n",
				arg->status);

	if ((arg->status == WSM_REQUEUE) &&
	    (arg->flags & WSM_TX_STATUS_REQUEUE)) {
		wifi_printk(WIFI_TX, "Requeue for link_id %d (try %d)."
			" STAs asleep: 0x%x\n",
			link_id,
			atbmwifi_queue_get_generation(arg->packetID) + 1,
			priv->sta_asleep_mask);

		atbmwifi_queue_requeue(queue,arg->packetID);
	} 
	else if (!atbmwifi_queue_get_skb(queue, arg->packetID, &skb, &txpriv))
	{

		struct wsm_tx *hdr = (struct wsm_tx *)ATBM_OS_SKB_DATA(skb);

		atbm_skb_pull(skb, sizeof(struct wsm_tx));

        /*
        *rate status
        */
        if(arg->flags & ATBM_BIT(8)){
            atbm_tx_rate_status(priv,arg, link_id);
        }else{
	        if(hdr->flags & WSM_TX_2BYTES_SHIFT){
				atbm_memmove(ATBM_OS_SKB_DATA(skb)+2, ATBM_OS_SKB_DATA(skb), sizeof(struct atbmwifi_ieee80211_hdr_3addr));
				atbm_skb_pull(skb, 2);
			}
#ifndef ATBM_RX_STATUS_USE_QUEUE
			atbmwifi_ieee80211_tx_status(priv, skb, arg);
#endif
        }

		atbmwifi_queue_remove(queue, arg->packetID);
	}
	else {
		ATBM_WARN_ON_FUNC(1);
	}
}

/**
 * @brief 计算发送数据包的链路 ID
 *
 * @param priv 虚拟接口指针，包含接口的配置和状态信息
 * @param t 发送信息结构体指针，包含发送相关的信息
 * @return int 0:成功
 *
 * @note 该函数根据数据包的目标地址和接口模式计算链路 ID：
 *       1. 如果目标站点已关联（sta_priv 存在且有 link_id），使用站点的 link_id
 *       2. 如果是 STA 模式，link_id 设置为 0（发送到 AP）
 *       3. 如果是组播地址，使用 link_id_after_dtim（用于 DTIM 后的组播发送）
 *       4. 其他情况（如未关联的单播），link_id 设置为 0
 *
 * 链路 ID 的作用:
 * - 在 AP 模式下，用于标识不同的 STA
 * - 在 STA 模式下，固定为 0（发送到 AP）
 * - 用于电源管理，判断数据包应该发送到哪个链路
 * - 用于队列管理，按链路组织数据包
 *
 * 特殊处理:
 * - raw_link_id: 原始链路 ID，用于内部处理
 * - link_id: 实际使用的链路 ID
 * - 组播数据包在 DTIM 后发送，使用特殊的 link_id_after_dtim
 *
 * @see atbmwifi_tx() - 调用该函数计算链路 ID
 * @see atbmwifi_tx_queue() - 调用该函数计算链路 ID
 * @see atbmwifi_sta_priv - 站点私有数据结构
 */
static int
atbmwifi_tx_h_calc_link_ids(struct atbmwifi_vif *priv,
			  struct atbmwifi_txinfo *t)
{
	/* 情况 1: 目标站点已关联，使用站点的 link_id */
	if (t->sta_priv && t->sta_priv->link_id){
		t->txpriv.raw_link_id =
				t->txpriv.link_id =
				t->sta_priv->link_id;
	}
	/* 情况 2: STA 模式，发送到 AP，link_id 为 0 */
	else if (atbmwifi_is_sta_mode(priv->iftype)){
		t->txpriv.raw_link_id =
		t->txpriv.link_id = 0;
	}
	/* 情况 3: 组播地址，使用 link_id_after_dtim 用于 DTIM 后发送 */
	else if (atbm_is_multicast_ether_addr(t->da)) {
		t->txpriv.raw_link_id = 0;
		t->txpriv.link_id = priv->link_id_after_dtim;
	}
	/* 情况 4: 其他情况（未关联的单播），link_id 为 0 */
	else {
		t->txpriv.link_id = 0;
		t->txpriv.raw_link_id = t->txpriv.link_id;
	}
	return 0;
}
/* Default mapping in classifier to work with default
 * queue setup.
 */
const int ieee802_1d_to_ac[8] = {
	ATBM_IEEE80211_AC_BE,
	ATBM_IEEE80211_AC_BK,
	ATBM_IEEE80211_AC_BK,
	ATBM_IEEE80211_AC_BE,
	ATBM_IEEE80211_AC_VI,
	ATBM_IEEE80211_AC_VI,
	ATBM_IEEE80211_AC_VO,
	ATBM_IEEE80211_AC_VO
};

static atbm_void  atbmwifi_notify_buffered_tx(struct atbmwifi_vif *priv, struct atbm_buff *skb,int link_id, atbm_uint32 tid)
{
	struct atbmwifi_sta_priv *sta;
	atbm_uint8 *buffered;
	atbm_uint8 still_buffered = 0;
	unsigned long flags;

	if (link_id && (link_id <= ATBMWIFI__MAX_STA_IN_AP_MODE) && (tid < ATBMWIFI__MAX_TID)) {
		atbm_spin_lock_irqsave(&priv->ps_state_lock, &flags);
		buffered = &priv->link_id_db[link_id - 1].buffered[0];
		sta = &priv->link_id_db[link_id - 1].sta_priv;
		if (!ATBM_WARN_ON(!buffered[tid])){
			still_buffered = --buffered[tid];
		}
		if (!still_buffered && (tid < ATBMWIFI__MAX_TID)) {
			if (sta && (priv->buffered_set_mask & ATBM_BIT(sta->link_id))){
				atbmwifi_sta_set_buffered(sta, tid, ATBM_FALSE);
				priv->buffered_set_mask &= ~ATBM_BIT(sta->link_id);
			}
		}
		atbm_spin_unlock_irqrestore(&priv->ps_state_lock, flags);
	}
}

static int __INLINE atbm_get_frame_type(struct atbmwifi_vif *priv, struct atbmwifi_txinfo *t){
	if(!atbm_is_multicast_ether_addr(t->hdr->addr1) &&
		(atbmwifi_ieee80211_is_data_present(t->hdr->frame_control)
#if CONFIG_IEEE80211W
			|| (atbmwifi_ieee80211_is_mgmt(t->hdr->frame_control)		
			&& atbmwifi_ieee80211_is_robust_mgmt_frame(t->hdr)
			&& atbm_get_crypto(priv,2))
#endif
		)){
		return 0;
	}else{
		if(atbmwifi_ieee80211_is_data_present(t->hdr->frame_control))
			return 1;
#if CONFIG_IEEE80211W
		else if(atbmwifi_ieee80211_is_mgmt(t->hdr->frame_control) &&
			atbmwifi_ieee80211_is_robust_mgmt_frame(t->hdr) &&
			atbm_get_crypto(priv,2))
			return 2;
#endif
	}
	return -1;
}

/**************************************************************************************************************
 * 函数名: atbmwifi_tx_h_crypt
 *
 * 功能描述:
 *   TX 流程中的加密处理函数，负责为数据包添加加密相关的头部信息（IV/ICV）。
 *   根据加密类型（WEP/TKIP/CCMP/WAPI）添加相应的初始化向量（IV）和完整性校验值（ICV）空间。
 *
 * 在 TX 流程中的位置:
 *   阶段 3: HAL 层处理 - 加密处理步骤
 *   在 atbmwifi_tx() 中调用，位于链路 ID 计算之后，WSM 头部构建之前
 *
 * 参数说明:
 *   @priv:  虚拟接口指针，包含接口的配置和状态信息
 *   @t:     发送信息结构体指针，包含数据包和发送相关信息
 *   @flags: 输出参数，用于设置 WSM 标志位（如 2 字节偏移标志）
 *
 * 返回值:
 *   0:  成功
 *   -ATBM_ENOMEM: 缓冲区空间不足
 *
 * 处理流程:
 *   1. 获取密钥索引：根据帧类型和链路 ID 获取加密密钥
 *   2. 计算对齐偏移：检查数据是否需要 2 字节对齐
 *   3. 根据加密类型确定 IV/ICV 长度：
 *      - WEP40/WEP104: IV=4, ICV=4
 *      - TKIP: IV=8, ICV=12 (包含 MIC)
 *      - CCMP: IV=8, ICV=8
 *      - WAPI: IV=18, ICV=16
 *   4. 设置加密标志：设置 Protected 帧控制位
 *   5. 检查缓冲区空间：确保有足够的头部和尾部空间
 *   6. 添加 IV 空间：在 802.11 头部后添加 IV 空间
 *   7. 预留 ICV 空间：在数据尾部预留 ICV 空间
 *
 * 支持的加密类型:
 *   - ATBM_WLAN_CIPHER_SUITE_WEP40: WEP 40 位
 *   - ATBM_WLAN_CIPHER_SUITE_WEP104: WEP 104 位
 *   - ATBM_WLAN_CIPHER_SUITE_TKIP: TKIP (WPA)
 *   - ATBM_WLAN_CIPHER_SUITE_CCMP: CCMP (WPA2/AES)
 *   - ATBM_WLAN_CIPHER_SUITE_SMS4: WAPI
 *   - ATBM_WLAN_CIPHER_SUITE_AES_CMAC: 802.11w 管理帧保护
 *
 * 注意事项:
 *   - 实际的加密操作由硬件完成，此函数只预留空间
 *   - 函数会修改 skb 的内容和长度
 *   - 如果不需要加密，函数直接返回 0
 *   - 2 字节对齐偏移用于满足硬件要求
 *
 * 相关函数:
 *   - atbm_get_key(): 获取密钥索引
 *   - atbm_get_crypto(): 获取加密类型
 *   - atbmwifi_tx_h_enchdr(): 添加加密头部信息
 *
 **************************************************************************************************************/
 static int
atbmwifi_tx_h_crypt(struct atbmwifi_vif *priv,struct atbmwifi_txinfo *t, atbm_uint8 *flags)
{
	atbm_size_t iv_len=0;
	atbm_size_t icv_len=0;
	atbm_uint8 *icv;
	atbm_uint8 *newhdr;
	int entryIndex = ATBM_INVALID_KEY;
	atbm_size_t offset = (atbm_size_t)ATBM_OS_SKB_DATA(t->skb) & 3;
	int frame_type = atbm_get_frame_type(priv, t);

	if(!(t->tx_info->flags & ATBM_IEEE80211_TX_INTFL_DONT_ENCRYPT)){
		entryIndex = atbm_get_key(priv,frame_type,t->txpriv.link_id);
	}
	wifi_printk(WIFI_DBG_MSG, "link_id:%d entryIndex:%x frame_type:%d crypto:%x",
		t->txpriv.link_id, entryIndex, frame_type, atbm_get_crypto(priv,!atbm_is_multicast_ether_addr(t->hdr->addr1)));

	t->enc_flags = (((priv->if_id) << ENC_INTERFACE_OFFSET) & ENC_INTERFACE_MASK);
	iv_len = offset ? 2 : 0;
	if(iv_len){
		*flags |= WSM_TX_2BYTES_SHIFT;
	}

	if(entryIndex == ATBM_INVALID_KEY){
		if(iv_len){
			goto copy_header;
		}
		return 0;
	}

	switch(atbm_get_crypto(priv, frame_type)){
	case ATBM_WLAN_CIPHER_SUITE_WEP40:
	case ATBM_WLAN_CIPHER_SUITE_WEP104:
		iv_len += WEP_IV_LEN;
		icv_len = WEP_ICV_LEN;
		break;
	case ATBM_WLAN_CIPHER_SUITE_TKIP:
		wifi_printk(WIFI_DBG_MSG, "tkip crypt\n");
		iv_len += TKIP_IV_LEN;
		icv_len = TKIP_ICV_LEN + 8;
		break;
	case ATBM_WLAN_CIPHER_SUITE_CCMP:
		iv_len += CCMP_HDR_LEN;
		icv_len = CCMP_MIC_LEN;
		break;
	case ATBM_WLAN_CIPHER_SUITE_SMS4:
		iv_len = WAPI_IV_LEN;
		icv_len = WAPI_ICV_LEN;
		break;
#if CONFIG_IEEE80211W
	case ATBM_WLAN_CIPHER_SUITE_AES_CMAC:
		if(t->sta_priv->ieee_80211w)
		{
			struct atbmwifi_ieee80211_mmie *mmie = (struct atbmwifi_ieee80211_mmie *) atbm_skb_put(t->skb, sizeof(struct atbmwifi_ieee80211_mmie));
			memset(mmie,0,sizeof(struct atbmwifi_ieee80211_mmie));
			mmie->element_id = ATBM_WLAN_EID_MMIE;
			mmie->length = sizeof(*mmie) - 2;
			mmie->key_id = atbm_cpu_to_le16(priv->connect.key_idx);
			t->hdr->duration_id = 0x00;
			t->enc_flags |= (ENC_CMD_ENCRYPT|ENC_FORCE_PROTECT_MGMT|ENC_ENABLE_11W);
		}
		return 0;
#endif //#if CONFIG_IEEE80211W
	default:
		ATBM_WARN_ON_FUNC(1);
		break;
	}	

	t->hdr->frame_control |= atbm_cpu_to_le16(ATBM_IEEE80211_FCTL_PROTECTED);
	t->enc_flags |= (ENC_CMD_ENCRYPT|ENC_ENABLE_11W);
copy_header:
	if ((atbm_skb_headroom(t->skb) + atbm_skb_tailroom(t->skb) <
			 iv_len + icv_len ) ||
			(atbm_skb_headroom(t->skb) <
			 iv_len )) {
		wifi_printk(WIFI_DBG_ERROR, "<ERROR>ATBM_ENOMEM\n");
		return -ATBM_ENOMEM;
	} 
	/*
	else if (atbm_skb_tailroom(t->skb) < icv_len) {
		atbm_size_t offset = icv_len - atbm_skb_tailroom(t->skb);
		atbm_uint8 *p;
		p = atbm_skb_push(t->skb, offset);
		atbm_memmove(p, &p[offset], OS_SKB_LEN(t->skb) - offset);
		atbm_skb_trim(t->skb, OS_SKB_LEN(t->skb) - offset);
		wifi_printk(WIFI_DBG_ERROR, "<ERROR>ENOMEM2\n");
	}*/

	newhdr = atbm_skb_push(t->skb, iv_len);
	atbm_memmove(newhdr, newhdr + iv_len, t->hdrlen);
	t->hdr = (struct atbmwifi_ieee80211_hdr *) newhdr;
	t->hdrlen += iv_len;
#if ATBM_QUEUE_8023_FRAMES
	t->tx_info->ivlen = iv_len;
	t->tx_info->icvlen = icv_len;
#endif
	icv = atbm_skb_put(t->skb, icv_len);
	return 0;
}

/**************************************************************************************************************
 * 函数名: atbmwifi_tx_h_wsm
 *
 * 功能描述:
 *   构建 WSM（WiFi System Message）传输头部。WSM 是驱动与硬件之间通信的协议格式，
 *   此函数在数据包前添加 WSM 头部，包含消息长度、消息 ID、队列 ID 等信息。
 *
 * 在 TX 流程中的位置:
 *   阶段 3: HAL 层处理 - WSM 头部构建步骤
 *   在 atbmwifi_tx() 中调用，位于加密处理之后，加密头部添加之前
 *
 * 参数说明:
 *   @priv: 虚拟接口指针，包含接口的配置和状态信息
 *   @t:    发送信息结构体指针，包含数据包和发送相关信息
 *
 * 返回值:
 *   成功: 返回 WSM 头部指针
 *   失败: 返回 NULL（缓冲区空间不足）
 *
 * 处理流程:
 *   1. 在 skb 前添加 WSM 头部空间（atbm_skb_push）
 *   2. 清零 WSM 头部结构体
 *   3. 设置消息长度（包含 WSM 头部的总长度）
 *   4. 设置消息 ID（WSM_TRANSMIT_REQ_MSG_ID）
 *   5. 设置队列 ID（链路 ID + WMM AC 队列号）
 *
 * WSM 头部结构:
 *   - len: 消息总长度
 *   - id: 消息 ID（WSM_TRANSMIT_REQ_MSG_ID = 0x0004）
 *   - queueId: 队列 ID（高位为链路 ID，低位为 WMM AC 队列号）
 *
 * 注意事项:
 *   - 函数会修改 skb 的数据指针和长度
 *   - WSM 头部大小为 sizeof(struct wsm_tx)
 *   - 队列 ID 编码：(raw_link_id << 2) | wsm_queue_id
 *
 * 相关函数:
 *   - atbmwifi_tx_h_crypt(): 加密处理（前一步）
 *   - atbmwifi_tx_h_enchdr(): 加密头部添加（后一步）
 *   - wsm_queue_id_to_wsm(): 将 WMM AC 队列号转换为 WSM 队列号
 *
 **************************************************************************************************************/
static struct wsm_tx *
atbmwifi_tx_h_wsm(struct atbmwifi_vif *priv,
		struct atbmwifi_txinfo *t)
{
	struct wsm_tx *wsm;

	wsm = (struct wsm_tx *)atbm_skb_push(t->skb, sizeof(struct wsm_tx));
	atbm_memset(wsm, 0, sizeof(*wsm));
	wsm->hdr.len = __atbm_cpu_to_le16(ATBM_OS_SKB_LEN(t->skb));
	wsm->hdr.id = __atbm_cpu_to_le16(WSM_TRANSMIT_REQ_MSG_ID);
	wsm->queueId =
		(t->txpriv.raw_link_id << 2) | wsm_queue_id_to_wsm(t->queue);

	return wsm;
}

/**************************************************************************************************************
 * 函数名: atbmwifi_tx_h_rate_build
 *
 * 功能描述:
 *   TX 流程中的速率选择函数，负责为数据包选择合适的发送速率。
 *   支持固定速率、自动速率选择、速率自适应等多种模式。
 *
 * 在 TX 流程中的位置:
 *   阶段 3: HAL 层处理 - 速率选择步骤
 *   在 atbmwifi_tx() 中调用，位于加密头部添加之后，队列管理之前
 *
 * 参数说明:
 *   @priv: 虚拟接口指针，包含接口的配置和状态信息
 *   @t:    发送信息结构体指针，包含数据包和发送相关信息
 *   @wsm:  WSM 头部指针，用于设置速率相关的参数
 *
 * 返回值:
 *   0:  成功
 *   负值: 失败
 *
 * 处理流程:
 *   1. 检查全局固定速率设置（globle_rate）
 *   2. 如果设置了固定速率：
 *      - 设置 TX_RATE_FIXED 标志
 *      - 可选：启用速率下降（tx_rate_down）
 *      - 可选：启用静态速率（tx_rate_static）
 *   3. 如果使用自动速率：
 *      - 调用 rate_control_get_rate() 获取推荐速率
 *   4. 5G 频段处理：禁用 CCK 速率
 *   5. 最小速率处理：管理帧或组播帧使用最小速率
 *
 * 速率模式:
 *   - TX_RATE_FIXED: 固定速率模式
 *   - TX_RATE_FIXED_AUTO_DOWN: 固定速率但允许自动降速
 *   - TX_RATE_FIXED_STATIC: 静态固定速率
 *   - 自动速率: 由速率控制算法决定
 *
 * 注意事项:
 *   - globle_rate 为 0xff 时使用自动速率选择
 *   - 5G 频段不支持 CCK 速率（11b）
 *   - 管理帧和组播帧通常使用最低速率
 *
 * 相关函数:
 *   - rate_control_get_rate(): 速率控制算法
 *   - atbmwifi_tx_h_enchdr(): 加密头部添加（前一步）
 *   - atbmwifi_queue_put(): 队列管理（后一步）
 *
 **************************************************************************************************************/
 static int
atbmwifi_tx_h_rate_build(struct atbmwifi_vif *priv,
			struct atbmwifi_txinfo *t,
			struct wsm_tx *wsm)
{

	atbm_int8 tx_rate = RATE_INDEX_B_1M;
	struct atbmwifi_sta_priv *sta_priv = t->sta_priv;
	struct ieee80211_tx_rate_control txrc;

	if(globle_rate)
		tx_rate = globle_rate;

	if(globle_rate != 0xff){
		//t->tx_info->flags |= ATBM_IEEE80211_TX_CTL_USE_FIXRATE;
		txrc.txrc_in.manual_rate  = tx_rate;
		txrc.txrc_in.manual = TX_RATE_FIXED;
	    t->tx_info->flags |= ATBM_IEEE80211_TX_FIXED_RATE;
		if(tx_rate_down) //fixed rate retry down rate.
		{
			txrc.txrc_in.manual |= TX_RATE_FIXED_AUTO_DOWN;  //manual have two options, 1, aways send on fixed rate. 2, down rate from set rate. 
		}
		if(tx_rate_static)
		{
			txrc.txrc_in.manual |= TX_RATE_FIXED_STATIC;  
		}
	}else{
		txrc.txrc_in.manual = 0;
	}

#if CONFIG_5G_SUPPORT
	if(priv->bss.sta_priv.band == ATBM_NL80211_BAND_5GHZ){
		t->tx_info->flags |= ATBM_IEEE80211_TX_CTL_NO_CCK_RATE;
	}
#endif

	txrc.info = t->tx_info;
	txrc.sband = priv->hw_priv->bands[priv->bss.sta_priv.band];
	if((t->tx_info->flags & ATBM_IEEE80211_TX_CTL_USE_MINRATE) || sta_priv == ATBM_NULL){
		txrc.txrc_in.lower = 1;
	}else
		txrc.txrc_in.lower = 0;

	rate_control_get_rate(sta_priv, &txrc);
	if(txrc.txrc_in.manual == TX_RATE_FIXED){
		t->tx_info->flags |= ATBM_IEEE80211_TX_FIXED_RATE;
	}
	
	wsm->maxTxRate   = t->tx_info->control.txrc_out.tx_max_rate;
	wsm->txRateSets  = atbm_cpu_to_le32(t->tx_info->control.txrc_out.tx_rate_sets);

	wsm->htTxParameters &= ~(atbm_cpu_to_le32(WSM_HT_TX_NEED_CONFIRM));
	
	if ((t->tx_info->flags & ATBM_IEEE80211_TX_FIXED_RATE)||(txrc.txrc_in.lower == 1)){
		wsm->htTxParameters |= atbm_cpu_to_le32(WSM_HT_TX_USE_FIXED_RATE);
	}

	if(t->tx_info->control.tx_update_rate == 1){
		wsm->htTxParameters |=atbm_cpu_to_le32(WSM_HT_TX_NEED_CONFIRM);
		wsm->htTxParameters |=atbm_cpu_to_le32(WSM_NEED_TX_RATE_CONFIRM);
		atbm_printk_rc(">>>>hmac_max_rate:%x  wsm->txRateSets :%x  WSM_NEED_TX_RATE_CONFIRM\n", wsm->maxTxRate,  wsm->txRateSets );
	}

    if(((t->tx_info->control.txrc_out.force_policyid) > 0)){
               wsm->flags &= 0x8f;  //clear bit[4:6]
               wsm->flags |= ((t->tx_info->control.txrc_out.force_policyid)<<4);  //set bit[4:6]
           //atbm_printk_rc("wsm->flags:%x \n", wsm->flags);
    }

	/*
	*try to set WSM_HT_AGGR_EN and WSM_HT_TX_WIDTH_40M
	*/
	if (RATE_HT <= (t->tx_info->control.txrc_out.tx_rate_sets & RATE_MODE_MASK)){
#if ATBM_DRIVER_PROCESS_BA
		wifi_printk(WIFI_DBG_MSG, "rate:%d sta_priv:%x qos:%d\n", wsm->maxTxRate, sta_priv, atbmwifi_ieee80211_is_data_qos(t->hdr->frame_control));
		if(priv->hw_priv->driver_setup_ba){
			/*
			*ba has been installed,so we can try to send ampdu
			*/
			if(sta_priv && atbmwifi_ieee80211_is_data_qos(t->hdr->frame_control)){
				/*
				*try to setup ba
				*/
				if(t->tx_info->flags & ATBM_IEEE80211_TX_CTL_AMPDU){
					wsm->htTxParameters |= WSM_HT_AGGR_EN;
				}else if(ieee80211_start_tx_ba_session(sta_priv,t->txpriv.tid,0)){
					wifi_printk(WIFI_DBG_MSG, "start tx ba session fail[%pM][%d]\n",MAC2STR(sta_priv->mac),t->txpriv.tid);
				}
			}
		}
#endif
		if(sta_priv->rate.channel_type >= ATBM_NL80211_CHAN_HT40MINUS ){
			wsm->htTxParameters |= atbm_cpu_to_le32(WSM_HT_TX_WIDTH_40M);
		}
	}

	if(!(ATBM_IEEE80211_TX_CTL_NOT_ASSIGN_SEQ & t->tx_info->flags)){
		wsm->htTxParameters |= atbm_cpu_to_le32(WSM_HT_NEED_SEQ);
	}

	if(RATE_HT ==  (t->tx_info->control.txrc_out.tx_rate_sets & RATE_MODE_MASK)) {
		wsm->htTxParameters |=  atbm_cpu_to_le32(WSM_HT_TX_MIXED);
	}

	//HT short GI and bandwidth set, MCS7 use short GI, MCS0,1 force to 20M
	if(t->tx_info->control.txrc_out.tx_rc_flag & ATBM_IEEE80211_TX_RC_SHORT_GI){ 

		if(wsm->htTxParameters &atbm_cpu_to_le32(WSM_HT_TX_WIDTH_40M)){
			if(t->tx_info->control.txrc_out.tx_rc_flag&ATBM_IEEE80211_TX_RC_SHORT_GI_40M)
			{
				wsm->htTxParameters |=  atbm_cpu_to_le32(WSM_HT_TX_SGI);
			}
		}else{  //20M
			if(t->tx_info->control.txrc_out.tx_rc_flag&ATBM_IEEE80211_TX_RC_SHORT_GI_20M)
			{
				wsm->htTxParameters |=  atbm_cpu_to_le32(WSM_HT_TX_SGI);
			}               
		}
	}

	if(t->tx_info->control.txrc_out.tx_rc_flag & ATBM_IEEE80211_TX_RC_40M_TO_20M){		

		if(t->tx_info->control.txrc_out.tx_rc_flag&ATBM_IEEE80211_TX_RC_SHORT_GI_20M)
        {
            wsm->htTxParameters |= atbm_cpu_to_le32(WSM_HT_TX_SGI);
        }
        else
        {
              wsm->htTxParameters &=~(atbm_cpu_to_le32(WSM_HT_TX_SGI));
        }
		wsm->htTxParameters  &=	~(atbm_cpu_to_le32(WSM_HT_TX_WIDTH_40M));
	}			

	if(t->tx_info->control.txrc_out.tx_rc_flag & ATBM_IEEE80211_TX_RC_HT_LDPC)
	{
		wsm->htTxParameters |=	atbm_cpu_to_le32(WSM_TX_HT_LDPC);
	}
	else
	{
		wsm->htTxParameters &=	~(atbm_cpu_to_le32(WSM_TX_HT_LDPC));
	}

	///difference

#if CONFIG_HE
	if(priv->hw_priv->chip_version != OCEANUS_NO_WIFI6){
		if(RATE_HE <=  (t->tx_info->control.txrc_out.tx_rate_sets & RATE_MODE_MASK)){
			wsm->heTxParameters  = WSM_TX_HE_HTC;


			if(t->tx_info->control.txrc_out.tx_rc_flag & ATBM_IEEE80211_TX_RC_SHORT_GI){
				// he_ltf_gi = ATBM_HE_2X08;
				wsm->heTxParameters |= atbm_cpu_to_le32(WSM_TX_HE_LTF_2X);		
			}   
			//  he_ldpc = 1;
			//wsm->heTxParameters  = WSM_TX_HE_HTC

			else {
				//if set we not support mode 1X, set LTF to default
				if((wsm->heTxParameters &(WSM_TX_HE_LTF_2X|WSM_TX_HE_LTF_4X))==0){
					wsm->heTxParameters |= atbm_cpu_to_le32(WSM_TX_HE_LTF_4X);         
					wsm->heTxParameters |= atbm_cpu_to_le32(WSM_TX_HE_GI_32);
					//  wsm->heTxParameters |= atbm_cpu_to_le32(WSM_TX_HE_LDPC);
				}
			}

	        if((t->tx_info->control.txrc_out.tx_rc_flag & ATBM_IEEE80211_TX_RC_HE_SUPPORT_DCM)){
				wsm->heTxParameters |= atbm_cpu_to_le32(WSM_TX_HE_SUPPORT_DCM); 
			}

			//set 2x 0.8 mode

			if((t->tx_info->control.txrc_out.tx_rc_flag & ATBM_IEEE80211_TX_RC_HE_DCM)){
				wsm->heTxParameters |= atbm_cpu_to_le32(WSM_TX_HE_DCM_USED);
			}       

			if((t->tx_info->control.txrc_out.tx_rc_flag & ATBM_IEEE80211_TX_RC_HE_LDPC)){
				wsm->heTxParameters |= atbm_cpu_to_le32(WSM_TX_HE_LDPC);
			}       

			if((RATE_HE_ER == (wsm->txRateSets & RATE_MODE_MASK))){
				//atbm_printk_rc("heTxParameters not 40M set %x\n",wsm->heTxParameters);
				wsm->htTxParameters &= ~(atbm_cpu_to_le32(WSM_HT_TX_WIDTH_40M));
			}
			// if(he_pad8us)      
			if(((t->tx_info->control.txrc_out.force_policyid & HE_MODE_DOWN_TO_11B) > 0))
		   {
			 wsm->heTxParameters |= atbm_cpu_to_le32(WSM_TX_HE_DOWN_TO_11B);
			  atbm_printk_rc("set down to 11B heTxParameters:%x \n",  wsm->heTxParameters);
		   }

			{
				wsm->heTxParameters |= atbm_cpu_to_le32(WSM_TX_HE_NORM_PAD8);
			}
			//  else  {
			//wsm->heTxParameters |= atbm_cpu_to_le32(WSM_TX_HE_NORM_PAD16);
			// }
			if(t->tx_info->control.tx_update_rate == 1){  //less print
				atbm_printk_rc("txRateSets:0x%x htTxParameters: 0x%x heTxParameters 0x%x\n",wsm->txRateSets, wsm->htTxParameters, wsm->heTxParameters);
			}
		}
		else {

		}
	}
#endif

	if (t->tx_info->b_eapol || (t->tx_info->b_net == 0)) {		
		wsm->htTxParameters |= atbm_cpu_to_le32(WSM_HT_TX_NEED_CONFIRM);
	}

	return 0;
}
static ATBM_BOOL
atbmwifi_tx_h_pm_state(struct atbmwifi_vif *priv,
	struct atbmwifi_txinfo *t)
{
	int was_buffered = 1;

	if (t->txpriv.link_id == priv->link_id_after_dtim &&
			!priv->buffered_multicasts) {
		priv->buffered_multicasts = ATBM_TRUE;
		if (priv->sta_asleep_mask){
			atbm_queue_work(priv->hw_priv,priv->set_tim_work);
		}
	}
	if (t->txpriv.raw_link_id && (t->txpriv.tid < ATBMWIFI__MAX_TID)){
		was_buffered = priv->link_id_db[t->txpriv.raw_link_id - 1].buffered[t->txpriv.tid]++;
	}
	return !was_buffered;
}
atbm_void atbmwifi_sta_set_buffered(struct atbmwifi_sta_priv *sta_priv,atbm_uint32 tid,ATBM_BOOL buffered)
{
	if (!atbmwifi_is_ap_mode(sta_priv->priv->iftype))
		return;

	if (ATBM_WARN_ON(tid >= ATBMWIFI__MAX_TID)){
		return;
	}
	if((sta_priv->link_id >0) &&(sta_priv->link_id <= ATBMWIFI__MAX_STA_IN_AP_MODE)){
		atbm_set_tim(sta_priv->priv, sta_priv,buffered);
	}
}

static struct wsm_tx *
atbmwifi_tx_h_enchdr(struct wsm_tx *wsm,
		struct atbmwifi_txinfo *t)
{
	atbm_uint8 mac_offset;
	atbm_uint32 data_len;

	wsm->hdr.enc_hdr.EncFlags = t->enc_flags;
	mac_offset = ((atbm_uint8 *)t->hdr - (atbm_uint8*)&wsm->hdr.enc_hdr.EncFlags);
	data_len = wsm->hdr.len - sizeof(struct wsm_tx);

	if(wsm->flags & WSM_TX_2BYTES_SHIFT){
		wsm->hdr.u.common.flag |= HW_INSERT_DUMMY;
		data_len -= 2;
	}

	wsm->hdr.enc_hdr.EncFlags |= (mac_offset << MAC_HEADER_OFFSET_OFFSET) & MAC_HEADER_OFFSET_MASK;
#ifdef ATBM_ENC_CHECKSUM_TEST
	wsm->hdr.enc_hdr.EncFlags |= (CHKSUM_ENABLE|CHKSUM_ENABLE_OPT);
#endif

	wsm->hdr.u.common.flag |= data_len & ENC_DATA_LEN_MASK;
	wsm->hdr.u.common.total_len = wsm->hdr.len;

	return wsm;
}

#ifdef ATBM_DHCP
int atbmwifi_tx_dhcp_frame(struct atbmwifi_txinfo *t)
{	
	atbm_uint16 ethertype;
	atbm_uint8 *payload;
	payload=ATBM_OS_SKB_DATA(t->skb)+ t->hdrlen;
	ethertype = (payload[12] << 8) | payload[13];
	if (ethertype == atbm_ntohs(ATBM_ETH_P_IP))	{
		struct ip_hdr *iph; 
		struct udp_hdr *udph;
		
		iph = (struct ip_hdr *)(payload+14);
		udph = (struct udp_hdr *)((atbm_uint8*)iph+(iph->_v_hl)*4);
		if(IS_BOOTP_PORT(atbm_ntohs(udph->src),atbm_ntohs(udph->dest))){
			wifi_printk(WIFI_CONNECT,"0---1 dhcp tx \n");
			dump_mem((atbm_uint8*)udph,udph->len);
			return 1;			
		}
	}

	return 0;
}
int atbmwifi_rx_dhcp_frame(atbm_uint8 *payload)
{
	atbm_uint16 ethertype;
	ethertype = (payload[6] << 8) | payload[7];
	if (ethertype == htons(ATBM_ETH_P_IP))	{
		struct ip_hdr *iph; //= ip_hdr(skb);
		struct udp_hdr *udph;
		
		iph = (struct ip_hdr *)(payload+14);
		udph = (struct udp_hdr *)((atbm_uint8*)iph+(iph->_v_hl)*4);
		if(IS_BOOTP_PORT(ntohs(udph->src),ntohs(udph->dest))){
			wifi_printk(WIFI_CONNECT,"0---1 dhcp rx \n");
			dump_mem((atbm_uint8*)udph,udph->len);
			return 1;			
		}
	}

	return 0;
}
#endif

/**************************************************************************************************************
 * 函数名: atbmwifi_tx
 *
 * 功能描述:
 *   HAL 层核心发送处理函数，这是 TX 流程中的核心函数，负责数据包的完整处理流程，包括：
 *   链路 ID 计算、加密处理、WSM 头部构建、加密头部添加、速率选择、队列管理、电源管理
 *   等。这是数据包在 HAL 层的主要处理函数。
 *
 * 在 TX 流程中的位置:
 *   阶段 3: HAL 层处理 → 队列管理
 *   这是 HAL 层发送处理的核心函数，完成所有必要的处理后，将数据包加入队列
 *
 * 参数说明:
 *   @hw_priv: 硬件私有数据指针，包含硬件状态和配置信息
 *   @skb: Socket Buffer 指针，包含要发送的数据包（已经是 802.11 格式）
 *   @priv: 虚拟接口指针，包含接口的配置和状态信息
 *
 * 返回值:
 *   无（void），错误通过丢弃数据包处理
 *
 * 处理流程:
 *   1. 初始化发送信息结构体（txinfo）
 *   2. 参数验证：检查接口是否有效、是否使能
 *   3. 链路 ID 计算：计算数据包的链路 ID
 *   4. 加密处理：对数据包进行加密（如果需要）
 *   5. WSM 头部构建：构建 WSM 传输头部
 *   6. 加密头部添加：添加加密相关的头部信息
 *   7. 速率选择：选择发送速率
 *   8. 队列管理：将数据包加入发送队列
 *   9. 电源管理：处理站点电源状态
 *   10. 唤醒 BH 任务：唤醒底层处理任务进行发送
 *
 * 错误处理:
 *   如果任何步骤失败，跳转到 drop 标签，清理资源并丢弃数据包
 *   错误码：-1(priv无效), -2(接口未使能), -6(链路ID计算失败), -7(加密失败),
 *          -8(WSM构建失败), -9(加密头部失败), -11(速率选择失败), -13(队列失败)
 *
 * 注意事项:
 *   - 数据包必须是 802.11 格式（已通过 atbmwifi_tx_queue() 转换）
 *   - 函数会修改数据包内容（加密、添加头部等）
 *   - 成功后将数据包加入队列，由 BH 任务处理实际发送
 *   - 失败时清理所有相关资源，避免内存泄漏
 *
 * 相关函数:
 *   - atbmwifi_tx_h_calc_link_ids(): 计算链路 ID
 *   - atbmwifi_tx_h_crypt(): 加密处理
 *   - atbmwifi_tx_h_wsm(): WSM 头部构建
 *   - atbmwifi_tx_h_enchdr(): 加密头部添加
 *   - atbmwifi_tx_h_rate_build(): 速率选择
 *   - atbmwifi_queue_put(): 队列管理
 *   - atbm_bh_schedule_tx(): 唤醒 BH 任务
 *
 **************************************************************************************************************/
atbm_void atbmwifi_tx(struct atbmwifi_common *hw_priv, struct atbm_buff *skb,struct atbmwifi_vif *priv)
{
	struct wsm_tx *wsm;
	atbm_uint8 flags = 0;
	int ret;
	ATBM_BOOL tid_update = 0;
	struct atbmwifi_txinfo t;
	unsigned long irq_flags;

	/* 步骤 1: 初始化发送信息结构体 */
	t.skb = skb;
	/* 根据优先级确定 WMM AC 队列号（0-3） */
	t.queue = ieee802_1d_to_ac[skb->priority & ATBM_IEEE80211_QOS_CTL_TAG1D_MASK];
	/* 获取发送信息结构体指针 */
	t.tx_info = ATBM_IEEE80211_SKB_TXCB(skb);
	/* 获取 802.11 头部指针 */
	t.hdr = (struct atbmwifi_ieee80211_hdr *)ATBM_OS_SKB_DATA(skb);
	/* 设置 TID（Traffic Identifier） */
	t.txpriv.tid = skb->priority;
	/* 初始化速率 ID 为无效值 */
	t.txpriv.rate_id = ATBMWIFI__INVALID_RATE_ID;

	/* 步骤 2: 参数验证 */
	if (!ATBM_OS_SKB_DATA(skb))
		ATBM_BUG_ON(1);
	
	if (!priv){
		ret = -1;
		goto drop; 
	}	
	/* 检查接口是否使能 */
	if (priv->enabled == 0) {		
		ret = -2;
		goto drop;
	}

	/* 步骤 3: 设置发送标志 */
	/* 如果不是网络数据包或是组播包，使用最小速率 */
	if((t.tx_info->b_net == 0) || (t.tx_info->b_multi == 1))
		t.tx_info->flags |= ATBM_IEEE80211_TX_CTL_USE_MINRATE;
	
	/* 步骤 4: 设置发送私有数据 */
	t.txpriv.if_id = priv->if_id;
	/* 计算 802.11 头部长度 */
	t.hdrlen = atbmwifi_ieee80211_hdrlen(t.hdr->frame_control);
	/* 获取目标地址 */
	t.da = atbmwifi_ieee80211_get_DA(t.hdr);
	/* 查找目标站点的私有数据 */
	t.sta_priv =(struct atbmwifi_sta_priv * )atbmwifi_sta_find(priv,t.da);
	
	/* 步骤 5: DHCP 帧处理（可选） */
#ifdef ATBM_DHCP
	atbmwifi_tx_dhcp_frame(&t);
#endif
	
	/* 步骤 6: 计算链路 ID */
	/* 链路 ID 用于标识不同的站点或链路 */
	ret = atbmwifi_tx_h_calc_link_ids(priv, &t);
	if (ret){
		ret = -6;
		goto drop;
	}

	/* 步骤 7: 加密处理 */
	/* 如果需要加密，对数据包进行加密处理 */
	ret = atbmwifi_tx_h_crypt(priv, &t, &flags);
	if (ret){		
		ret = -7;
		wifi_printk(WIFI_DBG_MSG, "crypt err\n");
		goto drop;
	}

	/* 步骤 8: 构建 WSM 头部 */
	/* WSM（WiFi System Message）是驱动与硬件通信的协议头部 */
	wsm = atbmwifi_tx_h_wsm(priv, &t);
	if (!wsm) {
		ret = -8;
		goto drop;
	}
	/* 设置加密标志 */
	wsm->flags |= flags;

	/* 步骤 9: 添加加密头部 */
	/* 如果需要加密，添加加密相关的头部信息 */
    wsm = atbmwifi_tx_h_enchdr(wsm, &t);
    if (!wsm) {
        ret = -9;
        goto drop;
    }

	/* 步骤 10: 速率选择 */
	/* 根据链路质量和策略选择发送速率 */
	ret = atbmwifi_tx_h_rate_build(priv, &t, wsm);
	if (ret){		
		ret = -11;
		goto drop;
	}

	/* 步骤 11: 将数据包加入发送队列 */
	/* 根据优先级将数据包加入对应的 WMM AC 队列 */
	ret = atbmwifi_queue_put(&hw_priv->tx_queue[t.queue],t.skb, &t.txpriv);	
	if(ret){
		ret = -13;
		goto drop;
	}

	/* 步骤 12: 电源管理处理 */
	/* 处理站点的电源状态，如果站点处于睡眠状态，标记数据包为缓冲 */
	atbm_spin_lock_irqsave(&priv->ps_state_lock, &irq_flags);
	tid_update = atbmwifi_tx_h_pm_state(priv, &t);

	/* 如果站点处于睡眠状态，设置缓冲标志 */
	/*If there are no stored packet,here no need updata tim elems*/
	if (tid_update && t.sta_priv && (priv->sta_asleep_mask & ATBM_BIT(t.txpriv.link_id))){
		atbmwifi_sta_set_buffered(t.sta_priv,t.txpriv.tid, ATBM_TRUE);
		priv->buffered_set_mask |= ATBM_BIT(t.txpriv.link_id);
	}
	atbm_spin_unlock_irqrestore(&priv->ps_state_lock, irq_flags);
	
	/* 步骤 13: 唤醒 BH 任务进行发送 */
	/* 设置 bh_tx 标志并唤醒底层处理任务，由 BH 任务从队列取出数据包并发送 */
	/*Do tx task schedule,change task excute text*/
	///TODO atbm_bh_schedule_tx or do wakeup Tx thread
	atbm_bh_schedule_tx(hw_priv);
	return;
	
drop:
	/* 错误处理：清理资源并丢弃数据包 */
	//if (atbm_atomic_add_return(1, &hw_priv->bh_tx) == 1){
	//	wifi_printk(WIFI_DBG_ERROR,"atbm_bh_wakeup tx2\n");
	//	atbm_os_wakeup_event(&hw_priv->bh_wq);
	//}
	/* 必须设置 link_id = 0，避免在 atbmwifi_skb_dtor 中调用 atbmwifi_notify_buffered_tx */
	//must set link_id = 0,inorder to not call atbmwifi_notify_buffered_tx in atbmwifi_skb_dtor
	t.skb = ATBM_NULL;
	t.tx_info = ATBM_NULL;
	t.hdr = ATBM_NULL;
	t.txpriv.raw_link_id = 0;
	t.txpriv.link_id = 0;
	wifi_printk((WIFI_TX|WIFI_WARN_CODE), "tx drop ret=%d \n",ret);
	//atbm_spin_lock(&priv->ps_state_lock);
#ifndef ATBM_RX_STATUS_USE_QUEUE
	/* 释放 Socket Buffer */
	atbm_dev_kfree_skb(skb);
#else
	/* 使用队列模式，通过状态队列释放 */
	atbmwifi_ieee80211_tx_status_irqsafe(priv,skb);
#endif
	//atbm_spin_unlock(&priv->ps_state_lock);
	return;
}

 atbm_void atbmwifi_skb_dtor(struct atbmwifi_common *hw_priv,
		     struct atbm_buff *skb,
		     const struct atbmwifi_txpriv *txpriv)
{
	struct atbmwifi_ieee80211_tx_info *tx_info;
	struct atbmwifi_vif *priv =_atbmwifi_hwpriv_to_vifpriv(hw_priv, txpriv->if_id);

	if (priv&& atbmwifi_is_ap_mode(priv->iftype) && txpriv->raw_link_id) {
		if(priv->link_id_db[txpriv->raw_link_id - 1].status == ATBMWIFI__LINK_HARD){
			atbmwifi_notify_buffered_tx(priv,skb,txpriv->raw_link_id,txpriv->tid);

			tx_info =  ATBM_IEEE80211_SKB_TXCB(skb);
		
			/*PS-Poll period end */
			if(tx_info->flags & ATBM_IEEE80211_TX_CTL_PS_POLL_RESP){
				priv->link_id_db[txpriv->raw_link_id-1].sta_priv.flags &= ~WLAN_STA_PS;
			}
		}
	}
	#ifndef ATBM_RX_STATUS_USE_QUEUE
	atbm_dev_kfree_skb(skb);
	#else
	atbmwifi_ieee80211_tx_status_irqsafe(priv,skb);
	#endif
}
/* ******************************************************************** */
#if NEW_SUPPORT_PS
int atbmwifi_deliver_poll_response(struct atbmwifi_vif *priv,struct atbmwifi_ieee80211_hdr * hdr,int link_id)
{
	//atbm_uint32 drop = 1;
	int i;
	atbm_uint32 pspoll_mask = 0;
	struct atbm_buff *Newskb;
	struct atbmwifi_ieee80211_pspoll *pspoll=(struct atbmwifi_ieee80211_pspoll *) hdr;
	/*Deal with the legency powersave*/
	pspoll_mask = ATBM_BIT(link_id);

	priv->pspoll_mask |= pspoll_mask;
	if (priv->join_status != ATBMWIFI__JOIN_STATUS_AP)
		goto DONE;
	/*PS-Poll period starts */
	//atbm_set_bit(WLAN_STA_PS,&priv->link_id_db[link_id-1].sta_priv.flags);
	priv->link_id_db[link_id-1].sta_priv.flags |= WLAN_STA_PS;
	/* Do not report pspols if data for given link id is
	 * queued already. */
	for (i = 0; i < ATBM_IEEE80211_NUM_ACS; ++i) {
		if (atbmwifi_queue_get_num_queued(priv,
				&priv->hw_priv->tx_queue[i],
				pspoll_mask)) {
			atbm_bh_schedule_tx(priv->hw_priv);
			break;
		}
	}
	if(i == ATBM_IEEE80211_NUM_ACS){
		/*If there is no data queued,should send NullData*/
		Newskb = (struct atbm_buff *)atbmwifi_ieee80211_NullData(priv,pspoll->ta,pspoll->bssid);
		atbmwifi_tx(priv->hw_priv,Newskb,priv);
	}

DONE:
	return 0;
}
int atbmwifi_deliver_uapsd_response(struct atbmwifi_vif *priv,struct atbmwifi_ieee80211_hdr * hdr,int link_id,int tid)
{
	int ac;	
	atbm_uint32 uapsd_mask=0;
	//atbm_uint8 delivery_enabled = priv->link_id_db[link_id-1].sta_priv.uapsd_support_queues;
	/*If this AC is not trigger-enabled do nothing,if receive a triger frame
	transmit all pending packet,if it need to transmit every tid.Modyfy--->*/
	if (priv->join_status != ATBMWIFI__JOIN_STATUS_AP)
		goto DONE;
	ac = ieee802_1d_to_ac[tid & 7];
	/*Deal with the uapsd powersave*/
	uapsd_mask |= ATBM_BIT(link_id);

	priv->link_id_uapsd_mask |= uapsd_mask;	

	atbm_bh_schedule_tx(priv->hw_priv);
DONE:
	return 0;
}
#endif
 int atbmwifi_rx_filter_retry(struct atbmwifi_vif *priv,atbm_uint8 link_id,struct atbm_buff *skb)
{
	struct atbmwifi_link_entry *sta_link_id_db = ATBM_NULL;
	struct atbmwifi_ieee80211_hdr *hdr = (struct atbmwifi_ieee80211_hdr *)ATBM_OS_SKB_DATA(skb);
	struct atbmwifi_ieee80211_rx_status *status = ATBM_IEEE80211_SKB_RXCB(skb);
	if(atbm_is_multicast_ether_addr(hdr->addr1)){
		return 0;
	}		
	if (!atbmwifi_ieee80211_is_data_present(hdr->frame_control)){
		return 0;
	}
	if(link_id == ATBMWIFI__LINK_ID_UNMAPPED)
		link_id = ATBMWIFI__MAX_STA_IN_AP_MODE+1;

	if(link_id>(ATBMWIFI__MAX_STA_IN_AP_MODE+1)){
		wifi_printk(WIFI_DBG_ERROR, "[rx_retry] link_id fail(%d)\n",link_id);
		return -1;
	}

	sta_link_id_db = &priv->link_id_db[link_id-1];
	if(sta_link_id_db->status == ATBMWIFI__LINK_OFF){
		wifi_printk(WIFI_DBG_ANY, "[rx_retry]lin_id(%d) status(%d)\n",link_id,sta_link_id_db->status);
		return 0;
	}
	atbmwifi_ieee80211_parse_qos(priv,skb);
	
	if (sta_link_id_db->sta_retry.last_rx_seq[status->seqno_idx] ==
		     hdr->seq_ctrl) {

		if( atbmwifi_ieee80211_has_retry(hdr->frame_control) ){
			wifi_printk(WIFI_DBG_MSG, "[rx_retry](%x)(%d)(%d(%d,(%x,%x)\n",hdr->frame_control,
										priv->if_id,link_id,status->seqno_idx,
										sta_link_id_db->sta_retry.last_rx_seq[status->seqno_idx],hdr->seq_ctrl);
			sta_link_id_db->sta_retry.num_duplicates++;
			return -1;
		}
		else {
			wifi_printk(WIFI_DBG_MSG, "[rx_retry1](%x)(%d)(%d(%d,(%x,%x) skb %x len %d %x\n",hdr->frame_control,
										priv->if_id,link_id,status->seqno_idx,
										sta_link_id_db->sta_retry.last_rx_seq[status->seqno_idx],hdr->seq_ctrl,(unsigned int)skb,skb->dlen,(unsigned int)ATBM_OS_SKB_DATA(skb));
		}
	}
	
	else{
		sta_link_id_db->sta_retry.last_rx_seq[status->seqno_idx] = hdr->seq_ctrl;
	}
	return 0;
}

int atbmwifi_rx_filter_mac(struct atbmwifi_vif *priv, struct atbm_buff *skb){
	struct atbmwifi_ieee80211_hdr *hdr = (struct atbmwifi_ieee80211_hdr *)ATBM_OS_SKB_DATA(skb);
	int i;
	ATBM_BOOL match = 0;

	if(!priv->config.filter_enable)
		return 0;

	for(i = 0; i < priv->config.mac_count; i++){
		if(atbm_memcmp(priv->config.mac_list[i], hdr->addr2, ATBM_ETH_ALEN) == 0){
			match = 1;
			break;
		}
	}

	return priv->config.filter_mode ^ match ? 0 : -1;
}
 
static atbm_uint8 tmprxhdr_buffer[32];

#if CONFIG_WPA2_REINSTALL_CERTIFICATION
static void atbmwifi_ccmp_hdr2pn(atbm_uint8 *pn, atbm_uint8 *hdr)
{
	pn[0] = hdr[7];
	pn[1] = hdr[6];
	pn[2] = hdr[5];
	pn[3] = hdr[4];
	pn[4] = hdr[1];
	pn[5] = hdr[0];
}
#endif

int atbmwifi_ccmp_replaycnt(struct atbmwifi_vif *priv,struct atbmwifi_ieee80211_mgmt *mgmt)
{
#if CONFIG_WPA2_REINSTALL_CERTIFICATION
	 atbm_uint8 pn[CCMP_PN_LEN];
	 static const atbm_uint8 zero_pn[6] = {0};
	 int hdrlen = atbmwifi_ieee80211_hdrlen(mgmt->frame_control);
	 atbm_uint8 *qc = atbmwifi_ieee80211_get_qos_ctl((struct atbmwifi_ieee80211_hdr *)mgmt);
		 /* frame has qos control */
	 atbm_uint8  tid = *qc & ATBM_IEEE80211_QOS_CTL_TID_MASK;
	 atbm_uint8 *pOldPN;
	 atbm_uint8 *b_pn_init;
	 if(!atbmwifi_is_sta_mode(priv->iftype)){
		 return 0;	 
	 }


	 atbmwifi_ccmp_hdr2pn(pn, (atbm_uint8 *)mgmt + hdrlen);
	 
	 if(atbm_is_multicast_ether_addr(mgmt->da)==0){	 
	 	if(atbmwifi_ieee80211_is_data_qos(mgmt->frame_control)){			
			pOldPN = priv->connect.ptk_pn[tid];
			b_pn_init= &priv->connect.ptk_pn_init[tid] ;
	 	}
		else {
			pOldPN = priv->connect.ptk_noqos_pn;
			b_pn_init = &priv->connect.ptk_noqos_pn_init;

		}
	 }
	 else {
		pOldPN = priv->connect.gtk_pn;
		b_pn_init = &priv->connect.gtk_pn_init;
	 }

	 //if first PN just update
	 if(*b_pn_init == 0) {
	 	if(memcmp(pn, pOldPN, CCMP_PN_LEN) <= 0) {
	 	 	if(!((memcmp(pn, zero_pn, CCMP_PN_LEN)==0 )&&(memcmp(pOldPN, zero_pn, CCMP_PN_LEN)==0 ))){
			 	 wifi_printk(WIFI_DBG_ERROR, "[RX]ccmp_replaycnt drop %x:%x:%x:%x:%x:%x > %x:%x:%x:%x:%x:%x \n",pOldPN[0]
									 	,pOldPN[1]
									 	,pOldPN[2]
									 	,pOldPN[3]
									 	,pOldPN[4]
									 	,pOldPN[5]
									 	,pn[0]
									 	,pn[1]
									 	,pn[2]
									 	,pn[3]
									 	,pn[4]
									 	,pn[5]);
		 	 }
			 return RX_DROP_UNUSABLE;
		 }
		 else {
		 	int i=0;
			for(i=0;i<CCMP_PN_LEN;i++){
				if(pn[i]>pOldPN[i]){
					//PN      = xx,xx,xx,xx,0,0
					if(i>=CCMP_PN_LEN-2){
						break;
					}
					else {
						//pOldPN[i];
						//PN      = 1,0,0,0,0,0
						//pOldPN= 0,0xff,0xff,0xff,0,0
						if((pn[i]!=(pOldPN[i]+1))
							||(pOldPN[i+1]!=0xff)
							||(pn[i+1]!=0)){
							wifi_printk(WIFI_DBG_ERROR, "[RX]ccmp_replaycnt not updata\n");
						    wifi_printk(WIFI_DBG_ERROR, "[RX]ccmp_replaycnt drop %x:%x:%x:%x:%x:%x > %x:%x:%x:%x:%x:%x \n",pOldPN[0]
												 	,pOldPN[1]
												 	,pOldPN[2]
												 	,pOldPN[3]
												 	,pOldPN[4]
												 	,pOldPN[5]
												 	,pn[0]
												 	,pn[1]
												 	,pn[2]
												 	,pn[3]
												 	,pn[4]
												 	,pn[5]);
							return 0;
						}
					}
				}
			}
	 	}
	 }
	 else {
		*b_pn_init = 0;
	 }
	 atbm_memcpy(pOldPN,pn, CCMP_PN_LEN);
#endif //#ifndef CONFIG_WPA2_REINSTALL_CERTIFICATION
	 return 0;
}

static int atbmwifi_class3_err(struct atbmwifi_vif *priv, struct atbmwifi_ieee80211_hdr *frame, struct wsm_rx *arg, int link_id)
{
	struct atbmwifi_sta_priv *sta_priv;

	if(!atbmwifi_is_ap_mode(priv->iftype))
		return 0;
	//is doing stop process,just drop rx frame
	if(priv->doing_stop_off == 1){
		return 3;
	}
	sta_priv = atbmwifi_sta_find_form_hard_linkid(priv, (atbm_uint8) link_id);
	if(sta_priv == ATBM_NULL || atbm_memcmp(sta_priv->mac, frame->addr2, ATBM_ETH_ALEN)){
		/*
		*must be outside lock due to cfg80211
		*be that's not a problem
		*/
		atbmwifi_ieee80211_send_deauth_disassoc(priv, frame->addr2, priv->bssid, ATBM_IEEE80211_STYPE_DEAUTH,
			ATBM_WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY,
			ATBM_NULL, ATBM_TRUE);
		wifi_printk(WIFI_ALWAYS, "atbmwifi_class3_err\n");
		return 1;
	}
	return 0;
}

/**************************************************************************************************************
 * 函数名: atbmwifi_rx_cb
 *
 * 功能描述:
 *   WSM RX 回调函数，处理从硬件接收到的数据包，进行状态检查、错误处理、
 *   并填充 RX 状态信息，然后分发到 802.11 层处理。
 *
 * 在 RX 流程中的位置:
 *   阶段 4: WSM 层处理 → 802.11 层
 *   在 wsm_receive_indication() 中调用，处理 WSM_RECEIVE_INDICATION_ID 消息
 *
 * 参数说明:
 *   @priv:    虚拟接口私有数据指针
 *   @arg:     WSM RX 指示结构体，包含接收状态、信道、速率等信息
 *   @skb_p:   指向 skb 指针的指针（可能被修改）
 *   @link_id: 链路 ID（AP 模式下标识 STA）
 *
 * 返回值:
 *   无
 *
 * 处理流程:
 *   1. 状态检查：检查 WSM 接收状态
 *      - MIC 失败：设置 ATBM_RX_FLAG_MMIC_ERROR 标志
 *      - 无密钥：丢弃数据包
 *      - 其他错误：丢弃数据包
 *   2. Class 3 错误处理：AP 模式下检查未关联的 STA
 *   3. 长度检查：验证数据包长度
 *   4. 填充 RX 状态：
 *      - band: 频段（2.4GHz/5GHz）
 *      - freq: 频率
 *      - signal: RSSI 信号强度
 *      - rate_idx: 速率索引
 *      - flag: 标志位（HT、解密、AMSDU 等）
 *   5. 包重排序：如果启用 ATBM_PKG_REORDER，进行包重排序处理
 *   6. 分发处理：调用 atbmwifi_ieee80211_rx_irqsafe() 分发到 802.11 层
 *
 * RX 状态标志:
 *   - ATBM_RX_FLAG_HT: HT（802.11n）帧
 *   - ATBM_RX_FLAG_DECRYPTED: 已解密
 *   - ATBM_RX_FLAG_AMSDU: AMSDU 聚合帧
 *   - ATBM_RX_FLAG_MMIC_ERROR: MIC 校验失败
 *
 * 注意事项:
 *   - 错误状态的数据包会被丢弃
 *   - AP 模式下需要检查 Class 3 错误
 *   - 包重排序功能用于处理乱序的数据包
 *
 * 相关函数:
 *   - wsm_receive_indication(): 调用此函数
 *   - atbmwifi_ieee80211_rx_irqsafe(): 802.11 层处理入口
 *   - atbm_reorder_skb_queue(): 包重排序处理
 *
 **************************************************************************************************************/
 atbm_void atbmwifi_rx_cb(struct atbmwifi_vif *priv,struct wsm_rx *arg,struct atbm_buff **skb_p,int link_id)
{
	struct atbm_buff *skb = *skb_p;
	struct atbmwifi_ieee80211_rx_status *hdr = ATBM_IEEE80211_SKB_RXCB(skb);
	struct atbmwifi_ieee80211_mgmt *mgmt = (struct atbmwifi_ieee80211_mgmt *)ATBM_OS_SKB_DATA(skb);

	hdr->flag = 0;

	if (atbm_unlikely(arg->status)) {
#if CONFIG_IEEE80211W
		if(priv->iftype == ATBM_NL80211_IFTYPE_STATION && priv->connect.crypto_igtkgroup){
			if(atbmwifi_ieee80211_is_robust_mgmt_frame((struct atbmwifi_ieee80211_hdr *)ATBM_OS_SKB_DATA(skb))){
				wifi_printk(WIFI_DBG_ERROR, "[RX]:drop PMF\n");
				goto drop;
			}
		}
#endif
		atbmwifi_class3_err(priv, (struct atbmwifi_ieee80211_hdr *)mgmt, arg, link_id);
		if (arg->status == WSM_STATUS_MICFAILURE) {
			/* IPRO7 port: do NOT log per-frame here. The firmware reports
			 * MICFAILURE on every encrypted bcast/mcast (or stale-key) frame
			 * we can't decrypt; at 115200 baud this floods the UART and starves
			 * the WPA event thread past the AP's EAPOL M3 retransmit deadline,
			 * breaking the 4-way handshake. The frame is dropped via the flag
			 * below regardless of the log. */
			hdr->flag |= ATBM_RX_FLAG_MMIC_ERROR;
		} else if (arg->status == WSM_STATUS_NO_KEY_FOUND) {
			wifi_printk(WIFI_DBG_ERROR, "[RX] No key\n");
			goto drop;
		} else {
			wifi_printk(WIFI_DBG_ERROR, "[RX] fail: %d\n",arg->status);
			goto drop;
		}
	}else{
		if(link_id > ATBMWIFI__MAX_STA_IN_AP_MODE && atbmwifi_ieee80211_is_data(mgmt->frame_control)){
			if(atbmwifi_class3_err(priv, (struct atbmwifi_ieee80211_hdr *)mgmt, arg, link_id))
				goto drop;
		}
	}

	if ( ATBM_OS_SKB_LEN(skb) < sizeof(struct atbmwifi_ieee80211_pspoll)) {
		wifi_printk(WIFI_DBG_ERROR, "<ERROR> [RX] len %d\n",ATBM_OS_SKB_LEN(skb));
		goto drop;
	}
	hdr->link_id = link_id;
	if(atbmwifi_is_sta_mode(priv->iftype)){
		link_id = 1;
	}

	hdr->band = (arg->channelNumber > 14) ?
			ATBM_IEEE80211_BAND_5GHZ : ATBM_IEEE80211_BAND_2GHZ;
	hdr->freq = atbmwifi_ieee80211_channel_to_frequency(
			arg->channelNumber,
			hdr->band);

	if (arg->rxedRate >= 14) {
		hdr->flag |= ATBM_RX_FLAG_HT;
		hdr->rate_idx = arg->rxedRate - 14;
	} else if (arg->rxedRate >= 4) {
			hdr->rate_idx = arg->rxedRate - 2;
	} else {
		hdr->rate_idx = arg->rxedRate;
	}

	hdr->signal = (atbm_int8)arg->rcpiRssi;
	hdr->rx_SNR = (atbm_int16)(arg->flags2>>16);
	//record rssi value
	atbmwifi_set_rssi(hdr->signal);
	atbmwifi_set_SNR(hdr->rx_SNR);

	hdr->antenna = 0;
    /*move the addtion header of the encrypt frame */
	if (WSM_RX_STATUS_ENCRYPTION(arg->flags)) {
		atbm_size_t iv_len = 0, icv_len = 0;
		atbm_size_t hdrlen = 0;
		hdrlen = atbmwifi_ieee80211_hdrlen(mgmt->frame_control);
		//wifi_printk(WIFI_DBG_MSG, "ENCRYPTION\n");
		hdr->flag |= ATBM_RX_FLAG_DECRYPTED;

		/* Oops... There is no fast way to ask mac80211 about
		 * IV/ICV lengths. Even defineas are not exposed.*/
		switch (WSM_RX_STATUS_ENCRYPTION(arg->flags)) {
		case WSM_RX_STATUS_WEP:
			iv_len = 4 /* WEP_IV_LEN */;
			icv_len = 4 /* WEP_ICV_LEN */;
			break;
		case WSM_RX_STATUS_TKIP:
			iv_len = 8 /* TKIP_IV_LEN */;
			icv_len = 4 /* TKIP_ICV_LEN */
				+ 8 /*MICHAEL_MIC_LEN*/;
			hdr->flag |= ATBM_RX_FLAG_MMIC_STRIPPED;
			break;
		case WSM_RX_STATUS_AES:
			iv_len = 8 /* CCMP_HDR_LEN */;
			icv_len = 8 /* CCMP_MIC_LEN */;
			if(atbmwifi_ccmp_replaycnt(priv,mgmt))
				goto drop;
			break;
		case WSM_RX_STATUS_WAPI:
			iv_len = 18 /* WAPI_HDR_LEN */;
			icv_len = 16 /* WAPI_MIC_LEN */;
			hdr->flag |= ATBM_RX_FLAG_IV_STRIPPED;
			break;
		default:
			ATBM_WARN_ON_FUNC("Unknown encryption type");
			goto drop;
		}

		/* Firmware strips ICV in case of MIC failure. */
		if (arg->status == WSM_STATUS_MICFAILURE) {
			icv_len = 0;
			hdr->flag |= ATBM_RX_FLAG_IV_STRIPPED;
		}

		if(ATBM_OS_SKB_LEN(skb) < hdrlen + iv_len + icv_len) {
			wifi_printk(WIFI_DBG_ERROR, "rxlen len lesser than crypto hdr.\n");
			goto drop;
		}

		/* Protocols not defined in mac80211 should be
		stripped/crypted in driver/firmware */
		atbm_skb_trim(skb,  ATBM_OS_SKB_LEN(skb) - icv_len);
		ATBM_BUG_ON(hdrlen > 32);
		atbm_memcpy(tmprxhdr_buffer, ATBM_OS_SKB_DATA(skb), hdrlen);
		atbm_memcpy(ATBM_OS_SKB_DATA(skb) + iv_len,tmprxhdr_buffer, hdrlen);
		atbm_skb_pull(skb, iv_len);
	}
#if ATBM_PKG_REORDER
	if(atbmwifi_ieee80211_is_data_qos(mgmt->frame_control))
	{
		if((link_id == 0)||(link_id>=WLAN_LINK_ID_MAX))
		{
			goto direct_queue;
		}
		if(atbm_reorder_skb_queue(priv,skb,link_id-1) == 0)
		{
			goto direct_queue;
		}
		else
		{
			*skb_p = ATBM_NULL;
			return;
		}
	}
direct_queue:
#endif //ATBM_PKG_REORDER
	if(atbmwifi_ieee80211_rx_irqsafe(priv,skb)==0){
		*skb_p = ATBM_NULL;
	}
	return;
drop:
	return;
}

#if ATBM_QUEUE_8023_FRAMES
/**************************************************************************************************************
 * 函数名: atbmwifi_tx_queue
 *
 * 功能描述:
 *   HAL 层队列处理函数，这是 TX 流程中用于处理 802.3 帧的队列函数。函数进行链路 ID 计算、
 *   帧类型判断、队列管理、电源管理处理，然后唤醒 BH 任务进行发送。与 atbmwifi_tx() 的区别是，
 *   此函数处理的是 802.3 帧，需要先进行格式转换。
 *
 * 在 TX 流程中的位置:
 *   阶段 3: HAL 层处理 → 队列管理
 *   这是处理 802.3 帧的队列函数，在 atbmwifi_tx_start() 中调用
 *
 * 参数说明:
 *   @hw_priv: 硬件私有数据指针，包含硬件状态和配置信息
 *   @skb: Socket Buffer 指针，包含要发送的数据包（802.3 格式）
 *   @priv: 虚拟接口指针，包含接口的配置和状态信息
 *
 * 返回值:
 *   0: 成功
 *   负值: 失败（-2: priv无效, -3: 接口未使能, -5: 链路ID计算失败, -6: 队列失败）
 *
 * 处理流程:
 *   1. 初始化发送信息结构体
 *   2. 参数验证：检查接口是否有效、是否使能
 *   3. 链路 ID 计算：计算数据包的链路 ID
 *   4. 帧类型判断：判断是 802.3 帧还是内部帧
 *   5. 队列管理：将数据包加入发送队列
 *   6. 电源管理：处理站点电源状态
 *   7. 唤醒 BH 任务：唤醒底层处理任务进行发送
 *
 * 注意事项:
 *   - 此函数处理的是 802.3 帧，需要先进行格式转换
 *   - 与 atbmwifi_tx() 的区别是，此函数不进行加密、WSM 构建等处理
 *   - 这些处理会在队列处理时进行（通过 atbmwifi_tx() 调用）
 *   - 成功后将数据包加入队列，由 BH 任务处理实际发送
 *
 * 相关函数:
 *   - atbmwifi_tx_h_calc_link_ids(): 计算链路 ID
 *   - atbmwifi_queue_put(): 队列管理
 *   - atbmwifi_tx_h_pm_state(): 电源管理状态处理
 *   - atbm_bh_schedule_tx(): 唤醒 BH 任务
 *
 **************************************************************************************************************/
int atbmwifi_tx_queue(struct atbmwifi_common *hw_priv, struct atbm_buff *skb,struct atbmwifi_vif *priv){
	struct atbmwifi_txinfo t;
	struct atbmwifi_ieee80211_tx_info *tx_info = ATBM_IEEE80211_SKB_TXCB(skb);
	ATBM_BOOL tid_update;
	int ret;
	unsigned long flags;

	/* 步骤 1: 初始化发送信息结构体 */
	/* 根据优先级确定 WMM AC 队列号（0-3） */
	t.queue = ieee802_1d_to_ac[skb->priority & ATBM_IEEE80211_QOS_CTL_TAG1D_MASK];
	/* 设置 TID（Traffic Identifier） */
	t.txpriv.tid = skb->priority;
	/* 初始化速率 ID 为无效值 */
	t.txpriv.rate_id = ATBMWIFI__INVALID_RATE_ID;

	/* 步骤 2: 参数验证 */
	if (!ATBM_OS_SKB_DATA(skb))
		ATBM_BUG_ON(1);

	if (!priv){
		ret = -2;
		goto drop; 
	}

	/* 检查接口是否使能 */
	if (priv->enabled == 0) {		
		ret = -3;
		goto drop;
	}

	/* 步骤 3: 设置发送私有数据 */
	t.txpriv.if_id = priv->if_id;
	t.sta_priv = tx_info->sta_priv;
	t.da = ATBM_OS_SKB_DATA(skb);
	
	/* 步骤 4: 计算链路 ID */
	/* 链路 ID 用于标识不同的站点或链路 */
	ret = atbmwifi_tx_h_calc_link_ids(priv, &t);
	if (ret){
		ret = -5;
		goto drop;
	}

	/* 步骤 5: 帧类型判断 */
	/* 如果不是 802.3 帧，判断是内部帧还是转发帧 */
	if(skb->Type != TX_SKB__TX_8023_FRAME){
		if(tx_info->b_eapol){
			/* EAPOL 帧，标记为内部 802.3 帧 */
			skb->Type = TX_SKB__INTERNAL_8023_FRAME;
		}else{
			/* 其他帧，标记为转发 802.3 帧 */
			skb->Type = TX_SKB__FORWARD_8023_FRAME;
		}
	}

	/* 步骤 6: 将数据包加入发送队列 */
	/* 根据优先级将数据包加入对应的 WMM AC 队列 */
	ret = atbmwifi_queue_put(&hw_priv->tx_queue[t.queue], skb, &t.txpriv); 
	if(ret){
		ret = -6;
		goto drop;
	}
	
	/* 步骤 7: 电源管理处理 */
	/* 处理站点的电源状态，如果站点处于睡眠状态，标记数据包为缓冲 */
	atbm_spin_lock_irqsave(&priv->ps_state_lock, &flags);
	tid_update = atbmwifi_tx_h_pm_state(priv, &t);

	/* 如果站点处于睡眠状态，设置缓冲标志 */
	/*If there are no stored packet,here no need updata tim elems*/
	if (tid_update && t.sta_priv && (priv->sta_asleep_mask & ATBM_BIT(t.txpriv.link_id))){
		atbmwifi_sta_set_buffered(t.sta_priv,t.txpriv.tid, ATBM_TRUE);
		priv->buffered_set_mask |= ATBM_BIT(t.txpriv.link_id);
	}
	atbm_spin_unlock_irqrestore(&priv->ps_state_lock, flags);
	
	/* 步骤 8: 唤醒 BH 任务进行发送 */
	/* 设置 bh_tx 标志并唤醒底层处理任务，由 BH 任务从队列取出数据包并发送 */
	/*Do tx task schedule,change task excute text*/
	///TODO atbm_bh_schedule_tx or do wakeup Tx thread
	atbm_bh_schedule_tx(hw_priv);
	return 0;
	
drop:
	/* 错误处理：释放数据包并返回错误码 */
	wifi_printk((WIFI_TX|WIFI_WARN_CODE), "tx drop ret=%d \n",ret);
	//atbm_spin_lock(&priv->ps_state_lock);
	atbm_dev_kfree_skb(skb);
	return ret;

}

/**************************************************************************************************************
 * 函数名: atbmwifi_tx_start
 *
 * 功能描述:
 *   HAL 层发送处理入口函数，进行发送前的预处理，包括连接检查、站点查找、流控检查、
 *   AMPDU 聚合处理等，然后调用队列处理函数进行实际的发送。
 *
 * 在 TX 流程中的位置:
 *   阶段 3: HAL 层入口 → HAL 层处理
 *   这是 HAL 层发送处理的入口点，进行各种预处理后调用 atbmwifi_tx_queue()
 *
 * 参数说明:
 *   @skb: Socket Buffer 指针，包含要发送的数据包
 *   @priv: 虚拟接口指针，包含接口的配置和状态信息
 *
 * 返回值:
 *   无（void），错误通过丢弃数据包处理
 *
 * 处理流程:
 *   1. 连接状态检查：STA 模式下检查是否已关联
 *   2. 站点查找：查找目标站点的私有数据
 *   3. QoS 判断：判断是否使用 QoS
 *   4. 流控检查：检查队列是否使能
 *   5. AMPDU 聚合处理（可选）：如果启用 BA，进行聚合处理
 *   6. 调用队列处理：调用 atbmwifi_tx_queue() 进行实际的发送处理
 *
 * 注意事项:
 *   - STA 模式下，如果未关联，直接丢弃数据包
 *   - 如果找不到目标站点，丢弃数据包
 *   - 流控检查可能会阻塞等待队列使能
 *   - AMPDU 聚合可能会将数据包加入聚合队列，不立即发送
 *
 * 相关函数:
 *   - atbmwifi_sta_find(): 查找站点私有数据
 *   - ieee80211_tx_prep_agg(): AMPDU 聚合预处理
 *   - atbmwifi_tx_queue(): 队列处理函数
 *
 **************************************************************************************************************/
atbm_void atbmwifi_tx_start(struct atbm_buff *skb,struct atbmwifi_vif *priv){
	struct wsm_tx *wsm;
	int ret;
	struct atbmwifi_common *hw_priv = priv->hw_priv;
	struct atbmwifi_ieee80211_tx_info *tx_info;
	ATBM_BOOL qos = ATBM_FALSE;
	struct atbmwifi_sta_priv *sta_priv = ATBM_NULL;
	unsigned long lock_flags;
	/* 根据优先级确定 WMM AC 队列号（0-3） */
	unsigned queue = ieee802_1d_to_ac[skb->priority & ATBM_IEEE80211_QOS_CTL_TAG1D_MASK];

	/* 步骤 1: 连接状态检查 */
	/* STA 模式下，如果未关联，直接丢弃数据包 */
	if(atbmwifi_is_sta_mode(priv->iftype)){
		if(!priv->assoc_ok){
			atbm_dev_kfree_skb(skb);
			return; 
		}
	}

	/* 步骤 2: 获取发送信息结构体指针 */
	tx_info = ATBM_IEEE80211_SKB_TXCB(skb);
	
	/* 步骤 3: 站点查找和 QoS 判断 */
	/* 如果不是组播/广播地址，查找目标站点的私有数据 */
	if(!atbm_is_multicast_ether_addr(ATBM_OS_SKB_DATA(skb))){
		/* 查找目标站点的私有数据 */
		sta_priv = atbmwifi_sta_find(priv,ATBM_OS_SKB_DATA(skb));
		if(!sta_priv){
			/* 找不到站点，丢弃数据包 */
			wifi_printk(WIFI_CONNECT,"atbmwifi_sta_find drop tx,%x\n",(unsigned int)skb);
			atbm_dev_kfree_skb(skb);	
			return;
		}
		/* 判断站点是否使用 QoS */
		qos = sta_priv->wmm_used;
	}else{
		/* 组播/广播数据包，标记为组播 */
		tx_info->b_multi = 1;
	}
	/* 标记为网络数据包 */
	tx_info->b_net = 1;

	/* 步骤 4: 设置发送信息 */
	tx_info->sta_priv = sta_priv;
	tx_info->qos = qos;

	/* 步骤 5: 流控检查 */
	/* 检查队列是否使能，如果未使能，等待或丢弃 */
#ifdef LINUX_OS
	if(priv->ndev->lwip_enable && !priv->ndev->lwip_queue_enable){
		atbm_dev_kfree_skb(skb);
		wifi_printk(WIFI_ALWAYS, "tx drop\n");
		return;
	}
#else
	/* 非 Linux 系统：等待队列使能 */
	atbm_spin_lock_irqsave(&hw_priv->tx_queue[queue].lock, &lock_flags);
	while(priv->ndev->lwip_enable && !priv->ndev->lwip_queue_enable){
		atbm_spin_unlock_irqrestore(&hw_priv->tx_queue[queue].lock,lock_flags);
		/* 等待队列使能，超时 50ms */
		if(atbm_os_wait_event_timeout(&priv->ndev->tx_enable, 50) != 1){
			/* 超时，丢弃数据包 */
			atbm_dev_kfree_skb(skb);
			return;
		}
		atbm_spin_lock_irqsave(&hw_priv->tx_queue[queue].lock, &lock_flags);
	}
	atbm_spin_unlock_irqrestore(&hw_priv->tx_queue[queue].lock,lock_flags);
#endif

	/* 步骤 6: AMPDU 聚合处理（可选） */
	/* 如果启用 BA（Block Ack）且不是 EAPOL 帧，进行聚合处理 */
#if ATBM_DRIVER_PROCESS_BA
	if(hw_priv->driver_setup_ba && (!tx_info->b_eapol)){
		int tid = 0;
		struct atbmwifi_ieee80211_hdr * hdr = (struct atbmwifi_ieee80211_hdr *)ATBM_OS_SKB_DATA(skb);
		if (sta_priv && qos){
			struct tid_ampdu_tx *tid_tx;
			atbm_uint8 *qc;

			tid = skb->priority;

			tx_info->tid = tid;
			tid_tx = sta_priv->ampdu_mlme.tid_tx[tid];
			if (tid_tx) {
				ATBM_BOOL queued;
		
				/* 尝试将数据包加入聚合队列 */
				queued = ieee80211_tx_prep_agg(sta_priv, skb, tx_info,
								   tid_tx, tid);

				/* 如果数据包已加入聚合队列，直接返回，不立即发送 */
				if (atbm_unlikely(queued))
					return;
			}
		}
	}
#endif
	/* 步骤 7: 调用队列处理函数进行实际的发送处理 */
	atbmwifi_tx_queue(hw_priv, skb, priv);
}

/**
 * @brief 慢速发送路径处理函数
 *
 * @param skb Socket Buffer 指针，包含要发送的数据包（802.3 格式）
 * @param priv 虚拟接口指针，包含接口的配置和状态信息
 * @param txpriv 发送私有数据指针，包含发送相关的私有信息
 * @return int 0:成功, 其他:失败错误码
 *
 * @note 该函数处理需要完整处理的慢速发送路径，主要完成以下工作：
 *       1. 将 802.3 格式的数据包转换为 802.11 格式
 *       2. 处理 AMPDU 聚合序列号
 *       3. 加密处理
 *       4. 构建 WSM 头部
 *       5. 添加加密头部
 *       6. 速率选择
 *
 * 适用场景:
 * - 第一次发送的数据包
 * - 需要完整处理的数据包
 * - 加密/解密需要处理的数据包
 *
 * @see atbmwifi_tx_fast() - 快速发送路径
 * @see atbmwifi_ieee80211_data_from_8023() - 802.3 转 802.11 格式转换
 * @see atbmwifi_tx_h_crypt() - 加密处理
 * @see atbmwifi_tx_h_wsm() - WSM 头部构建
 */
int atbmwifi_tx_slow(struct atbm_buff *skb,struct atbmwifi_vif *priv, const struct atbmwifi_txpriv *txpriv){
	struct atbmwifi_common * hw_priv = priv->hw_priv;
	struct atbmwifi_ieee80211_tx_info * tx_info = ATBM_IEEE80211_SKB_TXCB(skb);
	struct atbmwifi_sta_priv *sta_priv = tx_info->sta_priv;
	struct atbmwifi_txinfo t;
	atbm_uint8 flags = 0;
	struct wsm_tx *wsm;
	int ret;

	/* 计算数据包对齐偏移 */
	tx_info->align = ((atbm_uint32)ATBM_OS_SKB_DATA(skb)) & 0x3;

	/* 步骤 1: 将 802.3 格式转换为 802.11 格式 */
	ret = atbmwifi_ieee80211_data_from_8023(skb,priv->mac_addr,
			    priv->iftype,priv->bssid, tx_info->qos,priv->connect.encrype);

	if(ret < 0)
	{
		wifi_printk(WIFI_CONNECT,"8023=>80211 err(%d) skblen %d,%x\n",ret,ATBM_OS_SKB_LEN(skb),(unsigned int)skb);
		return ret;
	}

#if ATBM_DRIVER_PROCESS_BA
	/* 步骤 2: 处理 AMPDU 聚合序列号 */
	if(hw_priv->driver_setup_ba){
		if(tx_info->qos){
			struct atbmwifi_ieee80211_hdr *hdr = (struct atbmwifi_ieee80211_hdr *)ATBM_OS_SKB_DATA(skb);
			atbm_uint16 *seq = &sta_priv->tid_seq[skb->priority];

			tx_info->flags |= ATBM_IEEE80211_TX_CTL_NOT_ASSIGN_SEQ;
			hdr->seq_ctrl = atbm_cpu_to_le16(*seq);
			/* 增加序列号 */
			*seq = (*seq + 0x10) & ATBM_IEEE80211_SCTL_SEQ;
		}
	}
#endif

	/* 初始化发送信息结构体 */
	t.tx_info = tx_info;
	t.sta_priv = sta_priv;
	t.skb = skb;
	t.hdr = (struct atbmwifi_ieee80211_hdr *)ATBM_OS_SKB_DATA(skb);
	t.hdrlen = atbmwifi_ieee80211_hdrlen(t.hdr->frame_control);
	t.txpriv = *txpriv;
	t.queue = ieee802_1d_to_ac[skb->priority & ATBM_IEEE80211_QOS_CTL_TAG1D_MASK];
	tx_info->hdrlen = (atbm_uint8)t.hdrlen;

	/* 组播包使用最小速率 */
	if(tx_info->b_multi){
		tx_info->flags |= ATBM_IEEE80211_TX_CTL_USE_MINRATE;
	}

	/* 步骤 3: 加密处理 */
	ret = atbmwifi_tx_h_crypt(priv, &t, &flags);
	if (ret){
		ret = -7;
		wifi_printk(WIFI_DBG_MSG, "crypt err\n");
		goto drop;
	}

	/* AP 模式下设置源 MAC 地址 */
	if(atbmwifi_is_ap_mode(priv->iftype)){
		tx_info->smac = ((struct atbmwifi_ieee80211_hdr *)ATBM_OS_SKB_DATA(skb))->addr3;
	}

	/* 步骤 4: 构建 WSM 头部 */
	wsm = atbmwifi_tx_h_wsm(priv, &t);
	if (!wsm) {
		ret = -8;
		goto drop;
	}
	wsm->flags |= flags;

	/* 步骤 5: 添加加密头部 */
	wsm = atbmwifi_tx_h_enchdr(wsm, &t);
	if (!wsm) {
		ret = -9;
		goto drop;
	}

	/* 步骤 6: 速率选择 */
	ret = atbmwifi_tx_h_rate_build(priv, &t, wsm);
	if (ret){
		ret = -11;
		goto drop;
	}
drop:
	return ret;
}

/**
 * @brief 快速发送路径处理函数
 *
 * @param skb Socket Buffer 指针，包含要发送的数据包（已处理过）
 * @param prev_skb 前一个 Socket Buffer 指针，作为模板
 * @param priv 虚拟接口指针，包含接口的配置和状态信息
 * @return int 0:成功
 *
 * @note 该函数处理快速发送路径，通过复制前一个数据包的 WSM 头部来加速处理，主要完成以下工作：
 *       1. 复制前一个数据包的 WSM 头部和 802.11 头部
 *       2. 处理 AMPDU 聚合序列号
 *       3. 调整 WSM 头部参数
 *       4. 更新数据包长度
 *
 * 适用场景:
 * - AMPDU 聚合中的后续数据包
 * - 不需要重新加密的数据包
 * - 格式相同的数据包序列
 *
 * 性能优势:
 * - 避免重复的 802.3 到 802.11 转换
 * - 避免重复的加密处理
 * - 避免重复的 WSM 头部构建
 *
 * @see atbmwifi_tx_slow() - 慢速发送路径
 * @see atbm_skb_push() - 扩展 skb 头部空间
 * @see atbm_skb_put() - 扩展 skb 尾部空间
 */
int atbmwifi_tx_fast(struct atbm_buff *skb, struct atbm_buff *prev_skb, struct atbmwifi_vif *priv){
	struct atbmwifi_common * hw_priv = priv->hw_priv;
	struct atbmwifi_ieee80211_tx_info *prev_tx_info = ATBM_IEEE80211_SKB_TXCB(prev_skb);
	/* 计算需要复制的头部长度: WSM 头 + 802.11 头 + IV + 6字节额外空间 */
	int copy_len = sizeof(struct wsm_tx) + prev_tx_info->hdrlen + prev_tx_info->ivlen + 6;
	/* 为 skb 预留头部空间，并获取 WSM 头部指针 */
	struct wsm_tx *wsm = (struct wsm_tx *)atbm_skb_push(skb, copy_len - 12);
	struct atbmwifi_sta_priv *sta_priv = prev_tx_info->sta_priv;
	int i;

	/* 确保 WSM 头部 4 字节对齐 */
	ATBM_BUG_ON(((atbm_uint32)wsm) & 0x3);

	/* 步骤 1: 复制前一个数据包的头部信息（按 4 字节复制） */
	for(i = 0; i < copy_len; i += 4){
		*((atbm_uint32*)((atbm_uint8*)wsm + i)) = *((atbm_uint32*)(ATBM_OS_SKB_DATA(prev_skb) + i));
	}

	/* 处理剩余字节（如果有） */
	if(copy_len & 0x3){
		*((atbm_uint16*)((atbm_uint8*)wsm + copy_len - 2)) = *((atbm_uint16*)(ATBM_OS_SKB_DATA(prev_skb) + copy_len - 2));
		wifi_printk(WIFI_DBG_ERROR, "copy len:%d\n", copy_len);
	}

#if ATBM_DRIVER_PROCESS_BA
	/* 步骤 2: 处理 AMPDU 聚合序列号 */
	if(hw_priv->driver_setup_ba){
		if(prev_tx_info->qos){
			struct atbmwifi_ieee80211_hdr *hdr = (struct atbmwifi_ieee80211_hdr *)(wsm +  1);
			atbm_uint16 *seq = &sta_priv->tid_seq[prev_tx_info->tid];

			hdr->seq_ctrl = atbm_cpu_to_le16(*seq);

			/* 增加序列号 */
			*seq = (*seq + 0x10) & ATBM_IEEE80211_SCTL_SEQ;
		}
	}
#endif

	/* 步骤 3: 调整 WSM 头部参数 */
	/* 清除需要确认的标志，聚合包通常不需要每个包都确认 */
	wsm->htTxParameters &= ~atbm_cpu_to_le32(WSM_HT_TX_NEED_CONFIRM|WSM_NEED_TX_RATE_CONFIRM);

	/* 步骤 4: 更新数据包长度 */
	/* 添加 ICV 长度 */
	atbm_skb_put(skb, prev_tx_info->icvlen);
	/* 更新 WSM 头部中的长度字段 */
	wsm->hdr.u.common.total_len = wsm->hdr.len = ATBM_OS_SKB_LEN(skb);

	/* 根据芯片类型调整标志 */
	if(hw_priv->chip_priv->hdr_rollback){
		wsm->hdr.u.common.flag = wsm->hdr.enc_hdr.flag;
	}
	/* 更新标志中的长度信息 */
	wsm->hdr.u.common.flag += ATBM_OS_SKB_LEN(skb);
	wsm->hdr.u.common.flag -= ATBM_OS_SKB_LEN(prev_skb);

	return 0;
}
#else
 atbm_void atbmwifi_tx_start(struct atbm_buff *skb,struct atbmwifi_vif *priv)
{
	struct atbmwifi_common * hw_priv = priv->hw_priv;
	struct atbmwifi_ieee80211_tx_info * tx_info;
	struct atbmwifi_sta_priv *sta_priv = ATBM_NULL;
	int res;
	unsigned long lock_flags;
	ATBM_BOOL qos = ATBM_FALSE;
	unsigned queue = ieee802_1d_to_ac[skb->priority & ATBM_IEEE80211_QOS_CTL_TAG1D_MASK];

	if(atbmwifi_is_sta_mode(priv->iftype)){
		if(!priv->assoc_ok){
			atbm_dev_kfree_skb(skb);			
			wifi_printk(WIFI_CONNECT,"atbmwifi_tx_start not connect_ok \n");
			return;
		}
	}

	tx_info = ATBM_IEEE80211_SKB_TXCB(skb);
	if(!atbm_is_multicast_ether_addr(ATBM_OS_SKB_DATA(skb))){
		sta_priv = atbmwifi_sta_find(priv,ATBM_OS_SKB_DATA(skb));
		if(!sta_priv){
			wifi_printk(WIFI_CONNECT,"atbmwifi_sta_find drop tx,%x\n",(unsigned int)skb);
			atbm_dev_kfree_skb(skb);	
			return;
		}
		qos = sta_priv->wmm_used;
	}else{
		tx_info->b_multi = 1;
	}
	tx_info->b_net = 1;

	res = atbmwifi_ieee80211_data_from_8023(skb,priv->mac_addr,
			    priv->iftype,priv->bssid, qos,priv->connect.encrype);
	if(res<0)
	{
		wifi_printk(WIFI_CONNECT,"8023=>80211 err(%d) skblen %d,%x\n",res,ATBM_OS_SKB_LEN(skb),(unsigned int)skb);
		atbm_dev_kfree_skb(skb);
		return;
	}

#ifdef OS_LINUX
	if(priv->ndev->lwip_enable && !priv->ndev->lwip_queue_enable){
		atbm_dev_kfree_skb(skb);
		wifi_printk(WIFI_ALWAYS, "tx drop\n");
		return;
	}
#else
	atbm_spin_lock_irqsave(&hw_priv->tx_queue[queue].lock, &lock_flags);
	while(priv->ndev->lwip_enable && !priv->ndev->lwip_queue_enable){
		atbm_spin_unlock_irqrestore(&hw_priv->tx_queue[queue].lock,lock_flags);
		if(atbm_os_wait_event_timeout(&priv->ndev->tx_enable, 50) != 1){
			atbm_dev_kfree_skb(skb);
			return;
		}
		atbm_spin_lock_irqsave(&hw_priv->tx_queue[queue].lock, &lock_flags);
	}
	atbm_spin_unlock_irqrestore(&hw_priv->tx_queue[queue].lock,lock_flags);
#endif

#if ATBM_DRIVER_PROCESS_BA
	if(hw_priv->driver_setup_ba){
		int tid = 0;
		struct atbmwifi_ieee80211_hdr * hdr = (struct atbmwifi_ieee80211_hdr *)ATBM_OS_SKB_DATA(skb);
		if (sta_priv && qos){
			struct tid_ampdu_tx *tid_tx;
			atbm_uint8 *qc;
			
			qc = atbmwifi_ieee80211_get_qos_ctl(hdr);
			tid = *qc & ATBM_IEEE80211_QOS_CTL_TID_MASK;
		
			tid_tx = sta_priv->ampdu_mlme.tid_tx[tid];
			if (tid_tx) {
				ATBM_BOOL queued;
		
				queued = ieee80211_tx_prep_agg(sta_priv, skb, tx_info,
								   tid_tx, tid);

				if (atbm_unlikely(queued))
					return;

			}
		}
		ieee80211_tx_h_sequence(sta_priv, skb, tid);
	}
#endif
	atbmwifi_tx(hw_priv,skb,priv);
	
	return;
}
#endif

