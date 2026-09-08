/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_BT_HOST_ZEPHYR_SYSCALLS_KERNEL_H_
#define IPRO_BT_HOST_ZEPHYR_SYSCALLS_KERNEL_H_

/*
 * Zephyr normally generates this header to route userspace syscall wrappers
 * to kernel implementations.  The IPRO FreeRTOS port has no Zephyr userspace;
 * its kernel.h exposes the direct compatibility functions instead.
 */
#include <zephyr/kernel.h>

#endif /* IPRO_BT_HOST_ZEPHYR_SYSCALLS_KERNEL_H_ */
