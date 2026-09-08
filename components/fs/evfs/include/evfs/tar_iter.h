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

#ifndef TAR_ITER_H
#define TAR_ITER_H

#include "tar_common.h"



typedef struct TarFileIterator {
  EvfsFile *fd;

  TarHeader cur_header;

  evfs_off_t header_offset;  // Offset within tar file
  evfs_off_t file_size;      // Size of current archived file

} TarFileIterator;

#ifdef __cplusplus
extern "C" {
#endif

void tar_iter_init(TarFileIterator *tar_it, EvfsFile *fd);
void tar_iter_close(TarFileIterator *tar_it);
bool tar_iter_seek(TarFileIterator *tar_it, evfs_off_t offset);
#define tar_iter_begin(r)  tar_iter_seek(r, 0)
bool tar_iter_next(TarFileIterator *tar_it);
#define tar_iter_reset(r) tar_iter_seek((r), (r)->header_offset)

evfs_off_t tar_iter_file_offset(TarFileIterator *tar_it);

#ifdef __cplusplus
}
#endif

#endif // TAR_ITER_H

