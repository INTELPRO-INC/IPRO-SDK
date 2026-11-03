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

#include "generated/autoconf.h"

/* Speaker-embedding dimension. 256 is what res2dnet_ep75_int8 and mbv2.inm
 * produce, the models speaker_embed.c is written for. Originally defined in
 * tflite_infer.h on the Linux reference; broken out here so gallery.* has no
 * TFLite dependency on the RTOS target.
 *
 * An app that brings its own embedding model sets CONFIG_AI_SPK_EMBEDDING_DIM
 * (ipro_reef_teach voice mode: 192) and gallery.* / diarize.* follow it; speaker_embed()
 * is then not usable, it only knows the 256-d models. */
#ifdef CONFIG_AI_SPK_EMBEDDING_DIM
#define EMBEDDING_DIM CONFIG_AI_SPK_EMBEDDING_DIM
#else
#define EMBEDDING_DIM 256
#endif

#endif /* EMBEDDING_DIM_H */
