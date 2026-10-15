/**
 ****************************************************************************************
 *
 * @file fhost_connect.c
 *
 * @brief Implementation of the fully hosted Connect task.
 *
 * Copyright (C) RivieraWaves 2019-2021
 *
 ****************************************************************************************
 */

#include "fhost.h"
#include "fhost_connect.h"
#include "fhost_wpa.h"
#include "net_al_ext.h"
#include "wifi_mgmr.h"


/**
 ****************************************************************************************
 * @addtogroup FHOST_CONNECT
 * @{
 ****************************************************************************************
 */

/// Table to keep correspondance between fhost_vif_idx and task handle;
rtos_task_handle fhost_ipc_connect_task[NX_VIRT_DEV_MAX];

/* Covers allocation, task creation and the entry task's handoff. A task
 * name lookup cannot reserve this interval against another submitting task. */
static bool fhost_connect_entry_reserved;
static bool fhost_connect_closed;
static uint32_t fhost_connect_pause_sequence;
static uint32_t fhost_connect_pause_owner;

int fhost_ipc_connect_pause(uint32_t *lease)
{
    if (!lease)
        return -1;

    uint32_t protection = rtos_protect();
    if (fhost_connect_closed ||
        (*lease != 0 && *lease != fhost_connect_pause_owner) ||
        (*lease == 0 && (fhost_connect_pause_owner != 0 ||
                        fhost_connect_pause_sequence == UINT32_MAX)))
    {
        rtos_unprotect(protection);
        return -1;
    }
    if (*lease == 0)
    {
        fhost_connect_pause_owner = ++fhost_connect_pause_sequence;
        *lease = fhost_connect_pause_owner;
    }
    bool busy = fhost_connect_entry_reserved;
    for (unsigned i = 0; i < NX_VIRT_DEV_MAX; ++i)
        busy |= fhost_ipc_connect_task[i] != RTOS_TASK_NULL;
    rtos_unprotect(protection);
    return busy ? 1 : 0;
}

int fhost_ipc_connect_resume(uint32_t lease)
{
    uint32_t protection = rtos_protect();
    if (lease == 0 || lease != fhost_connect_pause_owner)
    {
        rtos_unprotect(protection);
        return -1;
    }
    fhost_connect_pause_owner = 0;
    /* Never clear fhost_connect_closed: a racing reset remains terminal. */
    rtos_unprotect(protection);
    return 0;
}

int fhost_ipc_connect_close(void)
{
    uint32_t protection = rtos_protect();
    fhost_connect_closed = true;
    bool busy = fhost_connect_entry_reserved || fhost_connect_pause_owner != 0;
    for (unsigned i = 0; i < NX_VIRT_DEV_MAX; ++i)
        busy |= fhost_ipc_connect_task[i] != RTOS_TASK_NULL;
    rtos_unprotect(protection);
    return busy ? 1 : 0;
}

static void fhost_connect_entry_release(void)
{
    uint32_t protection = rtos_protect();
    fhost_connect_entry_reserved = false;
    rtos_unprotect(protection);
}

/* rtos_now() returns ticks on this port. Both preparation phases share this
 * budget; unsigned subtraction also handles the tick counter wrapping. */
static bool fhost_connect_wait_ready(int vif, bool wait_supplicant,
                                     uint32_t started, uint32_t budget)
{
    for (;;)
    {
        if ((uint32_t)(rtos_now(false) - started) >= budget)
            return false;
        if ((!wait_supplicant || fhost_wpa_get_supplicant_state(vif) == 0) &&
            !fhost_env.vif[vif].scanning)
            return true;
        rtos_task_suspend(100);
    }
}

/**
 ****************************************************************************************
 * @brief Connect task main loop
 *
 * This task is only used to run fhost_sta_cfg outside of IPC task, so that it can be
 * killed if needed
 ****************************************************************************************
 */
