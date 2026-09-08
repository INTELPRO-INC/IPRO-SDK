/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/
#ifndef  ATBMWIFI__SDIO_H_INCLUDED
#define  ATBMWIFI__SDIO_H_INCLUDED
 /* DPLL initial values */
#define DPLL_INIT_VAL_9000		(0x00000191)
#define DPLL_INIT_VAL_CW1200		(0x0EC4F121)
#define ALTOBEAM_WIFI_HDR_FLAG  (0x34353677)

#define DOWNLOAD_ITCM_ADDR		(0x00060000)

#if CONFIG_WIFI_BT_COMB
#define DOWNLOAD_BLE_SRAM_ADDR		(0x09018000)
#define BLE_SRAM_CODE_SIZE			0x10000
#else//#ifdef CONFIG_WIFI_BT_COMB
#define DOWNLOAD_BLE_SRAM_ADDR		(0x0900A000)
#define BLE_SRAM_CODE_SIZE			0x16000
#endif  //#ifdef CONFIG_WIFI_BT_COMB

#define DOWNLOAD_ITCM_SIZE		(160*1024)
#define DOWNLOAD_DTCM_ADDR		(0x00800000)
#define DOWNLOAD_DTCM_SIZE		(48*1024)
#define DOWNLOAD_BLOCK_SIZE		(256)
#define ATBM_ALIGN_SIZE  		64
#define SYS_BASE_ADDR_SILICON		(0)
#define PAC_BASE_ADDRESS_SILICON	(SYS_BASE_ADDR_SILICON + 0x09000000)
#define PAC_SHARED_MEMORY_SILICON	(PAC_BASE_ADDRESS_SILICON)
#define CW12000_APB(addr)		(PAC_SHARED_MEMORY_SILICON + (addr)) 
 /* ***************************************************************
 *Device register definitions
 *************************************************************** */
 /* WBF - SPI Register Addresses */
#define ATBM_HIFREG_ADDR_ID_BASE		(0x0000)
	 /* 16/32 bits */
#define ATBM_HIFREG_CONFIG_REG_ID		(0x0000)
	 /* 16/32 bits */
#define ATBM_HIFREG_CONTROL_REG_ID		(0x0001)
	 /* 16 bits, Q mode W/R */
#define ATBM_HIFREG_IN_OUT_QUEUE_REG_ID	(0x0002)
	 /* 32 bits, AHB bus R/W */
#define ATBM_HIFREG_AHB_DPORT_REG_ID	(0x0003)
	 /* 16/32 bits */
#define ATBM_HIFREG_SRAM_BASE_ADDR_REG_ID   (0x0004)
	 /* 32 bits, APB bus R/W */
#define ATBM_HIFREG_SRAM_DPORT_REG_ID	(0x0005)
	 /* 32 bits, t_settle/general */
#define ATBM_HIFREG_TSET_GEN_R_W_REG_ID	(0x0006)
	 /* 16 bits, Q mode read, no length */
#define ATBM_HIFREG_FRAME_OUT_REG_ID	(0x0007)
	 /*32 bits 15:0 avilable buffer size; 31:16 avilable channel num*/
#define ATBM_6X65S_SDIO_FN1_TX_TOT_LEN	(0x0008)
	 /*32 bits 15:0 rx length*/
#define ATBM_6X65S_SDIO_FN1_RX_TOT_LEN	(0x0009)
	 /*32 bits 15:0 avilable buffer size; 31:16 avilable channel num*/
#define ATBM_6X65S_SDIO_FN2_TX_TOT_LEN	(0x000a)
	 /*32 bits 15:0 rx length*/
#define ATBM_6X65S_SDIO_FN2_RX_TOT_LEN	(0x000b)
	 /*32 bits 15:0 avilable buffer size; 31:16 avilable channel num*/
#define ATBM_6X65S_SDIO_FN3_TX_TOT_LEN	(0x000c)
	 /*32 bits 15:0 rx length*/
