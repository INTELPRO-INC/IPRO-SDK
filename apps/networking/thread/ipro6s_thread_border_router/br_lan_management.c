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

#include <generated/autoconf.h>
#include "br_lan_management.h"
#include "br_management.h"
#include "br_wifi_profile.h"
#include "br_ble_protocol.h"
#include "br_ble_management.h"
#include "br_factory_reset.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lwip/sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/sha256.h"
#include "easyflash.h"
#include "ipro_log.h"
#include <ipro_matter_wifi.h>
#include <openthread/mdns.h>
#include <string.h>
#include <stdio.h>

#define PORT 49154
#if defined(CONFIG_SHELL) && LWIP_TCP && LWIP_TCPIP_CORE_LOCKING
#include "shell.h"
#include "lwip/tcpip.h"
#include "lwip/priv/tcp_priv.h"

/* Copy scalar metadata under the core lock; never retain PCB pointers or log
 * while holding the lock. Restrict observation to the management listener. */
static void cmd_lan_tcp(int argc, char **argv)
{
    (void)argc; (void)argv;
    struct snapshot {
        uint32_t receive_next, send_next, acknowledged;
        uint16_t flags;
        uint8_t state, retries, unsent, unacked, refused;
    } samples[4];
    unsigned count = 0, omitted = 0;
    LOCK_TCPIP_CORE();
    for (const struct tcp_pcb *p = tcp_active_pcbs; p; p = p->next) {
        if (p->local_port != PORT) continue;
        if (count == sizeof(samples) / sizeof(samples[0])) {
            ++omitted;
            continue;
        }
        samples[count++] = (struct snapshot) {
            .receive_next = p->rcv_nxt, .send_next = p->snd_nxt,
            .acknowledged = p->lastack, .flags = p->flags,
            .state = p->state, .retries = p->nrtx,
            .unsent = p->unsent != NULL, .unacked = p->unacked != NULL,
            .refused = p->refused_data != NULL,
        };
    }
    UNLOCK_TCPIP_CORE();
    IPRO_LOGI("BR-LAN-TCP", "active=%u omitted=%u", count, omitted);
    for (unsigned i = 0; i < count; ++i) {
        const struct snapshot *s = &samples[i];
        IPRO_LOGI("BR-LAN-TCP", "slot=%u state=%u flags=%u rcv_nxt=%lu snd_nxt=%lu lastack=%lu retries=%u unsent=%u unacked=%u refused=%u",
                  i, s->state, s->flags, (unsigned long)s->receive_next,
                  (unsigned long)s->send_next, (unsigned long)s->acknowledged,
                  s->retries, s->unsent, s->unacked, s->refused);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_lan_tcp, br_lan_tcp, Snapshot management TCP metadata);
#endif
#define TAG "BR-LAN"
/* Certificate and key are device generated. Persist as one checked record.
 * No shared build key, plaintext network credentials, or TLS trust bypass. */
struct identity {
    uint32_t magic;
    uint16_t cert_len, key_len;
    uint8_t cert[512], key[160], token[32];
};
/* Allocate TLS state after boot networking, keeping its startup heap headroom. */
struct tls_state {
    struct identity identity;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context random;
    mbedtls_pk_context key;
    mbedtls_x509_crt cert;
    mbedtls_ssl_config config;
};
static struct tls_state *sTls;
#define sIdentity (sTls->identity)
#define sEntropy (sTls->entropy)
#define sRandom (sTls->random)
#define sKey (sTls->key)
#define sCert (sTls->cert)
#define sConfig (sTls->config)
static uint8_t sExport[70];
static bool sReady;
static char sName[40];
static const uint8_t sOwner;
static bool sPublished, sPublishing;
static uint32_t sRequest;
static TickType_t sPublishRetry;

static int prepare_identity(void)
{
    mbedtls_entropy_init(&sEntropy);
    mbedtls_ctr_drbg_init(&sRandom);
    mbedtls_pk_init(&sKey);
    mbedtls_x509_crt_init(&sCert);
    mbedtls_ssl_config_init(&sConfig);
    int rc = mbedtls_ctr_drbg_seed(&sRandom, mbedtls_entropy_func, &sEntropy,
                                 (const uint8_t *)"ipro-br-lan-v1", 14);
    if (rc) return rc;
    size_t copied = 0, total = 0;
    rc = ef_env_get_checked(BR_LAN_KEY, 0, &sIdentity, sizeof(sIdentity), &copied, &total);
    if (rc == EF_ENV_NOT_FOUND) {
        mbedtls_x509write_cert writer;
        mbedtls_mpi serial;
        mbedtls_x509write_crt_init(&writer);
        mbedtls_mpi_init(&serial);
        sIdentity.magic = 0x314e4c42;
        rc = mbedtls_pk_setup(&sKey, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
        if (!rc) rc = mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1, mbedtls_pk_ec(sKey), mbedtls_ctr_drbg_random, &sRandom);
        if (!rc) rc = mbedtls_ctr_drbg_random(&sRandom, sIdentity.token, sizeof(sIdentity.token));
        /* The certificate serial is public and must not reveal token bytes. */
        uint8_t serial_bytes[16];
        if (!rc) rc = mbedtls_ctr_drbg_random(&sRandom, serial_bytes, sizeof(serial_bytes));
        if (!rc) {
            serial_bytes[0] = (serial_bytes[0] & 0x7f) | 1;
            rc = mbedtls_mpi_read_binary(&serial, serial_bytes, sizeof(serial_bytes));
        }
        br_wifi_profile_wipe(serial_bytes, sizeof(serial_bytes));
        mbedtls_x509write_crt_set_version(&writer, MBEDTLS_X509_CRT_VERSION_3);
        mbedtls_x509write_crt_set_md_alg(&writer, MBEDTLS_MD_SHA256);
        mbedtls_x509write_crt_set_subject_key(&writer, &sKey);
        mbedtls_x509write_crt_set_issuer_key(&writer, &sKey);
        if (!rc) rc = mbedtls_x509write_crt_set_serial(&writer, &serial);
        if (!rc) rc = mbedtls_x509write_crt_set_subject_name(&writer, "CN=IPRO BR LAN");
        if (!rc) rc = mbedtls_x509write_crt_set_issuer_name(&writer, "CN=IPRO BR LAN");
        if (!rc) rc = mbedtls_x509write_crt_set_validity(&writer, "20260101000000", "20491231235959");
        if (!rc) rc = mbedtls_x509write_crt_set_basic_constraints(&writer, 0, -1);
        if (!rc) rc = mbedtls_x509write_crt_set_key_usage(&writer, MBEDTLS_X509_KU_DIGITAL_SIGNATURE);
        if (!rc) {
            rc = mbedtls_x509write_crt_der(&writer, sIdentity.cert, sizeof(sIdentity.cert), mbedtls_ctr_drbg_random, &sRandom);
            if (rc > 0) {
                sIdentity.cert_len = rc;
                memmove(sIdentity.cert, sIdentity.cert + sizeof(sIdentity.cert) - rc, rc);
                rc = mbedtls_pk_write_key_der(&sKey, sIdentity.key, sizeof(sIdentity.key));
                if (rc > 0) {
                    sIdentity.key_len = rc;
                    memmove(sIdentity.key, sIdentity.key + sizeof(sIdentity.key) - rc, rc);
                    rc = ef_env_set_checked(BR_LAN_KEY, &sIdentity, sizeof(sIdentity));
                }
            }
        }
        mbedtls_mpi_free(&serial);
        mbedtls_x509write_crt_free(&writer);
        if (rc) return rc;
    } else {
        if (rc || copied != sizeof(sIdentity) || total != sizeof(sIdentity) || sIdentity.magic != 0x314e4c42 ||
            !sIdentity.key_len || sIdentity.key_len > sizeof(sIdentity.key) ||
            !sIdentity.cert_len || sIdentity.cert_len > sizeof(sIdentity.cert)) return -1;
        rc = mbedtls_pk_parse_key(&sKey, sIdentity.key, sIdentity.key_len, NULL, 0);
        if (rc) return rc;
    }
    rc = mbedtls_x509_crt_parse_der(&sCert, sIdentity.cert, sIdentity.cert_len);
    if (!rc) rc = mbedtls_pk_check_pair(&sCert.pk, &sKey);
    if (!rc) rc = ipro_wifi_get_station_mac(sExport);
    if (!rc) rc = mbedtls_sha256_ret(sIdentity.cert, sIdentity.cert_len, sExport + 6, 0);
    if (!rc) memcpy(sExport + 38, sIdentity.token, 32);
    if (!rc) rc = mbedtls_ssl_config_defaults(&sConfig, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    mbedtls_ssl_conf_rng(&sConfig, mbedtls_ctr_drbg_random, &sRandom);
    mbedtls_ssl_conf_min_version(&sConfig, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
    mbedtls_ssl_conf_max_version(&sConfig, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
    static const int suites[] = { MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256, 0 };
    mbedtls_ssl_conf_ciphersuites(&sConfig, suites);
    /* Client authorization is a device-specific secret inside pinned TLS. */
    mbedtls_ssl_conf_authmode(&sConfig, MBEDTLS_SSL_VERIFY_NONE);
    if (!rc) rc = mbedtls_ssl_conf_own_cert(&sConfig, &sCert, &sKey);
    br_wifi_profile_wipe(sIdentity.key, sizeof(sIdentity.key));
    return rc;
}

int br_lan_credentials(uint8_t output[70])
{
    taskENTER_CRITICAL();
    bool ready = sReady;
    if (ready) memcpy(output, sExport, sizeof(sExport));
    taskEXIT_CRITICAL();
    return ready ? 0 : -1;
}

struct io_context {
    int fd;
    TickType_t deadline;
    TickType_t recv_ticks, send_ticks;
    unsigned recv_calls, send_calls;
};
static bool io_budget(struct io_context *io, int option)
{
    int32_t ticks = (int32_t)(io->deadline - xTaskGetTickCount());
    if (ticks <= 0) return false;
    unsigned ms = (unsigned)ticks * portTICK_PERIOD_MS;
    struct timeval timeout = {.tv_sec = ms / 1000, .tv_usec = (ms % 1000) * 1000};
    return lwip_setsockopt(io->fd, SOL_SOCKET, option, &timeout, sizeof(timeout)) == 0;
}
static int send_tls(void *ctx, const unsigned char *buf, size_t len)
{
    struct io_context *io = ctx;
    if (!io_budget(io, SO_SNDTIMEO)) return MBEDTLS_ERR_SSL_TIMEOUT;
    TickType_t start = xTaskGetTickCount();
    int n = lwip_send(io->fd, buf, len, 0);
    io->send_ticks += xTaskGetTickCount() - start;
    ++io->send_calls;
    return n > 0 ? n : MBEDTLS_ERR_SSL_INTERNAL_ERROR;
}
static int recv_tls(void *ctx, unsigned char *buf, size_t len)
{
    struct io_context *io = ctx;
    if (!io_budget(io, SO_RCVTIMEO)) return MBEDTLS_ERR_SSL_TIMEOUT;
    TickType_t start = xTaskGetTickCount();
    int n = lwip_recv(io->fd, buf, len, 0);
    io->recv_ticks += xTaskGetTickCount() - start;
    ++io->recv_calls;
    return n >= 0 ? n : MBEDTLS_ERR_SSL_TIMEOUT;
}
static bool transfer(mbedtls_ssl_context *ssl, uint8_t *data, size_t size, bool sending)
{
    while (size) {
        int n = sending ? mbedtls_ssl_write(ssl, data, size) : mbedtls_ssl_read(ssl, data, size);
        if (n <= 0) return false;
        data += n; size -= (size_t)n;
    }
    return true;
}
static void session(int fd)
{
    mbedtls_ssl_context ssl;
    mbedtls_ssl_init(&ssl);
    uint8_t token[32] = {0}, input[BR_BLE_MAX_PAYLOAD] = {0}, output[BR_BLE_MAX_PAYLOAD] = {0};
    bool reboot = false;
    if (mbedtls_ssl_setup(&ssl, &sConfig)) goto done;
    struct io_context io = {.fd = fd, .deadline = xTaskGetTickCount() + pdMS_TO_TICKS(10000)};
    mbedtls_ssl_set_bio(&ssl, &io, send_tls, recv_tls, NULL);
    TickType_t handshake_start = xTaskGetTickCount();
    int handshake_rc = mbedtls_ssl_handshake(&ssl);
    /* Fixed numeric timing only: never log peer identity, pin or token.
     * Socket-call time includes waiting and copying; the remainder includes
     * crypto and scheduling, and must not be labelled pure CPU time. */
    IPRO_LOGI(TAG, "TLS handshake rc=%d total_ms=%u recv_ms=%u send_ms=%u recv_calls=%u send_calls=%u",
              handshake_rc, (unsigned)((xTaskGetTickCount() - handshake_start) * portTICK_PERIOD_MS),
              (unsigned)(io.recv_ticks * portTICK_PERIOD_MS),
              (unsigned)(io.send_ticks * portTICK_PERIOD_MS), io.recv_calls, io.send_calls);
    if (handshake_rc || !transfer(&ssl, token, sizeof(token), false)) goto done;
    volatile unsigned mismatch = 0;
    for (unsigned i = 0; i < sizeof(token); ++i) mismatch |= token[i] ^ sIdentity.token[i];
    br_wifi_profile_wipe(token, sizeof(token));
    if (mismatch) goto done;
    uint8_t accepted = 1;
    if (!transfer(&ssl, &accepted, 1, true)) goto done;
    for (unsigned count = 0; count < 128; ++count) {
        io.deadline = xTaskGetTickCount() + pdMS_TO_TICKS(8000);
        uint8_t header[4];
        if (!transfer(&ssl, header, sizeof(header), false)) break;
        size_t size = ((size_t)header[2] << 8) | header[3];
        if (header[0] != 1 || !br_lan_opcode_allowed(header[1]) || size > sizeof(input)) break;
        if (!transfer(&ssl, input, size, false)) break;
        size_t length = br_management_execute(header[1], input, size, output, &reboot);
        br_wifi_profile_wipe(input, sizeof(input));
        header[1] |= 0x80; header[2] = length >> 8; header[3] = length;
        bool sent = transfer(&ssl, header, sizeof(header), true) && transfer(&ssl, output, length, true);
        br_wifi_profile_wipe(output, sizeof(output));
        /* A committed factory reset reboots even when its reply was lost. */
        if (!sent) { reboot = reboot && br_factory_reset_pending(); break; }
        if (reboot) break;
    }
done:
    br_wifi_profile_wipe(token, sizeof(token));
    br_wifi_profile_wipe(input, sizeof(input));
    br_wifi_profile_wipe(output, sizeof(output));
    mbedtls_ssl_free(&ssl);
    if (reboot) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        br_ble_management_quiesce();
        extern void hal_reboot(void);
        hal_reboot();
    }
}
static void worker(void *arg)
{
    (void)arg;
    ipro_wifi_connectivity_snapshot wifi;
    while (ipro_wifi_connectivity_read(&wifi) || !wifi.ready) vTaskDelay(pdMS_TO_TICKS(1000));
    sTls = pvPortCalloc(1, sizeof(*sTls));
    if (!sTls) { IPRO_LOGE(TAG, "TLS state allocation failed"); vTaskDelete(NULL); return; }
    int rc = prepare_identity();
    if (rc) { IPRO_LOGE(TAG, "Identity unavailable error=%d", rc); goto failed; }
    snprintf(sName, sizeof(sName), "IPRO-BR-%02X%02X%02X%02X%02X%02X", sExport[0], sExport[1], sExport[2], sExport[3], sExport[4], sExport[5]);
    int listener = lwip_socket(AF_INET6, SOCK_STREAM, 0);
    struct sockaddr_in6 address = { .sin6_family = AF_INET6, .sin6_port = PP_HTONS(PORT) };
    int dual = 0;
    if (listener < 0 || lwip_setsockopt(listener, IPPROTO_IPV6, IPV6_V6ONLY, &dual, sizeof(dual)) ||
        lwip_bind(listener, (struct sockaddr *)&address, sizeof(address)) || lwip_listen(listener, 1)) {
        if (listener >= 0) lwip_close(listener);
        IPRO_LOGE(TAG, "Listener unavailable"); goto failed;
    }
    taskENTER_CRITICAL(); sReady = true; taskEXIT_CRITICAL();
    IPRO_LOGI(TAG, "TLS management ready port=%u heap=%u stack_free_words=%u", PORT, (unsigned)xPortGetFreeHeapSize(), (unsigned)uxTaskGetStackHighWaterMark(NULL));
    for (;;) {
        int fd = lwip_accept(listener, NULL, NULL);
        if (fd < 0) { vTaskDelay(pdMS_TO_TICKS(1000)); continue; }
        IPRO_LOGI(TAG, "TCP accepted");
        struct timeval timeout = {.tv_sec = 8, .tv_usec = 0};
        lwip_setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        lwip_setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
        session(fd);
        lwip_close(fd);
        IPRO_LOGI(TAG, "Session closed heap=%u stack_free_words=%u", (unsigned)xPortGetFreeHeapSize(), (unsigned)uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(pdMS_TO_TICKS(100));
    }
failed:
    mbedtls_ssl_config_free(&sConfig);
    mbedtls_x509_crt_free(&sCert);
    mbedtls_pk_free(&sKey);
    mbedtls_ctr_drbg_free(&sRandom);
    mbedtls_entropy_free(&sEntropy);
    br_wifi_profile_wipe(sTls, sizeof(*sTls));
    vPortFree(sTls); sTls = NULL;
    br_wifi_profile_wipe(sExport, sizeof(sExport));
    vTaskDelete(NULL);
}
int br_lan_start(void) { return xTaskCreate(worker, "br_lan", 2048, NULL, 5, NULL) == pdPASS ? 0 : -1; }

static void published(otInstance *instance, otMdnsRequestId id, otError error)
{
    (void)instance;
    if (id != sRequest) return;
    sPublishing = false; sPublished = error == OT_ERROR_NONE;
    IPRO_LOGI(TAG, "mDNS registration error=%u", error);
}
void br_lan_publish(otInstance *instance, bool running)
{
    if (!running || !otMdnsIsEnabled(instance)) {
        if (sPublished || sPublishing) ++sRequest;
        sPublished = sPublishing = false; return;
    }
    taskENTER_CRITICAL(); bool ready = sReady; taskEXIT_CRITICAL();
    if (!ready || sPublished || sPublishing || (int32_t)(xTaskGetTickCount() - sPublishRetry) < 0) return;
    sPublishRetry = xTaskGetTickCount() + pdMS_TO_TICKS(1000);
    otMdnsService service = {0};
    service.mServiceInstance = sName; service.mServiceType = "_ipro-br._tcp";
    service.mPort = PORT;
    static const uint8_t txt[] = {3, 'v', '=', '1'};
    service.mTxtData = txt; service.mTxtDataLength = sizeof(txt);
    sPublishing = true;
    otError error = otMdnsRegisterServiceOwned(instance, &service, ++sRequest, published, &sOwner);
    if (error) sPublishing = false;
}
