/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * The SDK Wi-Fi stack's platform abstraction (platform_al.h) on Zephyr,
 * plus the start-up sequence the FreeRTOS applications perform by hand:
 * register the MAC ISR, run the firmware task (wifi_main), wait for
 * CODE_WIFI_ON_INIT_DONE, wifi_mgmr_init(), wait for CODE_WIFI_ON_MGMR_DONE.
 */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdarg.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
#include <string.h>
#include "platform_al.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"
#include <ipro_zephyr/ipro_wifi.h>
#include "ipro_irq.h"
#include "ipro6.h"

LOG_MODULE_REGISTER(ipro_wifi, CONFIG_LOG_DEFAULT_LEVEL);

extern void *rtos_malloc(uint32_t size);
extern void rtos_free(void *ptr);

/* ---- memory / identity --------------------------------------------------- */

void *platform_malloc(uint32_t size)
{
	return rtos_malloc(size);
}

void platform_free(void *mem_ptr)
{
	rtos_free(mem_ptr);
}

/* platform_get_random() comes from hal_glue/ipro_platform_glue.c (sys_rand). */
extern int platform_get_random(uint8_t *data, uint32_t len);

/* The RF parameter adapter logs through the SDK libc's vprint(). */
void vprint(const char *fmt, va_list argp)
{
	vprintk(fmt, argp);
}

int platform_get_mac(uint8_t *mac)
{
	extern int mfg_media_read_macaddr_with_lock(uint8_t mac[6], uint8_t reload);
	static uint8_t cached[6];
	static bool have;

	if (have) {
		memcpy(mac, cached, 6);
		return 0;
	}
	/* Production parts carry a factory MAC in efuse / mfg media; blank
	 * bench parts get a locally-administered address from the TRNG,
	 * fixed for the life of the boot. */
	if (mfg_media_read_macaddr_with_lock(cached, 1) != 0 ||
	    (cached[0] | cached[1] | cached[2] | cached[3] | cached[4] | cached[5]) == 0) {
		if (platform_get_random(cached, 6) != 0) {
			return -1;
		}
		cached[0] = (cached[0] & 0xfe) | 0x02;
		LOG_WRN("no factory MAC, using random %02x:%02x:%02x:%02x:%02x:%02x", cached[0],
			cached[1], cached[2], cached[3], cached[4], cached[5]);
	}
	have = true;
	memcpy(mac, cached, 6);
	return 0;
}

/* ---- timers -------------------------------------------------------------- */

struct sched_event {
	struct k_timer timer;
	platform_event_func_t handler;
	void *arg;
};

static void sched_expiry(struct k_timer *timer)
{
	struct sched_event *ev = CONTAINER_OF(timer, struct sched_event, timer);

	/* FreeRTOS passes the timer handle; callers reach their arg through it. */
	ev->handler(ev);
}

platform_event_handler_t platform_add_schedule_event(platform_event_func_t handler, void *arg,
						      uint32_t ms, uint8_t repeat)
{
	struct sched_event *ev = rtos_malloc(sizeof(*ev));

	if (ev == NULL) {
		return NULL;
	}
	ev->handler = handler;
	ev->arg = arg;
	k_timer_init(&ev->timer, sched_expiry, NULL);
	k_timer_start(&ev->timer, K_MSEC(ms), repeat ? K_MSEC(ms) : K_NO_WAIT);
	return ev;
}

int platform_delete_schedule_event(platform_event_handler_t handler)
{
	struct sched_event *ev = handler;

	if (ev == NULL) {
		return -1;
	}
	k_timer_stop(&ev->timer);
	rtos_free(ev);
	return 0;
}

/* pvTimerGetTimerID() equivalent for the handlers that need their arg */
void *ipro_wifi_sched_event_arg(platform_event_handler_t handler)
{
	return ((struct sched_event *)handler)->arg;
}

/* ---- events -------------------------------------------------------------- */

static ipro_wifi_event_cb_t listener;
static void *listener_user;
static K_SEM_DEFINE(init_done, 0, 1);
static K_SEM_DEFINE(mgmr_done, 0, 1);
static bool started;

void ipro_wifi_set_event_listener(ipro_wifi_event_cb_t cb, void *user)
{
	listener = cb;
	listener_user = user;
}

void platform_post_event(int catalogue, int code1, int code2)
{
	ARG_UNUSED(catalogue);
	switch (code1) {
	case CODE_WIFI_ON_INIT_DONE:
		k_sem_give(&init_done);
		break;
	case CODE_WIFI_ON_MGMR_DONE:
		k_sem_give(&mgmr_done);
		break;
	default:
		break;
	}
	if (listener != NULL) {
		listener(code1, code2, listener_user);
	}
}

/* ---- hooks the stack calls when defined --------------------------------- */