#define ATBM_6X65S_SDIO_FN3_RX_TOT_LEN	(0x000d)
	 /*32 bits base address of mem for SDIO fn4 access*/
#define ATBM_6X65S_SDIO_FN4_MEM_BASE_0		(0x000e)
	 /*32 bits base address of mem for SDIO fn4 access*/
#define ATBM_6X65S_SDIO_FN4_MEM_BASE_1		(0x000f)
	 /*32 bits base address of mem for SDIO fn5 access*/
#define ATBM_6X65S_SDIO_FN5_MEM_BASE_2		(0x0010)
	 /*32 bits base address of mem for SDIO fn5 access*/
#define ATBM_6X65S_SDIO_FN5_MEM_BASE_3		(0x0011)
	 /*32 bit reg used for reset chip*/
#define ATBM_6X65S_SDIO_FN1_RESET_REG       (0x0014)

#define ATBM_HIFREG_ADDR_ID_MAX		(ATBM_6X65S_SDIO_FN1_RESET_REG)

	 /* WBF - Control register bit set */
	 /* next o/p length, bit 11 to 0 */
#define ATBM_HIFREG_CONT_NEXT_LEN_MASK	(0xCFFF)
#define ATBM_HIFREG_CONT_NEXT_LEN_LSB_MASK	(0x0FFF)
#define ATBM_HIFREG_CONT_NEXT_LEN_MSB_MASK	(0xC000)
#define ATBM_HIFREG_CONT_WUP_BIT		(ATBM_BIT(12))
#define ATBM_HIFREG_CONT_RDY_BIT		(ATBM_BIT(13))
#define ATBM_HIFREG_CONT_IRQ_ENABLE		(ATBM_BIT(14))
#define ATBM_HIFREG_CONT_RDY_ENABLE		(ATBM_BIT(15))
#define ATBM_HIFREG_CONT_IRQ_RDY_ENABLE	(ATBM_BIT(14)|ATBM_BIT(15)) 
#define ATBM_HIFREG_PS_SYNC_SDIO_FLAG	(ATBM_BIT(23))
#define ATBM_HIFREG_PS_SYNC_SDIO_CLEAN	(ATBM_BIT(24))
/* SPI Config register bit set */
#define ATBM_HIFREG_CONFIG_FRAME_BIT	(ATBM_BIT(2))
#define ATBM_HIFREG_CONFIG_WORD_MODE_BITS	(ATBM_BIT(3)|ATBM_BIT(4))
#define ATBM_HIFREG_CONFIG_WORD_MODE_1	(ATBM_BIT(3))
#define ATBM_HIFREG_CONFIG_WORD_MODE_2	(ATBM_BIT(4))
#define ATBM_HIFREG_CONFIG_ERROR_0_BIT	(ATBM_BIT(5))
#define ATBM_HIFREG_CONFIG_ERROR_1_BIT	(ATBM_BIT(6))
#define ATBM_HIFREG_CONFIG_ERROR_2_BIT	(ATBM_BIT(7))
/* TBD: Sure??? */
#define ATBM_HIFREG_CONFIG_CSN_FRAME_BIT	(ATBM_BIT(7))
#define ATBM_HIFREG_CONFIG_ERROR_3_BIT	(ATBM_BIT(8))
#define ATBM_HIFREG_CONFIG_ERROR_4_BIT	(ATBM_BIT(9))
/* QueueM */
#define ATBM_HIFREG_CONFIG_ACCESS_MODE_BIT	(ATBM_BIT(10))
/* AHB bus */
#define ATBM_HIFREG_CONFIG_AHB_PFETCH_BIT	(ATBM_BIT(11))
#define ATBM_HIFREG_CONFIG_CPU_CLK_DIS_BIT	(ATBM_BIT(12))
/* APB bus */
#define ATBM_HIFREG_CONFIG_PFETCH_BIT	(ATBM_BIT(13))
/* cpu reset */
#define ATBM_HIFREG_CONFIG_CPU_RESET_BIT	(ATBM_BIT(14))
#define ATBM_HIFREG_CONFIG_CLEAR_INT_BIT	(ATBM_BIT(15))
/* For CW1200 the IRQ Enable and Ready Bits are in CONFIG register */
#define ATBM_HIFREG_CONF_IRQ_RDY_ENABLE	(ATBM_BIT(16)|ATBM_BIT(17))
#define ATBM_HIFREG_CONF_RDY_IRQ_ENABLE	(ATBM_BIT(17))
#define ATBM_HIFREG_CONF_DATA_IRQ_ENABLE	(ATBM_BIT(16))	 
#define ATBM_HIFREG_CONFIG_CPU_RESET_BIT_2	(ATBM_BIT(22))

