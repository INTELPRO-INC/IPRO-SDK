/*
 * Copyright (C) 2018-2024 AICSemi Ltd.
 *
 * All Rights Reserved
 *
 * SDIO port header: ops-dispatch macros.
 */

#ifndef _SDIO_PORT_H_
#define _SDIO_PORT_H_

#include "rtos_port.h"
#include "co_list.h"
#include "aic_sdio_types.h"

/*
 * Runtime-configurable SDIO behavior flags.
 *
 * These were originally compile-time #define macros. However, since host
 * layer code (sdio_co.c) references them via #if directives, changing the
 * macros after the host library (.a) is built had no effect. Converting to
 * runtime variables allows platform code to set them at init time without
 * recompiling the host library.
 *
 * Platform defines and initializes these variables (typically in sdio_port.c).
 * Host code reads them at runtime via the extern declarations below.
 *
 * Default values are provided as _DEFAULT macros for platform convenience.
 */
/*
 * 默认值策略: RX 在 SDIO 中断上下文内直接处理 (=1, 参考实现, 实时性最好)。
 * 仅当平台 SDIO 驱动移植时已将 RX 处理改到外部独立 task 执行
 * (sdio_port.c 中对 aic_sdio_rx_task 的调用路径已移出 ISR) 时才改为 0。
 * 详见 docs/aic8800-porting、aic8800-release-adaptation、aic8800-sync SKILL.md。
 */
#define CONFIG_RXTASK_INSDIO_DEFAULT    0   /**< RX in SDIO ISR context (1=in-ISR, 0=separate task, see strategy above) */
#define CONFIG_RX_POLLING_DEFAULT       0   /**< RX data mode (0=interrupt-driven, 1=polling) */
#define CONFIG_IRQ_SUS_DEFAULT          1   /**< Re-enable SDIO IRQ after RX (must be 1 on Novatek HDAL) */

extern uint8_t config_rxtask_insdio;    /**< runtime flag: CONFIG_RXTASK_INSDIO_DEFAULT */
extern uint8_t config_irq_sus;          /**< runtime flag: CONFIG_IRQ_SUS_DEFAULT */
extern unsigned char aicwifi_sdio_rxdata_mode; /**< runtime flag: CONFIG_RX_POLLING_DEFAULT (0=interrupt, 1=polling) */

/* Export variables (provided by platform or stubs) */
extern int msgcfm_poll_en;
extern uint8_t sdio_unexcept_test;
extern struct aic_sdio_dev sdio_dev;

#ifdef CONFIG_SDIO_ADMA
extern unsigned char sdio_tx_buf_fill[512];
extern unsigned char sdio_tx_buf_dummy[SDIO_TX_SLIST_MAX][8];
#endif

/* ---- SDIO abstraction — platform must implement ALL functions ---- */

/* Host control */
void aicwf_sdio_claim_host(uint32_t func_num);
void aicwf_sdio_release_host(uint32_t func_num);

/* Bulk data transfer */
int  aicwf_sdio_readsb(uint32_t func_num, void *dst, unsigned int addr, int count);
int  aicwf_sdio_writesb(uint32_t func_num, unsigned int addr, void *src, int count);
int  aicwf_sdio_writesb_msg(uint32_t func_num, unsigned int addr, void *src, int count);

/* Register access (CMD52) */
bool aicwf_sdio_readb_cmd52(uint32_t addr, uint8_t *data);
bool aicwf_sdio_writeb_cmd52(uint32_t addr, uint8_t data);
bool aicwf_sdio_readb_cmd52_func2(uint32_t addr, uint8_t *data);
bool aicwf_sdio_writeb_cmd52_func2(uint32_t addr, uint8_t data);

/* 固件加载完成后补写 intr_config_reg 使能 WiFi 芯片侧 SDIO 中断 (sdio_port.c) */
int aicwf_sdio_enable_irq_after_fw_boot(void);

/* Initialization */
int  aicwf_sdio_func_init(uint16_t chipid, struct aic_sdio_dev *sdiodev);
int  aicwf_sdiov3_func_init(uint16_t chipid, struct aic_sdio_dev *sdiodev);
void aicwf_sdio_release_func2(void);

/* Interrupt */
bool aicwf_sdio_host_enable_isr(bool enable);
void aicwf_sdio_set_irq_handler(void (*cb)(void));
int  aicwf_sdio_interrupt_init(void);
void aicwf_sdio_isr(uint8_t en);

/* Driver registration */
int  aicwf_sdio_drv_register(void);

#ifdef CONFIG_OOB
void aicwf_sdio_oob_enable(void);
#endif

#endif /* _SDIO_PORT_H_ */
