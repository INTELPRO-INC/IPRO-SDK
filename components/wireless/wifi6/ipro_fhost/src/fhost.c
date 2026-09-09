/**
 ****************************************************************************************
 *
 * @file fhost.c
 *
 * @brief Implementation of the fully hosted entry point.
 *
 * Copyright (C) RivieraWaves 2017-2021
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup FHOST
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "fhost.h"
#include "fhost_rx.h"
#include "fhost_tx.h"
#include "fhost_cntrl.h"
#include "fhost_config.h"
#include "fhost_wpa.h"
#include "fhost_ipc.h"
#include "net_al_ext.h"
#include "platform_al.h"

/*
 * DEFINITIONS
 ****************************************************************************************
 */

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
struct fhost_env_tag fhost_env;

/// Semaphore signaled when all fhost tasks are ready
static rtos_semaphore fhost_ready_sem;

/// Bitfield of task that are not yet fully initialized
static uint32_t fhost_pending_task;
static rtos_mutex fhost_pending_task_lock;

#ifndef offsetof
#define offsetof(st, m) \
    ((size_t)&(((st *)0)->m))
#endif

static const char *version_fhost __attribute__((used, section(".version.fhost"))) = "component_version_fhost_" VERSION_FHOST_NUMBER;

/*
 * FUNCTIONS
 ****************************************************************************************
 */
int fhost_init()
{
    // no need now for Initialize MAC

    // Initialize FHOST environment
    memset(&fhost_env, 0, sizeof(fhost_env));

    // Initialize the different modules
    if(rtos_semaphore_create(&fhost_ready_sem, 1, 0))
    {
        ASSERT_ERR(0);
        return 1;
    }

    if (rtos_mutex_create(&fhost_pending_task_lock))
    {
       ASSERT_ERR(0);
       return 1;
    }

    fhost_pending_task = (CO_BIT(IP_TASK)      |
                          CO_BIT(CONTROL_TASK) |
                          CO_BIT(RX_TASK)      |
                          CO_BIT(TX_TASK)      |
                          CO_BIT(WIFI_TASK)    |
                          CO_BIT(SUPPLICANT_TASK));

    // TCP/IP stack
    if (net_init())
    {
        ASSERT_ERR(0);
        return 1;
    }

    // Control task
    if (fhost_cntrl_init())
    {
        ASSERT_ERR(0);
        return 1;
    }

    // RX task
    if (fhost_rx_init())
    {
        ASSERT_ERR(0);
        return 1;
    }

    // TX task
    if (fhost_tx_init())
    {
        ASSERT_ERR(0);
        return 1;
    }

    // Wifi task
    if (rtos_wifi_task_create())
    {
        ASSERT_ERR(0);
        return 1;
    }

    // WPA supplicant environment
    if (fhost_wpa_init())
    {
        ASSERT_ERR(0);
        return 1;
    }

    return 0;
}

void fhost_task_ready(enum rtos_task_id task_id)
{
    rtos_mutex_lock(fhost_pending_task_lock);
    fhost_pending_task &= ~(CO_BIT(task_id));
    rtos_mutex_unlock(fhost_pending_task_lock);

    TRACE_FHOST(TASK_INIT, "%rT task is now initialized", task_id);

    if (fhost_pending_task == 0)
    {
        TRACE_FHOST(TASK_INIT, "All FHOST tasks are initialized");
        rtos_semaphore_signal(fhost_ready_sem, false);
    }
}

int fhost_wait_ready(void)
{
    if (fhost_ready_sem == NULL)
        return -1;

    if (rtos_semaphore_wait(fhost_ready_sem, -1))
        return -1;

    // always re-signal the semaphore in case it is called by several tasks.
    rtos_semaphore_signal(fhost_ready_sem, false);
    return 0;
}

int fhost_vif_name(int fhost_vif_idx, char *name, int len)
{
    if (fhost_vif_idx > NX_VIRT_DEV_MAX)
        return -1;

    return net_if_get_name(fhost_env.vif[fhost_vif_idx].net_if, name, len);
}

int fhost_vif_idx_from_name(const char *name)
{
    net_al_if_t net_if;
    int i;

    if (name == NULL)
        return -1;

    net_if = net_if_find_from_name(name);
    if (!net_if)
        return -1;

    for (i = 0 ; i < NX_VIRT_DEV_MAX ; i++)
    {
        if (fhost_env.vif[i].net_if == net_if)
            return i;
    }

    return -1;
}

int fhost_vif_set_uapsd_queues(int fhost_vif_idx, uint8_t uapsd_queues)
{
    if (fhost_vif_idx < 0)
    {
        int i;
        for (i = 0 ; i < NX_VIRT_DEV_MAX ; i++)
        {
            fhost_env.vif[i].uapsd_queues = uapsd_queues;
        }
    }
    else
    {
        if (fhost_vif_idx > NX_VIRT_DEV_MAX)
            return -1;
        fhost_env.vif[fhost_vif_idx].uapsd_queues = uapsd_queues;
    }

    return 0;
}

uint16_t fhost_ip_chksum(const void *dataptr, int len)
{
#if 0
    uint16_t checksum;

    // Try to compute the checksum using the HSU
    if (hsu_ip_checksum(CPU2HW(dataptr), len, &checksum))
        return checksum;
#endif

    // HSU cannot compute for some reason. Use the TCP/IP stack function instead.
    return net_ip_chksum(dataptr, len);
}

void fhost_get_status(struct fhost_status *status)
{
    int i;

    status->vif_max_cnt = NX_VIRT_DEV_MAX;
    status->vif_active_cnt = 0;
    status->vif_first_active = -1;

    for (i = 0 ; i < NX_VIRT_DEV_MAX ; i++)
    {
        if (fhost_env.vif[i].mac_vif != NULL)
        {
            status->vif_active_cnt++;
            if (status->vif_first_active < 0)
                status->vif_first_active = i;
        }
    }

    status->chan_2g4_cnt = fhost_chan.chan2G4_cnt;
    status->chan_2g4 = fhost_chan.chan2G4;

    status->chan_5g_cnt = fhost_chan.chan5G_cnt;
    status->chan_5g = fhost_chan.chan5G;
}

int fhost_get_vif_status(int fvif_idx, struct fhost_vif_status *status)
{
    struct fhost_vif_tag *fhost_vif;

    if (fvif_idx >= NX_VIRT_DEV_MAX)
        return -1;

    memset(status, 0, sizeof(*status));

    fhost_vif = &fhost_env.vif[fvif_idx];
    status->index = fvif_idx;
    if (fhost_vif->mac_vif)
    {
        void *mac_vif = fhost_vif->mac_vif;
        status->type = mac_vif_get_type(mac_vif);

        mac_vif_get_channel(mac_vif, &status->chan);
        mac_vif_get_sta_status(mac_vif, &status->sta.bssid, &status->sta.aid, &status->sta.rssi);
    }
    else
    {
        status->type = VIF_UNKNOWN;
    }

    status->mac_addr = (uint8_t *)&fhost_vif->mac_addr;

    return 0;
}

int fhost_get_vif_raw_state(int fvif_idx)
{
    struct fhost_vif_tag *fhost_vif;

    if (fvif_idx >= NX_VIRT_DEV_MAX)
        return -1;

    fhost_vif = &fhost_env.vif[fvif_idx];

    return fhost_vif->raw_state;
}

int fhost_get_vif_state(int fvif_idx)
{
    struct fhost_vif_tag *fhost_vif;

    if (fvif_idx >= NX_VIRT_DEV_MAX)
        return -1;

    fhost_vif = &fhost_env.vif[fvif_idx];

    return fhost_vif->state;
}

