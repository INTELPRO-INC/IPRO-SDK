/* SPDX-License-Identifier: MIT
Copyright 2020 Kevin Thibedeau
(kevin 'period' thibedeau 'at' gmail 'punto' com)
FreeRTOS port Copyright 2025 IPRO Technology Co., Ltd.

See LICENSE in the EVFS project root for details
*/

/*
------------------------------------------------------------------------------
Embedded Virtual Filesystem

  Configuration for thread API
------------------------------------------------------------------------------
*/

#ifndef EVFS_CUSTOM_THREADING_H
#define EVFS_CUSTOM_THREADING_H

// Enable declarations in evfs_internal.h
#define EVFS_SOURCE_USES_LOCK

// ******************** FreeRTOS ********************
#if defined USE_FREERTOS
#  include "FreeRTOS.h"
#  include "semphr.h"

typedef SemaphoreHandle_t EvfsLock;

// FreeRTOS requires dynamic init
#  define LOCK_INITIALIZER   NULL


// ******************** C11 threads ********************
#elif defined USE_C11_THREADS
#  include <threads.h>

typedef mtx_t EvfsLock;

// This is just a placeholder C11 threads require dynamic init
#  define LOCK_INITIALIZER   {0}


// ******************** pthreads ********************
#elif defined USE_PTHREADS
#  include <pthread.h>

typedef pthread_mutex_t EvfsLock;

#  define LOCK_INITIALIZER   PTHREAD_MUTEX_INITIALIZER
#  define HAVE_STATIC_LOCK_INIT


// ******************** No threading ********************
#elif defined EVFS_NO_THREADING
/* No threading support - single threaded operation only */
typedef int EvfsLock;
#  define LOCK_INITIALIZER   0


// ******************** Default ********************
#else
#  error "EVFS needs a threading library configured (USE_FREERTOS, USE_C11_THREADS, USE_PTHREADS, or EVFS_NO_THREADING)"
#endif


#endif // EVFS_CUSTOM_THREADING_H
