/*
 * IPRO SDK Zmodem file receiver: the receive state machine over the
 * library's protocol primitives. Header replies are not checked.
 */
#include <stdbool.h>
#include <stdint.h>

#include "zmodem.h"
#include "zreceive.h"

/* ZRINIT with a receive window of buf_len rounded down to 1 KB */
static void zm_rx_send_rinit(uint16_t buf_len)
{
    uint16_t win = (uint16_t)(buf_len & ~0x3FFu);

    zm_send_flags_hdr(ZRINIT, CANOVIO | CANFC32, 0,
                      (uint8_t)(win >> 8), (uint8_t)win);
}

/* Session abort as lrzsz canit(): 8 CAN, 8 backspace */
static void zm_rx_cancel(void)
{
    for (int i = 0; i < 16; i++)
        zm_send(i < 8 ? CAN : 0x08);
}

/* Header position, wire order: p0 is the low byte */
static uint32_t zm_rx_hdr_pos(const ZHDR *hdr)
{
    return (uint32_t)hdr->position.p0 |
           ((uint32_t)hdr->position.p1 << 8) |
           ((uint32_t)hdr->position.p2 << 16) |
           ((uint32_t)hdr->position.p3 << 24);
}

int zm_receive_file(uint8_t *buf, uint16_t buf_len,
                    const struct zm_rx_sink *sink)
{
    uint8_t  rzr_buf[4];
    uint16_t result, count;
    bool eof = false;
    ZHDR hdr;

    /* lrzsz `sz` auto-starts the remote receiver by sending "rz\r" first */
    if (zm_await("rz\r", (char *)rzr_buf, sizeof(rzr_buf)) != OK)
        return ZM_RX_NO_START;

    while (true) {
        result = zm_await_header(&hdr);
        if (result == CANCELLED || result == CLOSED)
            return ZM_RX_CLOSED;
        if (result == BAD_FRAME_TYPE)   /* data noise while resyncing        */
            continue;
        if (IS_ERROR(result)) {         /* damaged header -> ask to resend   */
            zm_send_pos_hdr(ZNAK, sink->pos(sink->ctx));
            continue;
        }

        switch (hdr.type) {
        case ZRQINIT:
            zm_rx_send_rinit(buf_len);
            continue;

        case ZSINIT:
            /* Attn subpacket: read and dropped */
            count  = buf_len;
            result = zm_read_data_block(buf, &count);
            if (result == CANCELLED || result == CLOSED)
                return ZM_RX_CLOSED;
            zm_send_pos_hdr(IS_ERROR(result) ? ZNAK : ZACK,
                            sink->pos(sink->ctx));
            continue;

        case ZFILE:
            /* filename subpacket: read and dropped. One file only: a
             * new file after data or ZEOF gets ZSKIP */
            count  = buf_len;
            result = zm_read_data_block(buf, &count);
            if (result == CANCELLED || result == CLOSED)
                return ZM_RX_CLOSED;
            if (IS_ERROR(result))
                zm_send_pos_hdr(ZNAK, sink->pos(sink->ctx));
            else
                zm_send_pos_hdr(eof || sink->pos(sink->ctx) != 0 ? ZSKIP
                                                                 : ZRPOS,
                                sink->pos(sink->ctx));
            continue;

        case ZDATA:
            /* data from another offset: skipped, resume from ours */
            if (zm_rx_hdr_pos(&hdr) != sink->pos(sink->ctx)) {
                zm_send_pos_hdr(ZRPOS, sink->pos(sink->ctx));
                continue;
            }
            while (true) {
                count  = buf_len;
                result = zm_read_data_block(buf, &count);
                if (result == CANCELLED || result == CLOSED)
                    return ZM_RX_CLOSED;
                if (IS_ERROR(result)) {
                    /* bad subpacket: ask sender to resume from our position */
                    zm_send_pos_hdr(ZRPOS, sink->pos(sink->ctx));
                    break;
                }
                /* count includes a trailing framing byte -> data is count-1 */
                if (count > 0 &&
                    sink->write(sink->ctx, buf, count - 1u) != 0) {
                    zm_rx_cancel();
                    return ZM_RX_SINK;
                }

                if (result == GOT_CRCE) {
                    break;                  /* end of frame, header follows  */
                } else if (result == GOT_CRCG) {
                    continue;               /* streaming, more data follows  */
                } else if (result == GOT_CRCQ || result == GOT_CRCW) {
                    zm_send_pos_hdr(ZACK, sink->pos(sink->ctx));
                    if (result == GOT_CRCW)
                        break;
                }
            }
            continue;

        case ZEOF:
            /* end at another offset: resume from ours */
            if (zm_rx_hdr_pos(&hdr) != sink->pos(sink->ctx)) {
                zm_send_pos_hdr(ZRPOS, sink->pos(sink->ctx));
                continue;
            }
            eof = true;
            zm_rx_send_rinit(buf_len);
            continue;

        case ZFIN:
            zm_send_pos_hdr(ZFIN, 0);
            return eof ? ZM_RX_OK : ZM_RX_SHORT;

        default:
            continue;                       /* ignore unknown header types   */
        }
    }
}
