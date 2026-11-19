/*
 *  Copyright (c) 2019, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file includes interface definitions for FreeRTOS.
 *
 */

#ifndef OPENTHREAD_PORT_H
#define OPENTHREAD_PORT_H

#if OT_FREERTOS_ENABLE
#include <FreeRTOS.h>
#include <portmacro.h>
#include <task.h>
#endif

#include <openthread/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OT_TASK_SIZE
/* OT thread stack in StackType_t units (4 bytes each on RISC-V).
 *
 * 1472 (≈ 5.8 KB) is too tight when CONFIG_OPENTHREAD_LOG_LEVEL is at
 * debug (LogVarArgs → vsnprintf chains chew ~1–2 KB per call) — saw a
 * confirmed stack overflow in ipro_thread_mtd at 34 s of SED runtime,
 * stack overran into adjacent task memory and the next ret popped a
 * garbage RA pointing into stack itself → Illegal Instruction. 2026-05-19.
 *
 * 4096 (≈ 16 KB) was the original size before someone downsized it
 * for memory-tight no-PSRAM builds. Apps that genuinely need that
 * tighter footprint can pre-define OT_TASK_SIZE, or set
 * CONFIG_OT_TASK_SIZE_WORDS in their .config (this is a static stack in
 * .bss, so on a no-PSRAM single-region build every word here comes
 * straight out of the FreeRTOS heap). Keep it well above 1472. */
#if defined(CONFIG_OT_TASK_SIZE_WORDS) && (CONFIG_OT_TASK_SIZE_WORDS > 0)
#define OT_TASK_SIZE CONFIG_OT_TASK_SIZE_WORDS
#else
#define OT_TASK_SIZE 4096
#endif
#endif

#ifndef OT_TASK_PRORITY
#define OT_TASK_PRORITY 20
#endif

#ifndef OT_UART_RX_BUFFSIZE
#define OT_UART_RX_BUFFSIZE 384
#endif

typedef enum _ot_system_event {
    OT_SYSTEM_EVENT_NONE                                = 0,

    OT_SYSTEM_EVENT_OT_TASKLET                          = 0x00000001,

    OT_SYSTEM_EVENT_ALARM_MS_EXPIRED                    = 0x00000002,
    OT_SYSTEM_EVENT_ALARM_US_EXPIRED                    = 0x00000004,
    OT_SYSTEM_EVENT_ALARM_ALL_MASK                      = OT_SYSTEM_EVENT_ALARM_MS_EXPIRED | OT_SYSTEM_EVENT_ALARM_US_EXPIRED,
    /* back from a platform PDS: run the radio restores (ot_pm.c) */
    OT_SYSTEM_EVENT_PM_RESUMED                          = 0x00000008,

    OT_SYSTEM_EVENT_UART_TXR                            = 0x00000010,
    OT_SYSTEM_EVENT_UART_TXD                            = 0x00000020,
    OT_SYSTEM_EVENT_UART_RXD                            = 0x00000040,
    OT_SYSETM_EVENT_UART_ALL_MASK                       = OT_SYSTEM_EVENT_UART_TXR | OT_SYSTEM_EVENT_UART_TXD | OT_SYSTEM_EVENT_UART_RXD,

    OT_SYSTEM_EVENT_RADIO_TX_DONE_NO_ACK_REQ            = 0x00000100,
    OT_SYSTEM_EVENT_RADIO_TX_ERROR                      = 0x00000200,
    OT_SYSTEM_EVENT_RADIO_TX_ACKED                      = 0x00000400,
    OT_SYSTEM_EVENT_RADIO_TX_NO_ACK                     = 0x00000800,
    OT_SYSTEM_EVENT_RADIO_TX_ALL_MASK                   = OT_SYSTEM_EVENT_RADIO_TX_DONE_NO_ACK_REQ |
        OT_SYSTEM_EVENT_RADIO_TX_ERROR | OT_SYSTEM_EVENT_RADIO_TX_ACKED | OT_SYSTEM_EVENT_RADIO_TX_NO_ACK,

    OT_SYSTEM_EVENT_RADIO_RX_NO_BUFF                    = 0x00001000,
    OT_SYSTEM_EVENT_RADIO_RX_DONE                       = 0x00002000,
    OT_SYSTEM_EVENT_RADIO_RX_CRC_FIALED                 = 0x00004000,
    OT_SYSTEM_EVENT_RADIO_RX_ALL_MASK                   = OT_SYSTEM_EVENT_RADIO_RX_NO_BUFF |
        OT_SYSTEM_EVENT_RADIO_RX_DONE | OT_SYSTEM_EVENT_RADIO_RX_CRC_FIALED,
    OT_SYSTEM_EVENT_RADIO_ALL_MASK                      = OT_SYSTEM_EVENT_RADIO_TX_ALL_MASK | OT_SYSTEM_EVENT_RADIO_RX_ALL_MASK,

    OT_SYSTEM_EVENT_APP                                 = 0xff000000,

    OT_SYSTEM_EVENT_ALL                                 = 0xffffffff,
} ot_system_event_t;