#define ATBM_SDIO_FN1_RESET_WIFI_CPU             (ATBM_BIT(4))
#define ATBM_SDIO_FN1_RESET_MMU_SYNC             (ATBM_BIT(11))
#define ATBM_SDIO_FN1_RESET_WIFI_CHANN           (ATBM_BIT(5))
#define ATBM_SDIO_FN1_RESET_WIFI_MAC_PHY         (ATBM_BIT(6))
#define ATBM_SDIO_FN1_RESET_CIPHER               (ATBM_BIT(7))
#define ATBM_SDIO_FN1_RESET_WIIF_CMD_CHANN       (ATBM_BIT(10))
#define ATBM_SDIO_CHAN_NUM_CONFIG                (0xa900010)
#define ATBM_HW_RESET_WIFI_RECOERY \
			(ATBM_SDIO_FN1_RESET_WIFI_CHANN \
			|ATBM_SDIO_FN1_RESET_WIFI_MAC_PHY \
			|ATBM_SDIO_FN1_RESET_CIPHER \
            |ATBM_SDIO_FN1_RESET_WIIF_CMD_CHANN)


#define WRITE_32K_ADDR_MSK	(0xfffff000)
#define WRITE_32K_ADDR	(0x16101000)

extern struct sbus_ops atbm_sdio_sbus_ops;
#define ATBM_SDIO_CCCR_IENx		0x04
#define ATBM_SDIO_CCCR_IOEx  0x02
#define ATBM_SDIO_CCCR_IORx  0x03
#define ATBM_SDIO_CCCR_ABORT		0x06	/* function abort/card reset */
#define ATBM_SDIO_CCCR_IF		0x07	/* bus interface controls */
#define ATBM_SDIO_CCCR_SPEED		0x13

#define  ATBM_SDIO_SPEED_SHS		0x01	/* Supports High-Speed mode */

#define  ATBM_SDIO_SPEED_BSS_SHIFT	1
#define  ATBM_SDIO_SPEED_SDR25	(1<<ATBM_SDIO_SPEED_BSS_SHIFT)
#define  ATBM_SDIO_SPEED_EHS		ATBM_SDIO_SPEED_SDR25	/* Enable High-Speed */
#define  ATBM_SDIO_BUS_WIDTH_4BIT	0x02
#define ATBM_SDIO_FBR_BLKSIZE   0x10

#define ATBM_SDIO_CCCR_POWER		0x12

#define  SDIO_POWER_SMPC	0x01	/* Supports Master Power Control */
#define  SDIO_POWER_EMPC	0x02	/* Enable Master Power Control */


#define ATBM_SDIO_CCCR_CAPS 0x08

