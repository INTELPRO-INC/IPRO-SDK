/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Shell FreeRTOS port for IPRO SDK
 */
#include <stdlib.h>
#include "shell.h"
#include "ring_buffer.h"
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "hal_uart.h"
#include "compiler/common.h"

#ifdef CONFIG_IPRO7
#include "hal_lp.h"
#endif

#if defined(CONFIG_SHELL_EXEC_THREAD) && CONFIG_SHELL_EXEC_THREAD
static int shell_exec_argc;
static char * ATTR_PSRAM_SECTION shell_exec_argv[SHELL_ARG_NUM + 1];
static char ATTR_PSRAM_SECTION shell_exec_line[SHELL_CMD_SIZE];
static ptrdiff_t shell_exec_line_diff;
TaskHandle_t shell_exec_handle;
static volatile bool shell_exec_end = true;

static void shell_exec_task(void *pvParameters)
{
    shell_exec_end = false;
    __asm volatile("fence");
    ((cmd_function_t)(pvParameters))(shell_exec_argc, shell_exec_argv);
    shell_exec_end = true;
    __asm volatile("fence");
    /* Delete SELF, not the global shell_exec_handle: a command dispatched
     * back-to-back with this one can reassign shell_exec_handle to a NEW exec
     * task before this line runs (shell_exec_end is set above, so the guard in
     * shell_start_exec/shell_abort_exec no longer protects it) — deleting the
     * global handle then frees the wrong (live) task's TCB -> heap-free assert
     * / prvTaskExitError. vTaskDelete(NULL) always targets the running task. */
    vTaskDelete(NULL);
}

void shell_dup_line(char *cmd, uint32_t length)
{
    /* shell_exec_line + shell_exec_argv are a SINGLE shared buffer the running
     * exec task reads for its whole lifetime. Serialize: do not overwrite them
     * while a previous command's exec task is still running, or that task would
     * suddenly parse THIS command's args (observed: a command dispatched
     * back-to-back with a slower one made two exec tasks both run `lp sleep`,
     * clobbering the shared retention stack -> heap-free assert / crash).
     * shell_exec_end flips true only after the command function returns; a fast
     * command clears it in ~µs, so a normal back-to-back pair (the trigger)
     * waits only microseconds. Bounded so a genuinely stuck command cannot hang
     * the shell forever — after the cap we proceed regardless (the new command's
     * args are for a fresh exec task; only another concurrent PDS sleep could
     * still conflict, which is not a realistic follow-on to a stuck command). */
    const uint32_t cap = pdMS_TO_TICKS(5000);
    for (uint32_t i = 0; i < cap && shell_exec_end == false; i++) {
        vTaskDelay(1);
    }
    memcpy(shell_exec_line, cmd, length + 1);
    shell_exec_line_diff = shell_exec_line - cmd;
}

void shell_abort_exec(int sig)
{
    (void)sig;
    if (shell_exec_end == false) {
        shell_exec_end = true;
        __asm volatile("fence");
        vTaskDelete(shell_exec_handle);
        shell_exec_handle = NULL;
    }
}

int shell_start_exec(cmd_function_t func, int argc, char *argv[])
{
    BaseType_t xReturned;
    shell_abort_exec(SHELL_SIGINT);
    shell_exec_argc = argc;

    for (uint8_t i = 0; i < argc; i++) {
        shell_exec_argv[i] = argv[i] + shell_exec_line_diff;
    }
    shell_exec_argv[argc] = NULL;

    /* Claim the buffer HERE, not in shell_exec_task. The exec task runs at a
     * lower priority than the shell task that creates it, so it does not get
     * scheduled before xTaskCreate returns -- leaving shell_exec_end stale-true
     * for the whole gap. A third command arriving in that gap (a pasted or
     * scripted burst does exactly this) sails through shell_dup_line's guard
     * and overwrites shell_exec_line under the task just created, and the next
     * shell_abort_exec() sees "idle" too, so it neither cancels that task nor
     * stops xTaskCreate from overwriting its handle. Setting the flag before
     * the task exists closes both. */
    shell_exec_end = false;

    __asm volatile("fence");
    xReturned = xTaskCreate(shell_exec_task, (char *)"shell_exec", SHELL_EXEC_THREAD_STACK_SIZE, func, SHELL_EXEC_THREAD_PRIO, &shell_exec_handle);

    if (xReturned == pdPASS) {
        return 0;
    } else {
        shell_exec_end = true;
        return -1;
    }
}

