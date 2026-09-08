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

/**
 * @file bt_cli.c
 * @brief `bt` shell command for the BLE / LE Audio bring-up.
 */

#include <generated/autoconf.h>

#if defined(CONFIG_BLUETOOTH_LE_ENABLE) && defined(CONFIG_SHELL)

#include "bt.h"
#include "bt_audio_out.h"
#include "audio.h"

#include "reg_rf.h"   /* RF_BASE */
#include "rf_reg.h"   /* RF_CAL_STATUS_* */
#include "ipro7_rf_private.h"

extern rf_calibration_data_tag rf_calibration_data;
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(void)
{
    printf(
      "bt - BLE with the new controller + Zephyr 4.4 host, LE Audio sink\n"
      "  bt init                 enable the controller and host\n"
      "  bt status               link state, address, LE Audio stream\n"
      "  bt adv on|off           connectable advertising\n"
      "  bt name <string>        advertised name (next adv start)\n"
      "  bt disconnect           drop the current link\n"
      "  bt audio                LE Audio stream state, decode and rate stats\n"
      "  bt sink [dac|uac]       where decoded PCM goes. uac makes the board\n"
      "                          a 48 kHz USB mic the host can record or\n"
      "                          monitor - and at 48 kHz it is a straight\n"
      "                          passthrough, no FIR and no rate matcher.\n"
      "                          Set it before the phone starts the stream.\n"
      "  bt dacrate [hz]         override the assumed DAC rate (0 = auto)\n"
      "  bt rf                   RF calibration status for THIS boot\n"
      "  bt vol <0-255>          drive the VCP volume locally, to tell a\n"
      "                          silent phone slider from a dead volume path\n"
      "  bt capture <sec>        capture decoded PCM, then `mic play` it\n"
      "  bt rate <16|24|32|48|0> publish only this LC3 rate (0 = all).\n"
      "                          16 takes the FIR and resampler out of the\n"
      "                          path entirely - use it to rule them out.\n");
}

static void print_status(void)
{
    bt_status_t st;

    bt_app_get_status(&st);

    printf("host    : %s\n", st.enabled ? "ready" : "NOT enabled (run `bt init`)");
    if (st.enabled) {
        printf("address : %02X:%02X:%02X:%02X:%02X:%02X\n",
               st.own_addr[5], st.own_addr[4], st.own_addr[3],
               st.own_addr[2], st.own_addr[1], st.own_addr[0]);
    }
    printf("name    : %s\n", bt_app_get_name());
    printf("adv     : %s\n", st.advertising ? "advertising" : "off");

    if (st.connected) {
        printf("link    : connected to %s\n", st.addr);
        if (st.conn_interval) {
            printf("params  : interval %u.%02u ms, latency %u, timeout %u ms\n",
                   (unsigned)(st.conn_interval * 125u / 100u),
                   (unsigned)((st.conn_interval * 125u) % 100u),
                   st.conn_latency, st.conn_timeout * 10u);
        }
    } else {
        printf("link    : not connected\n");
    }
#if defined(CONFIG_BT_AUDIO)
    printf("audio   : stream %s\n", bt_app_stream_state_name(st.stream));
#else
    printf("audio   : not built in (CONFIG_BT_AUDIO off)\n");
#endif
}

/*
 * RF calibration status for this boot.
 *
 * roscal is the receive DC-offset calibration, and its failure path in
 * ipro7_rf_private.c is broken by design: the abort is commented out
 * (`//break;` at :1233) and so is the "only store on success" guard
 * (:1253-1256), so when the search gives up the DAC value it happened to stop
 * on is written into the calibration table anyway. Worse, gain index 15 is the
 * anchor that indices 0-14 are copied from, so one failure poisons the whole
 * gain table and the receiver ends up 15-20 dB deaf.
 *
 * It fails on roughly half of cold boots, which is exactly the kind of fault
 * that looks like flaky audio rather than a broken radio - a desensitised
 * receiver just loses packets. Read the status before blaming anything else.
 */