int fhost_set_vif_type(struct fhost_cntrl_link *link, int fvif_idx, enum mac_vif_type type,
                       bool p2p)
{
    struct fhost_vif_tag *fhost_vif;
    enum mac_vif_type prev_type = VIF_UNKNOWN;
    struct net_al_ext_ip_addr_cfg ip_cfg;

    if (fvif_idx >= NX_VIRT_DEV_MAX)
        return -1;

    fhost_vif = &fhost_env.vif[fvif_idx];

    if (fhost_vif->mac_vif)
        prev_type = mac_vif_get_type(fhost_vif->mac_vif);

    // Do nothing if interface type is already the requested one
    if (prev_type == type)
        return 0;

    // Close current connection (if any)
    ip_cfg.mode = IP_ADDR_NONE;
    net_al_ext_set_vif_ip(fvif_idx, &ip_cfg);
    fhost_wpa_remove_vif(fvif_idx);
    if (prev_type == VIF_MONITOR)
        fhost_rx_set_monitor_cb(NULL, NULL);

    // Change type of the associated MAC vif
    return fhost_cntrl_set_mac_vif_type(link, fvif_idx, type, p2p);
}

int fhost_configure_vif(struct fhost_cntrl_link *link, int fvif_idx,
                        struct fhost_vif_cfg *cfg)
{
    struct fhost_vif_tag *fhost_vif;

    if (fvif_idx >= NX_VIRT_DEV_MAX)
        return -1;

    fhost_vif = &fhost_env.vif[fvif_idx];

    if ((!fhost_vif->mac_vif) || (mac_vif_get_type(fhost_vif->mac_vif) != cfg->cfg_type))
        return -1;

    switch (cfg->cfg_type)
    {
        case VIF_STA:
            return fhost_sta_cfg(fvif_idx, &cfg->sta);
        #if NX_FHOST_MONITOR
        case VIF_MONITOR:
            return fhost_cntrl_monitor_cfg(link, fvif_idx, &cfg->monitor);
        #endif
        #if NX_BEACONING
        case VIF_AP:
            return fhost_ap_cfg(fvif_idx, &cfg->ap, false);
        #endif
        #if RW_MESH_EN
        case VIF_MESH_POINT:
            return fhost_ap_cfg(fvif_idx, &cfg->mesh_point, true);
        #endif

        default:
            break;
    }

    return -1;
}

int fhost_scan(struct fhost_cntrl_link *link, int fvif_idx)
{
    struct cfgrwnx_scan cmd;
    struct cfgrwnx_resp resp;
    struct cfgrwnx_scan_ssid ssid;
#if 0
    struct cfgrwnx_msg_hdr msg_hdr;
#endif
    int nb_result = -1;

    ssid.len = 0;
    ssid.ssid = NULL;
    cmd.hdr.len = sizeof(cmd);
    cmd.hdr.id = CFGRWNX_SCAN_CMD;
    cmd.fhost_vif_idx = fvif_idx;
    cmd.freqs = NULL;
    cmd.chans = NULL;
    cmd.chans_cnt = 0;
    cmd.extra_ies = NULL;
    cmd.bssid = NULL;
    cmd.ssids = &ssid;
    cmd.extra_ies_len = 0;
    cmd.no_cck = 0;
    cmd.ssid_cnt = 1;
    cmd.duration = 0;
    cmd.sock = link->sock_send;
    cmd.hdr.resp_queue = link->queue;
    cmd.passive = false;

    resp.hdr.len = sizeof(resp);
    resp.hdr.id = CFGRWNX_SCAN_RESP;

    if (fhost_cntrl_cfgrwnx_cmd_send(&cmd.hdr, &resp.hdr) ||
        (resp.status != CFGRWNX_SUCCESS))
        return -1;

#if 0
    // Wait scan to complete
    while (fhost_cntrl_cfgrwnx_event_peek_header(link, &msg_hdr) == 0)
    {
        if (msg_hdr.id == CFGRWNX_SCAN_DONE_EVENT)
        {
            struct cfgrwnx_scan_completed res;
            if ((fhost_cntrl_cfgrwnx_event_get(link, &res, sizeof(res)) == sizeof(res)) &&
                (res.status == CFGRWNX_SUCCESS))
                nb_result = res.result_cnt;
            break;
        }
        else
        {
            fhost_cntrl_cfgrwnx_event_discard(link, &msg_hdr);
        }
    }

#endif
    return nb_result;
}

int fhost_get_scan_results(struct fhost_cntrl_link *link, int result_idx,
                           int max_nb_result, struct mac_scan_result *results)
{
    struct cfgrwnx_scan_results cmd;
    struct cfgrwnx_scan_results_resp resp;
    struct mac_scan_result *results_ptr = results;
    int nb_res = 0;

    cmd.hdr.len = sizeof(cmd);
    cmd.hdr.id = CFGRWNX_SCAN_RESULTS_CMD;
    cmd.hdr.resp_queue = link->queue;
    cmd.idx = result_idx;

    resp.hdr.len = sizeof(resp);
    resp.hdr.id = CFGRWNX_SCAN_RESULTS_RESP;

    while ((nb_res < max_nb_result) &&
           !fhost_cntrl_cfgrwnx_cmd_send(&cmd.hdr, &resp.hdr) &&
           (resp.status == CFGRWNX_SUCCESS) &&
           resp.scan_result.valid_flag)
    {
        *results_ptr++ = resp.scan_result;
        nb_res++;
        cmd.idx++;
    }

    return nb_res;
}

