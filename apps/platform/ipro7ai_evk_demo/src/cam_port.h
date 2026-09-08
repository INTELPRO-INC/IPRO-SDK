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
 * @file cam_port.h
 * @brief Board wiring for the SC101IOT DVP camera on the IPRO7AI_FMB board.
 *
 * Taken from the board schematic (U5 IPRO7AI_FMB sheet + the CN3 "Camera"
 * sheet), not from the SDK's generic EVB defaults - two of them differ.
 *
 * Sensor: SmartSens SC101IOT, 1280x720, SCCB address 0x68, PCLK 72 MHz,
 * total 1600x750. Driver: components/sensor/sc101iot.c (720p is hardcoded
 * there via `#define resolution_720P 1`).
 *
 * CN3 = FH34SRJ-24S-0.5SH, 24-pin 0.5 mm FPC:
 *
 *   1  AVDD2V8    2.8 V analog          (C34 4.7uF + C35 100nF)
 *   2  DOVDD1V8   1.8 V digital I/O     (C32 4.7uF + C33 100nF)
 *   3  DVDD1V8    1.8 V digital core
 *   4  MCLK   <- GPIO41  DVP_REF_CLK    (GPIO_FUN_CAM_MISC + GLB_Set_CAM_CLK)
 *   5  PCLK   -> GPIO38
 *   6  VSYNC  -> GPIO39
 *   7  AGND
 *   8  D0        NOT CONNECTED  <-- see the 8-of-10 note below
 *   9  D1        NOT CONNECTED
 *   10 DGND1
 *   11 D2     -> GPIO0   DVP_D0
 *   12 D3     -> GPIO1   DVP_D1
 *   13 DGND2
 *   14 D4     -> GPIO2   DVP_D2
 *   15 D5     -> GPIO3   DVP_D3
 *   16 DGND3
 *   17 D6     -> GPIO4   DVP_D4
 *   18 D7     -> GPIO5   DVP_D5
 *   19 D8     -> GPIO6   DVP_D6
 *   20 D9     -> GPIO7   DVP_D7
 *   21 HSYNC  -> GPIO40
 *   22 SCL    <- GPIO36
 *   23 SDA    <> GPIO37
 *   24 RESET  <- GPIO35 through Q3, active low
 *   25 GND1  26 GND2
 *
 * ---------------------------------------------------------------------------
 * THREE THINGS THIS BOARD DOES DIFFERENTLY
 * ---------------------------------------------------------------------------
 *
 * 1. THE SENSOR BUS IS 1.8 V, THE LCD BUS IS 3.3 V. DOVDD is 1.8 V, so the
 *    DVP data/sync lines and SCCB run at 1.8 V. On IPRO7 the VDDIO_1 bank
 *    covers GPIO0-9 and GPIO36-41 - exactly the camera pins - and the SoC
 *    brings that bank out on its own supply pin (U5 pin 36, VDDIO1). The LCD
 *    pins (GPIO10-17) sit in VDDIO_2 and stay at 3.3 V. So VDDIO1 must be fed
 *    1.8 V for the camera to work; that split is the whole reason the pin has
 *    its own net.
 *
 * 2. RESET IS LEVEL-SHIFTED, NOT DIRECT. GPIO35 is in the VDDIO_3 bank
 *    (GPIO20-35) at 3.3 V, so it cannot drive a 1.8 V input. Q3 (PJA7002H
 *    N-MOSFET, gate on DOVDD1V8, drain on the 3.3 V side, source on the 1.8 V
 *    side) with R56 4.7k pulling up to DOVDD1V8 translates it. R55 - the
 *    3.3 V-side pull-up - is DNI, so the link is one-way: the SoC drives
 *    GPIO35 push-pull and the sensor sees the same polarity at 1.8 V.
 *    Drive GPIO35 LOW to assert reset, HIGH to release it.
 *
 * 3. THERE IS NO PWDN PIN. The SDK's EVB camera code (uvc_isp.c, mc_camera.c)
 *    drives GPIO34 as sensor power/PWDN and GPIO35 as reset. On this board
 *    GPIO34 is SD_DET, so only reset exists. Do not touch GPIO34 here.
 *
 * ---------------------------------------------------------------------------
 * 8 DATA LINES OUT OF THE SENSOR'S 10
 * ---------------------------------------------------------------------------
 * The sensor's D0/D1 are left unconnected and its D2..D9 land on the SoC's
 * DVP_D0..D7. That is the standard MSB-aligned 8-bit tap on a 10-bit output:
 * the two least-significant bits are discarded, so the bus carries the top
 * 8 bits at full scale - no shifting or scaling is needed in firmware.
 *
 * ---------------------------------------------------------------------------
 * SCCB PIN NAMING - THE SDK HEADER IS LABELLED BACKWARDS (HARMLESS)
 * ---------------------------------------------------------------------------
 * components/sensor/sccb.h defines, for CONFIG_EVB + CONFIG_IPRO7,
 * SCCB_SDA_PIN = GPIO36 and SCCB_SCL_PIN = GPIO37. On IPRO7 silicon the I2C0
 * role is fixed by pin parity - EVEN pin = SCL, ODD pin = SDA - so GPIO36 is
 * SCL and GPIO37 is SDA, which is what this schematic wires. The header's
 * labels are simply swapped. It does not matter in practice because
 * SCCB_Init() only uses the two macros to build a 2-entry list and muxes both
 * pins to GPIO_FUN_I2C; nothing reads the SDA/SCL distinction. Do not "fix"
 * the shared header on account of this board - the behaviour is identical.
 */
#ifndef CAM_PORT_H
#define CAM_PORT_H

/* ---- SCCB (I2C0). Roles are fixed by silicon: even = SCL, odd = SDA. ---- */
#define CAM_PIN_SCL      36
#define CAM_PIN_SDA      37

/* ---- DVP ---- */
#define CAM_PIN_D0       0      /* .. CAM_PIN_D0 + 7 = GPIO0..GPIO7 */
#define CAM_PIN_PCLK     38
#define CAM_PIN_VSYNC    39
#define CAM_PIN_HSYNC    40

/* ---- Clock and reset ---- */
#define CAM_PIN_MCLK     41     /* GPIO_FUN_CAM_MISC + GLB_Set_CAM_CLK       */
#define CAM_PIN_RST      35     /* plain GPIO, active low, level-shifted     */
/* No PWDN on this board - GPIO34 is SD_DET. */

/* ---- Sensor geometry (SC101IOT, as built in components/sensor) ---- */
#define CAM_SENSOR_W     1280
#define CAM_SENSOR_H     720
#define CAM_SENSOR_NAME  "SC101IOT"

#endif /* CAM_PORT_H */
