/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/

/**
 * @file atbm_config.h
 * @brief ATBM WiFi 驱动配置文件
 *
 * @note 该文件包含 ATBM WiFi 驱动的所有配置选项，包括：
 *       - 硬件平台选择
 *       - WiFi 功能特性配置（802.11n/ac/ax, WPS, P2P, WPA3 等）
 *       - 性能优化选项
 *       - 调试选项
 *       - 总线接口配置（USB/SDIO）
 *
 * 修改说明:
 * - 根据实际硬件平台修改 ATBM_WIFI_PLATFORM 和 ATBM_PLATFORM
 * - 根据需要使能/禁用 WiFi 功能特性
 * - 根据内存和性能要求调整优化选项
 */

#ifndef ATBM_CFG_H_
#define ATBM_CFG_H_

/******************************************************************************
 * 硬件平台配置
 *****************************************************************************/

/* 5G 频段支持 */
#undef CONFIG_ATBMWIFI__5GHZ_SUPPORT
#undef CONFIG_5G_SUPPORT
#undef CONFIG_MONITOR

/* HT MCS 流数量: 1=单流, 2=双流 */
#define CONFIG_HT_MCS_STREAM_MAX_STREAMS	1

/* 数组大小计算宏 */
#define ATBM_ARRAY_SIZE(_array) (sizeof(_array)/sizeof(_array[0]))

#ifdef LINUX_OS
/* Linux 平台定义 */
#define PLATFORM_XUNWEI				(1)		/* 讯为平台 */
#define PLATFORM_SUN6I  			(2)		/* 全志 Sun6i 平台 */
#define PLATFORM_FRIENDLY			(3)		/* FriendlyARM 平台 */
#define PLATFORM_SUN6I_64			(4)		/* 全志 Sun6i 64位平台 */
#define PLATFORM_CDLINUX			(12)	/* CDLinux 平台 */
#define PLATFORM_AMLOGIC_S805		(13)	/* Amlogic S805 平台 */
#define PLATFORM_AMLOGIC_905		(8)		/* Amlogic 905 平台 */
#define PLATFORM_SUN50IWP1		    (26)	/* 全志 Sun50i 平台 */

#ifndef  ATBM_WIFI_PLATFORM
#define ATBM_WIFI_PLATFORM			PLATFORM_XUNWEI
#endif //ATBM_WIFI_PLATFORM
#endif //LINUX_OS

/******************************************************************************
 * 速率索引定义
 * 用于标识不同的传输速率
 *****************************************************************************/
/* 802.11b 速率 */
#define RATE_INDEX_B_1M           0		/* 1 Mbps */
#define RATE_INDEX_B_2M           1		/* 2 Mbps */
#define RATE_INDEX_B_5_5M         2		/* 5.5 Mbps */
#define RATE_INDEX_B_11M          3		/* 11 Mbps */
#define RATE_INDEX_PBCC_22M       4     /* PBCC 22M (不支持) */
#define RATE_INDEX_PBCC_33M       5     /* PBCC 33M (不支持) */

/* 802.11a/g 速率 */
#define RATE_INDEX_A_6M           6		/* 6 Mbps */
#define RATE_INDEX_A_9M           7		/* 9 Mbps */
#define RATE_INDEX_A_12M          8		/* 12 Mbps */
#define RATE_INDEX_A_18M          9		/* 18 Mbps */
#define RATE_INDEX_A_24M          10	/* 24 Mbps */
#define RATE_INDEX_A_36M          11	/* 36 Mbps */
#define RATE_INDEX_A_48M          12	/* 48 Mbps */
#define RATE_INDEX_A_54M          13	/* 54 Mbps */

/* 802.11n HT 速率 */
#define RATE_INDEX_N_6_5M         14	/* MCS0: 6.5 Mbps */
#define RATE_INDEX_N_13M          15	/* MCS1: 13 Mbps */
#define RATE_INDEX_N_19_5M        16	/* MCS2: 19.5 Mbps */
#define RATE_INDEX_N_26M          17	/* MCS3: 26 Mbps */
#define RATE_INDEX_N_39M          18	/* MCS4: 39 Mbps */
#define RATE_INDEX_N_52M          19	/* MCS5: 52 Mbps */
#define RATE_INDEX_N_58_5M        20	/* MCS6: 58.5 Mbps */
#define RATE_INDEX_N_65M          21	/* MCS7: 65 Mbps */
#define RATE_INDEX_N_MCS32_6M     22	/* MCS32: 6 Mbps */