int fhost_sta_cfg(int fhost_vif_idx, struct fhost_vif_sta_cfg *cfg)
{
    struct fhost_vif_tag *fhost_vif;
    char *cfg_str, *cfg_ssid_str, *cfg_key_str, *ptr, *ssid_ptr, *key_ptr;
    int res, cfg_str_len = 400, cfg_ssid_len = 40, cfg_key_len = 70;
    int key_len;

    if ((fhost_vif_idx >= NX_VIRT_DEV_MAX) || (cfg == NULL))
        return -1;

    fhost_vif = &fhost_env.vif[fhost_vif_idx];

    if ((!fhost_vif->mac_vif) || (mac_vif_get_type(fhost_vif->mac_vif) != VIF_STA))
        return -1;

    cfg_str = rtos_malloc(cfg_str_len + 1);
    cfg_ssid_str = rtos_malloc(cfg_ssid_len + 1);
    cfg_key_str = rtos_malloc(cfg_key_len + 1);
    if (!cfg_str || !cfg_ssid_str || !cfg_key_str)
        return -1;
    memset(cfg_key_str, 0, cfg_key_len + 1);
    ptr = cfg_str;
    ssid_ptr = cfg_ssid_str;
    key_ptr = cfg_key_str;

    // SSID
    res = cfg->ssid.length + 8; // 8 = 'ssid "";'
    if (cfg_ssid_len < res)
        goto end;

    memcpy(cfg_ssid_str, "ssid \"", 6);
    ssid_ptr += 6;
    memcpy(ssid_ptr, cfg->ssid.array, cfg->ssid.length);
    ssid_ptr += cfg->ssid.length;
    *ssid_ptr++ = '"';
    *ssid_ptr++ = 0;

    // SCAN
    if (cfg->duration) {
        res = dbg_snprintf(ptr, cfg_str_len, "duration %d;", cfg->duration);
        if (res >= cfg_str_len)
            goto end;

        ptr += res;
        cfg_str_len -= res;
    }
    if (cfg->probe_cnt) {
        res = dbg_snprintf(ptr, cfg_str_len, "probe_cnt %d;", cfg->probe_cnt);
        if (res >= cfg_str_len)
            goto end;

        ptr += res;
        cfg_str_len -= res;
    }

    // Timeout
    if (cfg->auth_timeout) {
        res = dbg_snprintf(ptr, cfg_str_len, "auth_timeout %d;", cfg->auth_timeout);
        if (res >= cfg_str_len)
            goto end;

        ptr += res;
        cfg_str_len -= res;
    }
    if (cfg->eapol_1_timeout) {
        res = dbg_snprintf(ptr, cfg_str_len, "eapol_1_timeout %d;", cfg->eapol_1_timeout);
        if (res >= cfg_str_len)
            goto end;

        ptr += res;
        cfg_str_len -= res;
    }
    if (cfg->eapol_rem_timeout) {
        res = dbg_snprintf(ptr, cfg_str_len, "eapol_rem_timeout %d;", cfg->eapol_rem_timeout);
        if (res >= cfg_str_len)
            goto end;

        ptr += res;
        cfg_str_len -= res;
    }

    // AKM
    key_len = strlen(cfg->key);
    if (!cfg->akm)
    {
        if (key_len < 8)
            // If key is less than 8, assume WEP key
            cfg->akm = CO_BIT(MAC_AKM_NONE);
        else
            cfg->akm = CO_BIT(MAC_AKM_PSK) | CO_BIT(MAC_AKM_PSK_SHA256)
                #if NX_CRYPTOLIB
                | CO_BIT(MAC_AKM_SAE);
                #endif
                ;
    }
    else
    {
        // remove unsupported AKM
        uint32_t akm_supported = CO_BIT(MAC_AKM_NONE) | CO_BIT(MAC_AKM_PSK) | CO_BIT(MAC_AKM_PSK_SHA256);
        #if NX_CRYPTOLIB
        akm_supported |= CO_BIT(MAC_AKM_SAE);
        #endif

        // For WEP the user select only MAC_AKM_PRE_RSN
        if (cfg->akm == CO_BIT(MAC_AKM_PRE_RSN))
            cfg->akm = CO_BIT(MAC_AKM_NONE);
        else if (!(cfg->akm & CO_BIT(MAC_AKM_PRE_RSN)))
        {
            // User doesn't allow WPA1 AP
             res = dbg_snprintf(ptr, cfg_str_len, "proto RSN;");
             if (res >= cfg_str_len)
                 goto end;
             ptr += res;
             cfg_str_len -= res;
        }

        cfg->akm &= akm_supported;
        if (cfg->akm == 0)
        {
            res = -1;
            goto end;
        }
    }
    res = 8;
    if (cfg_str_len < res)
        goto end;
    memcpy(ptr, "key_mgmt", res);
    ptr += res;
    cfg_str_len -= res;
    res = fhost_wpa_akm_name(cfg->akm, ptr, cfg_str_len);
    if (res < 0)
        goto end;
    ptr += res;
    cfg_str_len -= res;

    // Cipher suites for WPA
    if (cfg->akm & (CO_BIT(MAC_AKM_PSK) | CO_BIT(MAC_AKM_SAE)))
    {
        uint32_t cipher_supported = 0;
        uint32_t cipher_pairwise, cipher_group;

        if (inline_nxmac_tkip_getf())
            cipher_supported |= CO_BIT(MAC_CIPHER_TKIP);
        if (inline_nxmac_ccmp_getf())
        {
            cipher_supported |= CO_BIT(MAC_CIPHER_CCMP);
            #if NX_MAC_HE
            cipher_supported |= CO_BIT(MAC_CIPHER_CCMP_256);
            #endif
        }
        #if NX_MAC_HE
        if (inline_nxmac_gcmp_getf())
            cipher_supported |= CO_BIT(MAC_CIPHER_GCMP_128) | CO_BIT(MAC_CIPHER_GCMP_256);
        #endif

        if (cfg->unicast_cipher)
            cipher_pairwise = cfg->unicast_cipher & cipher_supported;
        else
            cipher_pairwise = cipher_supported;

        if (cfg->group_cipher)
            cipher_group = cfg->group_cipher & cipher_supported;
        else
            cipher_group = cipher_supported;


        if (!cipher_pairwise || !cipher_group)
        {
            res = -1;
            goto end;
        }

        // By default wpa_supplicant enable TKIP and CCMP. If we support something else need to
        // configure wpa_supplicant accordingly
        if (cipher_pairwise != (CO_BIT(MAC_CIPHER_TKIP) | CO_BIT(MAC_CIPHER_CCMP)))
        {
            res = 8;
            if (cfg_str_len < res)
                goto end;
            memcpy(ptr, "pairwise", res);
            ptr += res;
            cfg_str_len -= res;
            res = fhost_wpa_cipher_name(cipher_pairwise, ptr, cfg_str_len);
            if (res < 0)
                goto end;
            ptr += res;
            cfg_str_len -= res;
        }

        if (cipher_group != (CO_BIT(MAC_CIPHER_TKIP) | CO_BIT(MAC_CIPHER_CCMP)))
        {
            res = 5;
            if (cfg_str_len < res)
                goto end;
            memcpy(ptr, "group", res);
            ptr += res;
            cfg_str_len -= res;
            res = fhost_wpa_cipher_name(cipher_group, ptr, cfg_str_len);
            if (res < 0)
                goto end;
            ptr += res;
            cfg_str_len -= res;
        }
    }

    // Keys
    if (key_len > 0)
    {
        if (cfg->akm & CO_BIT(MAC_AKM_NONE))
        {
            // WEP keys
            res = dbg_snprintf(ptr, cfg_str_len, "wep_key0 %s;auth_alg OPEN SHARED;", cfg->key);
        }
        else
        {
            // PSK (works also for SAE)
            if (key_len == 64) {
                //support for psk setting
                res = dbg_snprintf(key_ptr, cfg_key_len, "psk %s", cfg->key);
            } else {
                res = dbg_snprintf(key_ptr, cfg_key_len, "psk \"%s\"", cfg->key);
            }
            if (res > cfg_key_len)
                goto end;
            key_ptr += res;
            *key_ptr++ = 0;;
            res = 0;
        }

        if (res >= cfg_str_len)
            goto end;

        ptr += res;
        cfg_str_len -= res;

        #if NX_MFP
        if (!(cfg->akm & CO_BIT(MAC_AKM_NONE))) {
            res = dbg_snprintf(ptr, cfg_str_len, "ieee80211w %d;", cfg->pmf);
            if (res >= cfg_str_len)
                goto end;

            ptr += res;
            cfg_str_len -= res;
        }
        #endif
    }

    res = dbg_snprintf(ptr, cfg_str_len, "scan_ssid 1;");
    if (res >= cfg_str_len)
        goto end;

    ptr += res;
    cfg_str_len -= res;


    // BSSID (optional)
    if (cfg->bssid.array[0] || cfg->bssid.array[1] || cfg->bssid.array[2])
    {
        res = dbg_snprintf(ptr, cfg_str_len, "bssid %02x:%02x:%02x:%02x:%02x:%02x;",
                           ((uint8_t *)cfg->bssid.array)[0], ((uint8_t *)cfg->bssid.array)[1],
                           ((uint8_t *)cfg->bssid.array)[2], ((uint8_t *)cfg->bssid.array)[3],
                           ((uint8_t *)cfg->bssid.array)[4], ((uint8_t *)cfg->bssid.array)[5]);
        if (res >= cfg_str_len)
            goto end;

        ptr += res;
        cfg_str_len -= res;
    }

    // Frequencies (optional)
    if (cfg->freq[0])
    {
        unsigned int i, j;

        // silently remove invalid frequencies
        for (i = 0, j = 0; i < CO_ARRAY_SIZE(cfg->freq); i++)
        {
            if (cfg->freq[i] == 0)
                break;
            if (fhost_chan_get(cfg->freq[i]))
            {
                if (j != i)
                    cfg->freq[j] = cfg->freq[i];
                j++;
            }
        }

        if (j > 0)
        {
            res = dbg_snprintf(ptr, cfg_str_len, "scan_freq ");
            if (res >= cfg_str_len)
                goto end;
            ptr += res;
            cfg_str_len -= res;

            while (j > 0)
            {
                j--;
                res = dbg_snprintf(ptr, cfg_str_len, "%d ", cfg->freq[j]);
                if (res >= cfg_str_len)
                    goto end;
                ptr += res;
                cfg_str_len -= res;
            }

            res = dbg_snprintf(ptr, cfg_str_len, ";");
            if (res >= cfg_str_len)
                goto end;
            ptr += res;
            cfg_str_len -= res;
        }
    }

    res = dbg_snprintf(ptr, cfg_str_len, "wpa_flags %d;", cfg->wpa_flags);
    if (res >= cfg_str_len)
        goto end;
    ptr += res;
    cfg_str_len -= res;

    #ifdef CFG_IPRO_WIFI_PS_ENABLE
    PLATFORM_HOOK(prevent_sleep, PSM_EVENT_CONNECT, 1);
    #endif
    res = fhost_wpa_create_network(fhost_vif_idx, cfg_str, cfg_ssid_str, cfg_key_str, true, cfg->timeout_ms);
    #ifdef CFG_IPRO_WIFI_PS_ENABLE
    if (res != 0) {
        PLATFORM_HOOK(prevent_sleep, PSM_EVENT_CONNECT, 0);
    }
    #endif

  end:
    if (res > 0)
    {
        TRACE_FHOST(ERR, "Missing at least %d character for wpa_supplicant config",
                    res - cfg_str_len);
        res = -1;
    }
    rtos_free(cfg_str);
    rtos_free(cfg_ssid_str);
    rtos_free(cfg_key_str);
    return res;
}

