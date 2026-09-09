/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "ipro6_sdio3.h"
#include "ipro_irq.h"

#ifdef IPRO6_SDIO3_DEBUG
#define IPRO6_SDIO3_DRV_DBG(a, ...) printf("[sdio3 drv]" a, ##__VA_ARGS__)
#else
#define IPRO6_SDIO3_DRV_DBG(a, ...)
#endif

#if (IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX & (IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX - 1) != 0)
#error "sdio3 queue num error, must be 2^n"
#elif (IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX > 16)
#error "sdio3 queue num error, must be <=16"
#endif
#define IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX_MASK  (IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX - 1)

/* cmd 53 def */
#define IPRO6_SDIO3_CMD53_RW_COUNT_SHIFT     (0)
#define IPRO6_SDIO3_CMD53_RW_COUNT_MASK      (0x1ff << IPRO6_SDIO3_CMD53_RW_COUNT_SHIFT)
#define IPRO6_SDIO3_CMD53_RW_ADDR_SHIFT      (9)
#define IPRO6_SDIO3_CMD53_RW_ADDR_MASK       (0x1ffff << IPRO6_SDIO3_CMD53_RW_ADDR_SHIFT)
#define IPRO6_SDIO3_CMD53_RW_OP_CODE_MASK    (1 << 26)
#define IPRO6_SDIO3_CMD53_RW_BLOCK_MODE_MASK (1 << 27)
#define IPRO6_SDIO3_CMD53_RW_FUNC_SHIFT      (28)
#define IPRO6_SDIO3_CMD53_RW_FUNC_MASK       (0x03 << IPRO6_SDIO3_CMD53_RW_FUNC_SHIFT)
#define IPRO6_SDIO3_CMD53_RW_DIR_MASK        (1 << 31)

/* isr (zero-arg ECLIC handler; recovers the device via the file-static below) */
static void ipro6_sdio3_isr(void);

struct ipro6_sdio3_queue_ctrl_s {
    uint8_t tran_sta;
    ipro6_sdio3_trans_desc_t *trans_desc;

    uint8_t dnld_queue_out[IPRO6_SDIO3_FUNC_NUM_MAX];
    uint8_t dnld_queue_in[IPRO6_SDIO3_FUNC_NUM_MAX];
    ipro6_sdio3_trans_desc_t dnld_queue_desc[IPRO6_SDIO3_FUNC_NUM_MAX][IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX];

    uint8_t upld_queue_out[IPRO6_SDIO3_FUNC_NUM_MAX];
    uint8_t upld_queue_in[IPRO6_SDIO3_FUNC_NUM_MAX];
    ipro6_sdio3_trans_desc_t upld_queue_desc[IPRO6_SDIO3_FUNC_NUM_MAX][IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX];
};

/* dnld queue */
#define IPRO6_SDIO3_GET_DNLD_OUT(ctrl, func) (ctrl.dnld_queue_out[func - 1] & IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX_MASK)
#define IPRO6_SDIO3_GET_DNLD_IN(ctrl, func)  (ctrl.dnld_queue_in[func - 1] & IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX_MASK)
#define IPRO6_SDIO3_GET_DNLD_NUM(ctrl, func) ((uint8_t)(ctrl.dnld_queue_in[func - 1] - ctrl.dnld_queue_out[func - 1]))
/* dnld get out(pop) desc */
#define IPRO6_SDIO3_GET_DNLD_OUT_DESC(ctrl, func) \
    (&ctrl.dnld_queue_desc[func - 1][IPRO6_SDIO3_GET_DNLD_OUT(ctrl, func)])
/* dnld get in(push) desc */
#define IPRO6_SDIO3_GET_DNLD_IN_DESC(ctrl, func) \
    (&ctrl.dnld_queue_desc[func - 1][IPRO6_SDIO3_GET_DNLD_IN(ctrl, func)])

/* upld queue */
#define IPRO6_SDIO3_GET_UPLD_OUT(ctrl, func) (ctrl.upld_queue_out[func - 1] & IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX_MASK)
#define IPRO6_SDIO3_GET_UPLD_IN(ctrl, func)  (ctrl.upld_queue_in[func - 1] & IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX_MASK)
#define IPRO6_SDIO3_GET_UPLD_NUM(ctrl, func) ((uint8_t)(ctrl.upld_queue_in[func - 1] - ctrl.upld_queue_out[func - 1]))
/* upld get out(pop) desc */
#define IPRO6_SDIO3_GET_UPLD_OUT_DESC(ctrl, func) \
    (&ctrl.upld_queue_desc[func - 1][IPRO6_SDIO3_GET_UPLD_OUT(ctrl, func)])
/* upld get in(push) desc */
#define IPRO6_SDIO3_GET_UPLD_IN_DESC(ctrl, func) \
    (&ctrl.upld_queue_desc[func - 1][IPRO6_SDIO3_GET_UPLD_IN(ctrl, func)])

/* isr event callback */
static void *sdio3_irq_arg = NULL;
static ipro6_sdio3_irq_cb_t sdio3_irq_event_cb = NULL;
/* device handle for the ISR (C2 fix: recovered here, not via a handler arg) */
static ipro6_smid_dev_t *sdio3_dev = NULL;
/* sdio3 ctrl */
static struct ipro6_sdio3_queue_ctrl_s sdio3_ctrl = { 0 };
/* diag: dnld length from Argument was out of range -> clamped to buffer size */
volatile uint32_t ipro6_sdio3_dnld_len_fallback = 0;
/* dnld flow control via DAT0 busy: set when a dnld completed with no free
 * posted buffer for the host's next write, so PROGRAM_DONE was withheld (DAT0
 * held busy to back-pressure the host). dnld_push releases it on the next
 * re-arm. See ipro6_sdio3_isr_cb_tran_complete + ipro6_sdio3_dnld_push.
 * Exported (read-only) so the trcver can bound the stall duration. */
volatile uint8_t ipro6_sdio3_dnld_pgmdone_deferred = 0;
#define sdio3_dnld_pgmdone_deferred ipro6_sdio3_dnld_pgmdone_deferred
/* diag: number of times PROGRAM_DONE was deferred for back-pressure */
volatile uint32_t ipro6_sdio3_dnld_backpressure = 0;
/* The underlying DMA engine (shared with the DMA1 path) has a 4 KB page
 * granularity and HALTS at a page boundary mid-transfer, expecting the address
 * to be reloaded -- but in ADMA mode the reload path is compiled out, so an ADMA
 * transfer whose data crosses a 4 KB page stalls forever (no TRANSFER_COMPLETE),
 * the host's CMD53 times out, and it drops the card (the D-2b flap). ADMA2
 * supports a descriptor CHAIN, so split every transfer at 4 KB page boundaries:
 * each line then stays within one page and the page-boundary interrupt never
 * fires. A 16-bit-length (<=64 KB) transfer needs at most 17 page-bounded lines. */
#define IPRO6_SDIO3_ADMA_PAGE_SIZE  (4096u)
#define IPRO6_SDIO3_ADMA_MAX_DESC   (18)
/* ADMA hw desc chain, must be nocache */
static ATTR_NOCACHE_RAM_SECTION ipro6_sdio3_adma_hw_desc_s adma_hw_desc_buff[IPRO6_SDIO3_ADMA_MAX_DESC];
_Static_assert(sizeof(ipro6_sdio3_adma_hw_desc_s) == 8, "ADMA descriptor must be 8 bytes");

