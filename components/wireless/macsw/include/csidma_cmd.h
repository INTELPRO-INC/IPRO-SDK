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

#ifndef _CISDMA_CMD_H_
#define _CISDMA_CMD_H_

void cmd_csidma_start(int argc, char **argv);
void cmd_csidma_stop(int argc, char **argv);
void cmd_csidma_ready(int argc, char **argv);
void cmd_csidma_debug(int argc, char **argv);
void cmd_csidma_force_ftm(int argc, char **argv);
void cmd_csidma_force_ftm_mac_set(int argc, char **argv);

#endif
