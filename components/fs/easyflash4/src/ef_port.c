/*
 * This file is part of the EasyFlash Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for IPRO7 platform.
 * Created on: 2015-01-16
 * Modified for IPRO SDK
 */

#include <easyflash.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <hal_boot2.h>
#include <hal_flash.h>
#include <ef_cfg.h>
#include <generated/autoconf.h>

/* PSM partition flash offset */
static uint32_t psm_flash_offset = 0;
static uint32_t psm_partition_size = 0;

uint32_t ENV_AREA_SIZE;
uint32_t SECTOR_NUM;

/* default environment variables set for user */
static const ef_env default_env_set[] = {
    {"boot_times", "3", 1}
};

static SemaphoreHandle_t env_cache_lock = NULL;
#if (configSUPPORT_STATIC_ALLOCATION == 1)
static StaticSemaphore_t env_cache_lock_buf;
#endif

/**
 * Flash port for hardware initialize.
 *
 * @param default_env default ENV set for user
 * @param default_env_size default ENV size
 *
 * @return result
 */
EfErrCode ef_port_init(ef_env const **default_env, size_t *default_env_size) {
    int ret;
    uint32_t addr, size;

#if defined(CONFIG_EASYFLASH4_FIXED_PARTITION)
    addr = CONFIG_EASYFLASH4_FIXED_PARTITION_OFFSET;
    size = CONFIG_EASYFLASH4_FIXED_PARTITION_SIZE;
    ret = (addr != 0u && size != 0u) ? 0 : -1;
#else
    /* Get PSM partition info from boot2 partition table */
    ret = hal_boot2_partition_addr_active("PSM", &addr, &size);
#endif
    if (ret != 0) {
        EF_INFO("[EF] [PART] error when get PSM partition %d\r\n", ret);
        printf("[EF] [PART] Dead Loop. Reason: no Valid PSM partition found\r\n");
        while (1) {
        }
    }

    psm_flash_offset = addr;
    psm_partition_size = size;

    EF_INFO("[EF] Found Valid PSM partition, flash addr 0x%08lx, size %lu\r\n",
            psm_flash_offset,
            psm_partition_size
    );

    if (psm_partition_size < 8 * 1024) {
        printf("[ERROR] PSM partition is less than 8K, easyflash can not work!\r\n");
        while (1);
    }

    ENV_AREA_SIZE = (psm_partition_size / EF_ERASE_MIN_SIZE) * EF_ERASE_MIN_SIZE;
    SECTOR_NUM = ENV_AREA_SIZE / EF_ERASE_MIN_SIZE;
    printf("ENV AREA SIZE %lu, SECTOR NUM %lu\r\n", ENV_AREA_SIZE, SECTOR_NUM);

    *default_env = default_env_set;
    *default_env_size = sizeof(default_env_set) / sizeof(default_env_set[0]);

    printf("*default_env_size = %u\r\n", (unsigned int)*default_env_size);

#if configUSE_RECURSIVE_MUTEXES
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    env_cache_lock = xSemaphoreCreateRecursiveMutexStatic(&env_cache_lock_buf);
#else
    env_cache_lock = xSemaphoreCreateRecursiveMutex();
#endif
#else
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    env_cache_lock = xSemaphoreCreateMutexStatic(&env_cache_lock_buf);
#else
    env_cache_lock = xSemaphoreCreateMutex();
#endif
#endif
    if (env_cache_lock == NULL) {
        printf("[EF] [PORT] failed to create env mutex\r\n");
        return EF_ENV_INIT_FAILED;
    }

    return EF_NO_ERR;
}

/**
 * Read data from flash.
 * @note This operation's units is word.
 *
 * @param addr flash address (relative to partition start)
 * @param buf buffer to store read data
 * @param size read bytes size
 *
 * @return result
 */
EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;

    /* Convert relative address to absolute flash address */
    uint32_t abs_addr = psm_flash_offset + addr;

    if (flash_read(abs_addr, (uint8_t *)buf, size) != 0) {
        result = EF_READ_ERR;
    }

    return result;
}

/**
 * Erase data on flash.
 * @note This operation is irreversible.
 * @note This operation's units is different which on many chips.
 *
 * @param addr flash address (relative to partition start)
 * @param size erase bytes size
 *
 * @return result
 */
EfErrCode ef_port_erase(uint32_t addr, size_t size) {
    EfErrCode result = EF_NO_ERR;

    /* make sure the start address is a multiple of EF_ERASE_MIN_SIZE */
    EF_ASSERT(addr % EF_ERASE_MIN_SIZE == 0);

    /* Convert relative address to absolute flash address */
    uint32_t abs_addr = psm_flash_offset + addr;

    if (flash_erase(abs_addr, size) != 0) {
        result = EF_ERASE_ERR;
    }

    return result;
}

/**
 * Write data to flash.
 * @note This operation's units is word.
 * @note This operation must after erase. @see flash_erase.
 *
 * @param addr flash address (relative to partition start)
 * @param buf the write data buffer
 * @param size write bytes size
 *
 * @return result
 */
EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;

    /* Convert relative address to absolute flash address */
    uint32_t abs_addr = psm_flash_offset + addr;

    if (flash_write(abs_addr, (uint8_t *)buf, size) != 0) {
        result = EF_WRITE_ERR;
    }

    return result;
}

/**
 * lock the ENV ram cache
 */
void ef_port_env_lock(void) {
    if (env_cache_lock == NULL) {
        printf("[EF] [PORT] env mutex is NULL\r\n");
        return;
    }
#if configUSE_RECURSIVE_MUTEXES
    xSemaphoreTakeRecursive(env_cache_lock, portMAX_DELAY);
#else
    xSemaphoreTake(env_cache_lock, portMAX_DELAY);
#endif
}

/**
 * unlock the ENV ram cache
 */
void ef_port_env_unlock(void) {
    if (env_cache_lock == NULL) {
        printf("[EF] [PORT] env mutex is NULL\r\n");
        return;
    }
#if configUSE_RECURSIVE_MUTEXES
    xSemaphoreGiveRecursive(env_cache_lock);
#else
    xSemaphoreGive(env_cache_lock);
#endif
}

/**
 * This function is print flash debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 *
 */
void ef_log_debug(const char *file, const long line, const char *format, ...) {
#ifdef PRINT_DEBUG
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    vprintf(format, args);

    va_end(args);
#else
    (void)file;
    (void)line;
    (void)format;
#endif
}

/**
 * This function is print flash routine info.
 *
 * @param format output format
 * @param ... args
 */
void ef_log_info(const char *format, ...) {
    va_list args;
    char buf[256];

    /* args point to the first variable parameter */
    va_start(args, format);

    vsnprintf(buf, sizeof(buf), format, args);
    printf("%s", buf);

    va_end(args);
}

/**
 * This function is print flash non-package info.
 *
 * @param format output format
 * @param ... args
 */
void ef_print(const char *format, ...) {
    va_list args;
    char buf[256];

    /* args point to the first variable parameter */
    va_start(args, format);

    vsnprintf(buf, sizeof(buf), format, args);
    printf("%s", buf);

    va_end(args);
}
