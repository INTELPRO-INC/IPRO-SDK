/*
 * Copyright (C) 2018-2025 AICSemi Ltd.
 *
 * All Rights Reserved
 *
 * USB port header: platform USB interface declarations.
 *
 * 架构 (参考 SDIO 侧 sdio_co.c ↔ sdio_port.c 对接模式):
 *
 *   ┌──────────────────────────────────────────────────────────┐
 *   │ 上层 (wifi_if / fhost_tx / fhost_rx)                     │
 *   │   调用 aicwf_hal_* (hal_co.c)                            │
 *   └────────────────────────┬─────────────────────────────────┘
 *                            │ #ifdef CONFIG_USB_SUPPORT
 *   ┌────────────────────────┴─────────────────────────────────┐
 *   │ CO 层 (usb_co.c) — 协议逻辑                              │
 *   │   msg 打包 / TX 聚合 / RX 解包 / 流控 / 缓冲管理         │
 *   │   通过下方接口调用 Port 层 (CO → Port 下行)              │
 *   │   通过回调向 Port 层提供 RX/TX 完成 (Port → CO 上行)     │
 *   └────────────────────────┬─────────────────────────────────┘
 *                            │
 *   ┌────────────────────────┴─────────────────────────────────┐
 *   │ Port 层 (usb_port.c) — 平台相关                          │
 *   │   URB 提交 / URB 池管理 / endpoint 解析                  │
 *   │   URB complete 回调 → aicwf_usb_rx_complete              │
 *   └──────────────────────────────────────────────────────────┘
 *
 * 对比 Linux 参考驱动 (aicwf_usb.c), USB 不需要 SDIO 的以下概念:
 *   - CMD52/CMD53 寄存器读写 (USB 用 bulk URB)
 *   - claim_host/release_host (USB 无 SDIO host 概念)
 *   - readsb/writesb (USB 用 URB bulk in/out)
 *   - release_ep2 (SDIO function 2 释放, USB 无此概念)
 *   - host_enable_isr/set_irq_handler/interrupt_init/isr (USB 用 URB completion)
 *
 * Platform 必须实现以下函数 (CONFIG_USB_SUPPORT 激活时)。
 * Stub 实现见 platform/hal/usb_port.c。
 */

#ifndef _USB_PORT_H_
#define _USB_PORT_H_

#include "rtos_port.h"
#include "co_list.h"
#include "aic_usb_types.h"
#include "usb_def.h"

#define CONFIG_RXTASK_INUSB    0
#define CONFIG_RX_POLLING      0
#define CONFIG_IRQ_SUS         0

/* USB TX endpoint selection (对应 SDIO FUNC_1/FUNC_2 区分) */
#define USB_EP_DATA            0    /* 数据端点 (bulk OUT data) */
#define USB_EP_MSG             1    /* 消息端点 (bulk OUT msg, 可选) */

/* Export variables (provided by platform or stubs) */
extern int  msgcfm_poll_en;
extern uint8_t usb_unexcept_test;
extern struct aic_usb_dev usb_dev;

/* ================================================================ */
/*  CO → Port 下行接口 (CO 层调用, Port 层实现)                     */
/*  对应 SDIO 侧: aicwf_sdio_writesb / aicwf_sdio_readsb 等         */
/* ================================================================ */

/*
 * USB 功能初始化: 解析 endpoint, 分配 URB 池, 初始化同步原语。
 * 不提交 RX URB (由 aicwf_usb_rx_submit_all 完成)。
 * 对应 Linux aicwf_usb_init + aicwf_parse_usb。
 * 不配置任何寄存器 (USB 无 SDIO function 寄存器)。
 *
 * @chipid: 芯片 ID (PRODUCT_ID_AIC8801/DC/DW/D80 等)
 * @usbdev: USB 设备结构体指针
 * Return: 0 成功, 负值失败
 */
int  aicwf_usb_func_init(uint16_t chipid, struct aic_usb_dev *usbdev);

/*
 * USB 驱动注册: 向平台 USB core 注册 driver。
 * 对应 Linux usb_register(&aicwf_usbdrvr)。
 * RTOS 平台可在初始化时直接调用 func_init 替代。
 */
int  aicwf_usb_drv_register(void);

/*
 * 提交 TX bulk OUT URB (异步)。
 * 对应 SDIO aicwf_sdio_writesb_msg / aicwf_sdio_writesb。
 * 对应 Linux aicwf_usb_bus_txmsg / aicwf_usb_bus_txdata 内的 usb_submit_urb。
 *
 * CO 层完成 msg header 打包后调用此接口提交 URB。
 * Port 层负责 URB 分配、fill_bulk_urb、submit_urb。
 * URB 完成后 Port 层调用 aicwf_usb_tx_complete 通知 CO 层。
 *
 * @ep: USB_EP_DATA 或 USB_EP_MSG (选择 bulk OUT 端点)
 * @buf: 待发送数据缓冲区 (已包含 4B usb_header)
 * @len: 数据长度 (字节)
 * Return: 0 成功 (URB 已提交), 负值失败
 */
