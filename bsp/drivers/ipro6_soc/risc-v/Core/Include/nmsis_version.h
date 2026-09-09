/*
 * Copyright (c) 2019 Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * NMSIS Version Header
 */
#ifndef __NMSIS_VERSION_H
#define __NMSIS_VERSION_H

/* NMSIS Version definitions */
#define __NMSIS_VERSION_MAIN    (1U)    /**< NMSIS main version */
#define __NMSIS_VERSION_SUB     (0U)    /**< NMSIS sub version */
#define __NMSIS_VERSION_PATCH   (0U)    /**< NMSIS patch version */

/** NMSIS version number (combined) */
#define __NMSIS_VERSION         ((__NMSIS_VERSION_MAIN << 16) | \
                                 (__NMSIS_VERSION_SUB  << 8 ) | \
                                 (__NMSIS_VERSION_PATCH     ))

#endif /* __NMSIS_VERSION_H */
