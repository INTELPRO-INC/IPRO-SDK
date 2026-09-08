/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_BT_HOST_ZEPHYR_ARCH_INTERFACE_H_
#define IPRO_BT_HOST_ZEPHYR_ARCH_INTERFACE_H_

/* The Bluetooth host port runs on FreeRTOS, not the Zephyr scheduler.  Imported
 * Zephyr headers include this file for optional architecture hooks; the IPRO
 * port provides those hooks directly in port/kernel.c where they are used. */

#endif /* IPRO_BT_HOST_ZEPHYR_ARCH_INTERFACE_H_ */