#define  SDIO_CCCR_CAP_SDC	0x01	/* can do CMD52 while data transfer */
#define  SDIO_CCCR_CAP_SMB	0x02	/* can do multi-block xfers (CMD53) */
#define  SDIO_CCCR_CAP_SRW	0x04	/* supports read-wait protocol */
#define  SDIO_CCCR_CAP_SBS	0x08	/* supports suspend/resume */
#define  SDIO_CCCR_CAP_S4MI	0x10	/* interrupt during 4-bit CMD53 */
#define  SDIO_CCCR_CAP_E4MI	0x20	/* enable ints during 4-bit CMD53 */
#define  SDIO_CCCR_CAP_LSC	0x40	/* low speed card */
#define  SDIO_CCCR_CAP_4BLS	0x80	/* 4 bit low speed card */

	
#define ATBM_SDIO_CCCR_CCCR  0x00
#define  SDIO_CCCR_REV_1_00	0	/* CCCR/FBR Version 1.00 */
#define  SDIO_CCCR_REV_1_10	1	/* CCCR/FBR Version 1.10 */
#define  SDIO_CCCR_REV_1_20	2	/* CCCR/FBR Version 1.20 */
	
#define  SDIO_SDIO_REV_1_00	0	/* SDIO Spec Version 1.00 */
#define  SDIO_SDIO_REV_1_10	1	/* SDIO Spec Version 1.10 */
#define  SDIO_SDIO_REV_1_20	2	/* SDIO Spec Version 1.20 */
#define  SDIO_SDIO_REV_2_00	3	/* SDIO Spec Version 2.00 */
#define ATBM_SDIO_CARE_NUM  (1)

#define ATBM_SDIO_BLOCK_SIZE 256
#define DOWNLOAD_BLOCK_SIZE_WR	(0x2000 - 4)

/**************************************************************************************************************
 * TX 聚合相关宏定义说明（SDIO 接口）
 * 
 * 这些宏定义了 SDIO 接口的 TX 聚合配置，通过 CONFIG_INF_AGGR_TX 宏控制是否启用聚合缓冲区机制。
 * 
 * 宏关系说明：
 *   1. PER_PACKET_LEN: 每个数据包的对齐长度（SDIO 块大小）
 *   2. TX_DESC_NUM: TX 描述符数量（描述符池大小）
 *   3. INF_BUF_SG_NUM: 聚合缓冲区中最多可聚合的数据包数量（直接影响聚合上限）
 *   4. TX_BUFFER_SIZE: 单个 TX 缓冲区大小
 *   5. BUFF_ALLOC_LEN: 每个描述符分配的聚合缓冲区总长度（TX_BUFFER_SIZE * INF_BUF_SG_NUM）
 * 
 * 聚合限制：
 *   - 实际聚合数量受两个因素限制：
 *     a) 驱动层限制：INF_BUF_SG_NUM（数组大小限制）
 *     b) 固件层限制：hw_priv->wsm_caps.numInpChBufs - 2（固件缓冲区限制）
 *   - 最终聚合数量 = MIN(INF_BUF_SG_NUM, numInpChBufs - 2)
 * 
 * 内存占用：
 *   - 启用聚合时：每个描述符分配 BUFF_ALLOC_LEN 字节（16KB = 2048 * 8）
 *   - 总内存 = TX_DESC_NUM * BUFF_ALLOC_LEN = 4 * 16KB = 64KB
 * 
 * 参考文档: doc/TX聚合宏定义关系说明.md, doc/TX发包流程文档.md
 **************************************************************************************************************/
#if CONFIG_INF_AGGR_TX
/**
 * @brief 每个数据包的对齐长度（字节）
 * 
 * 定义：等于 SDIO 块大小（256 字节）
 * 用途：用于数据包对齐，确保每个数据包在聚合缓冲区中按此大小对齐
 * 影响：影响聚合缓冲区的内存布局和计算
 * 
 * 注意：此宏定义了聚合缓冲区中每个数据包的对齐大小，实际数据包长度可能小于此值
 */
#define PER_PACKET_LEN ATBM_SDIO_BLOCK_SIZE