#endif

static TaskHandle_t shell_handle;
static StaticSemaphore_t sem_shell_buf;
static SemaphoreHandle_t sem_shell = NULL;

static StaticTask_t shell_task_tcb;
static StackType_t shell_task_stack[SHELL_THREAD_STACK_SIZE];

static Ring_Buffer_Type shell_rb;
static uint8_t ATTR_PSRAM_SECTION shell_buffer[512];

static int shell_uart_index = UART0_INDEX;

void shell_release_sem(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (xPortIsInsideInterrupt()) {
        int ret = xSemaphoreGiveFromISR(sem_shell, &xHigherPriorityTaskWoken);
        if (ret == pdPASS) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    } else {
        xSemaphoreGive(sem_shell);
    }
}

static void shell_uart_callback(void *dev, void *args, uint32_t size, uint32_t event)
{
    (void)dev;
    (void)args;
    (void)size;

    if (event == UART_EVENT_RX_FIFO) {
        uint8_t temp_buf[32];
        int len;

        /* Read available data from UART FIFO */
        len = uart_read_available(shell_uart_index, temp_buf, sizeof(temp_buf));
        if (len > 0) {
            Ring_Buffer_Write(&shell_rb, temp_buf, len);
            shell_release_sem();
        }
    }
}

static void shell_task(void *pvParameters)
{
    uint8_t data;
    uint32_t len;

    /* Enable UART RX FIFO and RTO interrupts via HAL */
    uart_ioctl(shell_uart_index, UART_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT | UART_RTO_IT));

    while (1) {
        if (xSemaphoreTake(sem_shell, portMAX_DELAY) == pdTRUE) {
            len = Ring_Buffer_Get_Length(&shell_rb);
            for (uint32_t i = 0; i < len; i++) {
                Ring_Buffer_Read_Byte(&shell_rb, &data);
                shell_handler(data);
            }
        }
    }
}

/**
 * @brief Reinitialize shell UART after low power wakeup
 *
 * After PDS wakeup, UART is reinitialized by LP firmware but the callback
 * and interrupt settings are lost. This function restores them.
 */
void shell_uart_reinit(void)
{
    uart_init(shell_uart_index, 0);

    /* Re-register UART RX callback */
    uart_set_callback(shell_uart_index, (void *)shell_uart_callback);

    /* Re-enable UART RX FIFO and RTO interrupts */
    uart_ioctl(shell_uart_index, UART_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT | UART_RTO_IT));
}

#if defined(CONFIG_DEBUG_CONSOLE_USB)
/* shell_rb lock/unlock: a short interrupt-masking critical section, safe from
 * both task and ISR context. Needed only when the USB CDC console mirrors the
 * shell -- it adds a SECOND ISR producer (cdc_out_cb) to shell_rb alongside the
 * UART RX ISR, breaking the single-producer lock-free SPSC assumption. The saved
 * mask lives in one static: the section is non-preemptible (interrupts masked)
 * so lock()/unlock() pairs cannot nest. */
static UBaseType_t s_shell_rb_key;
static void shell_rb_lock(void)   { s_shell_rb_key = portSET_INTERRUPT_MASK_FROM_ISR(); }
static void shell_rb_unlock(void) { portCLEAR_INTERRUPT_MASK_FROM_ISR(s_shell_rb_key); }
#endif

void shell_init_with_task(int uart_index)
{
    sem_shell = xSemaphoreCreateBinaryStatic(&sem_shell_buf);
    xSemaphoreGive(sem_shell);
    shell_uart_index = uart_index;

#if defined(CONFIG_DEBUG_CONSOLE_USB)
    /* USB console adds a 2nd ISR producer to shell_rb -- serialize all access. */
    Ring_Buffer_Init(&shell_rb, shell_buffer, sizeof(shell_buffer),
                     shell_rb_lock, shell_rb_unlock);
#else
    Ring_Buffer_Init(&shell_rb, shell_buffer, sizeof(shell_buffer), NULL, NULL);
#endif

    /* Register UART RX callback */
    uart_set_callback(shell_uart_index, (void *)shell_uart_callback);

    shell_init();
    shell_handle = xTaskCreateStatic(shell_task, "shell",
                                     SHELL_THREAD_STACK_SIZE, NULL,
                                     SHELL_THREAD_PRIO,
                                     shell_task_stack, &shell_task_tcb);
}

