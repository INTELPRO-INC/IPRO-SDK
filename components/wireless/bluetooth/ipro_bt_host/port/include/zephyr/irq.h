/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_BT_HOST_ZEPHYR_IRQ_H_
#define IPRO_BT_HOST_ZEPHYR_IRQ_H_

unsigned int irq_lock(void);
void irq_unlock(unsigned int key);

#endif
