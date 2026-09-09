/*
 * EL036MF1 bring-up app.
 *
 * Everything is a shell command rather than a fixed sequence in main(),
 * because the two things most likely to be wrong on a new board — the bus
 * mode strap and the pin assignment — are worth being able to change and
 * retry without reflashing.
 *
 * Suggested order:
 *   epd_rev            prove the interface before trusting anything else
 *   epd_init           run the register sequence
 *   epd_white / red    drive a solid colour
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <string.h>
#include <hal_uart.h>
#include <stdlib.h>

#include "freertos_app_hooks.h"
#include "shell.h"
#include "ipro_log.h"
#include "src/epd_el036.h"
#include "src/epd_pio.h"

#define TAG "EL036_APP"

static void banner(void)
{
    printf("\r\n");
    printf("E Ink Spectra 6 — EL036MF1 (400x600, six colours)\r\n");
    printf("  bus  : %s   (epd_bus 3|4 to switch)\r\n",
           epd_get_bus_mode() == EPD_BUS_3WIRE ? "3-wire 9-bit" : "4-wire");
    printf("  pins : CSB=%u SCL=%u SDA=%u DC=%u RSTN=%u BUSY=%u PWR=%u\r\n",
           (unsigned)epd_get_pin(EPD_PIN_IDX_CSB),
           (unsigned)epd_get_pin(EPD_PIN_IDX_SCL),
           (unsigned)epd_get_pin(EPD_PIN_IDX_SDA),
           (unsigned)epd_get_pin(EPD_PIN_IDX_DC),
           (unsigned)epd_get_pin(EPD_PIN_IDX_RSTN),
           (unsigned)epd_get_pin(EPD_PIN_IDX_BUSY),
           (unsigned)epd_get_pin(EPD_PIN_IDX_PWR));
    printf("\r\n");
    printf("  START HERE:  epd_rev\r\n");
    printf("    It reads the revision register back. That needs correct\r\n");
    printf("    wiring, bus mode, reset timing and BUSY — but no image and\r\n");
    printf("    no waveform. If it fails, nothing downstream can work.\r\n");
    printf("\r\n");
    printf("  then: epd_init, then epd_white / epd_black / epd_red /\r\n");
    printf("        epd_yellow / epd_blue / epd_green\r\n");
    printf("\r\n");
}

/* ------------------------------------------------------------------ */