#ifdef BL_RA_EN
uint8_t conn_flag = 0;
#endif
static RTOS_TASK_FCT(fhost_ipc_connect_main)
{
    struct fhost_ipc_connect_cfg *cfg = env;
    struct fhost_vif_status status;
    const int vif = cfg->fhost_vif_idx;
    struct net_al_ext_ip_addr_cfg ip_cfg;
    uint32_t protection;
    enum fhost_connect_result result = FHOST_CONNECT_PREPARATION_FAILED;
    const uint32_t started = rtos_now(false);
    /* Keep async/infinite association modes, but never wait indefinitely for
     * scan/old supplicant teardown. Match mgmr's 200-second default budget. */
    const uint32_t budget = rtos_al_ms2tick(cfg->cfg.timeout_ms > 0 ?
                                          cfg->cfg.timeout_ms : 200000);

    ip_cfg.mode = IP_ADDR_NONE;

    if (fhost_get_vif_status(cfg->fhost_vif_idx, &status))
        goto end;

    if (status.type != VIF_STA)
    {
        if (fhost_set_vif_type(cfg->link, cfg->fhost_vif_idx, VIF_STA, false))
            goto end;
    }
    else if (fhost_wpa_get_state(cfg->fhost_vif_idx) != FHOST_WPA_STATE_STOPPED)
    {
        if (net_al_ext_get_vif_ip(cfg->fhost_vif_idx, &ip_cfg))
            goto end;

#ifdef BL_RA_EN
        if (fhost_wpa_get_state(cfg->fhost_vif_idx) == FHOST_WPA_STATE_CONNECTED) {
            conn_flag = 1;
        }
#endif

        if (fhost_wpa_disable_network(cfg->fhost_vif_idx)) {
            goto end;
        }

        if (!fhost_connect_wait_ready(cfg->fhost_vif_idx, true, started, budget))
        {
            result = FHOST_CONNECT_PREPARATION_TIMEOUT;
            goto end;
        }

        // Reset STA interface (this will end previous wpa_supplicant task)
        if (fhost_set_vif_type(cfg->link, cfg->fhost_vif_idx, VIF_UNKNOWN, false) ||
            fhost_set_vif_type(cfg->link, cfg->fhost_vif_idx, VIF_STA, false))
            goto end;
    }

    if (!fhost_connect_wait_ready(cfg->fhost_vif_idx, false, started, budget))
    {
        result = FHOST_CONNECT_PREPARATION_TIMEOUT;
        goto end;
    }

    result = FHOST_CONNECT_FAILED;
    /* Managed provisioning resumes retries for the NEW network only. Do not
     * enable the old supplicant while preparation/teardown is still pending.
     * add_vif applies this policy to WPA and propagates command failure. */
    if (cfg->enable_autoreconnect)
    {
        protection = rtos_protect();
        wifiMgmr.disable_autoreconnect = 0;
        rtos_unprotect(protection);
    }
    if (fhost_sta_cfg(cfg->fhost_vif_idx, &(cfg->cfg)))
        goto end;

    if (cfg->cfg.timeout_ms == 0)
        result = FHOST_CONNECT_CONFIGURED;
    else if (fhost_wpa_get_state(cfg->fhost_vif_idx) == FHOST_WPA_STATE_CONNECTED)
        result = FHOST_CONNECT_CONNECTED;
    else
        result = FHOST_CONNECT_NOT_CONNECTED;

#if 0
    if (ip_cfg.mode != IP_ADDR_NONE)
    {
        if (ip_cfg.mode == IP_ADDR_DHCP_CLIENT)
            ip_cfg.dhcp.to_ms = 5000;
        ip_cfg.default_output = true;

        if (fhost_set_vif_ip(cfg->fhost_vif_idx, &ip_cfg))
            goto end;
    }
#endif

  end:
#ifdef BL_RA_EN
    conn_flag = 0;
#endif
    if (cfg->completion)
        cfg->completion(cfg->completion_context, cfg->fhost_vif_idx, result);
    rtos_free(cfg);
    protection = rtos_protect();
    fhost_ipc_connect_task[vif] = RTOS_TASK_NULL;
    rtos_unprotect(protection);
    rtos_task_delete(NULL);
}

