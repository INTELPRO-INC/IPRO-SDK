/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */


#ifndef _LWIPHOOKS_H_
#define _LWIPHOOKS_H_

#define LWIP_HOOK_UNKNOWN_ETH_PROTOCOL  net_eth_receive

/**
 ****************************************************************************************
 * @brief LWIP hook to process ethernet packet not supported.
 *
 * Check if a socket has been created for this (netif, ethertype) couple.
 * If so push the buffer in the socket buffer list. (no buffer copy is done)
 *
 * @param[in] pbuf  Buffer containing the ethernet frame
 * @param[in] netif Pointer to the network interface that received the frame
 *
 * @return ERR_OK if a L2 socket exist for this frame and buffer has been successfully
 * pushed, ERR_MEM/ERR_VAL otherwise.
 ****************************************************************************************
 */
err_t net_eth_receive(struct pbuf *pbuf, struct netif *netif);

#endif /* _LWIPHOOKS_H_ */
