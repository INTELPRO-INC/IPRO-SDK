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

#ifndef EMBEDDING_DIM_H
#define EMBEDDING_DIM_H

/* Speaker-embedding dimension produced by res2dnet_ep75_int8.
 * Originally defined in tflite_infer.h on the Linux reference; broken out
 * here so gallery.* has no TFLite dependency on the RTOS target. */
#define EMBEDDING_DIM 256

#endif /* EMBEDDING_DIM_H */
