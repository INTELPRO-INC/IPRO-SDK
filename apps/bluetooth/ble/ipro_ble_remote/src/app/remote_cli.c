/**
 * @file remote_cli.c
 * @brief CLI/Shell Commands for BLE Remote Control Demo
 *
 * Uses single "remote" command with subcommands for cleaner interface.
 *
 * Copyright (C) IPRO SDK 2025
 */

#include "remote_app.h"
#include "remote_services.h"
#include "pir_image_service.h"
#include "ipro_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <generated/autoconf.h>

#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#define LOG_TAG "REMOTE_CLI"

/*
 * SUBCOMMAND HANDLERS
 ****************************************************************************************
 */

static void subcmd_status(int argc, char **argv)
{
    (void)argc; (void)argv;
    remote_app_print_status();
}

static void subcmd_adv(int argc, char **argv)
{
    if (argc < 1) {
        printf("Usage: remote adv <start|stop>\n");
        return;
    }
    
    if (strcmp(argv[0], "start") == 0) {
        int ret = remote_app_start_advertising();
        printf("Advertising %s\n", ret == 0 ? "started" : "failed to start");
    } else if (strcmp(argv[0], "stop") == 0) {
        int ret = remote_app_stop_advertising();
        printf("Advertising %s\n", ret == 0 ? "stopped" : "failed to stop");
    } else {
        printf("Usage: remote adv <start|stop>\n");
    }
}

static void subcmd_led(int argc, char **argv)
{
    if (argc < 1) {
        printf("Usage: remote led <on|off> [brightness]\n");
        printf("       remote led bright <0-100>\n");
        return;
    }
    
    // Check for brightness subcommand
    if (strcmp(argv[0], "bright") == 0) {
        if (argc < 2) {
            printf("Usage: remote led bright <0-100>\n");
            return;
        }
        uint8_t brightness = (uint8_t)atoi(argv[1]);
        if (brightness > 100) brightness = 100;
        
        led_state_t state;
        remote_get_led_state(&state);
        remote_app_set_led(state.on, brightness);
        printf("LED brightness set to %d%%\n", brightness);
        return;
    }
    
    // Handle on/off
    bool on = (strcmp(argv[0], "on") == 0 || strcmp(argv[0], "1") == 0);
    uint8_t brightness = 100;
    
    if (argc >= 2) {
        brightness = (uint8_t)atoi(argv[1]);
        if (brightness > 100) brightness = 100;
    }
    
    remote_app_set_led(on, brightness);
    printf("LED set to %s, brightness %d%%\n", on ? "ON" : "OFF", brightness);
}

static void subcmd_btn(int argc, char **argv)
{
    if (argc < 1) {
        printf("Usage: remote btn <press|release|long|double>\n");
        return;
    }
    
    btn_event_t event = BTN_EVENT_PRESSED;
    const char *event_name = "press";
    
    if (strcmp(argv[0], "release") == 0) {
        event = BTN_EVENT_RELEASED;
        event_name = "release";
    } else if (strcmp(argv[0], "long") == 0) {
        event = BTN_EVENT_LONG_PRESS;
        event_name = "long press";
    } else if (strcmp(argv[0], "double") == 0) {
        event = BTN_EVENT_DOUBLE_CLICK;
        event_name = "double click";
    } else if (strcmp(argv[0], "press") == 0) {
        event = BTN_EVENT_PRESSED;
        event_name = "press";
    } else {
        printf("Unknown button event: %s\n", argv[0]);
        printf("Valid events: press, release, long, double\n");
        return;
    }
    
    int ret = remote_app_simulate_button(event);
    if (ret == 0) {
        printf("Button event sent: %s\n", event_name);
    } else {
        printf("Failed to send button event (not connected or notify not enabled)\n");
    }
}

static void subcmd_temp(int argc, char **argv)
{
    if (argc < 1) {
        printf("Usage: remote temp <temperature>\n");
        printf("Example: remote temp 25.5  (sets temperature to 25.50°C)\n");
        return;
    }
    
    float temp = atof(argv[0]);
    int16_t temp_int = (int16_t)(temp * 100);  // Convert to 0.01°C units
    
    const remote_app_context_t *ctx = remote_app_get_context();
    remote_app_update_environment(temp_int, ctx->sim_humidity);
    
    printf("Temperature set to %d.%02d°C\n", temp_int / 100, abs(temp_int % 100));
}

static void subcmd_humid(int argc, char **argv)
{
    if (argc < 1) {
        printf("Usage: remote humid <humidity>\n");
        printf("Example: remote humid 65  (sets humidity to 65%%)\n");
        return;
    }
    
    uint8_t humid = (uint8_t)atoi(argv[0]);
    if (humid > 100) humid = 100;
    
    const remote_app_context_t *ctx = remote_app_get_context();
    remote_app_update_environment(ctx->sim_temperature, humid);
    
    printf("Humidity set to %d%%\n", humid);
}