extern ot_system_event_t ot_system_event_var;

/****************************************************************************//**
 * @brief  Init openthread tack.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otrStackInit(void);

/****************************************************************************//**
 * @brief  Request the single SDK-owned OpenThread task once.
 *         Repeated task-context calls do not recreate the mutex or worker.
 *         ISR calls are ignored. Return does not imply stack readiness;
 *         use otrGetInstance() after the scheduler starts. No stop/restart.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otrStart(void);

/****************************************************************************//**
 * @brief  Initializes user code with OpenThread related before OpenThread
 *          main event loop execution. This function is called after
 *          Openthread instance created and by OpenThread task.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otrInitUser(otInstance * instance);

/****************************************************************************//**
 * @brief  Wait for the initialized instance with a five-second polling budget.
 *         Scheduling delays can extend the elapsed wall time.
 *         Task context with scheduler running only; ISR calls return NULL.
 *
 * @param  None
 *
 * @return Initialized instance, or NULL on timeout/unavailable instance.
 *
*******************************************************************************/
otInstance *otrGetInstance(void);

/* Immediate readiness snapshot; never waits, delays or takes the OT mutex.
 * Returns NULL before full initialization, in ISR, or unless the scheduler is
 * running. Safe while holding the OT mutex. The SDK instance is never torn
 * down/replaced during normal operation; callers must still hold the OT mutex
 * before invoking OT APIs on the returned pointer. */
otInstance *otrGetInstanceIfReady(void);

/* OpenThread stopped for good (a fatal error in the stack or its radio, see
 * ot_rcp_health.h). Sticky until reboot; both getters above then return NULL
 * at once instead of waiting. Safe from any task or ISR. */
void otrMarkStopped(void);
bool otrIsStopped(void);

/****************************************************************************//**
 * @brief  Handle UART task.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ot_uartTask (ot_system_event_t sevent);

/****************************************************************************//**
 * @brief  A wrapper to call otPlatUartReceived.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ot_uartRecieved(uint8_t * rxbuf, uint32_t rxlen);
void ot_uartSetFd(int fd);
void ot_uartLog(const char *fmt, va_list argp);

void ot_alarmInit(void);

/****************************************************************************//**
 * @brief  Handle alarm timer task.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ot_alarmTask(ot_system_event_t sevent);

/****************************************************************************//**
 * @brief  Init radio.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ot_radioInit(otInstance *aInstance);

/****************************************************************************//**
 * @brief  Handle radio task.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ot_radioTask(ot_system_event_t trxEvent);

/****************************************************************************//**
 * @brief  init for openthread cli command.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
extern void otAppCliInit(otInstance *aInstance);

/****************************************************************************//**
 * @brief  init for openthread ncp command.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
extern void otAppNcpInit(otInstance *aInstance);

#define OT_CLI_UART_OUTPUT_LOCK()
#define OT_CLI_UART_OUTPUT_UNLOCK()

#if OT_FREERTOS_ENABLE

/****************************************************************************//**
 * @brief  Give semphore to OpenThread task
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otSysEventSignalPending(void);

/****************************************************************************//**
 * @brief  Lock OpenThread task to acquire ciritcal section access
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otrLock(void);
/* Nonblocking acquisition of the SDK worker's existing nonrecursive mutex.
 * Returns false before mutex creation, on contention, or in ISR context.
 * Success requires a matching otrUnlock(); it does not imply stack readiness.
 * Call from task context with the scheduler running. */
