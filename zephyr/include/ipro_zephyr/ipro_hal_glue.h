/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * SDK-HAL facade used by the Zephyr driver adapters.
 *
 * Several SDK HAL entry points share their names with Zephyr's own driver
 * API (flash_read(), wdt_feed(), dma_reload(), the GPIO_PULL_* macros...),
 * so a translation unit cannot include both worlds. The adapters therefore
 * stay pure Zephyr and call these ipro_hal_* functions, implemented in
 * zephyr/hal_glue/ (one file per block) which include only the SDK side. No Zephyr driver
 * header is visible in the glue, no SDK header in the drivers.
 */

#ifndef IPRO_ZEPHYR_IPRO_HAL_GLUE_H_
#define IPRO_ZEPHYR_IPRO_HAL_GLUE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- GPIO (hal_glue/ipro_gpio_glue.c) --------------------------------- */
enum ipro_hal_gpio_dir {
	IPRO_HAL_GPIO_DISCONNECTED,
	IPRO_HAL_GPIO_INPUT,
	IPRO_HAL_GPIO_OUTPUT,
};

enum ipro_hal_gpio_pull {
	IPRO_HAL_GPIO_PULL_NONE,
	IPRO_HAL_GPIO_PULL_UP,
	IPRO_HAL_GPIO_PULL_DOWN,
};

enum ipro_hal_gpio_trig {
	IPRO_HAL_GPIO_TRIG_RISING_EDGE,
	IPRO_HAL_GPIO_TRIG_FALLING_EDGE,
	IPRO_HAL_GPIO_TRIG_HIGH_LEVEL,
	IPRO_HAL_GPIO_TRIG_LOW_LEVEL,
};

int ipro_hal_gpio_configure(uint8_t pad, enum ipro_hal_gpio_dir dir, enum ipro_hal_gpio_pull pull,
			    int init_level);
void ipro_hal_gpio_write(uint8_t pad, int level);
int ipro_hal_gpio_read(uint8_t pad);
void ipro_hal_gpio_int_enable(uint8_t pad, enum ipro_hal_gpio_trig trig);
void ipro_hal_gpio_int_disable(uint8_t pad);
bool ipro_hal_gpio_int_pending(uint8_t pad);
void ipro_hal_gpio_int_clear(uint8_t pad);
uint32_t ipro_hal_gpio_in_reg(uint8_t port);
uint32_t ipro_hal_gpio_out_reg(uint8_t port);
void ipro_hal_gpio_out_reg_write(uint8_t port, uint32_t value);

/* ---- Flash (hal_glue/ipro_flash_glue.c) -------------------------------- */
int ipro_hal_flash_init(void);
uint32_t ipro_hal_flash_size(void);
int ipro_hal_flash_read(uint32_t addr, void *buf, size_t len);
int ipro_hal_flash_write(uint32_t addr, const void *buf, size_t len);
int ipro_hal_flash_erase(uint32_t addr, size_t len);

/* ---- Watchdog (hal_glue/ipro_wdt_glue.c) ------------------------------- */
void ipro_hal_wdt_start(bool irq_mode, uint32_t timeout_ms);
void ipro_hal_wdt_stop(void);
void ipro_hal_wdt_feed(void);
void ipro_hal_wdt_set_isr(void (*isr)(void));

/* ---- DMA (hal_glue/ipro_dma_glue.c) ------------------------------------ */
enum ipro_hal_dma_dir {
	IPRO_HAL_DMA_M2M,
	IPRO_HAL_DMA_M2P,
	IPRO_HAL_DMA_P2M,
};

struct ipro_hal_dma_cfg {
	uint8_t channel;
	enum ipro_hal_dma_dir dir;
	uint32_t src_req;
	uint32_t dst_req;
	bool src_inc;
	bool dst_inc;
	uint8_t src_width_bytes;  /* 1, 2, 4 */
	uint8_t dst_width_bytes;
	uint8_t src_burst_beats;  /* 1, 4, 8, 16 */
	uint8_t dst_burst_beats;
	bool cyclic;
	/* Two equal, contiguous halves of one buffer (2 x the reload size)
	 * played in a loop, callback at the end of each half (HAL
	 * DMA_LLI_PINGPONG_MODE); implies cyclic. */
	bool pingpong;
	void (*callback)(uint8_t channel, bool error);
};

int ipro_hal_dma_controller_init(void);
int ipro_hal_dma_channel_configure(const struct ipro_hal_dma_cfg *cfg);
int ipro_hal_dma_channel_reload(uint8_t channel, uint32_t src, uint32_t dst, size_t size);
int ipro_hal_dma_channel_start(uint8_t channel);
void ipro_hal_dma_channel_stop(uint8_t channel);
bool ipro_hal_dma_channel_busy(uint8_t channel);
/* ISR-safe re-arm of a channel already configured by a reload of at most
 * one descriptor (<= 4064 bytes): patches the descriptor in place and starts. */
int ipro_hal_dma_channel_restart(uint8_t channel, uint32_t src, uint32_t dst, size_t size);
/* Transfers still pending on a running channel (source-width units). */
size_t ipro_hal_dma_channel_remaining(uint8_t channel);
/* Drop D-cache lines of a DMA destination range before the CPU reads it
 * (the glue does this on completion; partial readers call it themselves). */
void ipro_hal_dma_cache_invalidate(uint32_t addr, size_t size);

/*
 * D-cache maintenance (ipro_platform_glue.c, always available). Zephyr's
 * sys_cache_* are no-ops on this port (CONFIG_CACHE is off), so anything that
 * shares memory with a DMA engine or a bus master uses these:
 * clean = write back before the master reads, invalidate = drop before the
 * CPU reads what the master wrote.
 */
void ipro_hal_cache_clean(uint32_t addr, size_t size);
void ipro_hal_cache_invalidate(uint32_t addr, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_ZEPHYR_IPRO_HAL_GLUE_H_ */