static void print_rf_cal(void)
{
    uint32_t st  = *(volatile uint32_t *)(RF_BASE + RF_CAL_STATUS_OFFSET);
    uint32_t ros = (st >> RF_ROSCAL_STATUS_POS) & 0x3u;
    uint32_t kc  = (st >> RF_KCAL_STATUS_POS)   & 0x3u;
    uint32_t ac  = (st >> RF_ACAL_STATUS_POS)   & 0x3u;
    uint32_t rc  = (st >> RF_RCCAL_STATUS_POS)  & 0x3u;

    printf("RF_CAL_STATUS : 0x%08lX\n", (unsigned long)st);

    if (st == 0u) {
        /*
         * Calibration runs inside controller init, so before `bt init` the
         * status register is empty. Stop here rather than interpreting
         * rf_calibration_data: it lives in a noinit section, so what is in it
         * now is either a previous session's result or power-on garbage, and
         * reporting a verdict from that is worse than reporting nothing.
         */
        printf("\nRF calibration has not run yet - it happens inside the\n"
               "controller start. Run `bt init` first, then `bt rf`.\n");
        return;
    }
    printf("  acal        : %lu\n", (unsigned long)ac);
    printf("  kcal        : %lu\n", (unsigned long)kc);
    printf("  rccal       : %lu\n", (unsigned long)rc);
    printf("  roscal      : %lu  <-- the one that matters\n",
           (unsigned long)ros);

    /*
     * What actually landed in the table. Index 15 is the anchor - 0-14 are
     * copied from it - so this one entry decides the whole gain table.
     *
     * 32/32 means the mid-scale fallback in ipro7_rf_private.c engaged and the
     * part is running uncalibrated but at its power-up default. 0 or 63 means
     * the search pinned the DC-offset DAC to a rail and that got stored, which
     * is a large deliberate offset and much worse than not calibrating at all.
     */
    {
        volatile rosdac_gain_ctrl_tag *a = &rf_calibration_data.rx_offset_data[15];
        int i0 = a->gain_ctrl_rosdac_i_bw0, q0 = a->gain_ctrl_rosdac_q_bw0;
        int i1 = a->gain_ctrl_rosdac_i_bw1, q1 = a->gain_ctrl_rosdac_q_bw1;
        bool railed = (i0 == 0 || i0 == 63 || q0 == 0 || q0 == 63 ||
                       i1 == 0 || i1 == 63 || q1 == 0 || q1 == 63);
        bool mid    = (i0 == 32 && q0 == 32 && i1 == 32 && q1 == 32);

        printf("\nrosdac[15] (the anchor for 0-14):\n");
        printf("  bw0         : i=%d q=%d\n", i0, q0);
        printf("  bw1         : i=%d q=%d\n", i1, q1);
        if (mid) {
            printf("  -> mid-scale: the fallback engaged, so the part is\n"
                   "     uncalibrated but sane. No worse than not calibrating.\n");
        } else if (railed) {
            printf("  -> RAILED: a give-up value was stored as calibration.\n"
                   "     This is the 15-20 dB case.\n");
        } else {
            printf("  -> a converged value.\n");
        }
    }

    if (ros == 3u) {
        printf("\nroscal PASSED on this boot. The receiver is calibrated, so\n"
               "packet loss is not coming from a deaf front end.\n");
    } else if (ros == 2u) {
        printf("\nroscal did not converge on this boot. What that costs\n"
               "depends entirely on the anchor values above:\n"
               "  32/32   the fallback caught it. Uncalibrated, but at the\n"
               "          power-up default - some sensitivity lost, not a lot.\n"
               "  railed  a give-up value became the calibration. That is the\n"
               "          15-20 dB case and worth rebooting out of.\n");
    } else {
        printf("\nroscal status %lu - calibration did not run or did not\n"
               "finish. Treat the radio as uncalibrated.\n",
               (unsigned long)ros);
    }
}