/* 802.11ax HE 速率 */
#define RATE_INDEX_HE_MCSER0      23	/* HE MCS ER0 */
#define RATE_INDEX_HE_MCSER1      24	/* HE MCS ER1 */
#define RATE_INDEX_HE_MCSER2      25	/* HE MCS ER2 */
#define RATE_INDEX_HE_MC0         26	/* HE MCS 0 */
#define RATE_INDEX_HE_MC1         27	/* HE MCS 1 */
#define RATE_INDEX_HE_MC2         28	/* HE MCS 2 */
#define RATE_INDEX_HE_MC3         29	/* HE MCS 3 */
#define RATE_INDEX_HE_MC4         30	/* HE MCS 4 */
#define RATE_INDEX_HE_MC5         31	/* HE MCS 5 */
#define RATE_INDEX_HE_MC6         32	/* HE MCS 6 */
#define RATE_INDEX_HE_MC7         33	/* HE MCS 7 */
#define RATE_INDEX_HE_MC8         34	/* HE MCS 8 */
#define RATE_INDEX_HE_MC9         35	/* HE MCS 9 */
#define RATE_INDEX_HE_MC10        36	/* HE MCS 10 */
#define RATE_INDEX_HE_MC11        37	/* HE MCS 11 */

#define RATE_INDEX_MAX         	  38	/* 最大速率索引数 */

/******************************************************************************
 * 基础速率配置
 *****************************************************************************/
/* 基础速率位图 (最大 0xf) */
#define TEST_BASIC_RATE		(ATBM_BIT(0)|ATBM_BIT(1)|ATBM_BIT(2)|ATBM_BIT(3)|ATBM_BIT(6)|ATBM_BIT(8)|ATBM_BIT(10)|ATBM_BIT(11))

/* 重传次数配置 */
#define TEST_LONG_RETRY_NUM		4	/* 长帧重传次数 */
#define TEST_SHORT_RETRY_NUM	7	/* 短帧重传次数 */

/* 前导码类型: 0=长前导码, 1=短前导码 */
#define TEST_LONG_PREAMBLE		0
#define TEST_SHORT_PREAMBLE		1

/******************************************************************************
 * 时间参数配置 (单位: Beacon 间隔或毫秒)
 *****************************************************************************/
/*
具体来说：
- 单位 : Beacon 间隔（通常一个 Beacon 间隔 = 100 TU = 102.4 毫秒）
- 默认值 : 10
- 实际时间 : 10 × 100 TU = 1000 TU ≈ 1.024 秒
这意味着初始扫描的超时时间为 10 个 Beacon 周期。如果 Beacon 间隔配置为 100ms（ TEST_BEACON_INTV = 100 ），
那么实际超时时间约为 1 秒。
这个宏定义在 atbm_config.h 文件的第 130 行，用于控制 WiFi 初始扫描阶段的超时时间。当扫描开始时，
如果在 10 个 Beacon 周期内没有完成扫描，将会触发超时处理。
*/
#define INITIAL_SCAN_EXPIRE		10		/* 初始扫描超时时间 (Beacon 间隔) */
#define TEST_BEACON_INTV    	100		/* Beacon 间隔 (TU, 1024us) */
#define TEST_DTIM_INTV    		3		/* DTIM 周期 (Beacon 间隔倍数) */
#define DEFAULT_BEACON_LOSS_CNT 40		/* Beacon 丢失阈值 ，大于门限断开网络*/
#define KEEP_ALIVE_PERIOD		4		/* 保活周期 (秒)，保活周期没有发包会触发发送nulldata包，用于保持和AP连接*/

/******************************************************************************
 * 总线接口配置
 *****************************************************************************/
#define ATBM_USB_BUS 0		/* USB 总线接口 */
#define ATBM_SDIO_BUS 1		/* SDIO 总线接口 */

/******************************************************************************
 * 功能特性配置
 *****************************************************************************/
/* ampdu reorder 数据包重排序: 1=使能, 0=禁用 */
#define ATBM_PKG_REORDER 1

/* 40MHz 带宽支持: 1=使能, 0=禁用 */
#define BW_40M_SUPPORT  1

/* RX 任务队列模式:
 * 0=不使用队列
 * 1=队列数据包
 * 2=队列转发数据包
 */
#define ATBM_RX_TASK_QUEUE 2

/* 工作队列模式: 0=链表模式, 1=数组模式 */
#define QUEUE_LIST  0

/* RX 任务使能: 1=使能, 0=禁用 */
#define ATBM_RX_TASK 1