int ipro6_sdio3_init(ipro6_smid_dev_t *dev, ipro6_sdio3_config_s *cfg)
{
    uint32_t reg_base;
    uint32_t regval;

    if (cfg->func_num == 0 || cfg->func_num > IPRO6_SDIO3_FUNC_NUM_MAX) {
        return -1;
    }

    reg_base = dev->reg_base;
    sdio3_dev = dev;

    /* CCCR cfg */
    regval = getreg32(reg_base + IPRO6_SDIO3_SDIO_CCCR_CONTROL_REGISTER_OFFSET);
    /* asyn int */
    if (cfg->cap_flag & IPRO6_SDIO3_CAP_FLAG_SAI) {
        regval |= IPRO6_SDIO3_SAI;
    } else {
        regval &= ~IPRO6_SDIO3_SAI;
    }
    /* ddr50 */
    if (cfg->cap_flag & IPRO6_SDIO3_CAP_FLAG_DDR50) {
        regval |= IPRO6_SDIO3_DDR50;
    } else {
        regval &= ~IPRO6_SDIO3_DDR50;
    }
    /* sdr104 */
    if (cfg->cap_flag & IPRO6_SDIO3_CAP_FLAG_SDR104) {
        regval |= IPRO6_SDIO3_SDR104;
    } else {
        regval &= ~IPRO6_SDIO3_SDR104;
    }
    /* sdr50 */
    if (cfg->cap_flag & IPRO6_SDIO3_CAP_FLAG_SDR50) {
        regval |= IPRO6_SDIO3_SDR50;
    } else {
        regval &= ~IPRO6_SDIO3_SDR50;
    }
    /* block gap interrupt */
    if (cfg->cap_flag & IPRO6_SDIO3_CAP_FLAG_GAP_INT) {
        regval |= IPRO6_SDIO3_S4MI;
    } else {
        regval &= ~IPRO6_SDIO3_S4MI;
    }
    /* Read wait */
    if (cfg->cap_flag & IPRO6_SDIO3_CAP_FLAG_RD_WAIT) {
        regval |= IPRO6_SDIO3_SRW;
    } else {
        regval &= ~IPRO6_SDIO3_SRW;
    }
    /* disable bus suspend/resume */
    regval &= ~IPRO6_SDIO3_SBS;
    /* disable power ctrl */
    regval &= ~IPRO6_SDIO3_SMPC;
    /* disable 8bit */
    regval &= ~IPRO6_SDIO3_S8B;
    /* disable direct cmd52 */
    regval &= ~IPRO6_SDIO3_SDC;
    /* enable multiple block */
    regval |= IPRO6_SDIO3_SMB;
    /* enable sdr25/hs. CRITICAL: keep SHS=1 — it holds SMID in high-speed sampling
     * timing. Forcing Default-Speed (clearing SHS, since CCCR BSSx is forced to 0
     * when SHS=0) pins SMID to DS sampling timing, which mis-samples the first CMD52
     * after a host clock-gate -> hardware fills a bogus R5 COM_CRC_ERROR -> the host
     * sees -84 (EILSEQ) and never finishes enumeration. Never clear SHS. */
    regval |= IPRO6_SDIO3_SHS;
    /*  */
    putreg32(regval, reg_base + IPRO6_SDIO3_SDIO_CCCR_CONTROL_REGISTER_OFFSET);

    /* cmd and data output edge */
    regval = getreg32(reg_base + IPRO6_SDIO3_CONTROL_REGISTER_OFFSET);
    regval &= ~IPRO6_SDIO3_CMD_DATA_OUTPUT_EDGE;
    putreg32(regval, reg_base + IPRO6_SDIO3_CONTROL_REGISTER_OFFSET);

    /* AHB master burst size enable: INCR4, INCR8, INCR16, INCR */
    regval = getreg32(reg_base + IPRO6_SDIO3_AHB_MASTER_BURSTSIZE_REGISTER_OFFSET);
    regval &= ~IPRO6_SDIO3_AHBMAS_BURST_SIZE_ENABLE_MASK;
    regval |= (0x0f << IPRO6_SDIO3_AHBMAS_BURST_SIZE_ENABLE_SHIFT) & IPRO6_SDIO3_AHBMAS_BURST_SIZE_ENABLE_MASK;
    putreg32(regval, reg_base + IPRO6_SDIO3_AHB_MASTER_BURSTSIZE_REGISTER_OFFSET);

    /* ocr */
    regval = getreg32(reg_base + IPRO6_SDIO3_CARD_OCR_REGISTER_OFFSET);
    regval &= ~IPRO6_SDIO3_CARD_OCR_MASK;
    regval |= (cfg->ocr << IPRO6_SDIO3_CARD_OCR_SHIFT) & IPRO6_SDIO3_CARD_OCR_MASK;
    /* sig 1.8v */
    if (cfg->cap_flag & IPRO6_SDIO3_CAP_FLAG_SIG1V8) {
        regval |= IPRO6_SDIO3_SWITCHIN_TO_1P8V;
    } else {
        regval &= ~IPRO6_SDIO3_SWITCHIN_TO_1P8V;
    }
    /* func num */
    if (cfg->func_num == 1) {
        regval &= ~(0x0007 << 28);
        regval |= 0x0001 << 28;
    }
    putreg32(regval, reg_base + IPRO6_SDIO3_CARD_OCR_REGISTER_OFFSET);

    /* func ready */
    regval = getreg32(reg_base + IPRO6_SDIO3_IOREADY_REGISTER_OFFSET);
    for (uint8_t i = 1; i <= cfg->func_num; i++) {
        regval |= 0x01 << i;
    }
    putreg32(regval, reg_base + IPRO6_SDIO3_IOREADY_REGISTER_OFFSET);

    /* FBR1 SDIO Standard Function Interface Code (FBRx Control reg [3:0]); the host
     * reads it as the function class (e.g. 7h = WLAN). The CIS vendor/device is
     * silicon-baked and not settable, so std-IF/class is the identity we can set. */
    if (cfg->func1_if_code) {
        regval = getreg32(reg_base + IPRO6_SDIO3_FBR1_CONTROL_REGISTER_OFFSET);
        regval = (regval & ~IPRO6_SDIO3_FBR_STD_IF_CODE_MASK)
               | (cfg->func1_if_code & IPRO6_SDIO3_FBR_STD_IF_CODE_MASK);
        putreg32(regval, reg_base + IPRO6_SDIO3_FBR1_CONTROL_REGISTER_OFFSET);
    }

#if IPRO6_SDIO3_DMA1_MODE_ENABLE
    /* dma1 mode */
    regval = getreg32(reg_base + IPRO6_SDIO3_CONTROL2_REGISTER_OFFSET);
    regval &= ~IPRO6_SDIO3_ADMA_ENABLE;
    putreg32(regval, reg_base + IPRO6_SDIO3_CONTROL2_REGISTER_OFFSET);

    /* dma1 page size cfg */
    regval = getreg32(reg_base + IPRO6_SDIO3_DMA1_CONTROL_REGISTER_OFFSET);
    regval &= ~IPRO6_SDIO3_DMA1_BUFFER_SIZE_MASK;
    uint8_t size_bit = __builtin_ctz((IPRO6_SDIO3_DMA1_PAGA_SIZE >> 12));
    regval |= (size_bit << 1);
    putreg32(regval, reg_base + IPRO6_SDIO3_DMA1_CONTROL_REGISTER_OFFSET);

#else
    /* adma mode */
    regval = getreg32(reg_base + IPRO6_SDIO3_CONTROL2_REGISTER_OFFSET);
    regval |= IPRO6_SDIO3_ADMA_ENABLE;
    putreg32(regval, reg_base + IPRO6_SDIO3_CONTROL2_REGISTER_OFFSET);
#endif

    /* clean all interrupt_status */
    putreg32(0xffffffff, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
    putreg32(0x000000ff, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS2_REGISTER_OFFSET);

    /* Interrupt mask = exactly the interrupts the ISR dispatcher handles. The
     * DMA page-boundary interrupt (bit1) is handled in BOTH modes now (ADMA also
     * halts at 4 KB pages), so enable it unconditionally. */
    regval = IPRO6_SDIO3_TRANSFER_COMPLETE_INTERRUPT | IPRO6_SDIO3_WRITE_START_INTERRUPT | IPRO6_SDIO3_READ_START_INTERRUPT /* transfer int */
             | IPRO6_SDIO3_DMA1_INTERRUPT                                                                                  /* DMA page int */
             | IPRO6_SDIO3_CMD11_INTERRUPT | IPRO6_SDIO3_CMD11_CLK_START | IPRO6_SDIO3_CMD11_CLK_STOP                       /* vol switch int */
             | IPRO6_SDIO3_CMD0_CMD52_SOFT_RESET | IPRO6_SDIO3_FUNCTION1_RESET | IPRO6_SDIO3_FUNCTION2_RESET                /* host reset int */
             | IPRO6_SDIO3_FUNCTIONX_CRC_END_ERROR_INTERRUPT | IPRO6_SDIO3_FUNCTIONX_ABORT_INTERRUPT;                 /* crc/abort int */
    /* Constrain the STATUS-ENABLE (0x40) too, not just SIGNAL-ENABLE. It was left
     * at the reset default 0xFFFFFFFF, so EVERY status bit latched -- including
     * DMA1 page-boundary (bit1), which the HW still asserts during ADMA transfers
     * even though DMA1 mode is off and the ISR has no branch for it (the DMA1 path
     * is compiled out in ADMA mode). An unhandled, never-cleared status bit keeps
     * the IRQ line asserted -> the ISR re-enters forever (a prio-26 hardware storm
     * that starves every task) -> the device hangs the instant real data flows,
     * which the host then sees as a card flap (the D-2b post-GOT_IP hang/flap).
     * Latching only the handled interrupts stops the spurious DMA1 (or any other
     * unhandled) interrupt at the source. */
    putreg32(regval, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_ENABLE_REGISTER_OFFSET);
    putreg32(regval, reg_base + IPRO6_SDIO3_INTERRUPT_SIGNAL_ENABLE_REGISTER_OFFSET);
    /* sta2: only the ADMA error is handled */
    regval = IPRO6_SDIO3_ADMA_ERROR_INTERRUPT; /* adma err int */
    putreg32(regval, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS2_ENABLE_REGISTER_OFFSET);
    putreg32(regval, reg_base + IPRO6_SDIO3_INTERRUPT_SIGNAL_ENABLE2_REGISTER_OFFSET);

    /* write custom reg */
    /* write dnld size max */
    ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(1) + IPRO6_SDIO3_CUSTOM_REG_FUNC_DNLD_MAX_SIZE, &cfg->func1_dnld_size_max, 2);
    ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(2) + IPRO6_SDIO3_CUSTOM_REG_FUNC_DNLD_MAX_SIZE, &cfg->func2_dnld_size_max, 2);
    /* write queue depth */
    uint8_t queue_num = IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX;
    ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(1) + IPRO6_SDIO3_CUSTOM_REG_FUNC_QUEUE_MAX_DEPTH, &queue_num, 1);
    ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(2) + IPRO6_SDIO3_CUSTOM_REG_FUNC_QUEUE_MAX_DEPTH, &queue_num, 1);

    for (uint8_t i = 1; i <= IPRO6_SDIO3_FUNC_NUM_MAX; i++) {
        /* write status flag */
        regval = 0;
        ipro6_sdio3_custom_reg_read(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(i) + IPRO6_SDIO3_CUSTOM_REG_FUNC_STA_FLAG, &regval, 1);
        regval |= IPRO6_SDIO3_STA_FLAG_RD_LEN_COMPRESS_SUP;
        /* app build: advertise APP_RUN (bootrom SDIO-boot is a separate project) */
        regval |= IPRO6_SDIO3_STA_FLAG_APP_RUN;
        ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(i) + IPRO6_SDIO3_CUSTOM_REG_FUNC_STA_FLAG, &regval, 1);

        /* clean upld/dnld in/out ptr */
        regval = 0;
        ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(i) + IPRO6_SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_IN, &regval, 1);
        ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(i) + IPRO6_SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_OUT, &regval, 1);
        ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(i) + IPRO6_SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_IN, &regval, 1);
        ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(i) + IPRO6_SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_OUT, &regval, 1);
    }

    ipro_irq_attach(dev->irq_num, ipro6_sdio3_isr, dev);
    ipro_irq_enable(dev->irq_num);

    return 0;
}

