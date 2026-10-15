/**
 ****************************************************************************************
 *
 * @file fhost_connect.h
 *
 * @brief Definitions of the fully hosted Connect task.
 *
 * Copyright (C) RivieraWaves 2019-2021
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup FHOST_CONNECT FHOST_CONNECT
 * @ingroup FHOST_IPC
 * @brief Task started by @p connect command. This task is active until the connection is
 * completed or canceled.
 * @{
 ****************************************************************************************
 */

#include "fhost_api.h"
#include "rtos_def.h"
#include "fhost_ipc.h"
#include "fhost_connect_result.h"

/// Connect Task parameter
struct fhost_ipc_connect_cfg
{
    /// Index of the FHOST interface
    int fhost_vif_idx;
    /// Connection parameters
    struct fhost_vif_sta_cfg cfg;
    /// Link to CNTRL task
    struct fhost_cntrl_link *link;
    /// Optional completion observer. Zero-initialize the entire request.
    fhost_connect_completion_fn completion;
    void *completion_context;
    /// Enable retries only after old VIF preparation; false preserves policy.
    bool enable_autoreconnect;
};


/**
 ****************************************************************************************
 * @brief Start a task to complete the connection
 *
 * @param[in] arg  Task parameter (@ref fhost_ipc_connect_cfg)
 *
 * Task-context submission only. Copies arg; does not wait for another entry
 * task or CONNECT worker. A busy entry, active CONNECT on the requested VIF,
 * invalid argument, or allocation/task-creation failure
 * returns RTOS_TASK_NULL immediately without queuing another request.
 * A returned entry-task handle means admission only, not association or a
 * stable cancellation handle. Preparation polls (scan/old supplicant) share
 * a timeout_ms budget, or 200 seconds when timeout_ms is nonpositive. Polling
 * may overshoot by one 100-ms interval plus scheduler delay. Association keeps
 * its separate timeout; this is not a whole-operation deadline.
 * If admitted and completion is non-NULL, it is called once when the entry
 * fails to create CONNECT or CONNECT exits normally. Rejected submissions
 * have no callback. External task deletion/reset cannot promise a callback.
 * Callback runs in CONNECT/CONNECT_ENTRY (possibly before start returns),
 * outside critical sections but while admission remains busy. It must only
 * post a bounded notification, must return, and must not block, delete the
 * worker, or synchronously wait for another connection. Context must remain
 * valid through callback return. No CHIP APIs may be called directly here.
 * CONNECTED is a WPA-state snapshot, not DHCP/IPv6 readiness. CONFIGURED means
 * timeout_ms == 0 and no association wait. Cancellation/draining is separate.
 ****************************************************************************************
 **/
rtos_task_handle fhost_ipc_connect_task_start(void *arg);

/* Task-only, terminal until reboot. Close admission and poll for accepted
 * entry/CONNECT work, callbacks, request frees and pause leases: 1=busy,
 * 0=drained. Already
 * admitted work runs normally; no forced cancellation. After drained only
 * task self-deletion may remain. This does not fence manager-side mutations,
 * direct WPA calls, scan/DHCP/autoreconnect or stop any network stack. */
int fhost_ipc_connect_close(void);

/* Task-only temporary admission pause, for one externally serialized owner.
 * Start with *lease=0. A successful admission writes a nonzero lease even if
 * workers are still busy. Poll with that same lease: 1=draining, 0=drained,
 * -1=invalid/stale/another owner/terminal close/ID exhaustion (no new lease).
 * All VIFs are fenced; already accepted entry/CONNECT work runs normally.
 * Callback completion is not drainage: request cleanup must also finish.
 * Keep the lease until the exclusive operation ends; then resume exactly once.
 * After terminal close, polling returns -1 but the owner must still release
 * its lease; close waits for that release. Tokens are never reused this boot.
 * This is only the native CONNECT gate, NOT a manager/scan/WPA/DHCP fence.
 * The caller must separately exclude manager mutations and other Wi-Fi writers
 * before treating this as exclusive access to the network configuration. */
int fhost_ipc_connect_pause(uint32_t *lease);

/* Release a valid lease, including to abandon a pending pause. Does not cancel
 * existing workers or reopen terminal close. 0=released, -1=invalid/stale.
 * Stop exclusive work before release; clear the caller's token after success. */
int fhost_ipc_connect_resume(uint32_t lease);

/**
 ****************************************************************************************
 * @brief Kill pending connection
 *
 * @param[in] task  Task handle
 * @return always FHOST_IPC_SUCCESS
 ****************************************************************************************
 **/
int fhost_ipc_connect_task_kill(rtos_task_handle task);

/**
 ****************************************************************************************
 * @brief End the connection task of a VIF and wait until it has exited
 *
 * @param[in] vif         FHOST VIF index
 * @param[in] timeout_ms  Maximum wait for the task to exit
 * @return 0 if no task is left, -1 otherwise
 ****************************************************************************************
 **/
int fhost_ipc_connect_task_end(int vif, uint32_t timeout_ms);

/**
 * @}
 */
