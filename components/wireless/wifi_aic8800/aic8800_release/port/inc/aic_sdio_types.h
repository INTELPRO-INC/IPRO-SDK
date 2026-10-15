/*
 * Copyright (C) 2018-2025 AICSemi Ltd.
 *
 * All Rights Reserved
 *
 * Host-owned SDIO type definitions.
 * Provides SDIO constants without platform dependencies.
 */

#ifndef _AIC_SDIO_TYPES_H_
#define _AIC_SDIO_TYPES_H_

#include <stdint.h>

/* SDIO function numbers */
#define FUNC_0              0
#define FUNC_1              1
#define FUNC_2              2
#define SDIOM_MAX_FUNCS     3

/* TX slist max entries */
#define SDIO_TX_SLIST_MAX   136

/* SDIO scatter-gather list (ADMA mode) */
#ifdef CONFIG_SDIO_ADMA
typedef struct {
    void     *addr;
    uint32_t  len;
} sdio_sg_list;
#endif

#endif /* _AIC_SDIO_TYPES_H_ */