int ipro6_sdio3_deinit(ipro6_smid_dev_t *dev)
{
    ipro_irq_disable(dev->irq_num);

    /* reset sdio3_ctrl */
    arch_memset((void *)&sdio3_ctrl, 0, sizeof(sdio3_ctrl));

    /* clear custom reg */
    uint8_t buf[8] = { 0 };
    for (uint32_t i = 0; i < IPRO6_SDIO3_CUSTOM_REG_SIZE; i += sizeof(buf)) {
        ipro6_sdio3_custom_reg_write(dev, i, buf, sizeof(buf));
    }

    return 0;
}

void ipro6_sdio3_keep_ready(ipro6_smid_dev_t *dev)
{
    uint32_t reg_base = dev->reg_base;

    /* The host IO-reset (CCCR 0x06[3]) silently clears Card-Init-Done + IOREADY and
     * raises no interrupt on ipro6s, so re-assert them whenever they go down. */
    uint32_t ctrl = getreg32(reg_base + IPRO6_SDIO3_CONTROL_REGISTER_OFFSET);
    if (!(ctrl & IPRO6_SDIO3_CARD_INIT_DONE)) {
        putreg32(ctrl | IPRO6_SDIO3_CARD_INIT_DONE,
                 reg_base + IPRO6_SDIO3_CONTROL_REGISTER_OFFSET);
    }

    uint32_t want = 0;
    for (uint8_t i = 1; i <= IPRO6_SDIO3_FUNC_NUM_MAX; i++) {
        want |= (1u << i);
    }
    uint32_t ior = getreg32(reg_base + IPRO6_SDIO3_IOREADY_REGISTER_OFFSET);
    if ((ior & want) != want) {
        putreg32(ior | want, reg_base + IPRO6_SDIO3_IOREADY_REGISTER_OFFSET);
    }
}

#if IPRO6_SDIO3_DMA1_MODE_ENABLE
/* dma1 init */
static int ipro6_sdio3_dma1_start(ipro6_smid_dev_t *dev, uint32_t addr, uint16_t len)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;

    /* address */
    putreg32(addr, reg_base + IPRO6_SDIO3_DMA1_ADDRESS_REGISTER_OFFSET);

    /* start */
    regval = getreg32(reg_base + IPRO6_SDIO3_DMA1_CONTROL_REGISTER_OFFSET);
    regval |= IPRO6_SDIO3_DMA1_ADDRESS_VALID;
    putreg32(regval, reg_base + IPRO6_SDIO3_DMA1_CONTROL_REGISTER_OFFSET);

    return 0;
}
#else
/* Emit page-bounded ADMA lines for one contiguous [addr, addr+len) region
 * starting at chain index i; no single line crosses a 4 KB DMA page boundary
 * (which would stall the engine mid-transfer; see the adma_hw_desc_buff
 * comment). Returns the next free chain index, or -1 if the chain is full. */
static int ipro6_sdio3_adma_emit(ipro6_sdio3_adma_hw_desc_s *hw_desc, int i, uint32_t addr, uint32_t len)
{
    while (len && i < IPRO6_SDIO3_ADMA_MAX_DESC) {
        uint32_t to_boundary = IPRO6_SDIO3_ADMA_PAGE_SIZE - (addr & (IPRO6_SDIO3_ADMA_PAGE_SIZE - 1));
        uint32_t seg_len = (len < to_boundary) ? len : to_boundary;

        hw_desc[i].address   = addr;
        hw_desc[i].length    = (uint16_t)seg_len;   /* <= 4096, fits 16 bits */
        hw_desc[i].attribute = IPRO6_SDIO3_ADMA_ATTR_VALID | IPRO6_SDIO3_ADMA_ATTR_ACT_TRAN;
        addr += seg_len;
        len -= seg_len;
        i++;
    }
    return len ? -1 : i;
}

/* Terminate the chain (END on the last line) and kick the engine. */
static void ipro6_sdio3_adma_kick(ipro6_smid_dev_t *dev, ipro6_sdio3_adma_hw_desc_s *hw_desc, int lines)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;

    hw_desc[lines - 1].attribute |= IPRO6_SDIO3_ADMA_ATTR_END;

    /* adma desc addr reg = head of the chain */
    putreg32((uint32_t)(uintptr_t)hw_desc, reg_base + IPRO6_SDIO3_DMA1_ADDRESS_REGISTER_OFFSET);

    /* adma start */
    regval = getreg32(reg_base + IPRO6_SDIO3_DMA1_CONTROL_REGISTER_OFFSET);
    regval |= IPRO6_SDIO3_DMA1_ADDRESS_VALID;
    putreg32(regval, reg_base + IPRO6_SDIO3_DMA1_CONTROL_REGISTER_OFFSET);
}

/* adma desc init: single contiguous buffer, page-split into a chain. */
static int ipro6_sdio3_adma_start(ipro6_smid_dev_t *dev, ipro6_sdio3_adma_hw_desc_s *hw_desc, uint32_t addr, uint16_t len)
{
    int lines;

    if (len == 0) {
        return -1;
    }
    lines = ipro6_sdio3_adma_emit(hw_desc, 0, addr, len);
    if (lines <= 0) {
        return -1;
    }
    ipro6_sdio3_adma_kick(dev, hw_desc, lines);
    return 0;
}

/* ZCEN2-stride scatter dnld: the host packs its frames at a fixed wire stride,
 * one frame per stride; stride k's first slot_cap bytes land at sg->slot[k]
 * and the stride's zero-pad tail (slot_cap + pad_off == stride) is sunk into
 * the SAME slot's headroom (slot[k] - pad_off, rebuilt later by the TX path).
 * xfer_len is the actual CMD53 byte count decoded at WRITE_START, so the chain
 * covers exactly the received bytes: a partial last stride simply emits a
 * shorter tail, and slots beyond ceil(xfer_len/stride) get no lines at all.
 * Line budget: cnt<=4 strides x (<=2 data lines + <=2 pad lines) = 16 <= 18. */
static int ipro6_sdio3_adma_start_sg(ipro6_smid_dev_t *dev, ipro6_sdio3_adma_hw_desc_s *hw_desc,
                                     const ipro6_sdio3_sg_desc_t *sg, uint16_t xfer_len)
{
    uint32_t remaining = xfer_len;
    int i = 0;

    if (xfer_len == 0 || sg == NULL || sg->cnt == 0 || sg->cnt > IPRO6_SDIO3_SG_MAX ||
        sg->slot_cap == 0 || (uint32_t)sg->slot_cap + sg->pad_off != sg->stride) {
        return -1;
    }

    for (uint8_t k = 0; k < sg->cnt && remaining; k++) {
        uint32_t stride_bytes = (remaining < sg->stride) ? remaining : sg->stride;
        uint32_t data_len = (stride_bytes < sg->slot_cap) ? stride_bytes : sg->slot_cap;
        uint32_t pad_len = stride_bytes - data_len;
        uint32_t slot_addr = (uint32_t)(uintptr_t)sg->slot[k];

        i = ipro6_sdio3_adma_emit(hw_desc, i, slot_addr, data_len);
        if (i >= 0 && pad_len) {
            i = ipro6_sdio3_adma_emit(hw_desc, i, slot_addr - sg->pad_off, pad_len);
        }
        if (i < 0) {
            return -1;
        }
        remaining -= stride_bytes;
    }
    if (remaining || i == 0) {
        return -1;   /* host sent more than the chain capacity (caller clamps) */
    }
    ipro6_sdio3_adma_kick(dev, hw_desc, i);
    return 0;
}
#endif

static int ipro6_sdio3_adma_stop(ipro6_smid_dev_t *dev)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;

    /* adma stop */
    regval = getreg32(reg_base + IPRO6_SDIO3_DMA1_CONTROL_REGISTER_OFFSET);
    regval &= ~IPRO6_SDIO3_DMA1_ADDRESS_VALID;
    putreg32(regval, reg_base + IPRO6_SDIO3_DMA1_CONTROL_REGISTER_OFFSET);

    return 0;
}

