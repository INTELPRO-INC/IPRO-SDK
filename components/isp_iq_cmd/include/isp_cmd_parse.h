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

#ifndef __ISP_CMD_PARSE_H__
#define __ISP_CMD_PARSE_H__

int parse_isp_cmd(char* module, char* argv_reg, char* argv_val, int payload_len_in, char* ret_string);
void isp_cmd2(int argc, char **argv);

#endif /* __ISP_CMD_PARSE_H__ */