static void subcmd_env(int argc, char **argv)
{
    if (argc < 1) {
        printf("Usage: remote env <on|off> [interval_ms]\n");
        printf("       remote env set <temp> <humid>\n");
        printf("Example: remote env on 2000  (notify every 2 seconds)\n");
        printf("         remote env set 25.5 65\n");
        return;
    }
    
    // Handle 'set' subcommand
    if (strcmp(argv[0], "set") == 0) {
        if (argc < 3) {
            printf("Usage: remote env set <temp> <humid>\n");
            return;
        }
        float temp = atof(argv[1]);
        int16_t temp_int = (int16_t)(temp * 100);
        uint8_t humid = (uint8_t)atoi(argv[2]);
        if (humid > 100) humid = 100;
        
        remote_app_update_environment(temp_int, humid);
        printf("Environment set: %d.%02d°C, %d%% humidity\n", 
               temp_int / 100, abs(temp_int % 100), humid);
        return;
    }
    
    // Handle on/off
    if (strcmp(argv[0], "on") == 0 || strcmp(argv[0], "1") == 0) {
        uint32_t interval = 1000;  // Default 1 second
        if (argc >= 2) {
            interval = (uint32_t)atoi(argv[1]);
            if (interval < 100) interval = 100;
        }
        remote_app_start_env_notify(interval);
        printf("Environment notify started (interval: %lu ms)\n", interval);
    } else if (strcmp(argv[0], "off") == 0 || strcmp(argv[0], "0") == 0) {
        remote_app_stop_env_notify();
        printf("Environment notify stopped\n");
    } else {
        printf("Usage: remote env <on|off> [interval_ms]\n");
    }
}

static void subcmd_uart(int argc, char **argv)
{
    if (argc < 1) {
        printf("Usage: remote uart <message>\n");
        printf("Example: remote uart Hello World!\n");
        return;
    }
    
    // Concatenate all arguments as message
    char message[256] = {0};
    int offset = 0;
    for (int i = 0; i < argc && offset < 250; i++) {
        if (i > 0) message[offset++] = ' ';
        int arglen = strlen(argv[i]);
        if (offset + arglen > 250) arglen = 250 - offset;
        memcpy(message + offset, argv[i], arglen);
        offset += arglen;
    }
    message[offset++] = '\r';
    message[offset++] = '\n';
    message[offset] = '\0';
    
    int ret = remote_app_send_uart(message);
    if (ret == 0) {
        printf("Sent: %s", message);
    } else {
        printf("Failed to send (not connected or UART notify not enabled)\n");
    }
}

/*
 * PIR + IMAGE SUBCOMMANDS
 ****************************************************************************************
 */

static void subcmd_pir(int argc, char **argv)
{
    if (argc < 1) {
        printf("Usage: remote pir <status|start|stop|trigger>\n");
        printf("       remote pir start [gpio_pin]   Start PIR monitoring\n");
        printf("       remote pir stop               Stop PIR monitoring\n");
        printf("       remote pir trigger            Simulate motion detection\n");
        printf("       remote pir status             Show PIR status\n");
        return;
    }
    
    if (strcmp(argv[0], "status") == 0) {
        remote_app_print_pir_status();
    } else if (strcmp(argv[0], "start") == 0) {
        uint8_t gpio = CONFIG_PIR_GPIO_PIN;
        if (argc >= 2) {
            gpio = (uint8_t)atoi(argv[1]);
        }
        int ret = remote_app_start_pir(gpio);
        if (ret == 0) {
            printf("PIR sensor started on GPIO%d\n", gpio);
        } else {
            printf("Failed to start PIR sensor\n");
        }
    } else if (strcmp(argv[0], "stop") == 0) {
        remote_app_stop_pir();
        printf("PIR sensor stopped\n");
    } else if (strcmp(argv[0], "trigger") == 0) {
        int ret = remote_app_pir_trigger();
        if (ret == 0) {
            printf("PIR motion triggered (simulated)\n");
        } else {
            printf("Failed to trigger PIR\n");
        }
    } else {
        printf("Unknown pir subcommand: %s\n", argv[0]);
    }
}

static void subcmd_image(int argc, char **argv)
{
    if (argc < 1) {
        printf("Usage: remote image <capture|send|info|abort>\n");
        printf("       remote image capture   Capture image (or generate demo)\n");
        printf("       remote image send      Start sending image to phone\n");
        printf("       remote image info      Show image info\n");
        printf("       remote image abort     Abort current transfer\n");
        return;
    }

    if (strcmp(argv[0], "capture") == 0) {
        int ret = remote_app_capture_image();
        if (ret == 0) {
            img_info_t info;
            image_get_info(&info);
            printf("Image captured: %lu bytes, %d chunks\n", info.image_size, info.chunk_count);
        } else {
            printf("Failed to capture image\n");
        }
    } else if (strcmp(argv[0], "send") == 0) {
        int ret = remote_app_start_image_transfer();
        if (ret == 0) {
            printf("Image transfer started\n");
        } else {
            printf("Failed to start transfer (not connected or image not ready)\n");
        }
    } else if (strcmp(argv[0], "info") == 0) {
        img_info_t info;
        image_get_info(&info);
        const char *state_str[] = {"IDLE", "CAPTURING", "READY", "TRANSFERRING", "ERROR"};
        printf("Image Info:\n");
        printf("  State: %s\n", state_str[info.status]);
        printf("  Size: %lu bytes\n", info.image_size);
        printf("  Chunks: %d x %d bytes\n", info.chunk_count, info.chunk_size);
        printf("  Format: %s\n", info.format == 0 ? "JPEG" : "RAW");
    } else if (strcmp(argv[0], "abort") == 0) {
        image_abort_transfer();
        printf("Image transfer aborted\n");
    } else {
        printf("Unknown image subcommand: %s\n", argv[0]);
    }
}