int fhost_get_sta_idx(struct fhost_vif_tag *fhost_vif, uint8_t table_max_size,
                      uint8_t *idx_table)
{
    uint8_t size = 0, i = 0;
    struct vif_info_tag *vif = fhost_vif->mac_vif;
    void *sta = (void *)co_list_pick(mac_vif_get_sta_list(vif));

    while (sta != NULL)
    {
        if (i >= table_max_size)
            break;

        idx_table[i++] = sta_get_staid(sta);
        sta = sta_get_list_next(sta);
    }
    size = i;
    return size;
}

void *fhost_to_mac_vif(uint8_t fhost_vif_idx)
{
    void *mac_vif = fhost_env.vif[fhost_vif_idx].mac_vif;

    // Sanity check - Currently we consider that when this function is called there shall
    // be a MAC VIF attached to the FHOST VIF. If in the future this has to change then
    // this assertion will be removed
    // ASSERT_ERR(mac_vif != NULL);

    return mac_vif;
}

int is_sta_netif(net_al_if_t nif)
{
    net_al_if_t sta_nif = fhost_to_net_if(0);

    return nif == sta_nif;
}

int is_ap_netif(net_al_if_t nif)
{
    net_al_if_t ap_nif = fhost_to_net_if(1);

    return nif == ap_nif;
}

net_al_if_t fhost_to_net_if(uint8_t fhost_vif_idx)
{
    ASSERT_ERR(fhost_vif_idx < NX_VIRT_DEV_MAX);
    return fhost_env.vif[fhost_vif_idx].net_if;
}

struct fhost_vif_tag *fhost_from_mac_vif(uint8_t mac_vif_idx)
{
    struct fhost_vif_tag *fhost_vif;

    // Sanity check - Check that mac_vif_idx is valid
    ASSERT_ERR(mac_vif_idx < NX_VIRT_DEV_MAX);

    fhost_vif = fhost_env.mac2fhost_vif[mac_vif_idx];

    // Sanity check - Currently we consider that when this function is called there shall
    // be a FHOST VIF attached to the MAC VIF. If in the future this has to change then
    // this assertion will be removed
    ASSERT_ERR(fhost_vif != NULL);

    return fhost_vif;
}