static void print_audio(void)
{
    bt_status_t st;

    bt_app_get_status(&st);

#if defined(CONFIG_BT_AUDIO)
    printf("role    : LE Audio unicast SINK (headset), LC3 16/32/48 kHz\n");
    printf("stream  : %s\n", bt_app_stream_state_name(st.stream));
    printf("rx      : %u frames, %u bytes, %u concealed (%u flagged lost)\n",
           (unsigned)st.rx_frames, (unsigned)st.rx_bytes,
           (unsigned)st.rx_empty, (unsigned)st.rx_flagged);

    if (st.stream == BT_STREAM_STREAMING && st.rx_frames == 0) {
        printf("hint    : the stream started but no LC3 frames have arrived -\n"
               "          the ISO channel is set up and the source is not\n"
               "          sending, or the SDUs are all being dropped.\n");
    }

    {
        bt_audio_out_status_t ao;

        bt_audio_out_get_status(&ao);

        if (!ao.running) {
            printf("decode  : stopped\n");
        } else {
            printf("codec   : LC3 %u Hz, %u us, %u octets/frame\n",
                   (unsigned)ao.sample_rate, (unsigned)ao.frame_us,
                   (unsigned)ao.octets);
            printf("pcm     : %u Hz%s\n", (unsigned)ao.out_rate,
                   (ao.out_rate != ao.sample_rate) ? " (upsampled in the decoder)"
                                                   : " (decoder native rate)");
            printf("dac     : %u Hz true rate, %+d ppm off the nominal 32000\n",
                   (unsigned)ao.dac_rate,
                   (int)(((long long)ao.dac_rate - 32000) * 1000000 / 32000));
            if (ao.measured_rate) {
                printf("drained : %u Hz measured over the last stream\n",
                       (unsigned)ao.measured_rate);
                {
                    long long e = (long long)ao.measured_rate - ao.dac_rate;
                    if (e > 60 || e < -60) {
                        printf("hint    : measured and assumed rates disagree by\n"
                               "          %lld Hz - set the real one with\n"
                               "          `bt dacrate %u` and restart the stream.\n",
                               e, (unsigned)ao.measured_rate);
                    }
                }
            }
            printf("decode  : %u frames, %u errors, %u dropped\n",
                   (unsigned)ao.decoded, (unsigned)ao.errors,
                   (unsigned)ao.dropped);
            printf("conceal : %u lost SDUs, %u rejected by the decoder\n",
                   (unsigned)ao.plc, (unsigned)ao.dec_plc);
            printf("peak    : %d / 32767 on the last frame\n", ao.peak);

            if (ao.decoded > 0 && ao.peak == 0) {
                printf("hint    : frames decode but every sample is zero - the\n"
                       "          source is sending silence, not a broken path.\n");
            }
            if (ao.dropped > 0) {
                printf("hint    : dropped frames mean the decode task fell behind\n"
                       "          the ISO stream, which shows up as gaps.\n");
            }
        }
        {
            audio_level_t lvl;

            audio_get_level(&lvl);
            printf("amp     : %s, gain %+d dB (`spk gain <db>`, or the phone's\n"
                   "          volume slider via VCP)\n",
                   audio_spk_is_enabled() ? "on" : "off", audio_get_gain());
            if (lvl.clipped) {
                printf("clipped : %u samples hit full scale - turn the gain "
                       "down\n", (unsigned)lvl.clipped);
            }
        }
    }

    printf("note    : playback and the mic/camera cannot run together - the\n"
           "          FreeRTOS port does not save FP registers, so only one\n"
           "          float user may be active. Starting a stream stops them.\n");
#else
    (void)st;
    printf("LE Audio is not built into this image (CONFIG_BT_AUDIO off).\n");
#endif
}