int ipro6_sdio3_custom_reg_write(ipro6_smid_dev_t *dev, uint16_t reg_offset, void *buff, uint16_t len)
{
    uint32_t reg_base = dev->reg_base;

    if (reg_offset + len > IPRO6_SDIO3_CUSTOM_REG_SIZE) {
        return -1;
    }

    /* lock */
    uintptr_t flag = ipro_irq_save();

    /* Write card address register */
    putreg32(reg_offset, reg_base + IPRO6_SDIO3_CUSTOM_ADDRESS_REGISTER_OFFSET);
    /* Write card data register */
    for (uint16_t i = 0; i < len; i++) {
        putreg8(((uint8_t *)buff)[i], reg_base + IPRO6_SDIO3_CUSTOM_DATA_REGISTER_OFFSET);
    }

    /* unlock */
    ipro_irq_restore(flag);

    return 0;
}

int ipro6_sdio3_custom_reg_read(ipro6_smid_dev_t *dev, uint16_t reg_offset, void *buff, uint16_t len)
{
    uint32_t reg_base = dev->reg_base;

    if (reg_offset + len > IPRO6_SDIO3_CUSTOM_REG_SIZE) {
        return -1;
    }

    /* lock */
    uintptr_t flag = ipro_irq_save();

    /* Write card address register */
    putreg32(reg_offset, reg_base + IPRO6_SDIO3_CUSTOM_ADDRESS_REGISTER_OFFSET);
    /* Read card data register */
    for (uint16_t i = 0; i < len; i++) {
        ((uint8_t *)buff)[i] = getreg8(reg_base + IPRO6_SDIO3_CUSTOM_DATA_REGISTER_OFFSET);
    }
    /* unlock */
    ipro_irq_restore(flag);

    return 0;
}

int ipro6_sdio3_dnld_push(ipro6_smid_dev_t *dev, ipro6_sdio3_trans_desc_t *trans_desc)
{
    if (trans_desc == NULL || trans_desc->func == 0 || trans_desc->func > IPRO6_SDIO3_FUNC_NUM_MAX) {
        return -1;
    }

    /* lock */
    uintptr_t flag = ipro_irq_save();

    /* check queue */
    if (IPRO6_SDIO3_GET_DNLD_NUM(sdio3_ctrl, trans_desc->func) >= IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX) {
        ipro_irq_restore(flag);
        return -2;
    }

    /* update dnld queue */
    uint8_t dnld_in = sdio3_ctrl.dnld_queue_in[trans_desc->func - 1];
    ipro6_sdio3_trans_desc_t *dnld_desc = IPRO6_SDIO3_GET_DNLD_IN_DESC(sdio3_ctrl, trans_desc->func);
    *dnld_desc = *trans_desc;
    sdio3_ctrl.dnld_queue_in[trans_desc->func - 1] += 1;

    /* unlock */
    ipro_irq_restore(flag);

    /* clean dnld length */
    uint16_t dnld_elem = 0;
    uint8_t dnld_in_mask = dnld_in & IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX_MASK; /* IPRO6_SDIO3_GET_DNLD_IN(sdio3_ctrl, trans_desc->func) */
    ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(dnld_desc->func) + IPRO6_SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE + dnld_in_mask * 2, &dnld_elem, 2);

    /* update queue in ponit */
    /* lock */
    flag = ipro_irq_save();
    dnld_in = sdio3_ctrl.dnld_queue_in[dnld_desc->func - 1];
    ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(dnld_desc->func) + IPRO6_SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_IN, &dnld_in, 1);
    /* unlock */
    ipro_irq_restore(flag);

    /* Back-pressure handoff: a prior dnld completed with the pool drained and
     * withheld PROGRAM_DONE (DAT0 held busy). Now that a buffer is posted again,
     * release DAT0 so the host's blocked CMD53 write proceeds into it. */
    if (sdio3_dnld_pgmdone_deferred) {
        flag = ipro_irq_save();
        if (sdio3_dnld_pgmdone_deferred) {
            sdio3_dnld_pgmdone_deferred = 0;
            uint32_t regval = getreg32(dev->reg_base + IPRO6_SDIO3_CONTROL_REGISTER_OFFSET);
            regval |= IPRO6_SDIO3_PROGRAM_DONE;
            putreg32(regval, dev->reg_base + IPRO6_SDIO3_CONTROL_REGISTER_OFFSET);
        }
        ipro_irq_restore(flag);
    }

    return 0;
}

/* Set the function read-count "doorbell" to the head (next-to-read) upld frame's
 * byte count, or 0 if the upld queue is empty. The host polls this at SDIO Fn
 * register 0x00 (low) / 0x01 (high) -- the Function Read Count Register, datasheet
 * Tables 13-15 -- to learn how many bytes to CMD53-read; writing the high byte also
 * asserts the card->host interrupt. Caller must hold the upld queue state stable. */
static void ipro6_sdio3_upld_doorbell(ipro6_smid_dev_t *dev, uint8_t func)
{
    uint16_t cnt = 0;
    if (IPRO6_SDIO3_GET_UPLD_NUM(sdio3_ctrl, func) > 0) {
        cnt = IPRO6_SDIO3_GET_UPLD_OUT_DESC(sdio3_ctrl, func)->data_len;
    }
    if (func == 1) {
        putreg16(cnt, dev->reg_base + IPRO6_SDIO3_FUNCTION1_CONTROL_REGISTER_OFFSET);
    } else if (func == 2) {
        putreg16(cnt, dev->reg_base + IPRO6_SDIO3_FUNCTION2_CONTROL_REGISTER_OFFSET);
    }
}

int ipro6_sdio3_upld_push(ipro6_smid_dev_t *dev, ipro6_sdio3_trans_desc_t *trans_desc)
{
    if (trans_desc == NULL || trans_desc->func == 0 || trans_desc->func > IPRO6_SDIO3_FUNC_NUM_MAX) {
        return -1;
    }

    /* lock */
    uintptr_t flag = ipro_irq_save();

    /* check queue */
    if (IPRO6_SDIO3_GET_UPLD_NUM(sdio3_ctrl, trans_desc->func) >= IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX) {
        ipro_irq_restore(flag);
        return -2;
    }

    /* update upld queue */
    uint8_t upld_in = sdio3_ctrl.upld_queue_in[trans_desc->func - 1];
    ipro6_sdio3_trans_desc_t *upld_desc = IPRO6_SDIO3_GET_UPLD_IN_DESC(sdio3_ctrl, trans_desc->func);
    *upld_desc = *trans_desc;
    sdio3_ctrl.upld_queue_in[upld_desc->func - 1] += 1;

    /* unlock */
    ipro_irq_restore(flag);

    /* update queue elem (length)  */
    uint16_t blk_size, upld_elem;
    uint8_t upld_in_mask = upld_in & IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX_MASK; /* IPRO6_SDIO3_GET_UPLD_IN(sdio3_ctrl, upld_desc->func) */
    ipro6_sdio3_custom_reg_read(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(upld_desc->func) + IPRO6_SDIO3_CUSTOM_REG_FUNC_BLOCK_SIZE, &blk_size, 2);
    if (!blk_size || upld_desc->data_len % blk_size) {
        /* byte mode */
        if (upld_desc->data_len < 64) {
            upld_elem = (upld_desc->data_len << 2) | 0x00; /* 1-byte, byte mode */
        } else {
            upld_elem = (upld_desc->data_len << 2) | 0x01; /* 2-byte, byte mode */
        }
    } else {
        /* block mode */
        if (upld_desc->data_len / blk_size < 64) {
            upld_elem = ((upld_desc->data_len / blk_size) << 2) | 0x02; /* 1-byte, block mode */
        } else {
            upld_elem = ((upld_desc->data_len / blk_size) << 2) | 0x03; /* 2-byte, block mode */
        }
    }
    ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(upld_desc->func) + IPRO6_SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE + upld_in_mask * 2, &upld_elem, 2);

    /* update queue in ponit */
    /* lock */
    flag = ipro_irq_save();
    upld_in = sdio3_ctrl.upld_queue_in[upld_desc->func - 1];
    ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(upld_desc->func) + IPRO6_SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_IN, &upld_in, 1);
    /* unlock */
    ipro_irq_restore(flag);

    /* Doorbell the host with the head frame's byte count. Skip while an upld
     * transfer is in flight -- that transfer's completion sets the next doorbell,
     * so we never clobber the count the host is mid-reading. */
    flag = ipro_irq_save();
    if (sdio3_ctrl.tran_sta != IPRO6_SDIO3_TRAN_STA_BUSY_UPLD) {
        ipro6_sdio3_upld_doorbell(dev, upld_desc->func);
    }
    ipro_irq_restore(flag);

    return 0;
}