#if NX_BEACONING
int fhost_ap_cfg(int fhost_vif_idx, struct fhost_vif_ap_cfg *cfg, bool mesh_mode)
{
    struct fhost_vif_tag *fhost_vif;
    struct mac_chan_def *chan = NULL;
    char *cfg_str, *cfg_ssid_str, *cfg_key_str, *ptr, *ssid_ptr, *key_ptr;
    int res, cfg_str_len = 300, cfg_ssid_len = 40, cfg_key_len = 70;
    int key_len;
    uint32_t akm;
    int timeout;

    if (mesh_mode)
        timeout = 3000;
    else
        timeout = 500;

    if ((fhost_vif_idx >= NX_VIRT_DEV_MAX) || (cfg == NULL))
        return -1;

    fhost_vif = &fhost_env.vif[fhost_vif_idx];

    if ((!fhost_vif->mac_vif) || ((mac_vif_get_type(fhost_vif->mac_vif) != VIF_AP) && !mesh_mode))
        return -1;

    cfg_str = rtos_malloc(cfg_str_len + 1);
    cfg_ssid_str = rtos_malloc(cfg_ssid_len + 1);
    cfg_key_str = rtos_malloc(cfg_key_len + 1);
    if (!cfg_str || !cfg_ssid_str || !cfg_key_str)
        return -1;
    memset(cfg_key_str, 0, cfg_key_len + 1);
    ptr = cfg_str;
    ssid_ptr = cfg_ssid_str;
    key_ptr = cfg_key_str;

    if (mesh_mode)
        // Enable MESH mode
        res = dbg_snprintf(ptr, cfg_str_len, "mode 5;");
    else
        // Enable AP mode
        res = dbg_snprintf(ptr, cfg_str_len, "mode 2;");
    ptr += res;
    cfg_str_len -= res;

    // SSID
    res = cfg->ssid.length + 8; // 8 = 'ssid "";'
    if (cfg_ssid_len < res)
        goto end;

    memcpy(cfg_ssid_str, "ssid \"", 6);
    ssid_ptr += 6;
    memcpy(ssid_ptr, cfg->ssid.array, cfg->ssid.length);
    ssid_ptr += cfg->ssid.length;
    *ssid_ptr++ = '"';
    *ssid_ptr++ = 0;

    // Hidden SSID
    if (cfg->hidden_ssid)
    {
        res = dbg_snprintf(ptr, cfg_str_len, "ignore_broadcast_ssid %d;", 1);
        if (res >= cfg_str_len)
            goto end;
        ptr += res;
        cfg_str_len -= res;
    }

    // Operating Channel and Mode
    chan = fhost_chan_get(cfg->chan.prim20_freq);
    if (!chan || (chan->flags & (CHAN_NO_IR | CHAN_DISABLED | CHAN_RADAR)))
        goto end;

    res = dbg_snprintf(ptr, cfg_str_len, "frequency %d;", cfg->chan.prim20_freq);
    if (res >= cfg_str_len)
        goto end;
    ptr += res;
    cfg_str_len -= res;

    if (cfg->mode == FHOST_AP_NON_HT)
    {
        res = dbg_snprintf(ptr, cfg_str_len, "ht 0;");
        if (res >= cfg_str_len)
            goto end;
        ptr += res;
        cfg_str_len -= res;
    }
    else
    {
        int freq_offset = cfg->chan.center1_freq - cfg->chan.prim20_freq;
        int vht = 0, he = 0, ht40 = 0, chwidth = 0;

        #if NX_VHT
        if (phy_vht_supported() &&
            ((cfg->mode == FHOST_AP_AUTO) || (cfg->mode >= FHOST_AP_VHT)) &&
            (cfg->chan.band == PHY_BAND_5G))
            vht = 1;
        #endif

        #if NX_HE
        if (phy_he_supported() &&
            ((cfg->mode == FHOST_AP_AUTO) || (cfg->mode == FHOST_AP_HE)))
            he = 1;
        #endif

        if (cfg->chan.type == PHY_CHNL_BW_20)
        {
            if (freq_offset != 0)
                goto end;
        }
        else if (cfg->chan.type == PHY_CHNL_BW_40)
        {
            if (phy_get_bw() < PHY_CHNL_BW_40)
                goto end;

            if (freq_offset == -10)
                ht40 = -1;
            else if (freq_offset == 10)
                ht40 = 1;
            else
                goto end;
        }
        #if NX_VHT
        else if (cfg->chan.type == PHY_CHNL_BW_80)
        {
            int freq_offset = cfg->chan.center1_freq - cfg->chan.prim20_freq;

            if (phy_get_bw() < PHY_CHNL_BW_80)
                goto end;

            if (cfg->mode == FHOST_AP_HT)
                goto end;

            if ((freq_offset == 10) || (freq_offset == -30))
                ht40 = -1;
            else if ((freq_offset == -10) || (freq_offset == 30))
                ht40 = 1;
            else
                goto end;

            chwidth = 1;
        }
        #endif //NX_VHT
        else
        {
            // not supported
            goto end;
        }

        if (ht40)
            // time for ht40 overlap scan, fix wap timeout so increase 1000
            timeout += 2500;

        res = dbg_snprintf(ptr, cfg_str_len, "vht %d;he %d;ht40 %d;max_oper_chwidth %d;vht_center_freq1 %d;vht_center_freq2 %d;",
                           vht, he, ht40, chwidth, cfg->chan.center1_freq, cfg->chan.center2_freq);
        if (res >= cfg_str_len)
            goto end;
        ptr += res;
        cfg_str_len -= res;
    }

    // Beacon
    if (cfg->bcn_interval == 0)
        cfg->bcn_interval = 100;
    else if (cfg->bcn_interval < 15)
        cfg->bcn_interval = 15;
    if (cfg->dtim_period < 1)
        cfg->dtim_period = 1;
    res = dbg_snprintf(ptr, cfg_str_len, "beacon_int %d;dtim_period %d;",
                       cfg->bcn_interval, cfg->dtim_period);
    if (res >= cfg_str_len)
        goto end;
    ptr += res;
    cfg_str_len -= res;

    // AKM (remove unsupported ones)
    akm = cfg->akm & (CO_BIT(MAC_AKM_PSK) |
                      CO_BIT(MAC_AKM_PRE_RSN) |
                      #if NX_CRYPTOLIB
                      CO_BIT(MAC_AKM_SAE) |
                      #endif
                      CO_BIT(MAC_AKM_NONE));
    if (!akm)
        goto end;

    key_len = strlen(cfg->key);
    if (akm & CO_BIT(MAC_AKM_NONE))
    {
        if (cfg->akm & ~CO_BIT(MAC_AKM_NONE))
            goto end;
    }
    else if (akm & CO_BIT(MAC_AKM_PRE_RSN))
    {
        if (akm & CO_BIT(MAC_AKM_PSK))
        {
            if (key_len < 8)
                goto end;

            akm = CO_BIT(MAC_AKM_PSK);
            cfg->unicast_cipher = CO_BIT(MAC_CIPHER_TKIP);
            // WEP is no longer allowed for group cipher so always use TKIP
            cfg->group_cipher =  CO_BIT(MAC_CIPHER_TKIP);
            res = dbg_snprintf(ptr, cfg_str_len, "proto WPA;");
        }
        else if ((key_len == 5) || (key_len == 13))
        {
            akm = CO_BIT(MAC_AKM_NONE);
            res = dbg_snprintf(ptr, cfg_str_len, "wep_key0 \"%s\";wep_tx_keyidx 0;",
                               cfg->key);
        }
        else
            goto end;

        if (res >= cfg_str_len)
            goto end;
        ptr += res;
        cfg_str_len -= res;
    }
    else
    {
        if (key_len < 8)
            goto end;
        res = dbg_snprintf(ptr, cfg_str_len, "proto RSN;");
        if (res >= cfg_str_len)
            goto end;
        ptr += res;
        cfg_str_len -= res;
    }
    res = 8;
    if (cfg_str_len < res)
        goto end;
    memcpy(ptr, "key_mgmt", res);
    ptr += res;
    cfg_str_len -= res;
    res = fhost_wpa_akm_name(akm, ptr, cfg_str_len);
    if (res < 0)
        goto end;
    ptr += res;
    cfg_str_len -= res;

    // WPAx config (cipher, PSK, MFP)
    if (akm & (CO_BIT(MAC_AKM_PSK) | CO_BIT(MAC_AKM_SAE)))
    {
        uint32_t group, pairwise;
        uint32_t cipher_supported = 0;

        if (inline_nxmac_tkip_getf())
            cipher_supported |= CO_BIT(MAC_CIPHER_TKIP);
        if (inline_nxmac_ccmp_getf())
        {
            cipher_supported |= CO_BIT(MAC_CIPHER_CCMP);
            #if NX_MAC_HE
            cipher_supported |= CO_BIT(MAC_CIPHER_CCMP_256);
            #endif
        }
        #if NX_MAC_HE
        if (inline_nxmac_gcmp_getf())
            cipher_supported |= CO_BIT(MAC_CIPHER_GCMP_128) | CO_BIT(MAC_CIPHER_GCMP_256);
        #endif

        // Remove unsupported cipher or set default value if not set
        if (cfg->unicast_cipher)
        {
            pairwise = cfg->unicast_cipher & cipher_supported;
            if (!pairwise)
                goto end;
        }
        else
            pairwise = CO_BIT(MAC_CIPHER_CCMP);

        res = 8;
        if (cfg_str_len < res)
            goto end;
        memcpy(ptr, "pairwise", res);
        ptr += res;
        cfg_str_len -= res;
        res = fhost_wpa_cipher_name(pairwise, ptr, cfg_str_len);
        if (res < 0)
            goto end;
        ptr += res;
        cfg_str_len -= res;

        if (cfg->group_cipher)
        {
            group = cfg->group_cipher & cipher_supported;
            if (!group)
                goto end;
        }
        else
            group = CO_BIT(MAC_CIPHER_CCMP);

        res = 5;
        if (cfg_str_len < res)
            goto end;
        memcpy(ptr, "group", res);
        ptr += res;
        cfg_str_len -= res;
        res = fhost_wpa_cipher_name(group, ptr, cfg_str_len);
        if (res < 0)
            goto end;
        ptr += res;
        cfg_str_len -= res;

        res = dbg_snprintf(key_ptr, cfg_key_len, "psk \"%s\";", cfg->key);
        if (res > cfg_key_len)
            goto end;
        key_ptr += res;
        *key_ptr++ = 0;

        #if NX_MFP
        if (cfg->mfp < 0 || cfg->mfp > 2)
            goto end;

        res = dbg_snprintf(ptr, cfg_str_len, "ieee80211w %d;", cfg->mfp);
        if (res >= cfg_str_len)
            goto end;
        ptr += res;
        cfg_str_len -= res;
        #endif
    }

    if (mesh_mode)
    {
        int i = 0;

        if(cfg->basic_rates[i] != 0)
        {
           res = 16;
           if (cfg_str_len < res)
               goto end;
           memcpy(ptr, "mesh_basic_rates", res);
           ptr += res;
           cfg_str_len -= res;

           res = dbg_snprintf(ptr, cfg_str_len," %d", cfg->basic_rates[i]);
           if (res >= cfg_str_len)
               goto end;
           ptr += res;
           cfg_str_len -= res;

           while(cfg->basic_rates[++i] != 0 && (i < MAC_RATESET_LEN))
           {
               res = dbg_snprintf(ptr, cfg_str_len," %d", cfg->basic_rates[i]);
               if (res >= cfg_str_len)
                   goto end;
               ptr += res;
               cfg_str_len -= res;
           }
           res = 1;
           if (cfg_str_len < res)
               goto end;
           memcpy(ptr, ";", res);
           ptr++;
           cfg_str_len--;
        }
    }

    // Extra user config
    if (cfg->extra_cfg)
    {
        res = dbg_snprintf(ptr, cfg_str_len, "%s", cfg->extra_cfg);
        if (res >= cfg_str_len)
            goto end;
        ptr += res;
        cfg_str_len -= res;
    }

    #ifdef CFG_IPRO_WIFI_PS_ENABLE
    PLATFORM_HOOK(prevent_sleep, PSM_EVENT_AP, 1);
    #endif
    if (mesh_mode)
        res = fhost_wpa_create_network(fhost_vif_idx, cfg_str, cfg_ssid_str, cfg_key_str, true, timeout);
    else
        res = (fhost_wpa_create_network(fhost_vif_idx, cfg_str, cfg_ssid_str, cfg_key_str, false, 0) |
               fhost_wpa_execute_cmd(fhost_vif_idx, NULL, 0, 300, "AP_SCAN 2") |
               (cfg->ap_vendor_elements && fhost_wpa_execute_cmd(fhost_vif_idx, NULL, 0, 300, cfg->ap_vendor_elements)) |
               fhost_wpa_enable_network(fhost_vif_idx, timeout));

    if (res)
        fhost_wpa_remove_vif(fhost_vif_idx);

    // Isolation Mode
    fhost_vif->isolation_mode = cfg->isolation_mode;

  end:
    if (res > 0)
    {
        if (res >= cfg_str_len)
        {
            TRACE_FHOST(ERR, "Missing at least %d character for wpa_supplicant config (AP)",
                        res - cfg_str_len);
        }
        else
        {
            TRACE_FHOST(ERR, "Invalid AP config: chan_freq=%d chan_flags=%x "
                        "akm=%08lx unicast=%08lx group=%08lx key_len=%d",
                        cfg->chan.prim20_freq, (chan) ? chan->flags : 0xffff,
                        TR_32(cfg->akm), TR_32(cfg->unicast_cipher),
                        TR_32(cfg->group_cipher), strlen(cfg->key));
            TRACE_BUF_FHOST(ERR, "Config before error: %pBs", ptr - cfg_str, cfg_str);
        }
        res = -1;
    }
    rtos_free(cfg_str);
    rtos_free(cfg_ssid_str);
    rtos_free(cfg_key_str);

    #ifdef CFG_IPRO_WIFI_PS_ENABLE
    if (res) {
        PLATFORM_HOOK(prevent_sleep, PSM_EVENT_AP, 0);
    }
    #endif
    return res;
}
#endif