bool otrTryLock(void);
/* True only in the SDK OT worker, never before start or in interrupt context.
 * Use to reject blocking operations that require that worker to make progress. */
bool otrIsCurrentThread(void);
/* True when the calling task holds the OT mutex (task context only). */
bool otrIsLockOwner(void);

/****************************************************************************//**
 * @brief  Unlock OpenThread task to release ciritcal section access
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otrUnlock(void);

/**
 * @brief UART1 pads and baud rate of the host's Spinel link to its RCP.
 *
 * Called when the Spinel UART opens (OT start). The weak default returns
 * CONFIG_THREAD_RCP_UART_TX_PIN / _RX_PIN / _BAUD; an application with more
 * than one RCP wired to the board overrides it (the IPRO6S BR's rcp_select).
 * TX and RX must sit on different UART signal slots (pin % 8).
 */
void ot_spinel_uart_get_link(uint8_t *tx_pin, uint8_t *rx_pin, uint32_t *baud);

/**
 * @brief Get/set the OT task safety-net heartbeat (ms).
 *
 * Bounds the FreeRTOS ulTaskNotifyTake timeout in the OT task loop:
 *   timeout = min(sed_fastpoll_next_wake_ms(), heartbeat_ms).
 *
 * Default 1500 ms so the shell can recover from each PDS31 sleep
 * within ~1.5 s of a typed character (UART is power-gated during
 * PDS, chars between wakes are lost). PPK measurement runs that
 * want to see the engine's full idle savings should bump this to
 * a few times the SED poll period before capture.
 */
uint32_t ot_freertos_get_heartbeat_ms(void);
void ot_freertos_set_heartbeat_ms(uint32_t ms);

/****************************************************************************//**
 * @brief  Macro OT_THREAD_SAFE provides a method to access OpenThread with
 * thread-safe in other tasks running context.
 *
 * ot_initUser and OpenThread callback functions already are thread-safe protected.
 *
 * Note, do NOT call return in this macro block.
 *
 * @param ...  OpenThread api call statement
 *
*******************************************************************************/
#define OT_THREAD_SAFE(...)                 \
    otrLock();                              \
    do                                      \
    {                                       \
        __VA_ARGS__;                        \
    } while (0);                            \
    otrUnlock();

/****************************************************************************//**
 * @brief  Macro OT_THREAD_SAFE_RET provides a method to access OpenThread with
 * thread-safe in other tasks running context.
 *
 * ot_initUser and OpenThread callback functions already are thread-safe protected.
 *
 * Note, do NOT call return in this macro block.
 *
 * @param ret   return value
 * @param ...   OpenThread api call statement
 *
*******************************************************************************/
#define OT_THREAD_SAFE_RET(ret, ...)        \
    do                                      \
    {                                       \
        otrLock();                          \
        (ret) = __VA_ARGS__;                \
        otrUnlock();                        \
    } while (0)


#define OT_ENTER_CRITICAL()                 taskENTER_CRITICAL()
#define OT_ENTER_CRITICAL_ISR()             taskENTER_CRITICAL_FROM_ISR()
#define OT_EXIT_CRITICAL()                  taskEXIT_CRITICAL()
#define OT_EXIT_CRITICAL_ISR(x)             taskEXIT_CRITICAL_FROM_ISR(x)

/* The OR is a load/modify/store: an interrupt of a higher level (the RCP's
 * Spinel UART RX runs above the 802.15.4 MAC and timer interrupts) taken in
 * between would lose its own bit, e.g. UART_RXD - the host's frame then waits
 * in the ring until more bytes arrive. Hence the ISR-safe critical section. */
