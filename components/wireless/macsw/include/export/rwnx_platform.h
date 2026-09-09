/**
 ****************************************************************************************
 *
 * @file rwnx_platform.h
 *
 * @brief RWNX reference platform initialization.
 *
 * Copyright (C) RivieraWaves 2019-2021
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup PLATFORM_DRIVERS
 * @{
 ****************************************************************************************
 */

#include "mac_types.h"
/**
 ****************************************************************************************
 * @brief Initialize RWNX reference platform.
 *
 * It initializes all platform specific drivers (IPC, CRM, DMA, ...)
 ****************************************************************************************
 */
void rwnx_platform_init(void);

#ifdef CFG_IPRO_WIFI_PS_ENABLE
/**
 ****************************************************************************************
 * @brief Initialize RWNX reference platform.
 *
 * It initializes all platform specific drivers (IPC, CRM, DMA, ...)
 ****************************************************************************************
 */
void rwnx_platform_pds_init(void);
#endif

/// @}