/**
 * @brief TX 描述符数量（描述符池大小）
 * 
 * 定义：TX 描述符池的大小（4 个）
 * 用途：限制同时可以有多少个 TX 描述符在使用中（并发发送能力）
 * 
 * 影响：
 *   - ✅ 影响并发发送能力：同时可以有多少个数据包在发送中
 *   - ✅ 影响内存占用：总内存 = TX_DESC_NUM * BUFF_ALLOC_LEN = 4 * 16KB = 64KB
 *   - ❌ **不影响** TX 聚合个数（聚合个数由 INF_BUF_SG_NUM 决定）
 * 
 * 工作机制：
 *   - 每次发送数据包时，从描述符池获取一个描述符（ATBM_TX_DESC_GET）
 *   - 如果描述符池已满（desc_id < 0），需要等待描述符释放
 *   - 发送完成后，描述符返回到池中（ATBM_TX_DESC_PUT）
 * 
 * 与聚合的关系：
 *   - TX_DESC_NUM：控制**并发**（同时有多少个描述符在使用）
 *   - INF_BUF_SG_NUM：控制**聚合**（单个描述符中可以聚合多少个数据包）
 *   - 例如：TX_DESC_NUM=4, INF_BUF_SG_NUM=8
 *     → 最多可以同时有 4 个描述符在发送
 *     → 每个描述符最多可以聚合 8 个数据包
 *     → 理论上最多可以同时发送 4*8=32 个数据包（如果都聚合满）
 * 
 * 注意：
 *   - 描述符数量较少时，可能影响并发性能，但内存占用更小
 *   - 如果经常出现 desc_id < 0（描述符池满），考虑增加 TX_DESC_NUM
 */
#define TX_DESC_NUM 4

/**
 * @brief RX 描述符数量（描述符池大小）
 * 
 * 定义：RX 描述符池的大小（4 个）
 * 用途：限制同时可以有多少个 RX 描述符在使用中（并发接收能力）
 * 
 * 影响：
 *   - ✅ 影响并发接收能力：同时可以有多少个数据包在接收中
 *   - ❌ 与 TX 聚合无关（这是接收相关的）
 * 
 * 注意：如果接收频繁出现描述符不足，考虑增加 RX_DESC_NUM
 */
#define RX_DESC_NUM 4

/**
 * @brief 数据包对齐大小（V0 版本）
 * 
 * 定义：等于 PER_PACKET_LEN（256 字节）
 * 用途：用于 V0 版本的 WSM 头部构建，确保数据包对齐
 */
#define ATBM_PACKET_ALIGN_SIZE_V0 PER_PACKET_LEN

/**
 * @brief 聚合缓冲区中的最大数据包数量（直接影响聚合上限）
 * 
 * 定义：8 个数据包
 * 用途：
 *   - 定义 tx_desc->data[] 数组的大小
 *   - 限制单个描述符中最多可以聚合多少个数据包
 *   - 这是驱动层的聚合限制
 * 
 * 影响：
 *   - 直接影响 TX 聚合的数量上限
 *   - 必须与固件层限制（numInpChBufs-2）取最小值
 *   - 数组越界保护：确保 frame_cnt < INF_BUF_SG_NUM
 * 
 * 注意：这是影响发包个数和聚合的关键宏之一
 */
#define INF_BUF_SG_NUM 8

/**
 * @brief 单个 TX 缓冲区大小（字节）
 * 
 * 定义：2048 字节（2KB）
 * 用途：定义聚合缓冲区中单个数据包的最大大小
 * 影响：影响 BUFF_ALLOC_LEN 的计算
 * 
 * 注意：实际数据包可能小于此值，但聚合缓冲区按此大小分配
 */
#define TX_BUFFER_SIZE 2048

/**
 * @brief 每个描述符分配的聚合缓冲区总长度（字节）
 * 
 * 定义：TX_BUFFER_SIZE * INF_BUF_SG_NUM = 2048 * 8 = 16384 字节（16KB）
 * 用途：定义每个 TX 描述符预分配的聚合缓冲区大小
 * 影响：
 *   - 总内存占用 = TX_DESC_NUM * BUFF_ALLOC_LEN = 4 * 16KB = 64KB
 *   - 影响聚合缓冲区的容量
 * 
 * 注意：这是启用聚合缓冲区时的内存开销
 */
