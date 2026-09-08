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

#ifndef ATBM_BRIDGE_H

#define ATBM_BRIDGE_H
struct atbm_buff *get_sta_deliver_skb(struct atbm_buff *skb);

struct atbm_buff *get_ap_deliver_skb(struct atbm_buff *skb);

atbm_void atbm_brpool_init(struct atbmwifi_common *hw_priv);

atbm_void atbm_brpool_deinit(struct atbmwifi_common *hw_priv);

int remove_item_from_brpool(int id);

#endif