/* 新省电模式支持: 1=使能, 0=禁用 */
#define NEW_SUPPORT_PS 1

/* 802.11n 支持: 1=使能, 0=禁用 */
#define CONFIG_IEEE80211N 1

/* P2P 地址使用本地位: 1=使能, 0=禁用 ,用来设置p2p 的mac 地址与时间mac地址的变化bit，不能随便设置。会导致和lmac 不一致*/
#define ATBM_P2P_ADDR_USE_LOCAL_BIT 1

/* AP 模式最大 STA 数量 */
#define ATBMWIFI__MAX_STA_IN_AP_MODE 16

/******************************************************************************
 * 调试配置
 *****************************************************************************/
/* SKB 调试: 1=使能, 0=禁用 */
#define ATBM_SKB_DEBUG 0

/* HIF 调试: 1=使能, 0=禁用 */
#define ATBM_HIF_DEBUG 0

/******************************************************************************
 * WiFi 协议特性配置
 *****************************************************************************/
/* 802.11ax (WiFi 6) 支持: 1=使能, 0=禁用 */
#define CONFIG_HE 1

/* 802.11ac VHT 支持: 1=使能, 0=禁用 */
#define CONFIG_VHT 1

/* WPS (WiFi Protected Setup) 支持: 1=使能, 0=禁用 */
#define CONFIG_WPS 0
#define CONFIG_WPS2 0

/* P2P (WiFi Direct) 支持: 1=使能, 0=禁用 */
#define CONFIG_P2P 0
//#define CONFIG_WIFI_DISPLAY	/* WiFi Display 支持 (未使能) */

/* WPA3 支持: 1=使能, 0=禁用 */
#define CONFIG_SAE 0			/* SAE (Simultaneous Authentication of Equals) */
#define CONFIG_IEEE80211W 0		/* 802.11w (Protected Management Frames) */

/* BLE 广播共存: 1=使能, 0=禁用 */
#define CONFIG_ATBM_BLE_ADV_COEXIST 0

/* WiFi + BLE 组合: 1=使能, 0=禁用 */
#define CONFIG_WIFI_BT_COMB 0

/* 5G 频段支持: 1=使能, 0=禁用 */
#define CONFIG_5G_SUPPORT 1

/* 以下特性暂不支持，可能后续支持 */
#define CONFIG_WPS_UPNP 0		/* WPS UPNP 支持 */
#define CONFIG_IEEE80211R 0		/* 802.11r (快速漫游) */
#define CONFIG_PEERKEY 0		/* PeerKey 支持 */

/******************************************************************************
 * 速率控制配置
 *****************************************************************************/
/* 速率控制模式: 1=PID 算法, 2=HMAC 算法 */
#define RATE_CONTROL_MODE 2

/******************************************************************************
 * 快速连接配置
 *****************************************************************************/
/* 快速连接模式: 1=使能, 0=禁用 */
#define FAST_CONNECT_MODE 0

/* 快速连接无扫描: 1=使能, 0=禁用 */
#define FAST_CONNECT_NO_SCAN 0

/* SmartConfig 支持: 1=使能, 0=禁用 */
#define ATBM_SUPPORT_SMARTCONFIG 0

/******************************************************************************
 * 电源管理配置
 *****************************************************************************/
/* 轻睡眠支持: 1=使能, 0=禁用 */
#define SUPPORT_LIGHT_SLEEP 0

/* 轻睡眠延迟 (毫秒) */
#define LIGHT_SLEEP_DELAY 0

/* 使用压缩固件: 1=使能, 0=禁用 */
#define USE_COMPRESSED_FIRMWARE 0

/* 电源节省使能: 1=使能, 0=禁用 */
#define ENABLE_POWER_SAVE 0

/* STA 唤醒局域网 (WOL): 1=使能, 0=禁用 */
#define ATBM_STA_WOL 0

/* 支持重密钥: 1=使能, 0=禁用 */
#define CONFIG_ATBM_SUPPORT_REKEY 0

/******************************************************************************
 * P2P 模式自动配置
 * 当使能 P2P 时，自动使能 WPS 和 WPS2，禁用快速连接
 *****************************************************************************/
#if CONFIG_P2P
#undef CONFIG_WPS
#define CONFIG_WPS 1
#undef CONFIG_WPS2
#define CONFIG_WPS2 1
#undef FAST_CONNECT_MODE
#define FAST_CONNECT_MODE 0
#endif

/******************************************************************************
 * SDIO/USB 特定配置
 *****************************************************************************/
/* SDIO 多帧发送: 1=使能, 0=禁用 */
#define ATBM_WSM_SDIO_TX_MULT 0