void shell_exe_cmd(uint8_t *cmd, uint16_t len)
{
    Ring_Buffer_Write(&shell_rb, cmd, len);
    shell_release_sem();
}

/* Sized against the real task list, not a round number: this build runs ~16
 * tasks, and every entry costs configMAX_TASK_NAME_LEN + 8 bytes of .bss in a
 * config whose ram_tcm heap floor leaves a few hundred bytes of headroom. If it
 * is ever exceeded the snapshot truncates, so ps_cmd says so out loud rather
 * than quietly reporting the dropped tasks as brand new on the next call. */
#define PS_MAX_TASKS 20

/* Snapshot for delta CPU% calculation between ps invocations.
 * Match by task name (not xTaskNumber) so short-lived tasks like
 * shell_exec (recreated per command) aggregate correctly. xTaskNumber is kept
 * alongside anyway, because it is the only thing that tells a recreated task
 * apart from a wrapped counter - see ps_cmd(). */
struct ps_snap {
    char name[configMAX_TASK_NAME_LEN];
    uint32_t ulRunTimeCounter;
    UBaseType_t xTaskNumber;
};
static struct ps_snap ps_prev[PS_MAX_TASKS];
static uint32_t ps_prev_total;
static UBaseType_t ps_prev_count;
static TickType_t ps_prev_ticks;

static const char *ps_state_char(eTaskState s)
{
    switch (s) {
    case eRunning:   return "R";
    case eReady:     return "r";
    case eBlocked:   return "B";
    case eSuspended: return "S";
    case eDeleted:   return "D";
    default:         return "?";
    }
}

/* Returns the previous snapshot row for `name`, or NULL if this task was not
 * present at the last ps. */
static const struct ps_snap *ps_get_prev(const char *name)
{
    for (UBaseType_t i = 0; i < ps_prev_count; i++) {
        if (strncmp(ps_prev[i].name, name, configMAX_TASK_NAME_LEN) == 0)
            return &ps_prev[i];
    }
    return NULL;
}