#if NX_FTM_INITIATOR
int fhost_ftm(struct fhost_cntrl_link *link, int fvif_idx,
              struct mac_ftm_results *res)
{
    struct cfgrwnx_ftm_start_cmd cmd;
    struct cfgrwnx_resp resp;
    struct cfgrwnx_msg_hdr msg_hdr;
    struct cfgrwnx_ftm_done_event event;

    cmd.hdr.len = sizeof(cmd);
    cmd.hdr.id = CFGRWNX_FTM_START_CMD;
    cmd.hdr.resp_queue = link->queue;
    cmd.sock = link->sock_send;
    cmd.fhost_vif_idx = fvif_idx;

    resp.hdr.len = sizeof(resp);
    resp.hdr.id = CFGRWNX_FTM_START_RESP;

    if (fhost_cntrl_cfgrwnx_cmd_send(&cmd.hdr, &resp.hdr) ||
        (resp.status != CFGRWNX_SUCCESS))
        return -1;

    // Wait FTM measurements to complete
    while (fhost_cntrl_cfgrwnx_event_peek_header(link, &msg_hdr) == 0)
    {
        if (msg_hdr.id == CFGRWNX_FTM_DONE_EVENT)
        {
            if (fhost_cntrl_cfgrwnx_event_get(link, &event, sizeof(event)) != sizeof(event))
                return -1;
            break;
        }
        else
        {
            fhost_cntrl_cfgrwnx_event_discard(link, &msg_hdr);
        }
    }

    // Copy results
    *res = event.results;

    return 0;
}
#endif // NX_FTM_INITIATOR


char *fhost_ipc_next_token(char **params)
{
    char *ptr = *params, *next;
    char sep = ' ';

    if (!ptr)
        return NULL;

    if ((ptr[0] == '"') || (ptr[0] == '\''))
    {
        sep = ptr[0];
        ptr++;
    }

    next = strchr(ptr, sep);
    if (next)
    {
        *next++ = '\0';
        while (*next == ' ')
            next++;
        if (*next == '\0')
            next = NULL;
    }
    *params = next;
    return ptr;
}

void fhost_ipc_upper(char *str, char *stop)
{
    char *ptr = str;
    char c;

    if (stop)
    {
        c = *stop;
        *stop = 0;
    }

    while (*ptr)
    {
        if ((*ptr >= 'a') && (*ptr <= 'z'))
            *ptr -= 'a' - 'A';
        ptr++;
    }

    if (stop)
        *stop = c;
}

int fhost_ipc_parse_ip4(char *str, uint32_t *ip, uint32_t *mask)
{
    char *token;
    uint32_t a, i, j;

    #define check_is_num(_str)  for (j = 0 ; j < strlen(_str); j++) {  \
            if (_str[j] < '0' || _str[j] > '9')                        \
                return -1;                                             \
        }

    // Check if mask is present
    token = strchr(str, '/');
    if (token && mask) {
        *token++ = '\0';
        check_is_num(token);
        a = atoi(token);
        if (a == 0 || a > 32)
            return -1;
        *mask = (1<<a) - 1;
    }
    else if (mask)
    {
        *mask = 0xffffffff;
    }

    // parse the ip part
    *ip = 0;
    for (i = 0; i < 4; i ++)
    {
        if (i < 3)
        {
            token = strchr(str, '.');
            if (!token)
                return -1;
            *token++ = '\0';
        }
        check_is_num(str);
        a = atoi(str);
        if (a > 255)
            return -1;
        str = token;
        *ip += (a << (i * 8));
    }

    return 0;
}

int fhost_ipc_parse_mac_addr(const char *str, struct mac_addr *addr)
{
    char *ptr = str;
    uint32_t i;

    if (!str || strlen(str) < 17 || !addr)
        return -1;

    for (i = 0 ; i < 6 ; i++)
    {
        char *next;
        long int hex = strtol(ptr, &next, 16);
        if (((unsigned)hex > 255) || ((hex == 0) && (next == ptr)) ||
            ((i < 5) && (*next != ':')) ||
            ((i == 5) && (*next != '\0')))
            return -1;

        ((uint8_t *)addr)[i] = (uint8_t)hex;
        ptr = ++next;
    }

    return 0;
}

