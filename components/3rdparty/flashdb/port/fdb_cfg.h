/*
 * FlashDB configuration for IPRO7 platform
 * Based on fdb_cfg_template.h
 */

#ifndef _FDB_CFG_H_
#define _FDB_CFG_H_

#include <generated/autoconf.h>

/* KVDB feature */
#ifdef CONFIG_FLASHDB_USING_KVDB
#define FDB_USING_KVDB
#endif

/* TSDB feature */
#ifdef CONFIG_FLASHDB_USING_TSDB
#define FDB_USING_TSDB
#endif

/* Using FAL (Flash Abstraction Layer) storage mode */
#define FDB_USING_FAL_MODE

#ifdef FDB_USING_FAL_MODE
/* NOR flash write granularity: 1 bit */
#define FDB_WRITE_GRAN 1
#endif

/* MCU Endian: RISC-V is little-endian */
/* #define FDB_BIG_ENDIAN */

/* Log print */
#include <stdio.h>
#define FDB_PRINT(...)  printf(__VA_ARGS__)

/* Debug output */
#ifdef CONFIG_FLASHDB_DEBUG
#define FDB_DEBUG_ENABLE
#endif

#endif /* _FDB_CFG_H_ */