static int ps_cmd(int argc, char **argv)
{
    TaskStatus_t *pxTaskStatusArray;
    UBaseType_t uxArraySize, i;
    uint32_t ulTotalRunTime;

    uxArraySize = uxTaskGetNumberOfTasks();
    pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));
    if (!pxTaskStatusArray) {
        printf("ps: alloc failed\r\n");
        return -1;
    }

    uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize,
                                       &ulTotalRunTime);

    uint32_t delta_total = ulTotalRunTime - ps_prev_total;
    TickType_t now_ticks = xTaskGetTickCount();
    uint32_t delta_ms = (uint32_t)(now_ticks - ps_prev_ticks) * portTICK_PERIOD_MS;
    int have_prev = (ps_prev_count > 0 && delta_total > 0);

    printf("\r\n%-*s State  Prio  Stack   #   CPU%%\r\n",
           configMAX_TASK_NAME_LEN, "Task");
    printf("---------------------------------------------------\r\n");

    /* 64-bit: if the run-time counter ever does wrap, the per-task modular
     * deltas below can each approach 2^32 and a 32-bit sum would itself
     * overflow, destroying the very evidence the footer checks for. */
    uint64_t accounted = 0;
    uint32_t idle_delta = 0;
    unsigned suspect = 0;

    for (i = 0; i < uxArraySize; i++) {
        TaskStatus_t *t = &pxTaskStatusArray[i];

        if (have_prev) {
            const struct ps_snap *prev = ps_get_prev(t->pcTaskName);
            uint32_t delta_task;
            char flag = ' ';

            if (prev == NULL || prev->xTaskNumber != t->xTaskNumber) {
                /* Either brand new since the last ps, or destroyed and
                 * recreated under the same name - shell_exec does exactly that,
                 * once per command. Its counter restarted at zero inside this
                 * window, so the absolute value IS the delta.
                 *
                 * This used to be decided by `counter < previous`, which is
                 * also what a wrapped counter looks like, so a wrap was
                 * silently rewritten into a plausible-looking number instead of
                 * being reported. xTaskNumber separates the two cases: it
                 * changes on recreation and does not change on a wrap. */
                delta_task = t->ulRunTimeCounter;
            } else {
                /* Unsigned subtraction is already correct across one wrap of
                 * the counter; do not "fix" it. What is NOT recoverable is a
                 * window longer than the counter's whole period, and that shows
                 * up as a task appearing to run longer than the window. */
                delta_task = t->ulRunTimeCounter - prev->ulRunTimeCounter;
                if (delta_task > delta_total) {
                    flag = '!';
                    suspect++;
                }
            }

            accounted += delta_task;
            if (strncmp(t->pcTaskName, "IDLE", configMAX_TASK_NAME_LEN) == 0)
                idle_delta = delta_task;

            uint32_t pct_x10 = (uint32_t)((uint64_t)delta_task * 1000
                                           / delta_total);
            printf("%-*s  %s    %2lu   %5lu  %2lu  %2lu.%lu%%%c\r\n",
                   configMAX_TASK_NAME_LEN, t->pcTaskName,
                   ps_state_char(t->eCurrentState),
                   (unsigned long)t->uxCurrentPriority,
                   (unsigned long)t->usStackHighWaterMark,
                   (unsigned long)t->xTaskNumber,
                   (unsigned long)(pct_x10 / 10),
                   (unsigned long)(pct_x10 % 10),
                   flag);
        } else {
            printf("%-*s  %s    %2lu   %5lu  %2lu    --\r\n",
                   configMAX_TASK_NAME_LEN, t->pcTaskName,
                   ps_state_char(t->eCurrentState),
                   (unsigned long)t->uxCurrentPriority,
                   (unsigned long)t->usStackHighWaterMark,
                   (unsigned long)t->xTaskNumber);
        }
    }

    /* Show unaccounted CPU time (ephemeral tasks created/destroyed
     * between two ps invocations, e.g. shell_exec per command) */
    if (have_prev && accounted < delta_total) {
        uint32_t other = (uint32_t)(delta_total - accounted);
        uint32_t pct_x10 = (uint32_t)((uint64_t)other * 1000
                                       / delta_total);
        if (pct_x10 > 0) {
            printf("%-*s  -    --   -----  --  %2lu.%lu%%\r\n",
                   configMAX_TASK_NAME_LEN, "(other)",
                   (unsigned long)(pct_x10 / 10),
                   (unsigned long)(pct_x10 % 10));
        }
    }

    /* Self-check. Every percentage above is a fraction of delta_total, so the
     * one thing that can silently destroy all of them at once is delta_total
     * itself being wrong - which is what happens when the run-time counter
     * wraps inside the window. It is not hypothetical: with the counter reading
     * only the low 32 bits of mcycle it wrapped in seconds, and a session's
     * worth of "CPU%" summed to 809% before anyone noticed.
     *
     * So say it out loud. The rows cannot legitimately sum past 100% of the
     * window, and no single task can run longer than the window; either
     * violation means the numbers are not measurements. This needs no clock
     * constant and no calibration, which is the point - it stays true if the
     * core clock, the prescale or the tick rate ever change. Trust nothing here
     * unless the line ends in OK.
     *
     * cnt/ms is printed for a second reason. The run-time counter is derived
     * from mcycle, which STOPS when the core halts, so if an app's idle hook
     * executes WFI the idle time disappears from both the IDLE row and the
     * window, and idle% reads far too low while still summing to 100% - the
     * gate above cannot see it. A depressed cnt/ms is the tell: it sits at the
     * core clock over 256 (937/ms at 240 MHz) when the core never halts and
     * drops in proportion to the time it does. ipro6_if_wifi's idle hook is
     * empty today, so this build does not halt, but that rests on which of two
     * weak vApplicationIdleHook definitions the linker picked. */
    if (have_prev) {
        uint32_t sum_x10 = (uint32_t)(accounted * 1000 / delta_total);
        uint32_t idle_x10 = (uint32_t)((uint64_t)idle_delta * 1000
                                        / delta_total);
        int bad = (suspect > 0 || sum_x10 < 900 || sum_x10 > 1100);

        printf("---------------------------------------------------\r\n");
        printf("span %lu ms / %lu cnt (%lu cnt/ms)   sum %lu.%lu%%   "
               "idle %lu.%lu%%   %s\r\n",
               (unsigned long)delta_ms,
               (unsigned long)delta_total,
               (unsigned long)(delta_ms ? delta_total / delta_ms : 0),
               (unsigned long)(sum_x10 / 10), (unsigned long)(sum_x10 % 10),
               (unsigned long)(idle_x10 / 10), (unsigned long)(idle_x10 % 10),
               bad ? "INVALID" : "OK");
        if (bad)
            printf("ps: INVALID - %u row(s) exceed the window, sum %lu.%lu%%. "
                   "The run-time counter wrapped; shorten the interval. "
                   "Do not quote these numbers.\r\n",
                   suspect,
                   (unsigned long)(sum_x10 / 10), (unsigned long)(sum_x10 % 10));
    }

    /* Save snapshot for next invocation */
    if (uxArraySize > PS_MAX_TASKS)
        printf("ps: %lu tasks but the snapshot holds %u - the dropped ones will "
               "read as new next time and their CPU%% will be wrong\r\n",
               (unsigned long)uxArraySize, PS_MAX_TASKS);
    ps_prev_count = (uxArraySize < PS_MAX_TASKS) ? uxArraySize : PS_MAX_TASKS;
    for (i = 0; i < ps_prev_count; i++) {
        strncpy(ps_prev[i].name, pxTaskStatusArray[i].pcTaskName,
                configMAX_TASK_NAME_LEN);
        ps_prev[i].ulRunTimeCounter = pxTaskStatusArray[i].ulRunTimeCounter;
        ps_prev[i].xTaskNumber      = pxTaskStatusArray[i].xTaskNumber;
    }
    ps_prev_total = ulTotalRunTime;
    ps_prev_ticks = now_ticks;

    vPortFree(pxTaskStatusArray);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(ps_cmd, ps, show task list);

