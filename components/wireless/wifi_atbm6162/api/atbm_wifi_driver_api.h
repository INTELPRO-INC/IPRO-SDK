/**************************************************************************************************************
 * AltoBeam RTOS WiFi Driver API
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 * The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 * copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
 *
 ***************************************************************************************************************
 * @file    atbm_wifi_driver_api.h
 * @brief   AltoBeam WiFi驱动公共API头文件 (AltoBeam WiFi Driver Public API Header)
 * 
 * @details 本文件定义了AltoBeam WiFi驱动的所有公共API接口，包括：
 *          - WiFi初始化和去初始化
 *          - STA模式：扫描、连接、断开
 *          - AP模式：创建热点、获取已连接客户端
 *          - 快速连接：保存/恢复连接信息
 *          - 智能配网：SmartConfig、WPS
 *          - P2P (WiFi Direct)：设备发现、连接
 *          - 产测功能：TX/RX测试、功率设置
 *          - 其他：获取芯片信息、设置功率等
 *
 * @note    使用前请确保已正确初始化WiFi硬件 (调用atbm_wifi_hw_init)
 *
 * @version 1.0
 * @date    2024
 ***************************************************************************************************************/

#ifndef ATBM_WIFI_DRIVER_API_H
#define ATBM_WIFI_DRIVER_API_H
#include "atbm_type.h"
#ifdef __cplusplus
extern "C" {
#endif
	

//typedef int int32;
//typedef unsigned int uint32;
typedef atbm_void* wl_drv_hdl;

/////////////////////////////////////////////////////////////////////////////////

/////////////////////the following struct copy from wlan_ATBM.h, will delete after include wlan_ATBM.h

/////////////////////////////////////////////////////////////////////////////////

/**
 * @brief WiFi工作模式枚举 (WiFi Working Mode Enumeration)
 */
typedef enum __ATBM_WIFI_MODE
{
	ATBM_WIFI_STA_MODE,     /**< STA模式 - 作为客户端连接AP */
	ATBM_WIFI_AP_MODE,      /**< AP模式 - 作为热点供其他设备连接 */
	ATBM_WIFI_ADHOC,        /**< Ad-Hoc模式 - 点对点直连 */
	ATBM_WIFI_MONITOR,      /**< 监听模式 - 抓包/混杂模式 */
	ATBM_WIFI_P2P_CLIENT,   /**< P2P客户端模式 */
	ATBM_WIFI_P2P_GO,       /**< P2P GO模式 - Group Owner */
	ATBM_WIFI_RF_MODE       /**< RF测试模式 */
}ATBM_WIFI_MODE;
enum {
	WLAN_CHANNEL_1 = 1,
	WLAN_CHANNEL_2,
	WLAN_CHANNEL_3,
	WLAN_CHANNEL_4,
	WLAN_CHANNEL_5,
	WLAN_CHANNEL_6,
	WLAN_CHANNEL_7,
	WLAN_CHANNEL_8,
	WLAN_CHANNEL_9,
	WLAN_CHANNEL_10,
	WLAN_CHANNEL_11,
	WLAN_CHANNEL_12,
	WLAN_CHANNEL_13
};
typedef int WLAN_CHANNEL;

typedef enum {
	/* CCK rate. */	
	WLAN_RATE_1M	= 2,
	WLAN_RATE_2M	= 4,
	WLAN_RATE_5M5	= 11,
	WLAN_RATE_11M	= 22,

	/* OFDM Rate. */	   
	WLAN_RATE_6M	= 12,
	WLAN_RATE_9M	= 18,
	WLAN_RATE_12M   = 24,
	WLAN_RATE_18M	= 36,
	WLAN_RATE_24M	= 48,
	WLAN_RATE_36M	= 72,
	WLAN_RATE_48M	= 96,
	WLAN_RATE_54M	= 108,

	/* HT Rate. */	
	WLAN_MCS_RATE_0 = 128,			/*MCS0 128*/
	WLAN_MCS_RATE_1,
	WLAN_MCS_RATE_2,
	WLAN_MCS_RATE_3,
	WLAN_MCS_RATE_4,
	WLAN_MCS_RATE_5,
	WLAN_MCS_RATE_6,
	WLAN_MCS_RATE_7,				/*MCS7 135*/
	WLAN_MCS_RATE_8,
	WLAN_MCS_RATE_9,
	WLAN_MCS_RATE_10,
	WLAN_MCS_RATE_11,
	WLAN_MCS_RATE_12,
	WLAN_MCS_RATE_13,
	WLAN_MCS_RATE_14,
	WLAN_MCS_RATE_15,
	WLAN_MCS_RATE_16,
	WLAN_MCS_RATE_17,
	WLAN_MCS_RATE_18,
	WLAN_MCS_RATE_19,
	WLAN_MCS_RATE_20,
	WLAN_MCS_RATE_21,
	WLAN_MCS_RATE_22,
	WLAN_MCS_RATE_23,
	WLAN_MCS_RATE_24,
	WLAN_MCS_RATE_25,
	WLAN_MCS_RATE_26,
	WLAN_MCS_RATE_27,
	WLAN_MCS_RATE_28,
	WLAN_MCS_RATE_29,
	WLAN_MCS_RATE_30,
	WLAN_MCS_RATE_31				/*MCS31 159*/
}WLAN_RATE;


/* Supported authentication mode. */
/* Values are used to select the authentication mode used to join a network. */
enum {
	WLM_WPA_AUTH_DISABLED = 0x0000,	/* Legacy (i.e., non-WPA) */
	WLM_WPA_AUTH_NONE = 0x0001,		/* none (IBSS) */
	WLM_WPA_AUTH_PSK = 0x0004,		/* Pre-shared key */
	WLM_WPA2_AUTH_PSK = 0x0080		/* Pre-shared key */
};
typedef int WLM_AUTH_MODE;

/* WLAN Security Encryption. */
/* Values are used to select the type of encryption used for testing. */
enum {
	WLM_ENCRYPT_NONE = 0,    /* No encryption. */
	WLM_ENCRYPT_WEP = 1,     /* WEP encryption. */
	WLM_ENCRYPT_TKIP = 2,    /* TKIP encryption. */
	WLM_ENCRYPT_AES = 4,     /* AES encryption. */
	WLM_ENCRYPT_WSEC = 8,    /* Software WSEC encryption. */
	WLM_ENCRYPT_FIPS = 0x80  /* FIPS encryption. */
};
typedef int WLM_ENCRYPTION;


#define WLAN_MCSSET_LEN				16 

/**
 * @brief BSS信息结构体 (BSS Information Structure)
 *        用于存储扫描到的AP信息
 */
typedef struct _WLAN_BSS_INFO
{
    atbm_uint32      version;        /**< 版本号 */
    atbm_uint32      length;         /**< 数据长度 (从version开始，包含IEs) */
    atbm_uint8       BSSID[6];       /**< BSSID (AP的MAC地址) */
    atbm_uint16      beacon_period;  /**< Beacon周期 (单位: Kusec) */
    atbm_uint16      capability;     /**< 能力信息 */
    atbm_uint8       SSID_len;       /**< SSID长度 */
    atbm_uint8       SSID[32];       /**< SSID (网络名称) */
    atbm_uint8       chanspec;       /**< 信道规格 */
    atbm_uint8       dtim_period;    /**< DTIM周期 */
    atbm_int8        RSSI;           /**< 接收信号强度 (dBm) */
    atbm_int8        phy_noise;      /**< 噪声 (dBm) */
    atbm_uint8       n_cap;          /**< 是否支持802.11N */
    atbm_uint32      nbss_cap;       /**< 802.11N BSS能力 (基于HT_CAP_*) */
    atbm_uint8       ctl_ch;         /**< 802.11N BSS控制信道号 */
    atbm_int8        security;       /**< 安全类型 (ATBM_SECURITY_TYPE), -1表示未识别 */
} WLAN_BSS_INFO;

typedef struct wl_connection_info
{
	atbm_uint8	Ssid_len;
	atbm_uint8	Ssid[32];
	int	 	Rssi;
	int		Phy_rate;
	int 	channel;
} wl_connection_info_t;

typedef struct _WLAN_CONNECTION_INFO
{
	atbm_uint8   Ssid_len;
	atbm_uint8   Ssid[32];
	atbm_int32   Rssi;
	atbm_int32   Phy_rate;
	atbm_int32   channel;
} WLAN_CONNECTION_INFO;

/* Supported authentication mode. */
/* Values are used to select the authentication mode used to join a network. */
enum {
	WLAN_WPA_AUTH_DISABLED = 0x0000,	/* Legacy (i.e., non-WPA) */
	WLAN_WPA_AUTH_NONE = 0x0001,		/* none (IBSS) */
	WLAN_WPA_AUTH_PSK = 0x0004,		/* Pre-shared key */
	WLAN_WPA2_AUTH_PSK = 0x0080,		/* Pre-shared key */
	WLAN_MIX_AUTH_PSK = 0x0100,	        /* Pre-shared key */
	WLAN_ENCRYPT_WEP_SHARED = 0x0200,
	WLAN_WPA_AUTH_SAE = 0x0400,	        /* SAE */
	WLAN_WPA_AUTH_PSK_SAE = 0x0800,	        /* WPA2/SAE */
};
typedef int WLAN_AUTH_MODE;


/* WLAN Security Encryption. */
/* Values are used to select the type of encryption used for testing. */
enum {
	WLAN_ENCRYPT_NONE = 0,    /* No encryption. */
	WLAN_ENCRYPT_WEP = 1,     /* WEP encryption. */
	WLAN_ENCRYPT_TKIP = 2,    /* TKIP encryption. */
	WLAN_ENCRYPT_AES = 4,     /* AES encryption. */
	WLAN_ENCRYPT_WSEC = 8,    /* Software WSEC encryption. */
	WLAN_ENCRYPT_FIPS = 0x80  /* FIPS encryption. */
};
typedef int WLAN_ENCRYPTION;


/**
 * @brief 扫描结果结构体 (Scan Result Structure)
 *        用于存储WiFi扫描的完整结果
 */
typedef struct _WLAN_SCAN_RESULT {
	atbm_uint32 buflen;         /**< 缓冲区长度 */
	atbm_uint32 version;        /**< 版本号 */
	atbm_uint32 count;          /**< 扫描到的AP数量 */
	WLAN_BSS_INFO bss_info[1];  /**< BSS信息数组 (变长) */
} WLAN_SCAN_RESULT;

/**
 * @brief WiFi安全类型枚举 (WiFi Security Type Enumeration)
 *        用于指定连接AP或创建AP时的加密方式
 */
typedef enum _ATBM_SECURITY_TYPE
{
	ATBM_KEY_NONE = 0,      /**< 开放网络 - 无加密 */
	ATBM_KEY_WEP,           /**< WEP加密 - 开放认证 */
	ATBM_KEY_WEP_SHARE,     /**< WEP加密 - 共享密钥认证 */
	ATBM_KEY_WPA,           /**< WPA-PSK加密 (TKIP) */
	ATBM_KEY_WPA2,          /**< WPA2-PSK加密 (AES/CCMP) */
	ATBM_KEY_MIX,           /**< WPA/WPA2混合模式 */
	ATBM_KEY_SAE,           /**< WPA3-SAE加密 */
	ATBM_KEY_SAE_COMPIT,    /**< WPA2/WPA3兼容模式 (SAE+PSK) */
	ATBM_KEY_MAX,           /**< 枚举最大值 */
}ATBM_SECURITY_TYPE;

/**
 * @brief WiFi连接信息结构体 (WiFi Connection Information Structure)
 *        用于获取当前WiFi连接的详细信息
 */
typedef struct _ATBM_WLAN_CONNECTION_INFO
{
	atbm_uint8   Ssid_len;       /**< SSID长度 */
	atbm_uint8   Ssid[32];       /**< SSID (网络名称) */
	atbm_uint8   bssid[6];       /**< BSSID (AP的MAC地址) */
	atbm_int32   Rssi;           /**< 接收信号强度 (dBm) */
	atbm_int32   Phy_rate;       /**< 物理层速率 (Mbps) */
	atbm_int32   channel;        /**< 工作信道 */
} ATBM_WLAN_CONNECTION_INFO;

/**
 * @brief 以太网地址结构体 (Ethernet Address Structure)
 */
typedef struct _WLAN_ETHER_ADDR
{
	atbm_uint8 mac[6];          /**< MAC地址 (6字节) */
}WLAN_ETHER_ADDR;

/**
 * @brief MAC地址列表结构体 (MAC Address List Structure)
 *        用于AP模式下获取已连接的STA列表
 */
typedef struct _WLAN_MACLIST
{
	atbm_uint32 count;          /**< MAC地址数量 */
	WLAN_ETHER_ADDR ea[ATBMWIFI__MAX_STA_IN_AP_MODE];  /**< MAC地址数组 */
}WLAN_MACLIST;

/**
 * @brief 快速连接信息结构体 (Fast Link Information Structure)
 *        用于保存和恢复WiFi连接状态，实现快速重连
 */
typedef struct _FAST_LINK_INFO{
	atbm_int32 enable;          /**< 启用标志 */
	atbm_int32 ssid_offset;     /**< SSID在config中的偏移量 */
	atbm_int32 psk_offset;      /**< PSK在config中的偏移量 */
	atbm_uint8 config[200];     /**< 配置数据 (包含SSID、密码等) */
	atbm_uint8 bss[100];        /**< BSS信息 (包含AP的能力信息) */
	atbm_uint8 ie[350];         /**< IE信息 (Information Elements) */
}FAST_LINK_INFO;

/**
 * @brief 接收测试结果结构体 (RX Test Result Structure)
 *        用于产测中获取接收测试的统计数据
 */
typedef struct{
	int rxSuccess;      /**< 成功接收的数据包数量 */
	int rxError;        /**< 接收错误的数据包数量 */
	int rssi;           /**< 接收信号强度 (dBm) */
}get_result_rx_data;

/////////////////////////////////////////////////////////////////////////////////

/////////////////////end  wlan_ATBM.h defines

/////////////////////////////////////////////////////////////////////////////////
#define WIFI_POWER_TARGET_MAX			(30)
#define OFDM_SRRC_TX_DSSS 				(0)
#define OFDM_40M_MCS_LUT_OFFSET 		(44)
#define OFDM_20M_INDEX_MAX 				(33) //sum(1M ~HE-SU_MCS11)
#define OFDM_40M_INDEX_MAX 				(31) //sum(6M ~HE-SU_MCS11)
#define BLE_MCS_LUT_NUM					(4)	//ble MCS_LUT num
#define BLE_DSSS_MCS_LUT_NUM			(6) //ble MCS_LUT num + DSSS MCS_LUT num

#define OFDM_40M_OFFSET					(OFDM_40M_MCS_LUT_OFFSET+BLE_DSSS_MCS_LUT_NUM)


#define CRONUS_OFDM_40M_OFFSET 	(36)

#define CRONUS_OFDM_INDEX_MAX 			(36) //sum(1M ~HE-SU_MCS11) 20M as same as 40M


/****************************************************************************
* Function:   	atbm_wifi_hw_init
*
* Purpose:   	This function is used to initialize and start atbm wifi  hardware.
			may be GPO, BUS PROBE, firmware init etc.
*
* Parameters: none
*
* Returns:	Returns driver handle, otherwise a NULL pointer.
****************************************************************************/
atbm_int32  atbm_wifi_hw_init(atbm_void);

/****************************************************************************
* Function:   	atbm_wifi_hw_deinit
*
* Purpose:   	This function is used to release and clean up the driver
*
* Parameters: none
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
****************************************************************************/
atbm_int32  atbm_wifi_hw_deinit(atbm_void);


/****************************************************************************
* Function:   	atbm_wifi_on
*
* Purpose:   	This function is used to initialize and start atbm wifi  module as AP mode or STA mode.                     
*
* Parameters: AP_sta_mode     0: Ap Mode, 1 STA mode
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
****************************************************************************/
atbm_void* atbm_wifi_on(ATBM_WIFI_MODE AP_sta_mode);
atbm_void* atbm_wifi_on_vif(ATBM_WIFI_MODE AP_sta_mode,atbm_uint8 if_id);


/****************************************************************************
* Function:   	atbm_wifi_off
*
* Purpose:   	This function is used to stop atbm wifi  module.
*
* Returns:	Returns none.
*****************************************************************************/
atbm_void  atbm_wifi_off(atbm_uint8 if_id);
atbm_void  atbm_wifi_off_vif(atbm_uint8 if_id);


/****************************************************************************
* Function:   	atbm_wifi_scan_network
*
* Purpose:   	This function is used to ask driver to perform channel scan and return scan result.
*
* Parameters: scan_buf		Buffer to store the information of the found APs
*			buf_size		Size of the buffer
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
******************************************************************************/
int atbm_wifi_scan_network(char* scan_buf, atbm_uint32 buf_size);



/****************************************************************************
* Function:   	atbm_wifi_get_mode
*
* Purpose:   	This function is used to get  wifi mode 
*
* Parameters: None
*
* Returns:	Returns 0 if in STA mode, 1 in SW AP mode.
******************************************************************************/
atbm_int32 atbm_wifi_get_current_mode(void);   //0 : sta, 1: SW AP
atbm_int32 atbm_wifi_get_current_mode_vif(atbm_uint8 if_id);   //0 : sta, 1: SW AP



/****************************************************************************
* Function:   	atbm_wifi_get_mac_address
*
* Purpose:   	This function is used to get wifi MAC addressi
*
* Parameters: point to buffer of MAC address
*
* Returns:	None.
******************************************************************************/
#ifndef ATBM_COMB_IF
atbm_void atbm_wifi_get_mac_address(unsigned char *addr);
#else
atbm_void atbm_wifi_get_mac_address(atbm_uint8 if_id,unsigned char *addr);
#endif

/****************************************************************************
* Function:     wifi_ConnectAP_vif
*
* Purpose:      STA模式下连接指定AP (Station Connect to AP)
*               该函数用于在STA模式下连接到指定的无线接入点
*
* Parameters:   if_id       - 接口ID (0: 主接口, 1: 次接口)
*               ssid        - 目标AP的SSID (网络名称)
*               ssidlen     - SSID长度 (最大32字节)
*               password    - 连接密码 (对于开放网络可为NULL)
*               passwdlen   - 密码长度 (开放网络为0)
*               key_mgmt    - 安全类型，参见ATBM_SECURITY_TYPE枚举:
*                             ATBM_KEY_NONE     - 开放网络
*                             ATBM_KEY_WEP      - WEP加密
*                             ATBM_KEY_WEP_SHARE- WEP共享密钥
*                             ATBM_KEY_WPA      - WPA-PSK
*                             ATBM_KEY_WPA2     - WPA2-PSK
*                             ATBM_KEY_MIX      - WPA/WPA2混合模式
*                             ATBM_KEY_SAE      - WPA3-SAE
*                             ATBM_KEY_SAE_COMPIT- WPA2/WPA3兼容模式
*
* Returns:      成功返回0，失败返回负数错误码
*
* Note:         调用此函数前需确保WiFi已初始化并处于STA模式
*               连接过程是异步的，需要通过事件回调获取连接结果
******************************************************************************/
atbm_int32 wifi_ConnectAP_vif(atbm_uint8 if_id,atbm_uint8 * ssid,int ssidlen,atbm_uint8 * password,
		int passwdlen,ATBM_SECURITY_TYPE key_mgmt);

/****************************************************************************
* Function:     wifi_StartAP_vif
*
* Purpose:      启动软AP模式 (Start Soft AP)
*               该函数用于创建一个软AP热点，允许其他设备连接
*
* Parameters:   if_id       - 接口ID (0: 主接口, 1: 次接口)
*               ssid        - AP的SSID (网络名称)
*               ssidlen     - SSID长度 (最大32字节)
*               password    - AP密码 (开放网络可为NULL)
*               passwdlen   - 密码长度 (开放网络为0，WPA/WPA2至少8字节)
*               channel     - 工作信道 (1-13 for 2.4GHz)
*               key_mgmt    - 安全类型，参见ATBM_SECURITY_TYPE枚举
*               ssidBcst    - SSID广播控制:
*                             ATBM_FALSE(0) - 广播SSID
*                             ATBM_TRUE(1)  - 隐藏SSID
*
* Returns:      无返回值
*
* Note:         启动AP前需确保WiFi已初始化
*               如果同时运行STA和AP，需要在同一信道
******************************************************************************/
atbm_void wifi_StartAP_vif(atbm_uint8 if_id,atbm_uint8 * ssid,int ssidlen,atbm_uint8 * password,
	int passwdlen,int channel,ATBM_SECURITY_TYPE key_mgmt,ATBM_BOOL ssidBcst);
/****************************************************************************
* Function:   	atbm_wifi_tx_pkt
*
* Purpose:   	This function is used to send packet to wifi driver
*
* Parameters: point to buffer of packet
*
* Returns:	None.
******************************************************************************/
#ifndef ATBM_COMB_IF 
atbm_int32 atbm_wifi_tx_pkt(atbm_int8 *pbuf, atbm_uint32 pktlen, atbm_int8 if_id);
#else
atbm_int32 atbm_wifi_tx_pkt(atbm_int8 *pbuf, atbm_uint32 pktlen, atbm_int8 if_id);
#endif


//void atbm_wifi_rx_pkt(netif, total_len);   //not required here ,   lwip_tcp_opt.net_rx = ethernetif_input.



/****************************************************************************
* Function:   	atbm_wifi_get_avail_tx_queue_count
*
* Purpose:   	This function is used to return the available buffer count of driver TX queue
*
* Parameters: None
*
* Returns:	Returns number of available buffer count of driver TX queue.
******************************************************************************/
atbm_int32 atbm_wifi_get_avail_tx_queue_count(atbm_void);




/****************************************************************************
* Function:     atbm_wifi_sta_join_ap
*
* Purpose:      This function is used to ask driver to join a netwrok.
*
* Parameters: ssid          SSID of the AP used to join a network
*            authMode   authentication mode used to join a network
*            encryption encryption mode used to join a network
*            key            passphrase used to join a network
*
* Returns:  Returns 0 if succeed, otherwise a negative error code.
******************************************************************************/
atbm_int32 atbm_wifi_sta_join_ap(char *ssid, char *bssid, WLAN_AUTH_MODE authMode, WLAN_ENCRYPTION encryption, char *key);

/****************************************************************************
* Function:   	atbm_wifi_get_driver_version
*
* Purpose:   	This function is used to return the driver's release version
*
* Parameters: None
*
* Returns:	Returns the driver release version
*****************************************************************************
*/
signed char* atbm_wifi_get_driver_version(atbm_void);



/****************************************************************************
* Function:     wlan_get_connection_info
*
* Purpose:      This function is used to get the current connection information at STA mode
*
* Parameters: wlan connection information
*
* Returns:  Returns 0 if succeed, otherwise a negative error code.

typedef struct _WLAN_CONNECTION_INFO
{
    atbm_uint8   Ssid_len;
    atbm_uint8   Ssid[32];
    int32   Rssi;
    int32   Phy_rate;
    int32   channel;
} WLAN_CONNECTION_INFO;
*****************************************************************************
*/
int atbm_wifi_get_work_channel(void);

atbm_int32 atbm_wifi_get_connected_info(ATBM_WLAN_CONNECTION_INFO *wlan_connection_info);
atbm_int32 atbm_wifi_get_connected_info_vif(atbm_uint8 if_id,ATBM_WLAN_CONNECTION_INFO *wlan_connection_info);
/****************************************************************************
* Function:   	wlan_get_connection_info
*
* Purpose:   	This function is used to get the current connection information at STA mode
*
* Parameters: wlan connection information
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
****************************************************************************/














/*************************************************************************************

**************                SW AP functions                                                                        *********

**************************************************************************************/

/****************************************************************************
* Function:     atbm_wifi_ap_create
*
* Purpose:      This function is used to create a SW AP network
*
* Parameters: ssid          SSID of the SW AP to be created
*            authMode   Authentication mode used for the SW AP
*            encryption Encryption mode used for the SW AP
*            key            Passphrase used for the SW AP
*            channel        Channle used for the SW AP
*            ssidBcst       0: to broadcast SSID, 1: to hide SSID
*
* Returns:  Returns 0 if succeed, otherwise a negative error code.
*****************************************************************************
*/
atbm_int32 atbm_wifi_ap_create(char* ssid, int authMode, int encryption, 
							char *key, int channel, ATBM_BOOL ssidBcst );
atbm_int32 atbm_wifi_ap_create_vif(atbm_uint8 if_id,char* ssid, int authMode, int encryption, 
							char *key, int channel, ATBM_BOOL ssidBcst );



/****************************************************************************
* Function:     wlan_get_assoc_list
*
* Purpose:      This function is used to the associated client list in SW AP mode
*
* Parameters: buf           The buffer to store the associated client list
*                    uiBufSize       size of the buffer
*
* Returns:  Returns 0 if succeed, otherwise a negative error code.

* For ioctls that take a list of MAC addresses *
typedef struct _WLAN_MACLIST
{
	uint32 count;              // number of MAC addresses 
	WLAN_ETHER_ADDR ea[1];     // variable length array of MAC addresses 
}WLAN_MACLIST;
******************************************************************************/
	
atbm_int32 atbm_wifi_get_associated_client_list(atbm_uint8 *pchBuf, atbm_uint32 uiBufSize);






/****************************************************************************
* Function:     atbm_wifi_set_256BITSEFUSE
*
* Purpose:      写入256位eFuse数据 (Write 256-bit eFuse Data)
*               用于写入芯片的一次性可编程存储区域
*
* Parameters:   data    - 要写入的数据缓冲区指针
*               length  - 数据长度 (最大32字节/256位)
*
* Returns:      成功返回0，失败返回负数错误码
*
* Warning:      eFuse为一次性可编程，写入后不可更改，请谨慎操作!
******************************************************************************/
atbm_int32 atbm_wifi_set_256BITSEFUSE(atbm_uint8 *data, atbm_uint32 length);

/****************************************************************************
* Function:     atbm_wifi_get_256BITSEFUSE
*
* Purpose:      读取256位eFuse数据 (Read 256-bit eFuse Data)
*               用于读取芯片的一次性可编程存储区域内容
*
* Parameters:   data    - 存储读取数据的缓冲区指针
*               length  - 要读取的数据长度 (最大32字节/256位)
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
atbm_int32 atbm_wifi_get_256BITSEFUSE(atbm_uint8 *data, atbm_uint32 length);


/****************************************************************************
* 
* Warn:Temperature is vaild only when transmitting is going on
*
******************************************************************************/
int atbm_wifi_get_chip_temperature(int *temperature);


/*************************************************************************************
*
*                         产测功能函数 (Manufacturing Test Functions)
*
*  以下函数用于WiFi模块的生产测试，包括发射测试、接收测试等
*
**************************************************************************************/

/****************************************************************************
* Function:     atbm_wifi_mfg_start
*
* Purpose:      启动产测模式 (Start Manufacturing Test Mode)
*               进入产测模式后，WiFi模块将停止正常工作，专用于生产测试
*
* Parameters:   无
*
* Returns:      无返回值
*
* Note:         调用此函数后需要调用atbm_wifi_mfg_stop()退出产测模式
******************************************************************************/
atbm_void  atbm_wifi_mfg_start(void);


/**************************************************************************************
* Function:     atbm_wifi_mfg_set_pktTxBG
*
* Purpose:      This function is used to perform manufacturing 11b/g continuous TX test
*
* Parameters:   channel       Channel used for TX
*               	  rate          11b/g rate used for TX
*               	  powerValue    Output power index, -1 means default power
*
* Returns:      Returns 0 if succeed, otherwise a negative error code.
****************************************************************************************/
atbm_int32 atbm_wifi_mfg_set_pktTxBG(WLAN_CHANNEL channel, WLAN_RATE rate, atbm_int32 powerValue);


/***********************************************************************************
* Function:     atbm_wifi_mfg_set_PktTxN
*
* Purpose:      This function is used to perform manufacturing 11n continuous TX test
*
* Parameters:   channel       Channel used for TX
*              	   rate          11n rate used for TX
*               	  powerValue    Output power index, -1 means default power
*
* Returns:      Returns 0 if succeed, otherwise a negative error code.
************************************************************************************/
atbm_int32 atbm_wifi_mfg_set_PktTxN(WLAN_CHANNEL channel, WLAN_RATE rate, atbm_int32 powerValue);


/****************************************************************************
* Function:     atbm_wifi_mfg_CarrierTone
*
* Purpose:      This function is used to perform manufacturing non-modulation TX test
*
* Parameters:   channel       Channel used for test
*
* Returns:      Returns 0 if succeed, otherwise a negative error code.
******************************************************************************/
atbm_int32 atbm_wifi_mfg_CarrierTone(WLAN_CHANNEL channel);


/****************************************************************************
* Function:     atbm_wifi_mfg_set_PktRxMode
*
* Purpose:      This function  is used to perform manufacturing RX test
*
* Parameters:   channel     Channel used for RX
*
* Returns:      Returns 0 if succeed, otherwise a negative error code.
******************************************************************************/
atbm_int32 atbm_wifi_mfg_set_PktRxMode(WLAN_CHANNEL channel);


/****************************************************************************
* Function:     atbm_wifi_mfg_get_RxPkt
*
* Purpose:      This function is used to get received packet count
*
* Parameters:   uiCount     Received packet count
*
* Returns:      Returns 0 if succeed, otherwise a negative error code.
******************************************************************************/
atbm_int32 atbm_wifi_mfg_get_RxPkt(atbm_uint32* uiCount);



/****************************************************************************
* Function:     atbm_wifi_mfg_stop
*
* Purpose:      This function is used to stop manufacturing test
*
* Parameters:   None
*
* Returns:      Returns 0 if succeed, otherwise a negative error code.
******************************************************************************/
atbm_int32 atbm_wifi_mfg_stop(atbm_void);

/****************************************************************************
* Function:     atbm_wifi_etf_start_tx
*
* Purpose:      启动ETF发射测试 (Start ETF TX Test)
*               用于产测中的发射性能测试
*
* Parameters:   channel     - 测试信道 (1-14 for 2.4GHz, 36-165 for 5GHz)
*               mode        - 测试模式 (0: 11b, 1: 11g, 2: 11n, 3: 11ac, 4: 11ax)
*               rateIdx     - 速率索引
*               bw          - 带宽 (0: 20MHz, 1: 40MHz)
*               chOff       - 信道偏移 (0: 无偏移, 1: 上偏移, 2: 下偏移)
*               ldpc        - LDPC编码 (0: 禁用, 1: 启用)
*               packetLen   - 测试包长度
*               precom      - 预补偿值
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_wifi_etf_start_tx(int channel,int mode, int rateIdx,int  bw, int chOff, int ldpc, int packetLen,int precom);

/****************************************************************************
* Function:     atbm_wifi_etf_stop_tx
*
* Purpose:      停止ETF发射测试 (Stop ETF TX Test)
*
* Parameters:   无
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_wifi_etf_stop_tx(void);

/****************************************************************************
* Function:     atbm_wifi_etf_start_rx
*
* Purpose:      启动ETF接收测试 (Start ETF RX Test)
*               用于产测中的接收灵敏度测试
*
* Parameters:   channel     - 测试信道
*               bw          - 带宽 (0: 20MHz, 1: 40MHz)
*               chOff       - 信道偏移
*               mode        - 测试模式
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_wifi_etf_start_rx(int channel ,int bw, int chOff, int mode);

/****************************************************************************
* Function:     atbm_wifi_etf_stop_rx
*
* Purpose:      停止ETF接收测试并获取结果 (Stop ETF RX Test and Get Results)
*
* Parameters:   rx_data     - 接收测试结果结构体指针，包含:
*                             rxSuccess - 成功接收的包数
*                             rxError   - 接收错误的包数
*                             rssi      - 接收信号强度
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_wifi_etf_stop_rx(get_result_rx_data *rx_data);

/****************************************************************************
* Function:     atbm_wifi_ioctl_fwcmd
*
* Purpose:      发送固件命令 (Send Firmware Command)
*               用于向WiFi固件发送自定义命令
*
* Parameters:   cmd_len     - 命令字符串长度
*               cmd         - 命令字符串
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
atbm_int32 atbm_wifi_ioctl_fwcmd(int cmd_len,char *cmd);


/****************************************************************************
* Function:     atbm_wifi_isconnected
*
* Purpose:      检查WiFi连接状态 (Check WiFi Connection Status)
*               用于查询指定接口是否已连接到AP
*
* Parameters:   if_id       - 接口ID (0: 主接口, 1: 次接口)
*
* Returns:      1 - 已连接, 0 - 未连接
******************************************************************************/
int atbm_wifi_isconnected(atbm_uint8 if_id);

/****************************************************************************
* Function:     atbm_wifi_sta_disjoin_ap
*
* Purpose:      断开与AP的连接 (Disconnect from AP)
*               STA模式下主动断开与当前AP的连接
*
* Parameters:   无
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_wifi_sta_disjoin_ap(void);

/****************************************************************************
* Function:     atbm_akwifi_setup_sdio
*
* Purpose:      初始化SDIO接口 (Setup SDIO Interface)
*               用于初始化WiFi模块的SDIO通信接口
*
* Parameters:   无
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_akwifi_setup_sdio(void);

/****************************************************************************
* Function:     atbm_priv_get_netif
*
* Purpose:      获取网络接口 (Get Network Interface)
*               从网络设备结构获取对应的网络接口指针
*
* Parameters:   dev         - 网络设备结构指针
*
* Returns:      网络接口指针，失败返回NULL
******************************************************************************/
ATBM_NETIF *atbm_priv_get_netif(struct atbm_net_device *dev);

/****************************************************************************
* Function:     atbm_get_wifimode
*
* Purpose:      获取WiFi工作模式 (Get WiFi Working Mode)
*               查询指定接口当前的工作模式
*
* Parameters:   priv        - WiFi接口私有数据结构指针
*
* Returns:      WiFi模式值，参见ATBM_WIFI_MODE枚举
******************************************************************************/
atbm_uint8 atbm_get_wifimode(struct atbmwifi_vif *priv);

/****************************************************************************
* Function:     atbm_smartconfig_start
*
* Purpose:      启动智能配网 (Start SmartConfig)
*               启动智能配网模式，用于通过手机APP配置WiFi网络
*               支持的配网协议包括: AirKiss, ESP-Touch等
*
* Parameters:   无
*
* Returns:      成功返回0，失败返回非0值
*
* Note:         启动后设备将进入混杂模式监听配网数据包
******************************************************************************/
atbm_uint8 atbm_smartconfig_start(void);

#if ATBM_SUPPORT_SMARTCONFIG
/****************************************************************************
* Function:     atbm_smartconfig_stop
*
* Purpose:      停止智能配网 (Stop SmartConfig)
*               停止智能配网模式，退出混杂模式
*
* Parameters:   无
*
* Returns:      成功返回0，失败返回非0值
******************************************************************************/
atbm_uint8 atbm_smartconfig_stop(void);
#endif


/****************************************************************************
* Function:     atbm_wifi_set_fast_connect_mode
*
* Purpose:      设置快速连接模式 (Set Fast Connect Mode)
*               启用快速连接后，设备将跳过扫描直接连接到指定AP
*               适用于已知AP信息的场景，可显著减少连接时间
*
* Parameters:   enable      - 启用/禁用快速连接 (1: 启用, 0: 禁用)
*               channel     - AP工作信道
*               pmk         - 预计算的PMK密钥 (32字节)，可为NULL
*
* Returns:      成功返回0，失败返回负数错误码
*
* Note:         PMK可通过上次成功连接后调用atbm_wifi_get_fast_connect_info获取
******************************************************************************/
atbm_int32 atbm_wifi_set_fast_connect_mode(atbm_uint8 enable, atbm_uint8 channel, atbm_uint8 *pmk);

/****************************************************************************
* Function:     atbm_wifi_get_fast_connect_info
*
* Purpose:      获取快速连接信息 (Get Fast Connect Information)
*               获取当前连接的信道和PMK，用于下次快速连接
*
* Parameters:   channel     - 输出参数，存储当前工作信道
*               pmk         - 输出参数，存储PMK密钥 (32字节缓冲区)
*
* Returns:      成功返回0，失败返回负数错误码
*
* Note:         需要在WiFi连接成功后调用
******************************************************************************/
atbm_int32 atbm_wifi_get_fast_connect_info(atbm_uint8 *channel, atbm_uint8 *pmk);

/****************************************************************************
* Function:     atbm_wifi_fast_link_noscan
*
* Purpose:      无扫描快速连接 (Fast Link Without Scan)
*               使用保存的连接信息直接连接AP，跳过扫描过程
*               适用于从休眠唤醒后快速恢复连接的场景
*
* Parameters:   finfo       - 快速连接信息结构体指针，包含:
*                             enable      - 启用标志
*                             ssid_offset - SSID在config中的偏移
*                             psk_offset  - PSK在config中的偏移
*                             config      - 配置数据
*                             bss         - BSS信息
*                             ie          - IE信息
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
atbm_int32 atbm_wifi_fast_link_noscan(FAST_LINK_INFO * finfo);

/****************************************************************************
* Function:     atbm_wifi_get_linkinfo_noscan
*
* Purpose:      获取无扫描连接信息 (Get Link Info for No-Scan Connect)
*               获取当前连接的完整信息，用于保存到Flash供下次快速连接使用
*
* Parameters:   finfo       - 输出参数，快速连接信息结构体指针
*
* Returns:      无返回值
*
* Note:         需要在WiFi连接成功后调用
******************************************************************************/
atbm_void atbm_wifi_get_linkinfo_noscan(FAST_LINK_INFO * finfo);

/****************************************************************************
* Function:     atbm_wpspbc_start
*
* Purpose:      启动WPS PBC模式 (Start WPS Push Button Configuration)
*               通过按键方式进行WPS配网，需要在AP端同时按下WPS按钮
*
* Parameters:   AP_sta_mode - WiFi工作模式:
*                             ATBM_WIFI_STA_MODE - STA模式
*                             ATBM_WIFI_AP_MODE  - AP模式
*
* Returns:      成功返回0，失败返回非0值
*
* Note:         启动后有2分钟的配对窗口期
******************************************************************************/
atbm_uint8 atbm_wpspbc_start(ATBM_WIFI_MODE AP_sta_mode);

/****************************************************************************
* Function:     atbm_wpspin_start
*
* Purpose:      启动WPS PIN模式 (Start WPS PIN Configuration)
*               通过PIN码方式进行WPS配网
*
* Parameters:   AP_sta_mode - WiFi工作模式
*               pin         - 8位PIN码字符串
*
* Returns:      成功返回0，失败返回非0值
******************************************************************************/
atbm_uint8 atbm_wpspin_start(ATBM_WIFI_MODE AP_sta_mode,const char *pin);

/****************************************************************************
* Function:     atbm_wpsmode_cancel
*
* Purpose:      取消WPS配网 (Cancel WPS Configuration)
*               取消正在进行的WPS配网过程
*
* Parameters:   AP_sta_mode - WiFi工作模式
*
* Returns:      成功返回0，失败返回非0值
******************************************************************************/
atbm_uint8 atbm_wpsmode_cancel(ATBM_WIFI_MODE AP_sta_mode);

/*************************************************************************************
*
*                         WiFi Direct (P2P) 功能函数
*
*  以下函数用于WiFi Direct点对点连接功能
*
**************************************************************************************/

/****************************************************************************
* Function:     atbm_wifi_p2p_start
*
* Purpose:      启动P2P功能 (Start WiFi Direct/P2P)
*               初始化并启动WiFi Direct功能
*
* Parameters:   无
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_wifi_p2p_start(void);

/****************************************************************************
* Function:     atbm_wifi_p2p_find
*
* Purpose:      P2P设备发现 (P2P Device Discovery)
*               搜索周围的P2P设备
*
* Parameters:   timeout     - 搜索超时时间(秒)，0表示持续搜索
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_wifi_p2p_find(int timeout);

/****************************************************************************
* Function:     atbm_wifi_p2p_find_accept
*
* Purpose:      接受P2P连接请求 (Accept P2P Connection in Find Mode)
*               在发现模式下接受对端的连接请求
*
* Parameters:   go_intent   - GO意向值 (0-15)，值越大越倾向于成为GO
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_wifi_p2p_find_accept(int go_intent);

/****************************************************************************
* Function:     atbm_wifi_p2p_find_stop
*
* Purpose:      停止P2P设备发现 (Stop P2P Device Discovery)
*
* Parameters:   timeout     - 保留参数
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_wifi_p2p_find_stop(int timeout);

/****************************************************************************
* Function:     atbm_wifi_p2p_listen_accept
*
* Purpose:      P2P监听并接受连接 (P2P Listen and Accept Connection)
*               进入监听状态等待对端连接
*
* Parameters:   go_intent   - GO意向值 (0-15)
*               channel     - 监听信道
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_wifi_p2p_listen_accept(int go_intent, int channel);

/****************************************************************************
* Function:     atbm_wifi_p2p_show_peers
*
* Purpose:      显示发现的P2P设备列表 (Show Discovered P2P Peers)
*
* Parameters:   无
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_wifi_p2p_show_peers(void);

/****************************************************************************
* Function:     atbm_wifi_p2p_go_start
*
* Purpose:      启动P2P GO模式 (Start P2P Group Owner)
*               作为Group Owner启动P2P组
*
* Parameters:   无
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_wifi_p2p_go_start(void);

/****************************************************************************
* Function:     atbm_wifi_p2p_connect
*
* Purpose:      连接到P2P设备 (Connect to P2P Device)
*               主动连接到指定的P2P设备
*
* Parameters:   mac         - 目标设备的MAC地址 (6字节)
*               go_intent   - GO意向值 (0-15)
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_wifi_p2p_connect(atbm_uint8 *mac, int go_intent);

/****************************************************************************
* Function:     atbm_wifi_p2p_stop
*
* Purpose:      停止P2P功能 (Stop WiFi Direct/P2P)
*               停止P2P功能并释放相关资源
*
* Parameters:   无
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_wifi_p2p_stop(void);

/****************************************************************************
* Function:     atbm_hw_priv_chip_id
*
* Purpose:      获取芯片ID (Get Chip ID)
*               获取WiFi芯片的硬件ID
*
* Parameters:   无
*
* Returns:      芯片ID值
******************************************************************************/
atbm_uint32 atbm_hw_priv_chip_id(void);

/****************************************************************************
* Function:     atbm_set_target_power
*
* Purpose:      设置目标发射功率 (Set Target TX Power)
*               设置指定模式和速率下的发射功率
*
* Parameters:   mode        - 模式 (0: 11b, 1: 11g, 2: 11n, 3: 11ac, 4: 11ax)
*               rateIndex   - 速率索引
*               bw          - 带宽 (0: 20MHz, 1: 40MHz)
*               power       - 功率值 (单位: 0.5dBm)
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_set_target_power(int mode,int rateIndex,int bw,int power);

/****************************************************************************
* Function:     atbm_hweight32
*
* Purpose:      计算32位数中1的个数 (Count Set Bits in 32-bit Value)
*               汉明重量计算，统计二进制表示中1的个数
*
* Parameters:   data_mask   - 32位数据
*
* Returns:      1的个数
******************************************************************************/
int atbm_hweight32(atbm_uint32 data_mask);

/****************************************************************************
* Function:     atbm_scan_get_flag_status
*
* Purpose:      获取扫描标志状态 (Get Scan Flag Status)
*               查询当前扫描功能的启用状态
*
* Parameters:   无
*
* Returns:      扫描标志状态值
******************************************************************************/
int atbm_scan_get_flag_status(void);

/****************************************************************************
* Function:     atbm_scan_set_flag_status
*
* Purpose:      设置扫描标志状态 (Set Scan Flag Status)
*               设置扫描功能的启用状态
*
* Parameters:   val         - 状态值
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_scan_set_flag_status(int val);

/****************************************************************************
* Function:     atbm_wifi_scan_open
*
* Purpose:      打开扫描功能 (Open Scan Function)
*               启用WiFi扫描功能
*
* Parameters:   无
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_wifi_scan_open(void);

/****************************************************************************
* Function:     atbm_wifi_scan_close
*
* Purpose:      关闭扫描功能 (Close Scan Function)
*               禁用WiFi扫描功能
*
* Parameters:   无
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_wifi_scan_close(void);

/****************************************************************************
* Function:     atbm_wifi_scan_start
*
* Purpose:      启动WiFi扫描 (Start WiFi Scan)
*               执行WiFi扫描并返回扫描结果
*
* Parameters:   scan_buf    - 存储扫描结果的缓冲区
*               buf_size    - 缓冲区大小
*
* Returns:      成功返回0，失败返回负数错误码
*
* Note:         扫描结果格式为WLAN_SCAN_RESULT结构体
******************************************************************************/
int atbm_wifi_scan_start(char* scan_buf, atbm_uint32 buf_size);

/****************************************************************************
* Function:     atbm_ap_mode_get_sta_rssi
*
* Purpose:      AP模式获取STA的RSSI (Get STA RSSI in AP Mode)
*               在AP模式下获取已连接STA的信号强度
*
* Parameters:   mac         - STA的MAC地址 (6字节)
*
* Returns:      RSSI值 (dBm)，失败返回0
******************************************************************************/
int atbm_ap_mode_get_sta_rssi(unsigned char *mac);

/****************************************************************************
* Function:     atbmwifi_get_cca_threshold
*
* Purpose:      获取CCA阈值 (Get CCA Threshold)
*               获取当前的空闲信道评估阈值
*
* Parameters:   cca         - 输出参数，存储CCA阈值
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbmwifi_get_cca_threshold(int *cca);

/****************************************************************************
* Function:     atbm_set_ap_channel
*
* Purpose:      设置AP工作信道 (Set AP Channel)
*               动态修改AP模式的工作信道
*
* Parameters:   if_id       - 接口ID
*               channel     - 目标信道
*               chantype    - 信道类型:
*                             0 - 20MHz
*                             1 - 40MHz Upper
*                             2 - 40MHz Lower
*
* Returns:      成功返回0，失败返回负数错误码
******************************************************************************/
int atbm_set_ap_channel(int if_id, int channel, int chantype);


#if ATBM_STA_WOL
/****************************************************************************
* Function:     atbm_set_sta_wow
*
* Purpose:      设置STA唤醒功能 (Set STA Wake-on-WLAN)
*               配置WiFi唤醒功能，允许特定数据包唤醒休眠的设备
*
* Parameters:   enable      - 启用/禁用 (1: 启用, 0: 禁用)
*
* Returns:      成功返回0，失败返回负数错误码
*
* Note:         需要硬件支持WoWLAN功能
******************************************************************************/
	int atbm_set_sta_wow(atbm_uint8 enable);
#endif

#if CONFIG_ATBM_SUPPORT_REKEY
/****************************************************************************
* Function:     atbm_set_rekey_data
*
* Purpose:      设置重密钥数据 (Set Rekey Data)
*               配置GTK重密钥相关数据，用于休眠期间的密钥更新
*
* Parameters:   priv        - WiFi接口私有数据结构指针
*               ptk         - PTK密钥结构指针
*               enable      - 启用/禁用 (1: 启用, 0: 禁用)
*
* Returns:      成功返回0，失败返回负数错误码
*
* Note:         用于支持休眠期间的GTK更新，避免唤醒后需要重新连接
******************************************************************************/
	int atbm_set_rekey_data(struct atbmwifi_vif *priv,struct atbmwifi_wpa_ptk *ptk,  int enable);
#endif


#ifdef __cplusplus
	}
#endif

#endif	/* ATBM_WIFI_H */

