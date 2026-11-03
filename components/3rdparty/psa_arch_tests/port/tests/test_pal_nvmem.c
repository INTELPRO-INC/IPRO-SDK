/*
 * Host test of port/nspe/pal_driver_intf.c: the 1 KB RAM NVMEM reads
 * 0xFF after pal_nvmem_reset(), accesses stay inside it, and system
 * reset is unsupported. Prints "test_pal_nvmem: <pass>/<run>".
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "pal_interfaces_ns.h"

#define NVMEM_SIZE 1024u

static int g_run;
static int g_pass;

static void check(int cond, const char *what)
{
    g_run++;
    if (cond) {
        g_pass++;
    }
    printf("%s %s\n", cond ? "PASS" : "FAIL", what);
}

static int all_bytes(const uint8_t *buf, size_t len, uint8_t value)
{
    for (size_t i = 0; i < len; i++) {
        if (buf[i] != value) {
            return 0;
        }
    }
    return 1;
}

int main(void)
{
    static uint8_t buf[NVMEM_SIZE + 1];
    uint8_t word[4] = { 0xA5, 0x5A, 0x01, 0x02 };
    uint8_t back[4] = { 0 };

    memset(buf, 0x00, sizeof(buf));
    pal_nvm_write(0, buf, NVMEM_SIZE);
    pal_nvmem_reset();
    memset(buf, 0x00, sizeof(buf));
    check(pal_nvm_read(0, buf, NVMEM_SIZE) == PAL_STATUS_SUCCESS &&
          all_bytes(buf, NVMEM_SIZE, 0xFF),
          "reset: all 1024 bytes read 0xFF after earlier writes");

    check(pal_nvm_write(NVMEM_SIZE - 4, word, sizeof(word)) ==
              PAL_STATUS_SUCCESS &&
          pal_nvm_read(NVMEM_SIZE - 4, back, sizeof(back)) ==
              PAL_STATUS_SUCCESS &&
          memcmp(word, back, sizeof(word)) == 0 &&
          pal_nvm_write(NVMEM_SIZE - 3, word, sizeof(word)) ==
              PAL_STATUS_ERROR,
          "write: 4 bytes at 1020 succeed, at 1021 fail");

    check(pal_nvm_read(0, buf, NVMEM_SIZE + 1) == PAL_STATUS_ERROR,
          "read: 1025 bytes fail");

    check(pal_nvm_read(0, NULL, 4) == PAL_STATUS_ERROR &&
              pal_nvm_write(0, NULL, 4) == PAL_STATUS_ERROR &&
              pal_nvm_read(0, NULL, 0) == PAL_STATUS_ERROR,
          "NULL buffer: read and write fail, also with size 0");

    check(pal_nvm_read(NVMEM_SIZE + 1, back, 0) == PAL_STATUS_ERROR &&
              pal_nvm_write(NVMEM_SIZE + 1, word, 0) == PAL_STATUS_ERROR &&
              pal_nvm_read(UINT32_MAX, back, 1) == PAL_STATUS_ERROR &&
              pal_nvm_read(NVMEM_SIZE, back, 0) == PAL_STATUS_SUCCESS,
          "offset: past 1024 fails, also with size 0; 1024 + 0 succeeds");

    memset(back, 0, sizeof(back));
    check(pal_nvm_read(4, back, SIZE_MAX) == PAL_STATUS_ERROR &&
              pal_nvm_write(NVMEM_SIZE - 4, word,
                            SIZE_MAX - (NVMEM_SIZE - 4) + 2) ==
                  PAL_STATUS_ERROR &&
              pal_nvm_read(NVMEM_SIZE - 4, back,
                           SIZE_MAX - (NVMEM_SIZE - 4) + 2) ==
                  PAL_STATUS_ERROR &&
              pal_nvm_read(NVMEM_SIZE - 4, back, sizeof(back)) ==
                  PAL_STATUS_SUCCESS &&
              memcmp(word, back, sizeof(word)) == 0,
          "size near SIZE_MAX: offset + size wraps, refused, data kept");

    check(pal_system_reset() == PAL_STATUS_UNSUPPORTED_FUNC,
          "pal_system_reset: PAL_STATUS_UNSUPPORTED_FUNC");

    printf("test_pal_nvmem: %d/%d\n", g_pass, g_run);
    return g_pass == g_run ? 0 : 1;
}