static int cmd_epd_rev(int argc, char **argv)
{
    (void)argc; (void)argv;
    uint8_t rev = 0;

    /* Print the engine as well as the mode. Which one is actually in use
     * depends on both settings at once, and getting that wrong is the first
     * thing to suspect when a captured waveform does not look like the
     * configured clock rate. */
    const bool hw = epd_get_hw_spi() && epd_get_bus_mode() == EPD_BUS_4WIRE;
    printf("reading REV (0x70): bus=%s, writes=%s",
           epd_get_bus_mode() == EPD_BUS_3WIRE ? "3-wire" : "4-wire",
           hw ? "hardware SPI" : "bit-bang");
    if (hw) {
        printf(" @ %u Hz requested", (unsigned)epd_get_hw_spi_hz());
    }
    printf(", reads=bit-bang (always)\r\n");

    if (epd_get_hw_spi() && !hw) {
        printf("  note: hardware SPI is enabled but idle — it only applies in\r\n"
               "        4-wire mode. Run 'epd_bus 4' first.\r\n");
    }

    if (epd_read_rev(&rev) != 0) {
        printf("FAILED: BUSY never went high.\r\n");
        printf("  BUSY_N is stuck low or the pin is not connected.\r\n");
        printf("  Check EPD_PIN_BUSY wiring and that the panel has VDD.\r\n");
        return 0;
    }

    printf("REV = 0x%02X\r\n", rev);
    if (rev == 0x00 || rev == 0xFF) {
        printf("\r\n");
        printf("  0x%02X means nothing was driven back — the interface is\r\n", rev);
        printf("  not talking. In order of likelihood:\r\n");
        printf("\r\n");
        printf("  1. BUS MODE. The panel defaults to 3-wire (BS0 High).\r\n");
        printf("     If the board is wired 4-wire, BS0 must be pulled LOW —\r\n");
        printf("     floating is not enough. Try: epd_bus 4  then epd_rev\r\n");
        printf("  2. SDA is not bidirectional on this net, or a series\r\n");
        printf("     resistor / level shifter blocks the read direction.\r\n");
        printf("  3. SCL/SDA/CSB swapped, or CSB not reaching the panel.\r\n");
    } else {
        printf("\r\n");
        printf("  Plausible value — the interface works. Wiring, bus mode,\r\n");
        printf("  reset and BUSY are all good. Continue with epd_init.\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_rev, epd_rev, Read revision register - run this first);

static int cmd_epd_bus(int argc, char **argv)
{
    if (argc < 2) {
        printf("bus mode: %s\r\n",
               epd_get_bus_mode() == EPD_BUS_3WIRE ? "3-wire 9-bit" : "4-wire");
        printf("Usage: epd_bus 3|4\r\n");
        printf("  Panel default is 3-wire (BS0 High). 4-wire needs BS0 LOW.\r\n");
        return 0;
    }
    if (argv[1][0] == '3') {
        epd_set_bus_mode(EPD_BUS_3WIRE);
        printf("bus mode: 3-wire 9-bit (DC pin unused)\r\n");
    } else if (argv[1][0] == '4') {
        epd_set_bus_mode(EPD_BUS_4WIRE);
        printf("bus mode: 4-wire (DC on GPIO%u)\r\n",
               (unsigned)epd_get_pin(EPD_PIN_IDX_DC));
    } else {
        printf("epd_bus: expected 3 or 4\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_bus, epd_bus, Select 3-wire or 4-wire SPI);

/* epd_nobusy [0|1] -- run the sequences without a panel attached. */
static int cmd_epd_nobusy(int argc, char **argv)
{
    if (argc >= 2) {
        epd_set_ignore_busy(argv[1][0] != '0');
    }
    printf("ignore BUSY: %s\r\n", epd_get_ignore_busy() ? "ON (bench, no panel)"
                                                         : "off (normal)");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_nobusy, epd_nobusy, Ignore BUSY_N so the bus can be probed with no panel);

/* epd_spi [0|1] -- hardware SPI (4-wire only) vs bit-bang. */
static int cmd_epd_spi(int argc, char **argv)
{
    if (argc >= 2) {
        epd_set_hw_spi(argv[1][0] != '0');
    }
    if (epd_get_hw_spi()) {
        printf("SPI engine: hardware, %u Hz requested (4-wire only)\r\n",
               (unsigned)epd_get_hw_spi_hz());
    } else {
        printf("SPI engine: bit-bang\r\n");
    }
    if (epd_get_hw_spi() && epd_get_bus_mode() != EPD_BUS_4WIRE) {
        printf("  note: bus is 3-wire, so bit-bang is still in use.\r\n"
               "        run 'epd_bus 4' for the hardware engine to take effect.\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_spi, epd_spi, Hardware SPI vs bit-bang (4-wire only));

/* epd_swap [auto|0|1] -- hardware SPI MOSI/MISO swap. */
static int cmd_epd_swap(int argc, char **argv)
{
    if (argc >= 2) {
        if (strcmp(argv[1], "auto") == 0) {
            epd_set_hw_swap(-1);
        } else {
            epd_set_hw_swap(argv[1][0] != '0');
        }
    }

    int mode = epd_get_hw_swap();
    printf("MOSI/MISO swap: %s (effective: %s)\r\n",
           mode < 0 ? "auto" : (mode ? "forced ON" : "forced OFF"),
           epd_hw_swap_effective() ? "ON" : "OFF");
    printf("  auto = ON when sda sits on an n%%4==0 (MISO) pin.\r\n"
           "  sda is GPIO%u, %%4==%u.\r\n",
           (unsigned)epd_get_pin(EPD_PIN_IDX_SDA),
           (unsigned)epd_get_pin(EPD_PIN_IDX_SDA) % 4);
    printf("  usage: epd_swap auto|0|1   (takes effect on the next transfer)\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_swap, epd_swap, Hardware SPI MOSI/MISO swap - auto 0 or 1);

/* epd_timeout [ms] -- how long to wait for a refresh (DRF) to finish. */
static int cmd_epd_timeout(int argc, char **argv)
{
    if (argc >= 2) {
        long ms = atol(argv[1]);
        if (ms <= 0) {
            printf("usage: epd_timeout <ms>, e.g. epd_timeout 180000\r\n");
            return 0;
        }
        epd_set_refresh_timeout((uint32_t)ms);
    }
    printf("refresh (DRF) timeout: %lu ms\r\n",
           (unsigned long)epd_get_refresh_timeout());
    printf("  A Spectra 6 colour update is tens of seconds and gets much\r\n"
           "  slower when cold. Raising this distinguishes a slow panel from\r\n"
           "  a stuck one - reset/PON waits are unaffected.\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_timeout, epd_timeout, Refresh (DRF) timeout in ms);

/* epd_spi_hz [hz] -- hardware SPI SCL rate, for sweeping without a rebuild. */
static int cmd_epd_spi_hz(int argc, char **argv)
{
    if (argc >= 2) {
        long hz = atol(argv[1]);
        if (hz <= 0) {
            printf("usage: epd_spi_hz <hz>, e.g. epd_spi_hz 8000000\r\n");
            return 0;
        }
        epd_set_hw_spi_hz((uint32_t)hz);
        printf("hardware SPI target set to %ld Hz - applies on the next transfer\r\n", hz);
    }
    printf("hardware SPI target: %u Hz\r\n", (unsigned)epd_get_hw_spi_hz());
    printf("  Panel limits (Table 3): 20 MHz writes, 6.67 MHz reads.\r\n");
    printf("  hal_spi.c divides a 96 MHz source and truncates, so the rate\r\n"
           "  actually produced can be lower than asked for - measure SCL.\r\n");
    if (!(epd_get_hw_spi() && epd_get_bus_mode() == EPD_BUS_4WIRE)) {
        printf("  idle: hardware SPI needs 4-wire and 'epd_spi 1'.\r\n");
    }
    if (epd_get_engine() == EPD_ENGINE_PIO) {
        printf("  idle: engine is PIO - run 'epd_engine bitbang' to hand the\r\n"
               "        bus back to the hardware SPI path.\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_spi_hz, epd_spi_hz, Hardware SPI SCL clock (Hz));

/* epd_burst [0|1] -- stream frame data with one CSB per chunk. */
static int cmd_epd_burst(int argc, char **argv)
{
    if (argc >= 2) {
        epd_set_burst(argv[1][0] != '0');
    }
    printf("burst writes: %s\r\n", epd_get_burst() ? "ON" : "off");
    if (epd_get_burst()) {
        printf("  UNVERIFIED: E Ink's reference toggles CSB once per byte.\r\n"
               "  If a refresh comes out wrong or blank, turn this back off\r\n"
               "  before suspecting anything else.\r\n");
        if (!(epd_get_hw_spi() && epd_get_bus_mode() == EPD_BUS_4WIRE)) {
            printf("  idle: needs 4-wire + hardware SPI (epd_bus 4).\r\n");
        }
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_burst, epd_burst, Stream frame data for a faster refresh - unverified);

/* epd_engine [pio|bitbang] -- which engine drives the bus. */
static int cmd_epd_engine(int argc, char **argv)
{
    if (argc >= 2) {
        if (argv[1][0] == 'p' || argv[1][0] == 'P') {
            epd_set_engine(EPD_ENGINE_PIO);
        } else {
            epd_set_engine(EPD_ENGINE_BITBANG);
        }
    }

    if (epd_get_engine() == EPD_ENGINE_PIO) {
        bool three = epd_get_bus_mode() == EPD_BUS_3WIRE;
        printf("engine: PIO, %u-bit frames (%s)\r\n",
               three ? 9u : 8u, three ? "3-wire" : "4-wire");
        if (epd_pio_ready()) {
            uint16_t di; uint8_t df;
            epd_pio_divider(&di, &df);
            printf("  SCL = %u Hz expected, CLKDIV = %u + %u/256\r\n",
                   (unsigned)epd_pio_actual_hz(), (unsigned)di, (unsigned)df);
            printf("  (assumes a 96 MHz PIO source; measure SCL to confirm)\r\n");
            printf("  bulk feed: %s\r\n",
                   epd_pio_dma_ok() ? "DMA" : "CPU-fed FIFO (DMA failed)");
        } else {
            printf("  not armed yet - starts on the first transfer\r\n");
        }
        if (!three) {
            printf("  DC (GPIO%u) carries D/C; the frame is a plain byte.\r\n",
                   (unsigned)epd_get_pin(EPD_PIN_IDX_DC));
        }
    } else {
        printf("engine: bit-bang (reference port of E Ink's firmware)\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_engine, epd_engine, Select the bus engine - pio or bitbang);

/* epd_pio_hz [hz] -- PIO SCL clock, for sweeping without a rebuild.
 *
 * Panel limits (Application Note Table 3): 20 MHz for writes, 6.67 MHz for
 * reads (reads are always bit-bang here, so that ceiling never applies).
 *
 * This program's own ceiling is tighter than the panel's: each bit is 2 PIO
 * instructions (out=data phase, jmp=sample edge), so data setup/hold time
 * equals one full instruction cycle. tSDS/tSDH both require >=30 ns, which
 * caps the instruction rate at 33.3 MHz and SCL at 16.7 MHz -- with zero
 * margin. This command does not enforce that; it will let a value through
 * that fails Table 3 on this silicon's actual timing, which is the point of
 * being able to sweep and see where it breaks.
 */
static int cmd_epd_pio_hz(int argc, char **argv)
{
    if (argc >= 2) {
        long hz = atol(argv[1]);
        if (hz <= 0) {
            printf("usage: epd_pio_hz <hz>, e.g. epd_pio_hz 8000000\r\n");
            return 0;
        }
        epd_set_pio_hz((uint32_t)hz);
        printf("PIO target set to %ld Hz - takes effect on the next PIO transfer\r\n", hz);
    }
    printf("PIO target: %u Hz\r\n", (unsigned)epd_get_pio_hz());
    if (epd_pio_ready()) {
        uint16_t di; uint8_t df;
        epd_pio_divider(&di, &df);
        printf("  currently armed at %u Hz actual (CLKDIV %u+%u/256)\r\n",
               (unsigned)epd_pio_actual_hz(), (unsigned)di, (unsigned)df);
    } else {
        printf("  not armed yet - will apply on the next transfer\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_pio_hz, epd_pio_hz, Set/show the PIO SCL clock (Hz));

static int cmd_epd_init(int argc, char **argv)
{
    (void)argc; (void)argv;
    printf("%s\r\n", epd_init() == 0 ? "init OK" : "init FAILED (BUSY timeout)");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_init, epd_init, Reset and load the register sequence);

/* Colour fills. Each blocks for the whole refresh, which for Spectra 6 is
 * tens of seconds — that is the panel, not a hang. */
static int fill_and_report(epd_colour_t c, const char *name)
{
    printf("filling %s — a six-colour refresh takes tens of seconds...\r\n", name);
    TickType_t t0 = xTaskGetTickCount();
    int rc = epd_fill(c);
    uint32_t ms = (xTaskGetTickCount() - t0) * portTICK_PERIOD_MS;

    if (rc == 0) {
        printf("done in %lu ms\r\n", (unsigned long)ms);
    } else {
        printf("FAILED after %lu ms — BUSY timeout. If epd_rev passed, the\r\n",
               (unsigned long)ms);
        printf("  interface is fine, so suspect the power rails: VGH +20V,\r\n");
        printf("  VGL -20V, VSH +15V, VSL -15V measured at the FPC.\r\n");
    }
    return 0;
}

#define COLOUR_CMD(fn, alias, code, label)                                  \
    static int fn(int argc, char **argv)                                    \
    { (void)argc; (void)argv; return fill_and_report(code, label); }        \
    SHELL_CMD_EXPORT_ALIAS(fn, alias, Fill the panel with label)

COLOUR_CMD(cmd_epd_white,  epd_white,  EPD_WHITE,  "white");
COLOUR_CMD(cmd_epd_black,  epd_black,  EPD_BLACK,  "black");
COLOUR_CMD(cmd_epd_red,    epd_red,    EPD_RED,    "red");
COLOUR_CMD(cmd_epd_yellow, epd_yellow, EPD_YELLOW, "yellow");
COLOUR_CMD(cmd_epd_blue,   epd_blue,   EPD_BLUE,   "blue");
COLOUR_CMD(cmd_epd_green,  epd_green,  EPD_GREEN,  "green");

static int cmd_epd_cycle(int argc, char **argv)
{
    (void)argc; (void)argv;
    static const struct { epd_colour_t c; const char *n; } seq[] = {
        { EPD_WHITE,  "white"  }, { EPD_BLACK, "black" },
        { EPD_RED,    "red"    }, { EPD_YELLOW,"yellow"},
        { EPD_BLUE,   "blue"   }, { EPD_GREEN, "green" },
        { EPD_WHITE,  "white"  },
    };
    for (unsigned i = 0; i < sizeof(seq) / sizeof(seq[0]); i++) {
        printf("[%u/%u] ", i + 1, (unsigned)(sizeof(seq) / sizeof(seq[0])));
        if (fill_and_report(seq[i].c, seq[i].n) != 0) {
            return 0;
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    printf("cycle complete\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_cycle, epd_cycle, Run through all six colours);

/* epd_bars -- all six colours in one refresh, as horizontal bands. */
static int cmd_epd_bars(int argc, char **argv)
{
    (void)argc; (void)argv;
    printf("six colour bars in one refresh (black/white/yellow/red/blue/green,\r\n"
           "top to bottom) - takes tens of seconds...\r\n");
    uint32_t t0 = (uint32_t)xTaskGetTickCount();
    int rc = epd_bars();
    if (rc != 0) {
        printf("FAILED - the log line above names the stage that timed out.\r\n");
        printf("  reset/PON  : the panel is not answering at all.\r\n");
        printf("  DRF        : it accepted the image and started updating, so\r\n");
        printf("               the interface is fine. Either the update is\r\n");
        printf("               slower than the timeout (cold panels are much\r\n");
        printf("               slower - try 'epd_timeout 180000') or the supply\r\n");
        printf("               sags under refresh load: VGH +20V, VGL -20V,\r\n");
        printf("               VSH +15V, VSL -15V measured at the FPC.\r\n");
        return 0;
    }
    printf("done in %u ms\r\n",
           (unsigned)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS));
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_bars, epd_bars, All six colours in one refresh);

static int cmd_epd_sleep(int argc, char **argv)
{
    (void)argc; (void)argv;
    epd_sleep();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_sleep, epd_sleep, Deep sleep - needs HW reset to wake);

/* epd_pwr [0|1] | pol [0|1] -- board power switch and its polarity. */
static int cmd_epd_pwr(int argc, char **argv)
{
    if (argc >= 2 && strcmp(argv[1], "pol") == 0) {
        if (argc >= 3) {
            epd_set_pwr_active_high(argv[2][0] != '0');
        }
        printf("PWR polarity: active %s\r\n",
               epd_get_pwr_active_high() ? "HIGH" : "LOW");
        printf("  (active-high is an assumption from E Ink's reference, not a\r\n"
               "   documented fact - if the board gates its boost converter with\r\n"
               "   an active-low enable, the default holds it OFF permanently)\r\n");
        return 0;
    }

    if (argc < 2) {
        printf("usage: epd_pwr 0|1        cut / restore board power\r\n"
               "       epd_pwr pol 0|1    PWR active low / active high\r\n"
               "  gpio_setup() already turns power on before every command,\r\n"
               "  so 'epd_pwr 1' is not normally needed.\r\n");
        printf("current polarity: active %s\r\n",
               epd_get_pwr_active_high() ? "HIGH" : "LOW");
        return 0;
    }

    bool on = argv[1][0] != '0';
    epd_power_enable(on);
    printf("board power: %s (PWR pin driven %s)\r\n",
           on ? "ON" : "OFF",
           (on == epd_get_pwr_active_high()) ? "high" : "low");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_pwr, epd_pwr, Board power switch - VGH/VGL/VSH/VSL boost enable);

/* epd_pins [name gpio] -- show or change the pin map without a rebuild. */
static int cmd_epd_pins(int argc, char **argv)
{
    if (argc >= 3) {
        long gpio = atol(argv[2]);
        int found = 0;
        for (int i = 0; i < EPD_PIN_COUNT; i++) {
            if (strcmp(argv[1], epd_pin_name((epd_pin_idx_t)i)) == 0) {
                epd_set_pin((epd_pin_idx_t)i, (uint8_t)gpio);
                found = 1;
                break;
            }
        }
        if (!found) {
            printf("unknown pin '%s'\r\n", argv[1]);
            return 0;
        }
    }

    printf("pin map (epd_pins <name> <gpio> to change):\r\n");
    for (int i = 0; i < EPD_PIN_COUNT; i++) {
        printf("  %-4s = GPIO%u\r\n", epd_pin_name((epd_pin_idx_t)i),
               (unsigned)epd_get_pin((epd_pin_idx_t)i));
    }

    /* The SoC fixes each pin's SPI role by its number mod 4 (0=MISO, 1=MOSI,
     * 2=SS, 3=SCLK), so SCL and SDA are not freely placeable when the hardware
     * SPI engine is in use. The PIO engine and bit-bang have no such
     * constraint, which is why this is a warning rather than a refusal. */
    unsigned scl = epd_get_pin(EPD_PIN_IDX_SCL);
    unsigned sda = epd_get_pin(EPD_PIN_IDX_SDA);

    printf("\r\n  hardware SPI: ");
    if (scl % 4 != 3) {
        printf("UNUSABLE - scl GPIO%u is %%4==%u, needs %%4==3 (SCLK)\r\n",
               scl, scl % 4);
    } else if (sda % 4 == 1) {
        printf("ok, sda is MOSI directly (pin swap off)\r\n");
    } else if (sda % 4 == 0) {
        printf("ok, sda is the MISO pin so MOSI/MISO swap is enabled\r\n");
    } else {
        printf("UNUSABLE - sda GPIO%u is %%4==%u, needs %%4==0 or 1\r\n",
               sda, sda % 4);
    }
    printf("  bit-bang and PIO engines work on any pins.\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_pins, epd_pins, Show or change the GPIO pin map);

/* ------------------------------------------------------------------ */

/* Static task storage. Both reference apps known to run on this board --
 * ipro_ble_remote and ipro_lp_test -- create their tasks this way. Dynamic
 * xTaskCreate() resets the SoC during pre-scheduler bring-up here, so do not
 * "simplify" this back to xTaskCreate. */
static StackType_t  app_stack[1024];
static StaticTask_t app_tcb;

static void app_task(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(300));
    banner();
    vTaskDelete(NULL);
}

/* Non-weak override. The SDK default (freertos_app_hooks.c) calls
 * taskDISABLE_INTERRUPTS() before it prints, which swallows the message on a
 * UART whose output path needs interrupts -- an assert then looks exactly like
 * a silent hang. Print first, stop second. */
void vAssertCalled(const char *const pcFileName, unsigned long ulLine)
{
    printf("\r\n[!] ASSERT %s:%u\r\n", pcFileName ? pcFileName : "?",
           (unsigned)ulLine);
    taskDISABLE_INTERRUPTS();
    while (1) { }
}

int main(void)
{
    ipro_platform_init();

    xTaskCreateStatic(app_task, "epd_app", sizeof(app_stack) / sizeof(app_stack[0]),
                      NULL, 10, app_stack, &app_tcb);

    vTaskStartScheduler();

    printf("[epd] scheduler exited\r\n");
    while (1) { }
}