#define OT_NOTIFY_ISR(ebit)                 do { UBaseType_t ot_notify_mask = OT_ENTER_CRITICAL_ISR(); \
                                                 ot_system_event_var |= (ebit);                             \
                                                 OT_EXIT_CRITICAL_ISR(ot_notify_mask);                      \
                                                 otSysEventSignalPending(); } while (0)
#define OT_NOTIFY(ebit)                     OT_ENTER_CRITICAL(); ot_system_event_var |= ebit; OT_EXIT_CRITICAL(); otSysEventSignalPending()
#define OT_GET_NOTIFY(ebit)                 OT_ENTER_CRITICAL(); ebit = ot_system_event_var; ot_system_event_var = OT_SYSTEM_EVENT_NONE; OT_EXIT_CRITICAL()



/****************************************************************************//**
 * @brief  An weak function explore to applicaton layer to execute some application code.
 *          Note,
 *              1, this function is running in openthread main task, it is thread safed and
 *              it doesn't need OT_THREAD_SAFE/OT_THREAD_SAFE_RET protection.
 *              2, as it running with main task event let and radio/timer driver,
 *                  this function should be implemented very light without too
 *                  much execution time cost and delay or pending. And also do NOT
 *                  suspend and stop this task.
 *                  heavy execution or any delay or pending for some state transition should be
 *                  executed in another task.
 *
 * @param  sevent, event bit called by OT_APP_NOTIFY_ISR/OT_APP_NOTIFY from application code
 *
 * @return None
 *
*******************************************************************************/
void otrAppProcess(ot_system_event_t sevent);


/****************************************************************************//**
 * @brief  An interface to application to invoke openthread task to execute otrAppProcess,
 *          which is used to call in application interrupt context.
 *
 * @param  ebit, event bit for otrAppProcess.
 *              please reference to OT_SYSTEM_EVENT_APP for valid bits.
 *
 * @return None
 *
*******************************************************************************/
#define OT_APP_NOTIFY_ISR(ebit)             do { UBaseType_t ot_notify_mask = OT_ENTER_CRITICAL_ISR();                    \
                                                 ot_system_event_var |= ((ot_system_event_t)(ebit) & OT_SYSTEM_EVENT_APP); \
                                                 OT_EXIT_CRITICAL_ISR(ot_notify_mask);                                     \
                                                 otSysEventSignalPending(); } while (0)


/****************************************************************************//**
 * @brief  An interface to application to invoke openthread task to execute otrAppProcess,
 *          which is used to call in application task context.
 *
 * @param  ebit, event bit for otrAppProcess.
 *              please reference to OT_SYSTEM_EVENT_APP for valid bits.
 *
 * @return None
 *
*******************************************************************************/
#define OT_APP_NOTIFY(ebit)                 OT_ENTER_CRITICAL(); ot_system_event_var |=  ((ot_system_event_t)ebit & OT_SYSTEM_EVENT_APP); OT_EXIT_CRITICAL(); otSysEventSignalPending()

#else
/** openthread without rtos */

#define OT_ENTER_CRITICAL()                 __asm volatile( "csrc mstatus, 8" )
#define OT_EXIT_CRITICAL()                  __asm volatile( "csrs mstatus, 8" )
#define OT_ENTER_CRITICAL_ISR()             OT_ENTER_CRITICAL()
#define OT_EXIT_CRITICAL_ISR(x)             OT_EXIT_CRITICAL(x)

#define OT_NOTIFY_ISR(ebit)                 (ot_system_event_var |= ebit)
#define OT_NOTIFY(ebit)                     OT_HW_INT_DIS(); (ot_system_event_var |= ebit); OT_HW_INT_EN()
#define OT_GET_NOTIFY(ebit)                 OT_HW_INT_DIS(); (ebit = ot_system_event_var; ot_system_event_var = 0;); OT_HW_INT_EN()

#define OT_THREAD_SAFE(x)
#define OT_THREAD_SAFE_RET(x)

#endif

#ifdef __cplusplus
}
#endif

#endif // OPENTHREAD_PORT_H
