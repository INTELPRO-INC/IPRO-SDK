/*
 * psa-arch-tests driver PAL of the IPRO7 U-mode NSPE: console through
 * putchar(), no watchdog, no reset, NVMEM in a 1 KB RAM array.
 */
#include <stdio.h>
#include <string.h>

#include "pal_interfaces_ns.h"

/* Regression test status reporting buffer of pal_common_val_intf.h */
uint8_t test_status_buffer[256];

static uint8_t pal_nvmem_ram[1024];

static int pal_nvm_in_range(uint32_t offset, const void *buffer,
                            size_t size)
{
    return buffer != NULL && offset <= sizeof(pal_nvmem_ram) &&
           size <= sizeof(pal_nvmem_ram) - offset;
}

void pal_nvmem_reset(void)
{
    memset(pal_nvmem_ram, 0xFF, sizeof(pal_nvmem_ram));
}

int pal_uart_init_ns(void)
{
    return PAL_STATUS_SUCCESS;
}

int pal_print(uint8_t c)
{
    putchar(c);
    return PAL_STATUS_SUCCESS;
}

int pal_wd_timer_init_ns(uint32_t time_us, uint32_t timer_tick_us)
{
    (void)time_us;
    (void)timer_tick_us;
    return PAL_STATUS_SUCCESS;
}

int pal_watchdog_enable(void)
{
    return PAL_STATUS_SUCCESS;
}

int pal_watchdog_disable(void)
{
    return PAL_STATUS_SUCCESS;
}

int pal_nvm_read(uint32_t offset, void *buffer, size_t size)
{
    if (!pal_nvm_in_range(offset, buffer, size)) {
        return PAL_STATUS_ERROR;
    }
    memcpy(buffer, &pal_nvmem_ram[offset], size);
    return PAL_STATUS_SUCCESS;
}

int pal_nvm_write(uint32_t offset, void *buffer, size_t size)
{
    if (!pal_nvm_in_range(offset, buffer, size)) {
        return PAL_STATUS_ERROR;
    }
    memcpy(&pal_nvmem_ram[offset], buffer, size);
    return PAL_STATUS_SUCCESS;
}

/* val_entry() returns to its caller after the report */
void pal_terminate_simulation(void)
{
}

int pal_system_reset(void)
{
    return PAL_STATUS_UNSUPPORTED_FUNC;
}