int cmd_bt(int argc, char **argv)
{
    const char *sub;

    if (argc < 2) {
        usage();
        return 0;
    }
    sub = argv[1];

    if (strcmp(sub, "init") == 0) {
        int rc = bt_app_init();
        if (rc == BT_APP_OK) {
            print_status();
            printf("\nThe device is now advertising - scan for \"%s\".\n"
                   "If it does not appear, check the address above is not\n"
                   "all zeros (that means settings_load did not give it an\n"
                   "identity) and try `bt adv off` then `bt adv on`.\n",
                   bt_app_get_name());
        } else {
            printf("bt init failed (%d)\n", rc);
        }
    } else if (strcmp(sub, "status") == 0) {
        print_status();
    } else if (strcmp(sub, "adv") == 0) {
        if (argc < 3) {
            printf("usage: bt adv <on|off>\n");
        } else if (strcmp(argv[2], "on") == 0) {
            if (bt_app_adv_start() == BT_APP_OK) {
                printf("advertising as \"%s\"\n", bt_app_get_name());
            } else {
                printf("could not start advertising - run `bt init` first,\n"
                       "or disconnect if a link is already up\n");
            }
        } else {
            bt_app_adv_stop();
            printf("advertising stopped\n");
        }
    } else if (strcmp(sub, "sink") == 0) {
#if defined(CONFIG_BT_AUDIO)
        if (argc < 3) {
            printf("sink = %s\n",
                   bt_audio_out_get_sink() == BT_AO_SINK_UAC ? "uac" : "dac");
        } else {
            bt_ao_sink_t want = (strcmp(argv[2], "uac") == 0) ? BT_AO_SINK_UAC
                              : (strcmp(argv[2], "dac") == 0) ? BT_AO_SINK_DAC
                              : (bt_ao_sink_t)-1;

            if ((int)want < 0) {
                printf("usage: bt sink <dac|uac>\n");
            } else if (bt_audio_out_set_sink(want) != 0) {
                printf("a stream is running - stop it on the phone first\n");
            } else {
                printf("sink = %s (applies to the next stream)\n", argv[2]);
            }
        }
#else
        printf("LE Audio is not built into this image (CONFIG_BT_AUDIO off).\n");
#endif
    } else if (strcmp(sub, "name") == 0) {
        if (argc < 3) {
            printf("name = %s\n", bt_app_get_name());
        } else if (bt_app_set_name(argv[2]) == BT_APP_OK) {
            printf("name = %s (restart advertising to apply)\n",
                   bt_app_get_name());
        } else {
            printf("usage: bt name <string>\n");
        }
    } else if (strcmp(sub, "disconnect") == 0) {
        if (bt_app_disconnect() == BT_APP_ERR_STATE) {
            printf("not connected\n");
        } else {
            printf("disconnecting\n");
        }
    } else if (strcmp(sub, "audio") == 0) {
        print_audio();
    } else if (strcmp(sub, "vol") == 0) {
        if (argc < 3) {
            printf("usage: bt vol <0-255>\n");
            return 0;
        }
        {
            unsigned long v = strtoul(argv[2], NULL, 0);

            if (v > 255ul) {
                printf("volume is 0-255\n");
            } else {
                bt_app_set_volume((uint8_t)v);
            }
        }
    } else if (strcmp(sub, "capture") == 0) {
        uint32_t sec = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 5u;
        int rc = bt_audio_out_capture(sec);

        if (rc < 0) {
            printf("capture failed (%d) - a stream has to be playing\n", rc);
        } else {
            printf("capturing %d s of decoded PCM.\n"
                   "When it reports full: stop the stream, then `mic play`.\n"
                   "That replays through the same DAC path a tone uses, and a\n"
                   "tone is already known to come out clean - so if this replay\n"
                   "is clean the decode was fine and the fault is in real-time\n"
                   "delivery; if it is rough the fault is before the DAC.\n", rc);
        }
    } else if (strcmp(sub, "rate") == 0) {
        if (argc < 3) {
            printf("usage: bt rate <16|24|32|48|0>\n");
            return 0;
        }
        {
            unsigned k = (unsigned)strtoul(argv[2], NULL, 0);
            int rc = bt_app_set_sink_rate(k);

            if (rc != 0) {
                printf("rate must be 16, 24, 32, 48, or 0 for all (%d)\n", rc);
            } else if (k == 16u) {
                printf("sink now publishes 16 kHz only.\n"
                       "At 16 kHz the decoder upsamples to the DAC rate itself,\n"
                       "so no anti-alias FIR runs and the rate matcher sits at\n"
                       "~1:1 - this app's DSP is effectively out of the path.\n"
                       "Disconnect and reconnect for the phone to re-read it.\n");
            } else {
                printf("sink now publishes %s.\n"
                       "Disconnect and reconnect for the phone to re-read it.\n",
                       k ? "that rate only" : "16/24/32/48 kHz");
            }
        }
    } else if (strcmp(sub, "rf") == 0) {
        print_rf_cal();
    } else if (strcmp(sub, "dacrate") == 0) {
        if (argc < 3) {
            printf("usage: bt dacrate <hz>   (0 restores the clock-tree value)\n");
            return 0;
        }
        {
            uint32_t hz = (uint32_t)strtoul(argv[2], NULL, 0);

            if (hz != 0u && (hz < 16000u || hz > 64000u)) {
                printf("out of range - the DAC runs near 32000 Hz\n");
                return 0;
            }
            bt_audio_out_set_dac_rate(hz);
            printf("DAC rate %s - restart the stream to apply\n",
                   hz ? "overridden" : "back to the clock-tree value");
        }
    } else {
        usage();
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_bt, bt, BLE and LE Audio test);

#endif /* CONFIG_BLUETOOTH_LE_ENABLE && CONFIG_SHELL */