/* pop dnld queue, can only be used after reset. */
int ipro6_sdio3_dnld_pop(ipro6_smid_dev_t *dev, ipro6_sdio3_trans_desc_t *trans_desc, uint8_t func)
{
    arch_memset(trans_desc, 0, sizeof(ipro6_sdio3_trans_desc_t));

    /* lock */
    uintptr_t flag = ipro_irq_save();

    /* not IDLE */
    if (sdio3_ctrl.tran_sta != IPRO6_SDIO3_TRAN_STA_IDLE) {
        ipro_irq_restore(flag);
        return -1;
    }

    /* check queue */
    if (IPRO6_SDIO3_GET_DNLD_NUM(sdio3_ctrl, func) == 0) {
        ipro_irq_restore(flag);
        return 1;
    }

    *trans_desc = *(IPRO6_SDIO3_GET_DNLD_OUT_DESC(sdio3_ctrl, func));
    sdio3_ctrl.dnld_queue_out[func - 1] += 1;

    /* unlock */
    ipro_irq_restore(flag);

    return 0;
}

/* pop upld queue, can only be used after reset. */
int ipro6_sdio3_upld_pop(ipro6_smid_dev_t *dev, ipro6_sdio3_trans_desc_t *trans_desc, uint8_t func)
{
    arch_memset(trans_desc, 0, sizeof(ipro6_sdio3_trans_desc_t));

    /* lock */
    uintptr_t flag = ipro_irq_save();

    /* not IDLE */
    if (sdio3_ctrl.tran_sta != IPRO6_SDIO3_TRAN_STA_IDLE) {
        ipro_irq_restore(flag);
        return -1;
    }

    /* check queue */
    if (IPRO6_SDIO3_GET_UPLD_NUM(sdio3_ctrl, func) == 0) {
        ipro_irq_restore(flag);
        return 1;
    }

    *trans_desc = *(IPRO6_SDIO3_GET_UPLD_OUT_DESC(sdio3_ctrl, func));
    sdio3_ctrl.upld_queue_out[func - 1] += 1;

    /* unlock */
    ipro_irq_restore(flag);

    return 0;
}

int ipro6_sdio3_feature_control(ipro6_smid_dev_t *dev, int cmd, uintptr_t arg)
{
    int ret = 0;
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;

    switch (cmd) {
        case IPRO6_SDIO3_CMD_INIT_READY:
            /* queue ctrl ready */
            if (sdio3_ctrl.tran_sta == IPRO6_SDIO3_TRAN_STA_IDLE) {
                sdio3_ctrl.tran_sta = IPRO6_SDIO3_TRAN_STA_READY;
            }
            /* card ready */
            regval = getreg32(reg_base + IPRO6_SDIO3_CONTROL_REGISTER_OFFSET);
            regval |= IPRO6_SDIO3_CARD_INIT_DONE;
            putreg32(regval, reg_base + IPRO6_SDIO3_CONTROL_REGISTER_OFFSET);
            break;

        case IPRO6_SDIO3_CMD_GET_CARD_STA:
            /* get card status (Resp_1) */
            regval = getreg32(reg_base + IPRO6_SDIO3_COMMAND_REGISTER_OFFSET);
            regval = (regval & IPRO6_SDIO3_BLOCK_SIZE_MASK) >> IPRO6_SDIO3_BLOCK_SIZE_SHIFT;
            ret = regval;
            break;

        case IPRO6_SDIO3_CMD_GET_BUS_WIDTH:
            /* get bus width */
            regval = getreg32(reg_base + IPRO6_SDIO3_COMMAND_REGISTER_OFFSET);
            regval = (regval & IPRO6_SDIO3_CURRENT_BUS_WIDTH_MASK) >> IPRO6_SDIO3_CURRENT_BUS_WIDTH_SHIFT;
            if (regval == 0) {
                ret = 1;
            } else if (regval == 1) {
                ret = 4;
            } else if (regval == 2) {
                ret = 8;
            } else {
                ret = -1;
            }
            break;

        case IPRO6_SDIO3_CMD_SET_FUNC_CARD_READY:
            /* get func1/func2 ready sta, arg: func num */
            regval = 1;
            ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(arg) + IPRO6_SDIO3_CUSTOM_REG_FUNC_CARD_READY, &regval, 1);
            ret = (uint8_t)regval;
            break;

        case IPRO6_SDIO3_CMD_GET_FUNC_HOST_READY:
            /* get func1/func2 ready sta, arg: func num */
            regval = 0;
            ipro6_sdio3_custom_reg_read(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(arg) + IPRO6_SDIO3_CUSTOM_REG_FUNC_HOST_READY, &regval, 1);
            ret = (uint8_t)regval;
            break;

        case IPRO6_SDIO3_CMD_GET_FUNC_BLK_SIZE:
            /* get func1/func2 blk size, arg: func num */
            regval = 0;
            ipro6_sdio3_custom_reg_read(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(arg) + IPRO6_SDIO3_CUSTOM_REG_FUNC_BLOCK_SIZE, &regval, 2);
            ret = (uint16_t)regval;
            break;

        case IPRO6_SDIO3_CMD_GET_DNLD_MAX_SIZE:
            /* get func1/func2 dnld max size, arg: func num */
            regval = 0;
            ipro6_sdio3_custom_reg_read(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(arg) + IPRO6_SDIO3_CUSTOM_REG_FUNC_DNLD_MAX_SIZE, &regval, 2);
            ret = (uint16_t)regval;
            break;

        case IPRO6_SDIO3_CMD_GET_UPLD_MAX_SIZE:
            /* get func1/func2 upld max size, arg: func num */
            regval = 0;
            ipro6_sdio3_custom_reg_read(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(arg) + IPRO6_SDIO3_CUSTOM_REG_FUNC_UPLD_MAX_SIZE, &regval, 2);
            ret = (uint16_t)regval;
            break;

        case IPRO6_SDIO3_CMD_GET_DNLD_QUEUE_WAIT_NUM:
            ret = IPRO6_SDIO3_GET_DNLD_NUM(sdio3_ctrl, arg);
            break;

        case IPRO6_SDIO3_CMD_GET_DNLD_QUEUE_AVAILABLE:
            ret = IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX - IPRO6_SDIO3_GET_DNLD_NUM(sdio3_ctrl, arg);
            break;

        case IPRO6_SDIO3_CMD_GET_UPLD_QUEUE_WAIT_NUM:
            ret = IPRO6_SDIO3_GET_UPLD_NUM(sdio3_ctrl, arg);
            break;

        case IPRO6_SDIO3_CMD_GET_UPLD_QUEUE_AVAILABLE:
            ret = IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX - IPRO6_SDIO3_GET_UPLD_NUM(sdio3_ctrl, arg);
            break;

        default:
            ret = -1;
            break;
    }

    return ret;
}

int ipro6_sdio3_irq_attach(ipro6_smid_dev_t *dev, ipro6_sdio3_irq_cb_t irq_event_cb, void *arg)
{
    sdio3_irq_event_cb = irq_event_cb;
    sdio3_irq_arg = arg;

    return 0;
}

/* isr callback: cmd0/cmd52 soft reset and func_x reset  */
/* Re-sync the UPLD (device->host) side after a host reset (CMD0/CMD52 soft reset or
 * a function reset). A host reset resets the host's read position to zero but left
 * the device's upld queue in/out pointers + the read-count "doorbell" advertising a
 * frame the host will never ack -- a phantom read-count that the host then CMD53-read
 * into a stale/recycled buffer = descriptor-pointer garbage (is_amsdu/has_twin != 0)
 * -> read error -> another host reset -> the ~12 s D-2b flap cycle that never healed.
 * Reset the upld queue (driver counters + host-visible custom-reg copies) + doorbell
 * to empty so the next real upld re-establishes sync.
 *
 * The DNLD (host->device) side is re-synced too, for a clean symmetric reset: the
 * queue in/out pointers (driver counters + host-visible custom-reg copies) are zeroed
 * here, and the trcver's SMID_EVT_RESET handler (smid_rx_pool_reseed) reclaims every
 * armed RX buffer to the pool and re-arms from scratch -- restoring the exact
 * post-handshake pool/queue state, so a reset can never leak or strand a partially
 * consumed buffer. Safe on every enumeration reset because: the DMA is already stopped
 * above, so no in-flight transfer races the queue reset; with the dnld queue emptied,
 * the host's next CMD53 write hits GET_DNLD_NUM==0 -> DAT0 back-pressure holds it until
 * the worker re-arms (no lost write); and the current host transport never touches the
 * 0x48/0x4C custom-reg window (dnld = CMD53 + DAT0, upld = Fn1 read-count), so these
 * device-side custom-reg writes cannot race a host access. This does NOT touch
 * present / re-init the MAC (cf. smid_software_reset), so the handshake path is
 * undisturbed. */