static RTOS_TASK_FCT(fhost_ipc_connect_entry)
{
    rtos_task_handle task;
    struct fhost_ipc_connect_cfg *cfg = env;

    if (rtos_task_create(fhost_ipc_connect_main, "CONNECT", APPLICATION_TASK, 512,
                         env,  fhost_connect_priority, &task))
    {
        if (cfg->completion)
            cfg->completion(cfg->completion_context, cfg->fhost_vif_idx,
                            FHOST_CONNECT_TASK_FAILED);
        rtos_free(env);
        fhost_printf("Create CONNECT Task error\r\n");
        fhost_connect_entry_release();
        /* FreeRTOS task functions must not return to their startup frame. */
        rtos_task_delete(NULL);
        return;
    }

    /* CONNECT_ENTRY priority 10 must remain above CONNECT priority so this
     * handle is published before the worker can finish and clear it. */
    uint32_t protection = rtos_protect();
    fhost_ipc_connect_task[cfg->fhost_vif_idx] = task;
    rtos_unprotect(protection);
    fhost_connect_entry_release();
    rtos_task_delete(NULL);
}

rtos_task_handle fhost_ipc_connect_task_start(void *arg)
{
    struct fhost_ipc_connect_cfg *cfg;
    rtos_task_handle task;
    const struct fhost_ipc_connect_cfg *input = arg;

    if (!input || input->fhost_vif_idx < 0 || input->fhost_vif_idx >= NX_VIRT_DEV_MAX)
        return RTOS_TASK_NULL;

    uint32_t protection = rtos_protect();
    if (fhost_connect_closed || fhost_connect_pause_owner != 0 || fhost_connect_entry_reserved ||
        fhost_ipc_connect_task[input->fhost_vif_idx] != RTOS_TASK_NULL)
    {
        rtos_unprotect(protection);
        return RTOS_TASK_NULL;
    }
    fhost_connect_entry_reserved = true;
    rtos_unprotect(protection);

    cfg = rtos_malloc(sizeof(*cfg));
    if (!cfg)
    {
        fhost_connect_entry_release();
        return RTOS_TASK_NULL;
    }

    memcpy(cfg, arg, sizeof(*cfg));

    if (rtos_task_create(fhost_ipc_connect_entry, "CONNECT_ENTRY", APPLICATION_TASK, 512,
                        cfg, 10, &task))
    {
        rtos_free(cfg);
        fhost_connect_entry_release();
        fhost_printf("Create CONNECT_ENTRY Task error\r\n");
        return RTOS_TASK_NULL;
    }

    return task;
}

int fhost_ipc_connect_task_end(int vif, uint32_t timeout_ms)
{
    if (vif < 0 || vif >= NX_VIRT_DEV_MAX)
        return -1;

    uint32_t protection = rtos_protect();
    rtos_task_handle task = fhost_ipc_connect_task[vif];
    rtos_unprotect(protection);
    if (task == RTOS_TASK_NULL)
        return 0;

    fhost_ipc_connect_task_kill(task);
    for (uint32_t waited = 0; waited < timeout_ms; waited += 50)
    {
        if (fhost_ipc_connect_task[vif] == RTOS_TASK_NULL)
            return 0;
        rtos_task_suspend(50);
    }
    return -1;
}

int fhost_ipc_connect_task_kill(rtos_task_handle task)
{
    int i;

    for (i = 0; i < NX_VIRT_DEV_MAX ; i ++)
    {
        if (fhost_ipc_connect_task[i] == task)
        {
            fhost_wpa_remove_vif(i);
            break;
        }
    }

    return FHOST_IPC_SUCCESS;
}

/**
 * @}
 */
