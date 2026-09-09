/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * lwIP system-architecture types for the FreeRTOS port. This is the header
 * lwip/sys.h includes as "arch/sys_arch.h"; the implementation lives in
 * ../FreeRTOS/sys_arch.c. Names are the FreeRTOS V8+ typedefs, so the port
 * does not depend on configENABLE_BACKWARD_COMPATIBILITY.
 */

#ifndef LWIP_ARCH_SYS_ARCH_H
#define LWIP_ARCH_SYS_ARCH_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

typedef SemaphoreHandle_t sys_sem_t;
typedef SemaphoreHandle_t sys_mutex_t;
typedef QueueHandle_t     sys_mbox_t;
typedef TaskHandle_t      sys_thread_t;

#define SYS_MBOX_NULL   ((sys_mbox_t)0)
#define SYS_SEM_NULL    ((sys_sem_t)0)

/* Stack depth handed to xTaskCreate() when a caller passes 0: in StackType_t
 * words, like every other stack size in this port. */
#define SYS_DEFAULT_THREAD_STACK_DEPTH  configMINIMAL_STACK_SIZE

/* lwIP's fallback sys_msleep() builds a semaphore just to wait on it.
 * FreeRTOS has a delay primitive; use it. */
#define sys_msleep(ms)  vTaskDelay(pdMS_TO_TICKS(ms))

#endif /* LWIP_ARCH_SYS_ARCH_H */