void ipro6_sdio3_reset_resync(ipro6_smid_dev_t *dev)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;

    /* stop any in-flight / page-halted DMA so it can never resume into a stale
     * buffer after the upld queue is reset */
    regval = getreg32(reg_base + IPRO6_SDIO3_DMA1_CONTROL_REGISTER_OFFSET);
    regval &= ~IPRO6_SDIO3_DMA1_ADDRESS_VALID;
    putreg32(regval, reg_base + IPRO6_SDIO3_DMA1_CONTROL_REGISTER_OFFSET);

    /* clear the upld read-count doorbell -- a stale non-zero count is the phantom
     * the host reads against an empty queue (uQemp) that sustains the flap cycle */
    putreg16(0, reg_base + IPRO6_SDIO3_FUNCTION1_CONTROL_REGISTER_OFFSET);
    putreg16(0, reg_base + IPRO6_SDIO3_FUNCTION2_CONTROL_REGISTER_OFFSET);

    sdio3_ctrl.tran_sta   = IPRO6_SDIO3_TRAN_STA_IDLE;
    sdio3_ctrl.trans_desc = NULL;

    /* reset BOTH the upld and dnld queues (driver counters + host-visible custom-reg
     * copies) to empty -- the driver half of the clean symmetric re-sync. The trcver
     * SMID_EVT_RESET handler then reseeds + re-arms the dnld pool from this zeroed
     * state (dnld_push increments dnld_queue_in from 0). */
    for (uint8_t i = 1; i <= IPRO6_SDIO3_FUNC_NUM_MAX; i++) {
        sdio3_ctrl.upld_queue_in[i - 1]  = 0;
        sdio3_ctrl.upld_queue_out[i - 1] = 0;
        sdio3_ctrl.dnld_queue_in[i - 1]  = 0;
        sdio3_ctrl.dnld_queue_out[i - 1] = 0;
        regval = 0;
        ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(i) + IPRO6_SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_IN,  &regval, 1);
        ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(i) + IPRO6_SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_OUT, &regval, 1);
        ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(i) + IPRO6_SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_IN,   &regval, 1);
        ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(i) + IPRO6_SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_OUT,  &regval, 1);
    }
}

static void ipro6_sdio3_isr_cb_soft_reset(ipro6_smid_dev_t *dev, uint32_t int_sta1)
{
    uint32_t reg_base = dev->reg_base;

    /* FIXME: after a soft reset, CMD5 triggers IPRO6_SDIO3_PROGRAM_START, disable it */
    if (int_sta1 & IPRO6_SDIO3_PROGRAM_START) {
        /* clr int */
        putreg32(IPRO6_SDIO3_PROGRAM_START, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        /* disable int signal */
        int_sta1 = getreg32(reg_base + IPRO6_SDIO3_INTERRUPT_SIGNAL_ENABLE_REGISTER_OFFSET);
        int_sta1 &= ~IPRO6_SDIO3_PROGRAM_START;
        putreg32(int_sta1, reg_base + IPRO6_SDIO3_INTERRUPT_SIGNAL_ENABLE_REGISTER_OFFSET);
        return;
    }

    /* 1. cmd0/cmd52 soft reset */
    if (int_sta1 & IPRO6_SDIO3_CMD0_CMD52_SOFT_RESET) {
        /* clr int */
        putreg32(IPRO6_SDIO3_CMD0_CMD52_SOFT_RESET, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        IPRO6_SDIO3_DRV_DBG("sdio3 soft reset\r\n");

        /* re-sync the whole transport (queues + doorbell + DMA), not just tran_sta */
        ipro6_sdio3_reset_resync(dev);

        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_SOFT_RESET, NULL);
        }
        return;
    }

    /* 2. func1 reset */
    if (int_sta1 & IPRO6_SDIO3_FUNCTION1_RESET) {
        /* clr int */
        putreg32(IPRO6_SDIO3_FUNCTION1_RESET, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        int_sta1 = getreg32(reg_base + IPRO6_SDIO3_IOREADY_REGISTER_OFFSET);
        IPRO6_SDIO3_DRV_DBG("sdio3 func1 reset\r\n");

        /* same clean re-sync as the soft reset -- the func1 reset previously left
         * tran_sta/queues/doorbell stale (no restore at all), which desynced the
         * upld queue and drove the flap cycle */
        ipro6_sdio3_reset_resync(dev);

        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_FUNC1_RESET, NULL);
        }
    }

    /* 3. func2 reset */
    if (int_sta1 & IPRO6_SDIO3_FUNCTION2_RESET) {
        /* clr int */
        putreg32(IPRO6_SDIO3_FUNCTION2_RESET, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        int_sta1 = getreg32(reg_base + IPRO6_SDIO3_IOREADY_REGISTER_OFFSET);
        IPRO6_SDIO3_DRV_DBG("sdio3 func2 reset\r\n");

        /*TODO: func2 reset restore */

        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_FUNC2_RESET, NULL);
        }
    }
}

/* isr callback: func1/func2 dnld/upld transfer start int */
static void ipro6_sdio3_isr_cb_tran_start(ipro6_smid_dev_t *dev, uint32_t int_sta1)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;

    uint8_t func_n;
    uint16_t blk_cnt, blk_size;
    uint32_t cmd53_arg;

    cmd53_arg = getreg32(reg_base + IPRO6_SDIO3_ARGUMENT_REGISTER_OFFSET);
    func_n = (cmd53_arg & IPRO6_SDIO3_CMD53_RW_FUNC_MASK) >> IPRO6_SDIO3_CMD53_RW_FUNC_SHIFT;
    blk_cnt = (cmd53_arg & IPRO6_SDIO3_CMD53_RW_COUNT_MASK) >> IPRO6_SDIO3_CMD53_RW_COUNT_SHIFT;

    /* The write/read-start interrupt is not per-function, so func_n comes from
     * the Argument register, which the host's next command overwrites if this
     * ISR is delayed. A garbage func_n (e.g. 0) would index the queue arrays
     * out of bounds (func_n - 1). Clamp to a valid function. */
    if (func_n == 0 || func_n > IPRO6_SDIO3_FUNC_NUM_MAX) {
        func_n = 1;
    }

    if (cmd53_arg & IPRO6_SDIO3_CMD53_RW_BLOCK_MODE_MASK) {
        /* TODO: funcx block size? */
        regval = getreg32(reg_base + IPRO6_SDIO3_COMMAND_REGISTER_OFFSET);
        blk_size = (regval & IPRO6_SDIO3_BLOCK_SIZE_MASK) >> IPRO6_SDIO3_BLOCK_SIZE_SHIFT;
    } else {
        blk_size = 1;
        if (blk_cnt == 0) {
            blk_cnt = 512;
        }
    }
    IPRO6_SDIO3_DRV_DBG("cmd53 trans, arg:0x%08X, size:%d*%d\r\n", cmd53_arg, blk_size, blk_cnt);

    if (int_sta1 & IPRO6_SDIO3_WRITE_START_INTERRUPT) {
        /* dnld opt */
        putreg32(IPRO6_SDIO3_WRITE_START_INTERRUPT, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        /* ckeck queue */
        if (IPRO6_SDIO3_GET_DNLD_NUM(sdio3_ctrl, func_n) == 0) {
            if (sdio3_irq_event_cb != NULL) {
                sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_ERR_DNLD_QUEUE, NULL);
            }
            return;
        }
        /* update transfer status */
        sdio3_ctrl.tran_sta = IPRO6_SDIO3_TRAN_STA_BUSY_DNLD;
        sdio3_ctrl.trans_desc = IPRO6_SDIO3_GET_DNLD_OUT_DESC(sdio3_ctrl, func_n);

        /* dnld byte count from the Argument (count) + Command (block size)
         * registers, decoded above. These are loaded by the host's CMD53 write
         * and stay stable for the whole data phase: SDIO serialises the host's
         * next command until the data + busy completes, so even a briefly
         * delayed ISR still reads THIS write's count. (Measured on HW: under
         * concurrent CMD52 read-count polling the Argument is never clobbered.)
         * Do NOT use the custom_reg host_dnld_len — the 0x48/0x4C indirect
         * window is reposition-corrupted by the controller servicing the host;
         * nor the Block Count register — it counts down during the transfer. */
        uint32_t dnld_len = (uint32_t)blk_size * blk_cnt;
        if (dnld_len == 0 || dnld_len > sdio3_ctrl.trans_desc->data_len) {
            ipro6_sdio3_dnld_len_fallback++;
            dnld_len = sdio3_ctrl.trans_desc->data_len;
        }
        sdio3_ctrl.trans_desc->data_len = (uint16_t)dnld_len;

    } else if (int_sta1 & IPRO6_SDIO3_READ_START_INTERRUPT) {
        /* upld opt */
        putreg32(IPRO6_SDIO3_READ_START_INTERRUPT, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        /* ckeck queue */
        if (IPRO6_SDIO3_GET_UPLD_NUM(sdio3_ctrl, func_n) == 0) {
            if (sdio3_irq_event_cb != NULL) {
                sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_ERR_UPLD_QUEUE, NULL);
            }
            return;
        }
        /* update transfer status */
        sdio3_ctrl.tran_sta = IPRO6_SDIO3_TRAN_STA_BUSY_UPLD;
        sdio3_ctrl.trans_desc = IPRO6_SDIO3_GET_UPLD_OUT_DESC(sdio3_ctrl, func_n);
        /* clear upld length */
        if (func_n == 1) {
            putreg32(0, reg_base + IPRO6_SDIO3_FUNCTION1_CONTROL_REGISTER_OFFSET);
        } else if (func_n == 2) {
            putreg32(0, reg_base + IPRO6_SDIO3_FUNCTION2_CONTROL_REGISTER_OFFSET);
        }
    }

    /* dnld: use the decoded download length (data_len, set above, with the
     * out-of-range clamp applied). upld: the device is the source, so it sends
     * exactly the host's read count (blk_size * blk_cnt) = the doorbell'd byte
     * count. (Both are the same blk_size*blk_cnt unless the dnld was clamped.) */
    uint32_t adma_len = (int_sta1 & IPRO6_SDIO3_WRITE_START_INTERRUPT)
                            ? sdio3_ctrl.trans_desc->data_len
                            : (uint32_t)blk_size * blk_cnt;