static void print_help(void)
{
    printf("\n");
    printf("========== BLE Remote Control Commands ==========\n");
    printf("\n");
    printf("Usage: remote <subcommand> [options]\n");
    printf("\n");
    printf("Subcommands:\n");
    printf("  status                 Show application status\n");
    printf("  adv <start|stop>       Control BLE advertising\n");
    printf("  led <on|off> [bright]  Control LED state and brightness\n");
    printf("  led bright <0-100>     Set LED brightness only\n");
    printf("  btn <event>            Simulate button event\n");
    printf("      Events: press, release, long, double\n");
    printf("  temp <value>           Set temperature (e.g., 25.5)\n");
    printf("  humid <0-100>          Set humidity percentage\n");
    printf("  env <on|off> [ms]      Start/stop periodic env notify\n");
    printf("  env set <temp> <humid> Set temperature and humidity\n");
    printf("  uart <message>         Send UART message to phone\n");
    printf("\n");
    printf("PIR + Image Commands:\n");
    printf("  pir status             Show PIR sensor status\n");
    printf("  pir start [gpio]       Start PIR monitoring (default GPIO%d)\n", CONFIG_PIR_GPIO_PIN);
    printf("  pir stop               Stop PIR monitoring\n");
    printf("  pir trigger            Simulate motion detection\n");
    printf("  image capture          Capture image (or demo pattern)\n");
    printf("  image send             Send image to phone\n");
    printf("  image info             Show image info\n");
    printf("  image abort            Abort current transfer\n");
    printf("\n");
    printf("  help                   Show this help\n");
    printf("\n");
    printf("Examples:\n");
    printf("  remote status          # Show current status\n");
    printf("  remote adv start       # Start advertising\n");
    printf("  remote led on 80       # Turn on LED with 80%% brightness\n");
    printf("  remote btn press       # Simulate button press\n");
    printf("  remote env set 25.5 60 # Set temp=25.5C, humidity=60%%\n");
    printf("  remote env on 2000     # Start env notify every 2s\n");
    printf("  remote uart Hello!     # Send 'Hello!' to phone\n");
    printf("  remote pir start 10    # Start PIR on GPIO10\n");
    printf("  remote pir trigger     # Simulate motion detection\n");
    printf("  remote image capture   # Capture demo image\n");
    printf("  remote image send      # Send image to phone\n");
    printf("=================================================\n\n");
}

/*
 * MAIN COMMAND HANDLER (shared logic)
 ****************************************************************************************
 */

static int cmd_remote_handler(int argc, char **argv)
{
    if (argc < 2) {
        print_help();
        return 0;
    }

    const char *subcmd = argv[1];
    int sub_argc = argc - 2;
    char **sub_argv = &argv[2];

    if (strcmp(subcmd, "status") == 0) {
        subcmd_status(sub_argc, sub_argv);
    } else if (strcmp(subcmd, "adv") == 0) {
        subcmd_adv(sub_argc, sub_argv);
    } else if (strcmp(subcmd, "led") == 0) {
        subcmd_led(sub_argc, sub_argv);
    } else if (strcmp(subcmd, "btn") == 0) {
        subcmd_btn(sub_argc, sub_argv);
    } else if (strcmp(subcmd, "temp") == 0) {
        subcmd_temp(sub_argc, sub_argv);
    } else if (strcmp(subcmd, "humid") == 0) {
        subcmd_humid(sub_argc, sub_argv);
    } else if (strcmp(subcmd, "env") == 0) {
        subcmd_env(sub_argc, sub_argv);
    } else if (strcmp(subcmd, "uart") == 0) {
        subcmd_uart(sub_argc, sub_argv);
    } else if (strcmp(subcmd, "pir") == 0) {
        subcmd_pir(sub_argc, sub_argv);
    } else if (strcmp(subcmd, "image") == 0 || strcmp(subcmd, "img") == 0) {
        subcmd_image(sub_argc, sub_argv);
    } else if (strcmp(subcmd, "help") == 0) {
        print_help();
    } else {
        printf("Unknown subcommand: %s\n", subcmd);
        printf("Use 'remote help' for available commands\n");
    }
    return 0;
}

/*
 * SHELL COMMAND REGISTRATION
 ****************************************************************************************
 */

#ifdef CONFIG_SHELL
static int cmd_remote(int argc, char **argv)
{
    return cmd_remote_handler(argc, argv);
}
SHELL_CMD_EXPORT_ALIAS(cmd_remote, remote, BLE Remote Control commands);
#endif