void platform_hook_prevent_sleep(enum PSM_EVENT event, uint8_t prevent)
{
	ARG_UNUSED(event); ARG_UNUSED(prevent);
}

/* ---- start-up ------------------------------------------------------------ */

extern void wifi_main(void *param);
extern void interrupt0_handler(void);

/* CPU spent in the MAC ISR (mcycle @ MCU clock); read by `wifi_ipro txsplit`. */
volatile uint64_t ipro_wifi_isr_cycles;
volatile uint32_t ipro_wifi_isr_count;

static inline uint32_t rdcycle(void)
{
	uint32_t c;

	__asm__ volatile("csrr %0, mcycle" : "=r"(c));
	return c;
}

/* The MAC's TX watchdog can be left disarmed by its own recovery paths; the
 * SDK re-arms it from an always-alive task (sdiowifi keep-ready / the LP
 * tick). Do the same from a kernel timer. */
extern void txl_watchdog_backstop_poll(void);
extern void dbg_mac_reset_request(void);
extern volatile uint32_t fhost_tx_cfm_cnt;
extern long ipro_tx_inflight_get(void);
uint32_t ipro_wifi_cfm_watchdog_resets;

/* Confirm watchdog: the MAC HW has been seen to wedge with a handful of
 * frames in its BE queue (head pointer frozen, DMA state 2) without any of
 * the SDK's own detectors firing; the link then dies of beacon loss. A MAC
 * reset (the SDK's KE_EVT_RESET path) recovers it, so request one when
 * frames are in flight and no confirmation has arrived for
 * CONFIG_IPRO_WIFI_CFM_WATCHDOG_MS. */
static void backstop_tick(struct k_timer *t)
{
	static uint32_t last_cfm, stuck_ms;

	ARG_UNUSED(t);
	txl_watchdog_backstop_poll();
	if (ipro_tx_inflight_get() > 0 && fhost_tx_cfm_cnt == last_cfm) {
		stuck_ms += 100;
		if (stuck_ms >= CONFIG_IPRO_WIFI_CFM_WATCHDOG_MS) {
			stuck_ms = 0;
			ipro_wifi_cfm_watchdog_resets++;
			extern void macsw_txl_diag_print(void), fhost_tx_diag_print(void);

			printk("[wifi] no TX confirm for %d ms with %ld frames in flight: MAC reset\n",
			       CONFIG_IPRO_WIFI_CFM_WATCHDOG_MS, ipro_tx_inflight_get());
			macsw_txl_diag_print();
			fhost_tx_diag_print();
			dbg_mac_reset_request();
		}
	} else {
		stuck_ms = 0;
	}
	last_cfm = fhost_tx_cfm_cnt;
}
K_TIMER_DEFINE(backstop_timer, backstop_tick, NULL);

/* Word memcpy resident in the hot path (libc's stays in flash for the early
 * boot). CONFIG_IPRO_WIFI_TX_PROFILE adds a caller histogram. */
#ifdef CONFIG_IPRO_WIFI_TX_PROFILE
struct ipro_memcpy_site { uint32_t ra, cycles, bytes, calls, min, unaligned; };
struct ipro_memcpy_site ipro_memcpy_sites[48];
#endif
__attribute__((optimize("O2", "no-tree-loop-distribute-patterns")))
void *memcpy(void *restrict dst, const void *restrict src, size_t n)
{
	uint8_t *d = dst;
	const uint8_t *s = src;
#ifdef CONFIG_IPRO_WIFI_TX_PROFILE
	uint32_t c0 = rdcycle(), ra = (uint32_t)__builtin_return_address(0), n0 = n;
#endif

	if (n >= 16) {
		/* Align the destination, then copy words: directly when the
		 * source is co-aligned, else assembled from two aligned source
		 * words (UDP payloads sit at a 2-byte offset behind the 42-byte
		 * Ethernet/IP/UDP headers; a byte loop there costs 6x). */
		while (((uintptr_t)d & 3) != 0) {
			*d++ = *s++;
			n--;
		}
		{
			uint32_t *dw = (uint32_t *)d;
			uintptr_t off = (uintptr_t)s & 3;

			if (off == 0) {
				const uint32_t *sw = (const uint32_t *)s;

				while (n >= 16) {
					uint32_t a = sw[0], b = sw[1], c = sw[2], e = sw[3];

					dw[0] = a; dw[1] = b; dw[2] = c; dw[3] = e;
					dw += 4; sw += 4; n -= 16;
				}
				while (n >= 4) {
					*dw++ = *sw++;
					n -= 4;
				}
				s = (const uint8_t *)sw;
			} else {
				const uint32_t *sw = (const uint32_t *)(s - off);
				unsigned int sh = off * 8, rsh = 32 - sh;
				uint32_t cur = *sw++;

				while (n >= 4) {
					uint32_t nxt = *sw++;

					*dw++ = (cur >> sh) | (nxt << rsh);
					cur = nxt;
					n -= 4;
				}
				s = (const uint8_t *)sw - 4 + off;
			}
			d = (uint8_t *)dw;
		}
	}
	while (n--) {
		*d++ = *s++;
	}
#ifdef CONFIG_IPRO_WIFI_TX_PROFILE
	for (int i = 0; i < 48; i++) {
		if (ipro_memcpy_sites[i].ra == ra || ipro_memcpy_sites[i].ra == 0) {
			uint32_t d = rdcycle() - c0;

			ipro_memcpy_sites[i].ra = ra;
			ipro_memcpy_sites[i].cycles += d;
			ipro_memcpy_sites[i].bytes += n0;
			ipro_memcpy_sites[i].calls++;
			if (ipro_memcpy_sites[i].min == 0 || d < ipro_memcpy_sites[i].min) {
				ipro_memcpy_sites[i].min = d;
			}
			if ((((uintptr_t)dst | (uintptr_t)src) & 3) != 0) {
				ipro_memcpy_sites[i].unaligned++;
			}
			break;
		}
	}
#endif
	return dst;
}