#define BUFF_ALLOC_LEN (TX_BUFFER_SIZE*INF_BUF_SG_NUM)

#else //CONFIG_INF_AGGR_TX
/**
 * @brief RX 描述符数量（非聚合模式）
 * 
 * 定义：8 个
 * 要求：必须是 2 的幂次方（2^N）
 */
#define RX_DESC_NUM 8

/**
 * @brief TX 描述符数量（非聚合模式）
 * 
 * 定义：8 个
 * 要求：必须是 2 的幂次方（2^N）
 */
#define TX_DESC_NUM 8

/**
 * @brief TX 缓冲区大小（非聚合模式，字节）
 * 
 * 定义：1680 字节
 * 用途：定义单个 TX 数据包的最大大小（不使用聚合缓冲区）
 */
#define TX_BUFFER_SIZE (1680)

/**
 * @brief 数据包对齐大小（V0 版本，非聚合模式）
 * 
 * 定义：0（不对齐）
 * 用途：非聚合模式下，数据包不需要对齐
 */
#define ATBM_PACKET_ALIGN_SIZE_V0 0
#endif //CONFIG_INF_AGGR_TX


/**
 * @brief 数据包对齐大小（V1 版本）
 * 
 * 定义：4 字节对齐
 * 用途：用于 V1 版本的 WSM 头部构建，确保数据包按 4 字节对齐
 * 
 * 注意：V0 和 V1 版本使用不同的对齐方式，取决于接口类型和配置
 */
#define ATBM_PACKET_ALIGN_SIZE_V1 4

/* an SPI message cannot be bigger than (2"12-1)*2 bytes
 * "*2" to cvt to bytes */
#define PIGGYBACK_CTRL_REG	(2)
#define EFFECTIVE_BUF_SIZE	(16*1024)
#define RX_BUFFER_SIZE (16*1024) //must biger than amsdu
#define WAKE_UP_HOST_REQ_FLAG ATBM_BIT(15)

/**
 * @brief 聚合缓冲区中的最大数据包数量（非聚合模式下的定义）
 * 
 * 定义：8 个数据包
 * 用途：定义 sdio_inf_desc 结构体中 buf[] 和 len[] 数组的大小
 * 
 * 注意：
 *   - 此宏在非聚合模式下也定义，用于 sdio_inf_desc 结构体的数组大小
 *   - 在非聚合模式下，此宏不影响实际的聚合数量（因为不使用聚合缓冲区）
 *   - 但在循环中仍可使用此宏作为循环上限（如 for(i = 0; i < INF_BUF_SG_NUM; i++)）
 */
#define INF_BUF_SG_NUM 8

#define ATBM_HDR_ROLLBACK ATBM_FALSE
#define SET_USB_EP_NUM(hw_priv)
#define DOWNLOAD_BLOCK_SIZE_V0 508
#define DOWNLOAD_BLOCK_SIZE_V1 ATBM_SDIO_BLOCK_SIZE //must equal


#define ATBM_SDIO_WAKEUP_REASON_RX_PROGRESS 0
#define ATBM_SDIO_WAKEUP_REASON_TX_PROGRESS 1
#define ATBM_SDIO_WAKEUP_REASON_ETF_TEST	2

typedef void (*sdio_complete_t)(void *);

struct sdio_inf_desc{
	int len[INF_BUF_SG_NUM];
	void *buf[INF_BUF_SG_NUM];
	void *data;
	void *context;
	int actual_length;
	int status;
	sdio_complete_t complete;
};

typedef struct sdio_inf_desc ATBM_INF_DESC;
struct dvobj_priv{
	atbm_uint32 tx_desc_num_get;
	atbm_uint32 tx_desc_num_put;
	atbm_uint32 rx_desc_num_get;
	atbm_uint32 rx_desc_num_put;
	int suspend_skb_len;
};