/* 网桥支持: 1=使能, 0=禁用 */
#define ATBM_SUPPORT_BRIDGE 0

#if (ATBM_SDIO_BUS)
/* SDIO 接口配置 */
#define PROCESS_RX_IN_IRQ_THREAD 1		/* 在 IRQ 线程中处理 RX */
#define PROCESS_TXRX_IN_ONE_THREAD 0	/* TX/RX 在同一个线程中处理 */
#define ATBM_DIRECT_TX 0				/* 直接 TX */
#define ATBM_IMMD_RX 0					/* 立即 RX */
#define ATBM_SUPPORT_SG 0				/* Scatter/Gather 支持 */
#define ATBM_TX_WRITE_TASK 1			/* TX 写任务 (可用值: 0,1,2) */
#else
/* USB 接口配置 */
#define ATBM_DIRECT_TX 1				/* 直接 TX */
#define ATBM_IMMD_RX 0					/* 立即 RX */
#define ATBM_SUPPORT_SG 0				/* Scatter/Gather 支持 */
#define ATBM_TX_WRITE_TASK 1			/* TX 写任务 (固定为 1) */
#endif

/******************************************************************************
 * 性能优化配置
 *****************************************************************************/
/* 无限聚合 TX: 1=使能, 0=禁用 */
#define CONFIG_INF_AGGR_TX 1

/* 接口 RX 聚合多帧: 1=使能, 0=禁用 ，功能必须和lmac一样，不然会有问题，默认打开*/
#define HI_RX_MUTIL_FRAME 1

/* 802.3 帧队列: 1=使能, 0=禁用 */
#define ATBM_QUEUE_8023_FRAMES 1

/* TX 完成使用单独task: 1=使能, 0=禁用 */
#define ATBM_TX_CMPL_TASK 0

/* RX 数据包重用: 1=使能, 0=禁用 ，主要用于零拷贝功能*/
#define ATBM_RX_REUSE_PACKET 1

/******************************************************************************
 * 硬件芯片支持配置
 *****************************************************************************/
/* DCXO DPLL 配置测试: 1=使能, 0=禁用 */
#define TEST_DCXO_DPLL_CONFIG 0

/*
 * 芯片型号定义:
 * SUPPORT_CRONUS      : ATBM6062
 * SUPPORT_CRONUS_LITE : ATBM6062C
 * SUPPORT_OCEANUS     : ATBM6162/ATBM6132-C
 * SUPPORT_COEUS       : ATBM6165
 * SUPPORT_OCEANUS_LITE: ATBM6162 Lite
 * SUPPORT_DEMETER     : ATBM6062C Lite
 */
#define SUPPORT_CRONUS 0
#define SUPPORT_CRONUS_LITE 0
#define SUPPORT_OCEANUS 1
#define SUPPORT_COEUS 0
/* IPRO7 port: ATBM6162 is OCEANUS (chip id 0x30) only. Disable LITE/DEMETER so their
 * firmware blobs are not compiled into the 2MB flash image. */
#define SUPPORT_OCEANUS_LITE 0
#define SUPPORT_DEMETER 0

/******************************************************************************
 * RTOS 平台选择
 *****************************************************************************/
/* 自定义平台选择 */
#define JIANRONG_RTOS_3298 0
#define JIANRONG_RTOS_3268 1
#define ALI_RTOS 2
#define ANYKA_RTOS 3
#define STM32_UCOS 4
#define SUN6I_LINUX 5
#define FH8852_RTT 6
#define FH_EVIZE_RTT 7
#define ASR_THREADX 8
#define UIS8310_THREADX 9

/* 当前选择的平台 */
#define ATBM_PLATFORM SUN6I_LINUX

/* 驱动处理块确认: 1=使能, 0=禁用 */
#define ATBM_DRIVER_PROCESS_BA	1

/******************************************************************************
 * 安全认证配置
 *****************************************************************************/
/* WPA2 重装攻击认证: 1=使能, 0=禁用 */
#define CONFIG_WPA2_REINSTALL_CERTIFICATION  0

/******************************************************************************
 * 信道切换配置
 *****************************************************************************/
/* CSA (Channel Switch Announcement) 支持: 1=使能, 0=禁用 */
#define CONFIG_ATBM_SUPPORT_CSA 0

/* 信道切换测试: 1=使能, 0=禁用 */
#define CONFIG_ATBM_SUPPORT_CHANSWITCH_TEST 0

#endif /*ATBM_CFG_H_*/