static void wifi_isr(void)
{
	uint32_t c = rdcycle();

	interrupt0_handler();
	ipro_wifi_isr_cycles += rdcycle() - c;
	ipro_wifi_isr_count++;
}

/* wifi_main() only brings the firmware up and ends (rtos_task_delete(NULL))
 * once the MAC tasks run, so its stack comes from the kernel heap and goes
 * back after the join instead of holding 6 KB of RAM for good. */
static struct k_thread wifi_fw_thread;
static k_thread_stack_t *wifi_fw_stack; /* until wifi_fw has been joined */

/* Frees wifi_fw's stack and task record once the thread has ended. */
static int wifi_fw_release(k_timeout_t wait)
{
	if (wifi_fw_stack == NULL) {
		return 0;
	}
	if (k_thread_join(&wifi_fw_thread, wait) != 0) {
		return -EBUSY;
	}
	/* the record rtos_task_delete(NULL) gave the thread on the way out */
	free(wifi_fw_thread.custom_data);
	wifi_fw_thread.custom_data = NULL;
	k_thread_stack_free(wifi_fw_stack);
	wifi_fw_stack = NULL;
	return 0;
}

static void wifi_fw_entry(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
	wifi_main(NULL);
}

bool ipro_wifi_started(void)
{
	return started;
}

int ipro_wifi_start(void)
{
	static wifi_conf_t conf = {.country_code = "CN", .channel_nums = 13};

	if (started) {
		return 0;
	}
	/* A start that timed out may have left wifi_fw running: never create
	 * a second one on the same thread object. */
	if (wifi_fw_release(K_NO_WAIT) != 0) {
		LOG_ERR("the previous Wi-Fi firmware start is still running");
		return -EBUSY;
	}
	/* The MAC shared memory (.wifibss, NOLOAD in WRAM) is not zeroed by the
	 * kernel's startup; the SDK's start-up code clears it, so do the same. */
	{
		extern uint8_t __wifi_bss_start[], __wifi_bss_end[];

		memset(__wifi_bss_start, 0, (size_t)(__wifi_bss_end - __wifi_bss_start));
	}
	Interrupt_Handler_Register(WIFI_IRQn, wifi_isr);
	k_timer_start(&backstop_timer, K_MSEC(100), K_MSEC(100));
	CPU_Interrupt_Enable(WIFI_IRQn);
	wifi_fw_stack = k_thread_stack_alloc(CONFIG_IPRO_WIFI_FW_STACK_SIZE, 0);
	if (wifi_fw_stack == NULL) {
		LOG_ERR("no memory for the Wi-Fi firmware stack");
		return -ENOMEM;
	}
	k_thread_create(&wifi_fw_thread, wifi_fw_stack, CONFIG_IPRO_WIFI_FW_STACK_SIZE,
			wifi_fw_entry, NULL, NULL, NULL, K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
	k_thread_name_set(&wifi_fw_thread, "wifi_fw");
	if (k_sem_take(&init_done, K_SECONDS(5)) != 0) {
		LOG_ERR("Wi-Fi firmware did not come up");
		(void)wifi_fw_release(K_NO_WAIT);
		return -ETIMEDOUT;
	}
	if (wifi_fw_release(K_SECONDS(5)) != 0) {
		LOG_WRN("wifi_fw still running: its stack stays allocated");
	}
	wifi_mgmr_init(&conf);
	if (k_sem_take(&mgmr_done, K_SECONDS(5)) != 0) {
		LOG_ERR("Wi-Fi manager did not come up");
		return -ETIMEDOUT;
	}
	started = true;
	LOG_INF("Wi-Fi stack up");
	return 0;
}
