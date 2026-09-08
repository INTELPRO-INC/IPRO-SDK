/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_BT_HOST_ZEPHYR_INIT_H_
#define IPRO_BT_HOST_ZEPHYR_INIT_H_

/*
 * The IPRO Bluetooth host port does not provide Zephyr's init-level linker
 * machinery. Bluetooth services are registered explicitly by the application
 * after bt_enable(), so upstream SYS_INIT users are intentionally compiled out
 * in this port layer.
 */
#define APPLICATION 0
#define POST_KERNEL 0
#define PRE_KERNEL_1 0
#define PRE_KERNEL_2 0
#define CONFIG_APPLICATION_INIT_PRIORITY 0

#define SYS_INIT(init_fn, level, prio)

#endif /* IPRO_BT_HOST_ZEPHYR_INIT_H_ */