static int free_cmd(int argc, char **argv)
{
    printf("Heap: total %lu, free %lu, min free %lu\r\n",
           (unsigned long)configTOTAL_HEAP_SIZE,
           (unsigned long)xPortGetFreeHeapSize(),
           (unsigned long)xPortGetMinimumEverFreeHeapSize());
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(free_cmd, free, show memory info);

static int reboot_cmd(int argc, char **argv)
{
    extern void hal_reboot(void);
    hal_reboot();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(reboot_cmd, reboot, reboot system);

static int reboot_dl_cmd(int argc, char **argv)
{
    extern void hal_reboot_download(void);
    printf("reboot to download mode\r\n");
    hal_reboot_download();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(reboot_dl_cmd, reboot_dl, reboot to download mode);

static int reboot_usb_cmd(int argc, char **argv)
{
    /* Write USB download magic + timeout code to HBN_RSV3 (survives reset).
     * Boot2 checks HBN_RSV3 on startup and enters USB download mode if magic
     * matches. Timeout code in [7:0]: 0x01=2s, 0x02=5s, 0xFF=infinite wait.
     * Usage: reboot_usb [code]  (default 0xFF) */
    uint32_t code = 0xFF;
    if (argc >= 2) {
        code = (uint32_t)strtoul(argv[1], NULL, 0);
        if (code > 0xFF) {
            printf("timeout code must be 0..0xFF\r\n");
            return -1;
        }
    }
    uint32_t val = 0x55534200U | (code & 0xFF);
    printf("reboot to boot2 USB download (HBN_RSV3=0x%08lX)\r\n",
           (unsigned long)val);
    *(volatile uint32_t *)0x3008F10C = val;
    extern void hal_reboot(void);
    hal_reboot();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(reboot_usb_cmd, reboot_usb,
                       reboot to boot2 USB download [code=0xFF]);
