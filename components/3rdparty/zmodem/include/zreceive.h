/*
 * IPRO SDK Zmodem file receiver on the zm_recv/zm_send port: waits for
 * the sender's "rz\r", then takes one file and hands its data, in order,
 * to a caller sink. Silent: no printf, the wire stays byte-clean.
 */

#ifndef __ZRECEIVE_H
#define __ZRECEIVE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ZM_RX_OK          0
#define ZM_RX_NO_START   -1     /* no "rz\r" before the port timeout   */
#define ZM_RX_CLOSED     -2     /* sender cancelled, or idle timeout   */
#define ZM_RX_SHORT      -3     /* ZFIN before ZEOF at our offset      */
#define ZM_RX_SINK       -4     /* sink refused data, sender cancelled */

struct zm_rx_sink {
    /* Appends len bytes; nonzero stops the transfer (ZM_RX_SINK) */
    int (*write)(void *ctx, const uint8_t *data, uint32_t len);
    /* Bytes taken so far: the ZRPOS / ZACK position */
    uint32_t (*pos)(void *ctx);
    void *ctx;
};

/*
 * Receives one file until ZFIN. buf holds one data subpacket plus its
 * framing byte, so buf_len must exceed the sender's subpacket size.
 */
int zm_receive_file(uint8_t *buf, uint16_t buf_len,
                    const struct zm_rx_sink *sink);

#ifdef __cplusplus
}
#endif

#endif /* __ZRECEIVE_H */
