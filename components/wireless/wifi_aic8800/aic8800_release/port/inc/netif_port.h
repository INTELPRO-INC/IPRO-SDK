/*
 * Copyright (C) 2018-2024 AICSemi Ltd.
 *
 * All Rights Reserved
 *
 * Network port header: ops-dispatch macros.
 */

#ifndef NET_AL_H_
#define NET_AL_H_

#include "aic_netbuf.h"
#include "netif_def.h"

/* Common macros */
#define AICWF_WIFI_HDR          0
#define AICWF_WIFI_HDR_LEN      0

#ifndef CONFIG_LWIP
typedef int             err_t;
#endif

extern struct aic_netif_dev netif_dev;

/* ---- Network abstraction — platform must implement ALL functions ---- */

/* Platform buffer functions */
void *aicwf_platform_net_buf_rx_alloc(uint32_t length);
void *aicwf_platform_net_buf_rx_desc(uint8_t *buffer);
void  aicwf_platform_net_buf_rx_free(void *ref);
void  aicwf_platform_net_buf_tx_free(void *ref);
/* [PLATFORM] RX 零拷贝回退: 分配 pbuf 并将 eth_data 拷入其 payload 后返回
 * pbuf 指针（host 层不持有 lwip 头，由 platform 层完成 alloc+copy）。
 * 失败返回 NULL。CONFIG_RX_NOCOPY 下 host 库 rwnx_utils.c 调用。 */
void  *aicwf_platform_net_buf_rx_alloc_copy(const uint8_t *eth_data, uint32_t len);

/* [PLATFORM] nocopy TX (NET_IF_UAP_NCPYBUF=1): SDIO 聚合阶段拷贝 hook。
 * 单段 pbuf 直接 memcpy；链式 pbuf 按段 scatter copy（首段含 hostdesc 区域）。
 * 由 host 库 sdio_co.c 调用，platform netif_port.c 实现（需 lwip pbuf 访问）。 */
void  aicwf_platform_nocopy_aggr_copy(void *pmsg, const uint8_t *src,
                                      uint32_t len, uint8_t *dst);

#ifdef CONFIG_RX_NOCOPY
/* [PLATFORM] RX 零拷贝 hook (CONFIG_RX_NOCOPY=1): 建 custom pbuf 直指
 * SDIO/USB RX node 缓冲内以太网帧并持 node 引用；失败返回 NULL（调用方回退
 * alloc+memcpy）。由 host 库 fhost_rx.c / rwnx_utils.c 调用，platform
 * netif_port.c 实现（需 lwip custom pbuf 支持）。 */
void  *aicwf_platform_rx_zero_copy(uint8_t *eth_data, uint32_t len, void *node);
/* [PLATFORM] RX 零拷贝反初始化: 清理 custom pbuf 头池, 需在 node 池 free 前调用 */
void   aicwf_rx_zcopy_deinit(void);
#endif /* CONFIG_RX_NOCOPY */

/* Interface management */
int   aicwf_net_if_add(net_if_t *net_if, const uint32_t *ipaddr,
                       const uint32_t *netmask, const uint32_t *gw, void *vif);
int   aicwf_net_if_get_name(net_if_t net_if, char *buf, int len);
int   aicwf_net_if_get_wifi_idx(net_if_t net_if);
void  aicwf_net_if_up(net_if_t net_if);
void  aicwf_net_if_down(net_if_t net_if);
void  aicwf_net_if_set_default(net_if_t net_if);
void  aicwf_net_if_set_ip(net_if_t net_if, uint32_t ip, uint32_t mask, uint32_t gw);
int   aicwf_net_if_get_ip(net_if_t net_if, uint32_t *ip, uint32_t *mask, uint32_t *gw);
int   aicwf_net_init(void);
void  aicwf_net_deinit(void);
void  aicwf_net_predeinit(void);
uint8_t aicwf_net_if_is_up(net_if_t net_if);
void  aicwf_netif_mode_switch(uint8_t mode);
uint8_t aicwf_net_if_dhcp_done(uint8_t act);
void  aicwf_net_if_plat_init(void);
net_if_t aicwf_net_if_get(uint8_t index);

/* Buffer management */
net_buf_tx_t *aicwf_net_buf_tx_alloc(const uint8_t *payload, uint32_t length);
void  aicwf_net_buf_tx_free(net_buf_tx_t *buf);
uint8_t *aicwf_net_buf_rx_alloc(uint32_t length);
void  aicwf_net_buf_rx_free(void *msg);
bool  aicwf_net_buf_tx_free_list_is_full(void);

/* Data path */
int   aicwf_tx_eth_data_process(uint8_t *data, uint32_t len, void *msg);
int   aicwf_rx_eth_data_process(uint8_t *pdata, uint16_t len, net_if_t netif,
                                void *lwip_msg, uint8_t is_reord);

/* DHCP */
int   aicwf_net_dhcp_start(net_if_t net_if);
void  aicwf_net_dhcp_stop(net_if_t net_if);
int   aicwf_net_dhcp_start_status(void);
int   aicwf_net_dhcp_release(net_if_t net_if);
int   aicwf_net_dhcp_address_obtained(net_if_t net_if);
#if defined(CONFIG_DHCPD) && CONFIG_DHCPD
void  aicwf_net_if_dhcp_server_start(net_if_t net_if);
void  aicwf_net_if_dhcp_server_stop(net_if_t net_if);
#endif

/* DNS */
int   aicwf_net_set_dns(uint32_t dns_server);
int   aicwf_net_get_dns(uint32_t *dns_server);

/* TCP ACK filter */
#ifdef CONFIG_FILTER_TCP_ACK
int   aicwf_tx_eth_data_no_tcpack_process(uint8_t *data, uint32_t len, void *msg);
#endif

/* IP config */
uint32_t aicwf_net_if_get_tousedip(void);
void     aicwf_net_if_set_tousedip(uint32_t ip);
uint32_t aicwf_net_if_get_netmask(void);
void     aicwf_net_if_set_netmask(uint32_t netmask);

#endif /* NET_AL_H_ */
