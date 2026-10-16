/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Interrupt seam between the SDK HAL and the Zephyr kernel.
 *
 * bsp/drivers/ipro7_soc/startup/system_soc.h maps the HAL's
 * Interrupt_Handler_Register() / CPU_Interrupt_Enable() macros to the
 * ipro_os_irq_* functions when built for Zephyr. Here they are implemented
 * with the kernel's dynamic-interrupt API, so a HAL-registered ISR ends up
 * in Zephyr's _sw_isr_table like any other driver ISR (same context save,
 * same tracing, same nesting rules).
 *
 * The HAL's ISRs take no argument; Zephyr passes a parameter. A tiny
 * trampoline table keeps the SDK signature intact.
 */

#include <zephyr/kernel.h>
#include <zephyr/irq.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(ipro_irq_glue, CONFIG_INTC_LOG_LEVEL);

/* Also declared in system_soc.h; repeated here so this file does not need
 * the NMSIS headers. */
void ipro_os_irq_register(uint32_t irq, void (*handler)(void));
void ipro_os_irq_enable(uint32_t irq);
void ipro_os_irq_disable(uint32_t irq);
void ipro_os_irq_pending_clear(uint32_t irq);
void (*ipro_os_irq_handler_get(uint32_t irq))(void);

/* From the IPRO ECLIC driver. */
void riscv_clic_irq_pending_clear(uint32_t irq);

static void (*hal_isr[CONFIG_NUM_IRQS])(void);
static bool hal_isr_connected[CONFIG_NUM_IRQS];

static void ipro_irq_trampoline(const void *arg)
{
	uint32_t irq = (uint32_t)(uintptr_t)arg;
	void (*fn)(void) = hal_isr[irq];

	if (fn != NULL) {
		fn();
	} else {
		LOG_ERR("unhandled HAL IRQ %u", irq);
	}
}

void ipro_os_irq_register(uint32_t irq, void (*handler)(void))
{
	__ASSERT(irq < CONFIG_NUM_IRQS, "IRQ %u out of range", irq);
	if (irq >= CONFIG_NUM_IRQS) {
		return;
	}

	unsigned int key = irq_lock();

	hal_isr[irq] = handler;
	if (handler == NULL) {
		/* SDK idiom for "unregister" (hal_isp.c: CPU_Interrupt_Disable,
		 * then Register(irq, NULL)): leave the line disabled instead of
		 * re-enabling it into the trampoline's unhandled path. */
		irq_disable(irq);
		irq_unlock(key);
		return;
	}
	/*
	 * The SDK registers with level 1 / priority 0 (INTLEVEL/INTPRIORITY);
	 * CONFIG_IPRO_HAL_IRQ_PRIO is the Zephyr priority used instead so
	 * applications can shift the whole HAL relative to native drivers.
	 *
	 * Connect the trampoline once per line. HAL drivers call
	 * Interrupt_Handler_Register() on every (re)init (hal_pwm.c on each
	 * channel start, hal_dma.c on controller init) and
	 * irq_connect_dynamic() asserts if the line is already enabled; the
	 * trampoline reads hal_isr[] at run time, so re-registering only has
	 * to update the table.
	 */
	if (!hal_isr_connected[irq]) {
		irq_connect_dynamic(irq, CONFIG_IPRO_HAL_IRQ_PRIO, ipro_irq_trampoline,
				    (const void *)(uintptr_t)irq, 0);
		hal_isr_connected[irq] = true;
	}
	irq_unlock(key);

	/* The SDK macro also enables the line (ECLIC_Register_IRQ). */
	irq_enable(irq);
}

void ipro_os_irq_enable(uint32_t irq)
{
	if (irq < CONFIG_NUM_IRQS) {
		irq_enable(irq);
	}
}

void ipro_os_irq_disable(uint32_t irq)
{
	if (irq < CONFIG_NUM_IRQS) {
		irq_disable(irq);
	}
}

void ipro_os_irq_pending_clear(uint32_t irq)
{
	if (irq < CONFIG_NUM_IRQS) {
		riscv_clic_irq_pending_clear(irq);
	}
}

void (*ipro_os_irq_handler_get(uint32_t irq))(void)
{
	return irq < CONFIG_NUM_IRQS ? hal_isr[irq] : NULL;
}

/*
 * ECLIC_Register_IRQ(): the SDK's lower-level registration used directly by
 * the 802.15.4 lower MAC and the prebuilt BLE controller (interrupt.c is not
 * compiled under Zephyr). Level/priority map onto the Zephyr priority the
 * IPRO ECLIC driver encodes as the interrupt level; trigger flags are the
 * ECLIC encoding Zephyr's CLIC layer already understands.
 */
int32_t ECLIC_Register_IRQ(uint32_t irq, uint8_t shv, uint8_t trig_mode, uint8_t lvl,
			   uint8_t priority, void *handler)
{
	ARG_UNUSED(shv);
	ARG_UNUSED(priority);

	if (irq >= CONFIG_NUM_IRQS) {
		return -1;
	}

	unsigned int key = irq_lock();

	hal_isr[irq] = (void (*)(void))handler;
	irq_connect_dynamic(irq, lvl, ipro_irq_trampoline, (const void *)(uintptr_t)irq,
			    trig_mode & 0x3);
	irq_unlock(key);
	irq_enable(irq);
	return 0;
}

/*
 * __irq_save()/__irq_restore() are used by the flash HAL around XIP-unsafe
 * windows. They are the SDK spelling of irq_lock()/irq_unlock() and live in
 * interrupt.c, which is not compiled under Zephyr.
 */
uint32_t __irq_save(void)
{
	return irq_lock();
}

void __irq_restore(uint32_t flags)
{
	irq_unlock(flags);
}