static inline int atbm_sdio_desc_get(atbm_spinlock_t	*lock, atbm_uint32 *desc_get, int desc_put, int total){
	int id = 0;

	atbm_spin_lock(lock);
	if(*desc_get == desc_put){
		atbm_spin_unlock(lock);
		return -1;
	}
	id = *desc_get%total;
	*desc_get = *desc_get + 1;
	atbm_spin_unlock(lock);
	return id;
}

static inline void atbm_sdio_desc_put(atbm_spinlock_t	*lock, atbm_uint32 *desc_put){
	atbm_spin_lock(lock);
	*desc_put = *desc_put + 1;
	atbm_spin_unlock(lock);
}

static inline void atbm_sdio_desc_restore(atbm_spinlock_t	*lock, atbm_uint32 *desc_get){
	atbm_spin_lock(lock);
	*desc_get = *desc_get - 1;
	atbm_spin_unlock(lock);
}


#define ATBM_SDIO_DESC_FILL(id,desc,buff_t,len_t,complete_t,context_t) do { \
			desc->buf[id] = buff_t; \
			desc->len[id] = len_t; \
			desc->context = context_t; \
			desc->complete = (sdio_complete_t)complete_t; \
		}while(0);


#define ATBM_RX_DESC_GET(self,drvobj,num) atbm_sdio_desc_get(&self->lock,&drvobj->rx_desc_num_get,drvobj->rx_desc_num_put,num)
#define ATBM_RX_DESC_PUT(self,drvobj,num) atbm_sdio_desc_put(&self->lock,&drvobj->rx_desc_num_put)
#define ATBM_RX_DESC_RESTORE(self,drvobj,num) atbm_sdio_desc_restore(&self->lock,&drvobj->rx_desc_num_get)

/**
 * @brief 获取 TX 描述符
 * 
 * 功能：从 TX 描述符池中获取一个可用的描述符
 * 参数：
 *   @self: 总线私有数据指针
 *   @drvobj: 驱动对象指针
 *   @num: 描述符池总数量（TX_DESC_NUM）
 * 返回值：描述符 ID（>=0）或 -1（池已满）
 * 
 * 注意：使用自旋锁保护，确保并发安全
 */
#define ATBM_TX_DESC_GET(self,drvobj,num) atbm_sdio_desc_get(&self->lock,&drvobj->tx_desc_num_get,drvobj->tx_desc_num_put,num)

/**
 * @brief 释放 TX 描述符
 * 
 * 功能：将 TX 描述符返回到描述符池中
 * 参数：
 *   @self: 总线私有数据指针
 *   @drvobj: 驱动对象指针
 *   @num: 描述符池总数量（未使用，保持接口一致性）
 * 
 * 注意：使用自旋锁保护，确保并发安全
 */
#define ATBM_TX_DESC_PUT(self,drvobj,num) atbm_sdio_desc_put(&self->lock,&drvobj->tx_desc_num_put)

/**
 * @brief 恢复 TX 描述符（回退操作）
 * 
 * 功能：当获取描述符后发生错误时，将描述符 ID 回退
 * 参数：
 *   @self: 总线私有数据指针
 *   @drvobj: 驱动对象指针
 *   @num: 描述符池总数量（未使用，保持接口一致性）
 * 
 * 使用场景：获取描述符后，如果初始化失败，需要恢复描述符 ID
 * 
 * 注意：使用自旋锁保护，确保并发安全
 */
#define ATBM_TX_DESC_RESTORE(self,drvobj,num) atbm_sdio_desc_restore(&self->lock,&drvobj->tx_desc_num_get)

#define ATBM_RX_ALLOC_SKB_EARLY(skb,len)
#define ATBM_RX_DESC_FILL(desc,skb,len,drvobj,complete,context) ATBM_SDIO_DESC_FILL(0,desc,ATBM_NULL,len,complete,context)