void fhost_ipc_print_akm(enum mac_akm_suite suite, bool sep)
{
    if (sep)
        fhost_print(RTOS_TASK_NULL, "-");

    switch (suite)
    {
        case MAC_AKM_8021X:
            fhost_print(RTOS_TASK_NULL, "8021X");
            break;
        case MAC_AKM_PSK:
            fhost_print(RTOS_TASK_NULL, "PSK");
            break;
        case MAC_AKM_FT_8021X:
            fhost_print(RTOS_TASK_NULL, "FT_8021X");
            break;
        case MAC_AKM_FT_PSK:
            fhost_print(RTOS_TASK_NULL, "FT_PSK");
            break;
        case MAC_AKM_8021X_SHA256:
            fhost_print(RTOS_TASK_NULL, "8021X_SHA256");
            break;
        case MAC_AKM_PSK_SHA256:
            fhost_print(RTOS_TASK_NULL, "PSK_SHA256");
            break;
        case MAC_AKM_TDLS:
            fhost_print(RTOS_TASK_NULL, "TDLS");
            break;
        case MAC_AKM_SAE:
            fhost_print(RTOS_TASK_NULL, "SAE");
            break;
        case MAC_AKM_FT_OVER_SAE:
            fhost_print(RTOS_TASK_NULL, "FT_OVER_SAE");
            break;
        case MAC_AKM_8021X_SUITE_B:
            fhost_print(RTOS_TASK_NULL, "8021X_SUITE_B");
            break;
        case MAC_AKM_8021X_SUITE_B_192:
            fhost_print(RTOS_TASK_NULL, "8021X_SUITE_B_192");
            break;
        case MAC_AKM_FILS_SHA256:
            fhost_print(RTOS_TASK_NULL, "FILS_SHA256");
            break;
        case MAC_AKM_FILS_SHA384:
            fhost_print(RTOS_TASK_NULL, "FILS_SHA384");
            break;
        case MAC_AKM_FT_FILS_SHA256:
            fhost_print(RTOS_TASK_NULL, "FT_FILS_SHA256");
            break;
        case MAC_AKM_FT_FILS_SHA384:
            fhost_print(RTOS_TASK_NULL, "FT_FILS_SHA384");
            break;
        case MAC_AKM_OWE:
            fhost_print(RTOS_TASK_NULL, "OWE");
            break;
        default:
            break;
    }
}

void fhost_ipc_print_cipher(enum mac_cipher_suite suite, bool sep)
{
    if (sep)
        fhost_print(RTOS_TASK_NULL, "-");

    switch (suite)
    {
        case MAC_CIPHER_WEP40:
            fhost_print(RTOS_TASK_NULL, "WEP40");
            break;
        case MAC_CIPHER_TKIP:
            fhost_print(RTOS_TASK_NULL, "TKIP");
            break;
        case MAC_CIPHER_CCMP:
            fhost_print(RTOS_TASK_NULL, "CCMP");
            break;
        case MAC_CIPHER_WEP104:
            fhost_print(RTOS_TASK_NULL, "WEP104");
            break;
        case MAC_CIPHER_WPI_SMS4:
            fhost_print(RTOS_TASK_NULL, "WPI_SMS4");
            break;
        case MAC_CIPHER_BIP_CMAC_128:
            fhost_print(RTOS_TASK_NULL, "BIP_CMAC_128");
            break;
        case MAC_CIPHER_GCMP_128:
            fhost_print(RTOS_TASK_NULL, "GCMP_128");
            break;
        case MAC_CIPHER_GCMP_256:
            fhost_print(RTOS_TASK_NULL, "GCMP_256");
            break;
        case MAC_CIPHER_CCMP_256:
            fhost_print(RTOS_TASK_NULL, "CCMP_256");
            break;
        case MAC_CIPHER_BIP_GMAC_128:
            fhost_print(RTOS_TASK_NULL, "BIP_GMAC_128");
            break;
        case MAC_CIPHER_BIP_GMAC_256:
            fhost_print(RTOS_TASK_NULL, "BIP_GMAC_256");
            break;
        case MAC_CIPHER_BIP_CMAC_256:
            fhost_print(RTOS_TASK_NULL, "BIP_CMAC_256");
            break;
        default:
            break;
    }
}

int fhost_ipc_read_akm(char *akm_str, uint32_t *akm, bool show_error)
{
    char *next_akm, *cur_akm = akm_str;
    int ret = 0;

    fhost_ipc_upper(akm_str, NULL);
    next_akm = strchr(cur_akm, ',');

    *akm = 0;
    while (cur_akm)
    {
        if (strncmp(cur_akm, "OPEN", 4) == 0)
        {
            *akm |= CO_BIT(MAC_AKM_NONE);
        }
        else if (strncmp(cur_akm, "WEP", 3) == 0)
        {
            *akm |= CO_BIT(MAC_AKM_PRE_RSN);
        }
        else if ((strncmp(cur_akm, "RSN", 3) == 0) ||
                 (strncmp(cur_akm, "WPA2", 4) == 0))
        {
            *akm |= CO_BIT(MAC_AKM_PSK) | CO_BIT(MAC_AKM_PSK_SHA256);
        }
        else if ((strncmp(cur_akm, "SAE", 3) == 0) ||
                 (strncmp(cur_akm, "WPA3", 4) == 0))
        {
            *akm |= CO_BIT(MAC_AKM_SAE);
        }
        else if (strncmp(cur_akm, "WPA", 4) == 0)
        {
            *akm |= CO_BIT(MAC_AKM_PRE_RSN) | CO_BIT(MAC_AKM_PSK);
        }
        else
        {
            ret++;
        }

        cur_akm = next_akm;
        if (cur_akm)
        {
            cur_akm++; //skip coma
            next_akm = strchr(cur_akm, ',');
        }
    }

    if (ret && show_error)
    {
        fhost_print(RTOS_TASK_NULL, "[%s] Only the following AKM are supported:\n"
                    "OPEN: For open AP\n"
#if 0 // not supported now
                    "WEP: For AP with WEP security\n"
#endif
                    "WPA: For AP with WPA/PSK security (pre WPA2)\n"
                    "RSN (or WPA2): For AP with WPA2/PSK security\n"
                    "SAE (or WPA3): For AP with WPA3/PSK security\n"
                    , akm_str);
    }
    return ret;
}

int fhost_ipc_read_cipher(char *cipher_str, uint32_t *cipher, bool show_error)
{
    char *next_cipher, *cur_cipher = cipher_str;
    int ret = 0;

    fhost_ipc_upper(cipher_str, NULL);
    next_cipher = strchr(cur_cipher, ',');

    *cipher = 0;
    while (cur_cipher)
    {
        if (strncmp(cur_cipher, "CCMP-256", 8) == 0)
        {
            *cipher |= CO_BIT(MAC_CIPHER_CCMP_256);
        }
        else if (strncmp(cur_cipher, "CCMP", 4) == 0)
        {
            *cipher |= CO_BIT(MAC_CIPHER_CCMP);
        }
        else if (strncmp(cur_cipher, "TKIP", 4) == 0)
        {
            *cipher |= CO_BIT(MAC_CIPHER_TKIP);
        }
        else if (strncmp(cur_cipher, "WEP40", 5) == 0)
        {
            *cipher |= CO_BIT(MAC_CIPHER_WEP40);
        }
        else if (strncmp(cur_cipher, "WEP104", 6) == 0)
        {
            *cipher |= CO_BIT(MAC_CIPHER_WEP104);
        }
        else if (strncmp(cur_cipher, "SMS4", 4) == 0)
        {
            *cipher |= CO_BIT(MAC_CIPHER_WPI_SMS4);
        }
        else if (strncmp(cur_cipher, "GCMP-256", 8) == 0)
        {
            *cipher |= CO_BIT(MAC_CIPHER_GCMP_256);
        }
        else if (strncmp(cur_cipher, "GCMP", 4) == 0)
        {
            *cipher |= CO_BIT(MAC_CIPHER_GCMP_128);
        }
        else
        {
            ret++;
        }

        cur_cipher = next_cipher;
        if (cur_cipher)
        {
            cur_cipher++; //skip coma
            next_cipher = strchr(cur_cipher, ',');
        }
    }

    if (ret && show_error)
    {
        fhost_print(RTOS_TASK_NULL, "[%s] Only the following cipher suites are supported:\n"
                    "CCMP, TKIP, WEP40, WEP104, SMS4, CCMP-256, GCMP, GCMP-256", cipher_str);
    }

    return ret;
}

