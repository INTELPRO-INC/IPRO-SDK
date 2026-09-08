/* SPDX-License-Identifier: MIT
 * EVFS Build Configuration for IPRO SDK (FreeRTOS)
 */

#ifndef EVFS_BUILD_CONFIG_H
#define EVFS_BUILD_CONFIG_H

#define EVFS_VERSION  "1.0.0"

/* Thread library - using FreeRTOS */
#define USE_FREERTOS
#define EVFS_USE_THREADING

/* Memory allocation */
#include "FreeRTOS.h"
#define EVFS_MALLOC     pvPortMalloc
#define EVFS_FREE       vPortFree

#endif /* EVFS_BUILD_CONFIG_H */