int  aicwf_usb_submit_tx_urb(uint8_t ep, uint8_t *buf, uint32_t len);

/*
 * DATA 单包发送: 在 Port 拷贝时前置 4B USB header (len+type)。
 * 供 aicwf_usb_send 非 AGGR 路径使用；payload 为 hostdesc+frame，不含 USB 头。
 */
int  aicwf_usb_submit_tx_data(uint8_t *payload, uint32_t payload_len);

/*
 * 同步等待 TX 消息完成 (仅用于 msg 路径)。
 * 对应 Linux aicwf_usb_bus_txmsg 内的 wait_event_timeout(msg_wait)。
 *
 * CO 层调用 aicwf_usb_submit_tx_urb(USB_EP_MSG, ...) 后调用此接口等待。
 * Port 层在 msg URB complete 回调中唤醒等待。
 *
 * @timeout_ms: 超时时间 (毫秒)
 * Return: 0 成功 (URB 完成), 负值超时或失败
 */
int  aicwf_usb_wait_tx_msg_complete(uint32_t timeout_ms);

/*
 * 查询空闲 TX URB 数量 (流控水位)。
 * 对应 Linux aicwf_usb_dev.tx_free_count。
 *
 * CO 层 aicwf_usb_send_check 调用此接口判断是否允许发送。
 * 当返回值低于 LOW_WATER 时, CO 层应停止发送。
 *
 * Return: 空闲 TX URB 数量
 */
uint32_t aicwf_usb_tx_free_count(void);

/*
 * 提交所有 RX bulk IN URB (常驻)。
 * 对应 Linux aicwf_usb_rx_submit_all_urb。
 *
 * CO 层 aicwf_usb_host_init 调用此接口启动 RX 路径。
 * Port 层从 rx_free_list 取出所有 URB, 调用 usb_submit_urb。
 * URB 完成后 Port 层调用 aicwf_usb_rx_complete 上报数据。
 *
 * Return: 0 成功, 负值失败
 */
int  aicwf_usb_rx_submit_all(void);

/*
 * FW start_app 前调用：芯片会 soft-disconnect 并以 8d81/8d83 重枚举。
 * 随后调用 aicwf_usb_wait_reconnect() 等待新设备就绪。
 */
void aicwf_usb_expect_reconnect(void);
int  aicwf_usb_wait_reconnect(uint32_t timeout_ms);

/*
 * 取消所有 pending TX/RX URB。
 * 对应 Linux usb_kill_urb + usb_unlink_urb。
 *
 * CO 层 aicwf_usb_host_predeinit/deinit 调用此接口停止收发。
 */
void aicwf_usb_kill_all_tx_urb(void);
void aicwf_usb_kill_all_rx_urb(void);

#ifdef CONFIG_OOB
/* USB 一般不使用 OOB, 保留接口供特殊平台 */
void aicwf_usb_oob_enable(void);
#endif

/* ================================================================ */
/*  Port → CO 上行回调 (CO 层实现, Port 层调用)                     */
/*  对应 SDIO 侧: SDIO ISR 触发 → aicwf_sdio_rx_task               */
/*  USB 侧: URB complete 回调 → aicwf_usb_rx_complete              */
/* ================================================================ */

/*
 * RX URB 完成回调 (Port 层调用)。
 * 对应 Linux aicwf_usb_rx_complete。
 *
 * Port 层在 bulk IN URB 完成时调用此接口, 将数据上报给 CO 层。
 * CO 层负责解析 4B header、解包、入队到 rxq、唤醒 rx_task。
 *
 * @buf: 接收数据缓冲区 (URB transfer_buffer)
 * @actual_len: URB actual_length (实际接收字节数)
 */
void aicwf_usb_rx_complete(uint8_t *buf, uint32_t actual_len);

/*
 * TX URB 完成回调 (Port 层调用)。
 * 对应 Linux TX URB complete 回调内唤醒 msg_wait。
 *
 * Port 层在 bulk OUT URB 完成时调用此接口, 通知 CO 层。
 * CO 层 (或 Port 层) 释放 URB 到 tx_free_list, 唤醒 msg 等待。
 *
 * @ep: USB_EP_DATA 或 USB_EP_MSG
 * @status: URB 完成状态 (0 成功, 负值失败)
 */
void aicwf_usb_tx_complete(uint8_t ep, int status);

#endif /* _USB_PORT_H_ */
