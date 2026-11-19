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

/**
 * @file thread_ftd_cli.h
 * @brief IPRO Thread FTD CLI Commands Header
 * 
 * This header defines the CLI interface for Thread FTD application.
 */

#ifndef THREAD_FTD_CLI_H
#define THREAD_FTD_CLI_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize Thread FTD CLI commands
 * 
 * This function registers all Thread-related CLI commands.
 */
void thread_ftd_cli_init(void);

#ifdef __cplusplus
}
#endif

#endif /* THREAD_FTD_CLI_H */