int fhost_ipc_read_mode(char *mode_str, enum fhost_ap_mode *mode, bool show_error)
{
    int ret = 0;

    fhost_ipc_upper(mode_str, NULL);

    if (strncmp(mode_str, "HT", 2) == 0)
    {
        *mode = FHOST_AP_HT;
    }
    else if (strncmp(mode_str, "VHT", 3) == 0)
    {
        *mode = FHOST_AP_VHT;
    }
    else if (strncmp(mode_str, "HE", 2) == 0)
    {
        *mode = FHOST_AP_HE;
    }
    else if (strncmp(mode_str, "NON-HT", 6) == 0)
    {
        *mode = FHOST_AP_NON_HT;
    }
    else if (strncmp(mode_str, "AUTO", 4) == 0)
    {
        *mode = FHOST_AP_AUTO;
    }
    else
    {
        if (show_error)
            fhost_print(RTOS_TASK_NULL, "[%s] Only the following mode are supported:\n"
                        "AUTO, NON-HT(11g/11a), HT(11n), VHT(11ac), HE(11ax)\n", mode_str);
        ret = -1;
    }
    return ret;
}

int fhost_search_itf(char *name)
{
    int fhost_vif_idx = fhost_vif_idx_from_name(name);

    if (fhost_vif_idx >= 0)
        return fhost_vif_idx;

    fhost_print(RTOS_TASK_NULL, "Cannot find interface %s\n", name);
    return -1;
 }

int fhost_search_first_valid_itf()
{
    struct fhost_status status;
    fhost_get_status(&status);

    if (status.vif_active_cnt)
        return status.vif_first_active;

    fhost_print(RTOS_TASK_NULL, "No active interface\n");
    return -1;
 }

// akm_str must be all in upper case
int fhost_read_akm(const char *akm_str, uint32_t *akm, bool show_error)
{
    const char *next_akm, *cur_akm = akm_str;
    int ret = 0;

    next_akm = strchr(cur_akm, ',');

    *akm = 0;
    while (cur_akm)
    {
        if (strncmp(cur_akm, "OPEN", 4) == 0)
        {
            *akm |= CO_BIT(MAC_AKM_NONE);
        }
        else if (strncmp(cur_akm, "WEP", 3) == 0)
        {
            *akm |= CO_BIT(MAC_AKM_PRE_RSN);
        }
        else if ((strncmp(cur_akm, "RSN", 3) == 0) ||
                 (strncmp(cur_akm, "WPA2", 4) == 0))
        {
            *akm |= CO_BIT(MAC_AKM_PSK) | CO_BIT(MAC_AKM_PSK_SHA256);
        }
        else if ((strncmp(cur_akm, "SAE", 3) == 0) ||
                 (strncmp(cur_akm, "WPA3", 4) == 0))
        {
            *akm |= CO_BIT(MAC_AKM_SAE);
        }
        else if (strncmp(cur_akm, "WPA", 4) == 0)
        {
            *akm |= CO_BIT(MAC_AKM_PRE_RSN) | CO_BIT(MAC_AKM_PSK);
        }
        else
        {
            ret++;
        }

        cur_akm = next_akm;
        if (cur_akm)
        {
            cur_akm++; //skip coma
            next_akm = strchr(cur_akm, ',');
        }
    }

    if (ret && show_error)
    {
        fhost_print(RTOS_TASK_NULL, "[%s] Only the following AKM are supported:\n"
                    "OPEN: For open AP\n"
#if 0 // not supported now
                    "WEP: For AP with WEP security\n"
#endif
                    "WPA: For AP with WPA/PSK security (pre WPA2)\n"
                    "RSN (or WPA2): For AP with WPA2/PSK security\n"
                    "SAE (or WPA3): For AP with WPA3/PSK security\n"
                    , akm_str);
    }
    return ret;
}

/// Command message ID associated to RTOS Task handle
struct fhost_task_msg_id {
    /// task handle
    rtos_task_handle task_handle;
    /// sigkill callback
    fhost_ipc_sigkill_cb sigkill_cb;
    /// message id
    uint32_t msg_id;
};
/// Length of @ref task_handle_msg_id
#define FHOST_TASK_HANDLE_LEN 5
static struct fhost_task_msg_id task_handle_msg_id[FHOST_TASK_HANDLE_LEN];

struct fhost_task_msg_id *fhost_search_task_hdl_msg(rtos_task_handle handle)
{
    int i;
    struct fhost_task_msg_id *ret = NULL;

    for (i = 0; i < FHOST_TASK_HANDLE_LEN; i++)
    {
        if (task_handle_msg_id[i].task_handle == handle)
            ret = &task_handle_msg_id[i];
    }

    return ret;
}

/**
 ****************************************************************************************
 * @brief Search a free fhost_task_msg_id in task_handle_msg_id
 *
 * @return pointer to fhost_task_msg_id on success, NULL if error.
 ****************************************************************************************
 */
static struct fhost_task_msg_id *fhost_search_free_task_hdl_msg()
{
    return fhost_search_task_hdl_msg(RTOS_TASK_NULL);
}

/**
 ****************************************************************************************
 * @brief Start an ipc task saving the task handle.
 *
 * @param[in] msg_id            Message ID
 * @param[in] start_task_func   Function that starts the ipc task
 * @param[in] start_task_args   Argument pointer to pass to start_task_func
 * @param[in] sigkill_func      Callback for SIGKILL signal
 *
 * @return FHOST_IPC_NO_RESP if successful, FHOST_IPC_ERROR otherwise.
 ****************************************************************************************
 */
int fhost_ipc_start_task(int msg_id, fhost_ipc_start_task_cb start_task_func,
                                void *start_task_args, fhost_ipc_sigkill_cb sigkill_func)
{
    struct fhost_task_msg_id *task_hdl_msg_ptr = fhost_search_free_task_hdl_msg();
    if (!task_hdl_msg_ptr)
        return FHOST_IPC_ERROR;

    if ((task_hdl_msg_ptr->task_handle = start_task_func(start_task_args)))
    {
        task_hdl_msg_ptr->msg_id = msg_id;
        task_hdl_msg_ptr->sigkill_cb = sigkill_func;
    }
    else
    {
        return FHOST_IPC_ERROR;
    }

    return FHOST_IPC_NO_RESP;
}

int fhost_vif_is_up(int fhost_vif_idx)
{
    void *mac_vif = fhost_to_mac_vif(fhost_vif_idx);
    if (mac_vif && (mac_vif_get_type(mac_vif) != VIF_UNKNOWN))
        return 1;
    else
        return 0;
}

/// @}