#if IPRO6_SDIO3_DMA1_MODE_ENABLE
    /* dma1 init */
    IPRO6_SDIO3_DRV_DBG("dma1 start %p len:%u\r\n", sdio3_ctrl.trans_desc->buff, (unsigned)adma_len);
    ipro6_sdio3_dma1_start(dev, (uint32_t)(uintptr_t)sdio3_ctrl.trans_desc->buff, adma_len);
#else
    /* adma init */
    IPRO6_SDIO3_DRV_DBG("adma start %p len:%u\r\n", sdio3_ctrl.trans_desc->buff, (unsigned)adma_len);
    if (sdio3_ctrl.trans_desc->flags & IPRO6_SDIO3_TDF_SG) {
        /* ZCEN2-stride dnld: buff is the caller's scatter descriptor */
        ipro6_sdio3_adma_start_sg(dev, adma_hw_desc_buff,
                                  (const ipro6_sdio3_sg_desc_t *)sdio3_ctrl.trans_desc->buff, adma_len);
    } else {
        ipro6_sdio3_adma_start(dev, adma_hw_desc_buff, (uint32_t)(uintptr_t)sdio3_ctrl.trans_desc->buff, adma_len);
    }
#endif
}

#if IPRO6_SDIO3_DMA1_MODE_ENABLE
/* isr callback: dma1 page int */
static void ipro6_sdio3_isr_cb_dma1_page(ipro6_smid_dev_t *dev, uint32_t int_sta1)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;

    /* clr int */
    putreg32(IPRO6_SDIO3_DMA1_INTERRUPT, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);

    if (sdio3_ctrl.tran_sta == IPRO6_SDIO3_TRAN_STA_BUSY_DNLD || sdio3_ctrl.tran_sta == IPRO6_SDIO3_TRAN_STA_BUSY_UPLD) {
        ipro6_sdio3_trans_desc_t *trans_desc = sdio3_ctrl.trans_desc;
        regval = getreg32(reg_base + IPRO6_SDIO3_DMA1_ADDRESS_REGISTER_OFFSET);
        if (regval % IPRO6_SDIO3_DMA1_PAGA_SIZE) {
            regval += IPRO6_SDIO3_DMA1_PAGA_SIZE - ((uint32_t)(uintptr_t)trans_desc->buff % IPRO6_SDIO3_DMA1_PAGA_SIZE);
        } else {
            regval += IPRO6_SDIO3_DMA1_PAGA_SIZE;
        }

        /* address */
        putreg32(regval, reg_base + IPRO6_SDIO3_DMA1_ADDRESS_REGISTER_OFFSET);
        IPRO6_SDIO3_DRV_DBG("dma1 page addr: 0x%08X\r\n", regval);

        /* start */
        regval = getreg32(reg_base + IPRO6_SDIO3_DMA1_CONTROL_REGISTER_OFFSET);
        regval |= IPRO6_SDIO3_DMA1_ADDRESS_VALID;
        putreg32(regval, reg_base + IPRO6_SDIO3_DMA1_CONTROL_REGISTER_OFFSET);
    } else {
        IPRO6_SDIO3_DRV_DBG("dma1 err\r\n");
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_ERR_UNKNOW, NULL);
        }
    }
}
#endif

/* isr callback: func1/func2  dnld/upld transfer complete int */
static void ipro6_sdio3_isr_cb_tran_complete(ipro6_smid_dev_t *dev, uint32_t int_sta1)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;

    /* clr int */
    putreg32(IPRO6_SDIO3_TRANSFER_COMPLETE_INTERRUPT, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
    IPRO6_SDIO3_DRV_DBG("trans complete\r\n");

#if IPRO6_SDIO3_DMA1_MODE_ENABLE
    putreg32(IPRO6_SDIO3_DMA1_INTERRUPT, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
#endif

    if (sdio3_ctrl.tran_sta == IPRO6_SDIO3_TRAN_STA_BUSY_DNLD) {
        /* data_len was set at WRITE_START from the Argument/Command registers
         * (the host's CMD53 byte count); deliver it as-is. */
        /* dnld complete, update dnld queue */
        sdio3_ctrl.dnld_queue_out[sdio3_ctrl.trans_desc->func - 1] += 1;
        ipro6_sdio3_trans_desc_t comp_desc = *sdio3_ctrl.trans_desc;
        /* update custom reg */
        uint8_t func = comp_desc.func;
        uint8_t dnld_out = sdio3_ctrl.dnld_queue_out[func - 1];
        ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(func) + IPRO6_SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_OUT, &dnld_out, 1);
        /* update transfer status */
        sdio3_ctrl.tran_sta = IPRO6_SDIO3_TRAN_STA_READY;
        sdio3_ctrl.trans_desc = NULL;

        /* callback first: it re-arms a fresh dnld buffer (dnld_push) if the
         * pool has one, so the queue count below reflects post-re-arm state. */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_DNLD_CPL, &comp_desc);
        }

        /* dnld flow control via DAT0 busy. Release DAT0 (PROGRAM_DONE) only if a
         * posted buffer remains for the host's NEXT write. If the re-arm above
         * could not replenish one (the buffer pool is momentarily drained — e.g.
         * a cold-start burst before the worker runs), WITHHOLD PROGRAM_DONE: the
         * device keeps DAT0 busy so the host's next CMD53 write naturally blocks
         * until the worker frees a buffer and dnld_push releases DAT0. This
         * back-pressures the host with zero host-side flow control and prevents
         * the dnld-queue-empty -110 that used to wedge the host. */
        if (IPRO6_SDIO3_GET_DNLD_NUM(sdio3_ctrl, func) > 0) {
            regval = getreg32(reg_base + IPRO6_SDIO3_CONTROL_REGISTER_OFFSET);
            regval |= IPRO6_SDIO3_PROGRAM_DONE;
            putreg32(regval, reg_base + IPRO6_SDIO3_CONTROL_REGISTER_OFFSET);
        } else {
            sdio3_dnld_pgmdone_deferred = 1;
            ipro6_sdio3_dnld_backpressure++;
        }

    } else if (sdio3_ctrl.tran_sta == IPRO6_SDIO3_TRAN_STA_BUSY_UPLD) {
        /* upld complete, update upld queue */
        sdio3_ctrl.upld_queue_out[sdio3_ctrl.trans_desc->func - 1] += 1;
        ipro6_sdio3_trans_desc_t comp_desc = *sdio3_ctrl.trans_desc;
        /* update custom reg */
        uint8_t func = comp_desc.func;
        uint8_t upld_out = sdio3_ctrl.upld_queue_out[func - 1];
        ipro6_sdio3_custom_reg_write(dev, IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(func) + IPRO6_SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_OUT, &upld_out, 1);
        /* update transfer status */
        sdio3_ctrl.tran_sta = IPRO6_SDIO3_TRAN_STA_READY;
        sdio3_ctrl.trans_desc = NULL;

        /* doorbell the next head frame's byte count (0 if the queue is now empty) */
        ipro6_sdio3_upld_doorbell(dev, comp_desc.func);
        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_UPLD_CPL, &comp_desc);
        }
    } else {
        IPRO6_SDIO3_DRV_DBG("complete err\r\n");
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_ERR_UNKNOW, NULL);
        }
    }
}

/* isr callback: func1/func2  dnld/upld transfer abort int */
static void ipro6_sdio3_isr_cb_tran_abort(ipro6_smid_dev_t *dev, uint32_t int_sta1)
{
    uint32_t reg_base = dev->reg_base;

    /* clr int */
    putreg32(IPRO6_SDIO3_FUNCTIONX_ABORT_INTERRUPT, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
    IPRO6_SDIO3_DRV_DBG("trans abort\r\n");

    if (sdio3_ctrl.tran_sta == IPRO6_SDIO3_TRAN_STA_BUSY_DNLD) {
        /* dnld abort */
        ipro6_sdio3_adma_stop(dev);
        ipro6_sdio3_trans_desc_t abort_desc = *sdio3_ctrl.trans_desc;
        /* update transfer status, and clean data_len */
        sdio3_ctrl.tran_sta = IPRO6_SDIO3_TRAN_STA_READY;
        sdio3_ctrl.trans_desc->data_len = 0;
        sdio3_ctrl.trans_desc = NULL;

        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_ERR_DNLD_ABORT, &abort_desc);
        }

    } else if (sdio3_ctrl.tran_sta == IPRO6_SDIO3_TRAN_STA_BUSY_UPLD) {
        /* upld abort */
        ipro6_sdio3_adma_stop(dev);
        ipro6_sdio3_trans_desc_t abort_desc = *sdio3_ctrl.trans_desc;
        /* update transfer status */
        sdio3_ctrl.tran_sta = IPRO6_SDIO3_TRAN_STA_READY;
        sdio3_ctrl.trans_desc = NULL;

        /* doorbell the next head frame's byte count (0 if the queue is now empty) */
        ipro6_sdio3_upld_doorbell(dev, abort_desc.func);
        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_ERR_UPLD_ABORT, &abort_desc);
        }
    }