/**
 * @brief 填充 TX 描述符
 * 
 * 功能：填充 TX 描述符的缓冲区、长度、完成回调等信息
 * 参数：
 *   @id: 描述符中的缓冲区索引（0 到 INF_BUF_SG_NUM-1）
 *   @desc: 描述符指针
 *   @buff: 数据缓冲区指针
 *   @len: 数据长度
 *   @drvobj: 驱动对象指针（未使用，保持接口一致性）
 *   @complete: 完成回调函数
 *   @context: 上下文指针
 * 
 * 注意：
 *   - 在聚合模式下（CONFIG_INF_AGGR_TX），id=0 表示统一填充整个聚合缓冲区
 *   - 在非聚合模式下，id 表示第几个数据包（0 到 INF_BUF_SG_NUM-1）
 *   - 此宏最终调用 ATBM_SDIO_DESC_FILL 来填充 sdio_inf_desc 结构体
 */
#define ATBM_TX_DESC_FILL(id,desc,buff,len,drvobj,complete,context) ATBM_SDIO_DESC_FILL(id,desc,buff,len,complete,context)

/**
 * @brief 提交 TX 描述符到硬件
 * 
 * 功能：将填充好的 TX 描述符提交到 SDIO 硬件进行发送
 * 参数：
 *   @desc: TX 描述符指针
 *   @x: GFP 标志（未使用，保持接口一致性）
 * 返回值：0 表示成功，非 0 表示失败
 * 
 * 注意：提交后，硬件会异步处理发送，完成后会调用 complete 回调函数
 */
#define ATBM_TX_SUBMIT_DESC(desc,x) atbm_sdio_tx_submit(desc)
#define ATBM_RX_SUBMIT_DESC(desc,x) atbm_sdio_rx_submit(desc)

void atbm_build_wsm_header_v0(struct atbmwifi_common *hw_priv, struct wsm_hdr_tx *wsm_tx, int *tx_len);


int __atbm_irq_enable(struct atbmwifi_common *priv, int enable);
atbm_void atbm_sdio_module_init(atbm_void);
int atbm_sdio_suspend(struct atbmwifi_common *hw_priv);
int atbm_sdio_resume(struct atbmwifi_common *hw_priv);
atbm_void atbm_sdio_module_exit(atbm_void);

void atbm_irq_handler(struct atbmwifi_common *hw_priv);
int atbm_device_wakeup(struct atbmwifi_common *hw_priv);
int atbm_sdio_tx_submit(ATBM_INF_DESC *desc);
int atbm_sdio_rx_submit(ATBM_INF_DESC *desc);
void atbm_sdio_queue_init(struct atbmwifi_common *hw_priv);
void atbm_sdio_queue_exit(struct atbmwifi_common *hw_priv);
int atbm_sdio_tx_queue_process(struct atbmwifi_common *hw_priv, atbm_uint32 timeout, ATBM_BOOL wait);


#if SUPPORT_LIGHT_SLEEP
static inline void atbm_light_sleep_try_to_wakeup(struct atbmwifi_common *hw_priv, int reason){
	_atbm_light_sleep_try_to_wakeup(hw_priv, reason);
}
static inline void atbm_light_sleep_try_to_keepalive(struct atbmwifi_common *hw_priv, int reason){
	_atbm_light_sleep_try_to_keepalive(hw_priv, reason);
}
static inline void atbm_light_sleep_try_to_sleep(struct atbmwifi_common *hw_priv, int reason){
	_atbm_light_sleep_try_to_sleep(hw_priv, reason);
}
#else
static inline void atbm_light_sleep_try_to_wakeup(struct atbmwifi_common *hw_priv, int reason){
}
static inline void atbm_light_sleep_try_to_keepalive(struct atbmwifi_common *hw_priv, int reason){
}
static inline void atbm_light_sleep_try_to_sleep(struct atbmwifi_common *hw_priv, int reason){
}
#endif

#endif /*  ATBMWIFI__HWIO_SDIO_H_INCLUDED */