#if 0
        /* TODO: this interrupt occurs during initialization */
        else {
            IPRO6_SDIO3_DRV_DBG("abort err\r\n");
            if (sdio3_irq_event_cb != NULL) {
                sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_ERR_UNKNOW, NULL);
            }
        }
#endif
}

/* isr callback: func1/func2  dnld/upld transfer abort int */
static void ipro6_sdio3_isr_cb_crc_err(ipro6_smid_dev_t *dev, uint32_t int_sta1)
{
    uint32_t reg_base = dev->reg_base;

    /* clr int */
    putreg32(IPRO6_SDIO3_FUNCTIONX_CRC_END_ERROR_INTERRUPT, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
    IPRO6_SDIO3_DRV_DBG("crc error\r\n");

    if (sdio3_ctrl.tran_sta == IPRO6_SDIO3_TRAN_STA_BUSY_DNLD) {
        /* dnld crc error */
        ipro6_sdio3_adma_stop(dev);
        ipro6_sdio3_trans_desc_t crc_desc = *sdio3_ctrl.trans_desc;
        /* update transfer status, and clean data_len */
        sdio3_ctrl.tran_sta = IPRO6_SDIO3_TRAN_STA_READY;
        sdio3_ctrl.trans_desc->data_len = 0;

        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_ERR_CRC, &crc_desc);
        }
        return;
    }
#if 1
    /* callback */
    if (sdio3_irq_event_cb != NULL) {
        sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_ERR_UNKNOW, NULL);
    }
#else
    else if (sdio3_ctrl.tran_sta == IPRO6_SDIO3_TRAN_STA_BUSY_UPLD) {

        /* upld abort */
        ipro6_sdio3_trans_desc_t abort_desc = *sdio3_ctrl.trans_desc;
        /* update transfer status */
        sdio3_ctrl.tran_sta = IPRO6_SDIO3_TRAN_STA_READY;
        sdio3_ctrl.trans_desc = NULL;

        /* TODO: write read_len reg (card2host int) */
        if (abort_desc.func == 1 && IPRO6_SDIO3_GET_UPLD_NUM(sdio3_ctrl, 1)) {
            putreg16(1, dev->reg_base + IPRO6_SDIO3_FUNCTION1_CONTROL_REGISTER_OFFSET);
        } else if (abort_desc.func == 2 && IPRO6_SDIO3_GET_UPLD_NUM(sdio3_ctrl, 2)) {
            putreg16(1, dev->reg_base + IPRO6_SDIO3_FUNCTION2_CONTROL_REGISTER_OFFSET);
        }

        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_ERR_CRC, &abort_desc);
        }
    }
#endif
}

/* isr callback: voltage switch to 1.8v int (cmd11) */
static void ipro6_sdio3_isr_cb_vol_switch(ipro6_smid_dev_t *dev, uint32_t int_sta1)
{
    uint32_t reg_base = dev->reg_base;

    /* voltage switch to 1.8v int (cmd11) */
    if (int_sta1 & IPRO6_SDIO3_CMD11_INTERRUPT) {
        /* clr int */
        putreg32(IPRO6_SDIO3_CMD11_INTERRUPT, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        IPRO6_SDIO3_DRV_DBG("cmd11 come\r\n");
        return;
    }
    /* clock stop after cmd11, disable output */
    if (int_sta1 & IPRO6_SDIO3_CMD11_CLK_STOP) {
        /* clr init */
        putreg32(IPRO6_SDIO3_CMD11_CLK_STOP, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        IPRO6_SDIO3_DRV_DBG("cmd11 clk stop\r\n");
        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_SIG1V8_SWITCH, NULL);
        }
        return;
    }
    /* clock start after cmd11, enable output */
    if (int_sta1 & IPRO6_SDIO3_CMD11_CLK_START) {
        /* clr init */
        putreg32(IPRO6_SDIO3_CMD11_CLK_START, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        IPRO6_SDIO3_DRV_DBG("cmd11 clk start\r\n");
        return;
    }
}

static void ipro6_sdio3_isr(void)
{
    ipro6_smid_dev_t *dev = sdio3_dev;   /* C2 fix: ECLIC handler receives no arg */
    uint32_t reg_base = dev->reg_base;
    uint32_t int_sta;
    uint32_t sta1_entry;

    int_sta = getreg32(reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
    sta1_entry = int_sta;      /* save: branches below overwrite int_sta */
    IPRO6_SDIO3_DRV_DBG("isr sta 0x%08X\r\n", int_sta);

    /* 1. cmd0/cmd52 soft reset or func_x reset  */
    /* TODO: IPRO6_SDIO3_PROGRAM_START,it will trigger when the card is recognized again. */
    if (int_sta & (IPRO6_SDIO3_CMD0_CMD52_SOFT_RESET | IPRO6_SDIO3_FUNCTION1_RESET | IPRO6_SDIO3_FUNCTION2_RESET | IPRO6_SDIO3_PROGRAM_START)) {
        ipro6_sdio3_isr_cb_soft_reset(dev, int_sta);
        return;
    }

    /* 2. func1/func2  dnld/upld start int */
    if ((int_sta & IPRO6_SDIO3_WRITE_START_INTERRUPT) || (int_sta & IPRO6_SDIO3_READ_START_INTERRUPT)) {
        ipro6_sdio3_isr_cb_tran_start(dev, int_sta);
        return;
    }

    /* 3. func1/func2  dnld/upld complete int */
    if (int_sta & IPRO6_SDIO3_TRANSFER_COMPLETE_INTERRUPT) {
        ipro6_sdio3_isr_cb_tran_complete(dev, int_sta);
        return;
    }

    /* 3.5 DMA page-boundary (bit1): the underlying DMA engine halts at a 4 KB
     * page mid-transfer and clears ADDRESS_VALID, waiting to be resumed. In DMA1
     * mode reload the next page address; in ADMA mode the chained descriptors
     * already hold each page-bounded segment's address, so just re-assert
     * ADDRESS_VALID to let the engine advance to the next descriptor. Without
     * this the transfer stalls forever -> host CMD53 timeout -> card flap. */
    if (int_sta & IPRO6_SDIO3_DMA1_INTERRUPT) {
#if IPRO6_SDIO3_DMA1_MODE_ENABLE
        ipro6_sdio3_isr_cb_dma1_page(dev, int_sta);
#else
        uint32_t regval;
        putreg32(IPRO6_SDIO3_DMA1_INTERRUPT, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        regval = getreg32(reg_base + IPRO6_SDIO3_DMA1_CONTROL_REGISTER_OFFSET);
        regval |= IPRO6_SDIO3_DMA1_ADDRESS_VALID;
        putreg32(regval, reg_base + IPRO6_SDIO3_DMA1_CONTROL_REGISTER_OFFSET);
#endif
        return;
    }

    /* 4. func1/func2  dnld/upld abort int */
    if (int_sta & IPRO6_SDIO3_FUNCTIONX_ABORT_INTERRUPT) {
        ipro6_sdio3_isr_cb_tran_abort(dev, int_sta);
        return;
    }

    /* 5. voltage switch to 1.8v int (cmd11) */
    if (int_sta & (IPRO6_SDIO3_CMD11_INTERRUPT | IPRO6_SDIO3_CMD11_CLK_STOP | IPRO6_SDIO3_CMD11_CLK_START)) {
        ipro6_sdio3_isr_cb_vol_switch(dev, int_sta);
        return;
    }

    /* 6. CRC error int */
    if (int_sta & IPRO6_SDIO3_FUNCTIONX_CRC_END_ERROR_INTERRUPT) {
        ipro6_sdio3_isr_cb_crc_err(dev, int_sta);
        return;
    }

    /* 7. int sta2, adma error int */
    int_sta = getreg32(reg_base + IPRO6_SDIO3_INTERRUPT_STATUS2_REGISTER_OFFSET);
    if (int_sta & IPRO6_SDIO3_ADMA_ERROR_INTERRUPT) {
        /* clr int */
        putreg32(IPRO6_SDIO3_ADMA_ERROR_INTERRUPT, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS2_REGISTER_OFFSET);
        IPRO6_SDIO3_DRV_DBG("adma err\r\n");

        /* TODO: error restore */
        sdio3_ctrl.tran_sta = IPRO6_SDIO3_TRAN_STA_READY;

        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_ERR_ADMA, NULL);
        }

        return;
    }

    /* 4. unknown int: an enabled status bit no branch above handled. Leaving it
     * set re-asserts the IRQ line -> ISR storm -> hang. The restricted STATUS-
     * ENABLE (init) should keep this unreachable, but defensively CLEAR both
     * status registers (W1C) so a stray unhandled interrupt can never storm. */
    putreg32(sta1_entry, reg_base + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
    putreg32(getreg32(reg_base + IPRO6_SDIO3_INTERRUPT_STATUS2_REGISTER_OFFSET),
             reg_base + IPRO6_SDIO3_INTERRUPT_STATUS2_REGISTER_OFFSET);
    if (sdio3_irq_event_cb != NULL) {
        IPRO6_SDIO3_DRV_DBG("sdio3 unknow int\r\n");
        sdio3_irq_event_cb(sdio3_irq_arg, IPRO6_SDIO3_IRQ_EVENT_ERR_UNKNOW, NULL);
    }
}
