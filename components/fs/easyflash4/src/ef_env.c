/*
 * This file is part of the EasyFlash Library.
 *
 * Copyright (c) 2019, Armink, <armink.ztl@gmail.com>
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
 * Function: Environment variables operating interface. This is the Next Generation version.
 * Created on: 2019-02-02
 */

#include <string.h>
#include <stdio.h>
#include <easyflash.h>

/* Define log_warn if not already defined */
#ifndef log_warn
#define log_warn(...) printf(__VA_ARGS__)
#endif

#if defined(EF_USING_ENV) && !defined(EF_ENV_USING_LEGACY_MODE)

#ifndef EF_WRITE_GRAN
#error "Please configure flash write granularity (in ef_cfg.h)"
#endif

#if EF_WRITE_GRAN != 1 && EF_WRITE_GRAN != 8 && EF_WRITE_GRAN != 32 && EF_WRITE_GRAN != 64
#error "the write gran can be only setting as 1, 8, 32 and 64"
#endif

/* magic word(`E`, `F`, `4`, `0`) */
#define SECTOR_MAGIC_WORD                        0x30344645
/* magic word(`K`, `V`, `4`, `0`) */
#define ENV_MAGIC_WORD                           0x3034564B

/* the using status sector table length */
#ifndef USING_SECTOR_TABLE_LEN
#define USING_SECTOR_TABLE_LEN                   4
#endif

/* the string ENV value buffer size for legacy ef_get_env() function */
#ifndef EF_STR_ENV_VALUE_MAX_SIZE
#define EF_STR_ENV_VALUE_MAX_SIZE                (3979)
#endif

/* the sector remain threshold before full status */
#ifndef EF_SEC_REMAIN_THRESHOLD
#define EF_SEC_REMAIN_THRESHOLD                  (ENV_HDR_DATA_SIZE + EF_ENV_NAME_MAX)
#endif

/* the total remain empty sector threshold before GC */
#ifndef EF_GC_EMPTY_SEC_THRESHOLD
#define EF_GC_EMPTY_SEC_THRESHOLD                1
#endif

/* the ENV cache table size, it will improve ENV search speed when using cache */
#ifndef EF_ENV_CACHE_TABLE_SIZE
#define EF_ENV_CACHE_TABLE_SIZE                  16
#endif

/* the sector cache table size, it will improve ENV save speed when using cache */
#ifndef EF_SECTOR_CACHE_TABLE_SIZE
#define EF_SECTOR_CACHE_TABLE_SIZE               4
#endif

#if EF_ENV_CACHE_TABLE_SIZE > 0xFFFF
#error "The ENV cache table size must less than 0xFFFF"
#endif

#if (EF_ENV_CACHE_TABLE_SIZE > 0) && (EF_SECTOR_CACHE_TABLE_SIZE > 0)
#define EF_ENV_USING_CACHE
#endif

/* the sector is not combined value */
#define SECTOR_NOT_COMBINED                      0xFFFFFFFF
/* the next address is get failed */
#define FAILED_ADDR                              0xFFFFFFFF
/* default sector hdr gc_flag*/
#define DEFAULT_GC_FLAG                          0xFFFFFFFF

/* Return the most contiguous size aligned at specified width. RT_ALIGN(13, 4)
 * would return 16.
 */
#define EF_ALIGN(size, align)                    (((size) + (align) - 1) & ~((align) - 1))
/* align by write granularity */
#define EF_WG_ALIGN(size)                        (EF_ALIGN(size, (EF_WRITE_GRAN + 7)/8))
/**
 * Return the down number of aligned at specified width. RT_ALIGN_DOWN(13, 4)
 * would return 12.
 */
#define EF_ALIGN_DOWN(size, align)               ((size) & ~((align) - 1))
/* align down by write granularity */
#define EF_WG_ALIGN_DOWN(size)                   (EF_ALIGN_DOWN(size, (EF_WRITE_GRAN + 7)/8))

#if (EF_WRITE_GRAN == 1)
#define STATUS_TABLE_SIZE(status_number)         ((status_number * EF_WRITE_GRAN + 7)/8)
#else
#define STATUS_TABLE_SIZE(status_number)         (((status_number - 1) * EF_WRITE_GRAN + 7)/8)
#endif

#define STORE_STATUS_TABLE_SIZE                  STATUS_TABLE_SIZE(SECTOR_STORE_STATUS_NUM)
#define DIRTY_STATUS_TABLE_SIZE                  STATUS_TABLE_SIZE(SECTOR_DIRTY_STATUS_NUM)
#define ENV_STATUS_TABLE_SIZE                    STATUS_TABLE_SIZE(ENV_STATUS_NUM)

#define SECTOR_SIZE                              EF_ERASE_MIN_SIZE
extern uint32_t SECTOR_NUM;

#if 0
#if (SECTOR_NUM < 2)
#error "The sector number must lager then or equal to 2"
#endif

#if (EF_GC_EMPTY_SEC_THRESHOLD == 0 || EF_GC_EMPTY_SEC_THRESHOLD >= SECTOR_NUM)
#error "There is at least one empty sector for GC."
#endif
#endif

#define SECTOR_HDR_DATA_SIZE                     (EF_WG_ALIGN(sizeof(struct sector_hdr_data)))
#define SECTOR_HDR_GC_FLAG_OFFSET                ((unsigned long)(&((struct sector_hdr_data *)0)->gc_flag))
#define SECTOR_DIRTY_OFFSET                      offsetof(struct sector_hdr_data, status_table.dirty)
#define ENV_HDR_DATA_SIZE                        (EF_WG_ALIGN(sizeof(struct env_hdr_data)))
#define ENV_MAGIC_OFFSET                         ((unsigned long)(&((struct env_hdr_data *)0)->magic))
#define ENV_LEN_OFFSET                           ((unsigned long)(&((struct env_hdr_data *)0)->len))
#define ENV_NAME_LEN_OFFSET                      ((unsigned long)(&((struct env_hdr_data *)0)->name_len))

#define VER_NUM_ENV_NAME                         "__ver_num__"

enum sector_store_status {
    SECTOR_STORE_UNUSED,
    SECTOR_STORE_EMPTY,
    SECTOR_STORE_USING,
    SECTOR_STORE_FULL,
    SECTOR_STORE_STATUS_NUM,
};
typedef enum sector_store_status sector_store_status_t;

enum sector_dirty_status {
    SECTOR_DIRTY_UNUSED,
    SECTOR_DIRTY_FALSE,
    SECTOR_DIRTY_TRUE,
    SECTOR_DIRTY_GC,
    SECTOR_DIRTY_STATUS_NUM,
};
/** If the value of SECTOR_DIRTY_STATUS_NUM is changed, 
 * please evaluate size of status_table in del_env. */
typedef enum sector_dirty_status sector_dirty_status_t;

struct sector_hdr_data {
    struct {
        uint8_t store[STORE_STATUS_TABLE_SIZE];  /**< sector store status @see sector_store_status_t */
        uint8_t dirty[DIRTY_STATUS_TABLE_SIZE];  /**< sector dirty status @see sector_dirty_status_t */
    } status_table;
    uint32_t magic;                              /**< magic word(`E`, `F`, `4`, `0`) */
    uint32_t combined;                           /**< the combined next sector number, 0xFFFFFFFF: not combined */
    uint32_t gc_flag;                            /**< sector gc_flag 0xFFFFFFFF-NO GC  0-NEED GC */
};
typedef struct sector_hdr_data *sector_hdr_data_t;

struct sector_meta_data {
    bool check_ok;                               /**< sector header check is OK */
    struct {
        sector_store_status_t store;             /**< sector store status @see sector_store_status_t */
        sector_dirty_status_t dirty;             /**< sector dirty status @see sector_dirty_status_t */
    } status;
    uint32_t addr;                               /**< sector start address */
    uint32_t magic;                              /**< magic word(`E`, `F`, `4`, `0`) */
    uint32_t combined;                           /**< the combined next sector number, 0xFFFFFFFF: not combined */
    size_t remain;                               /**< remain size */
    uint32_t empty_env;                          /**< the next empty ENV node start address */
    uint32_t gc_flag;                            /**< sector gc_flag 0xFFFFFFFF-NO GC  0-NEED GC */
};
typedef struct sector_meta_data *sector_meta_data_t;

struct env_hdr_data {
    uint8_t status_table[ENV_STATUS_TABLE_SIZE]; /**< ENV node status, @see node_status_t */
    uint32_t magic;                              /**< magic word(`K`, `V`, `4`, `0`) */
    uint32_t len;                                /**< ENV node total length (header + name + value), must align by EF_WRITE_GRAN */
    uint32_t crc32;                              /**< ENV node crc32(name_len + data_len + name + value) */
    uint8_t name_len;                            /**< name length */
    uint32_t value_len;                          /**< value length */
};
typedef struct env_hdr_data *env_hdr_data_t;

struct env_cache_node {
    uint16_t name_crc;                           /**< ENV name's CRC32 low 16bit value */
    uint16_t active;                             /**< ENV node access active degree */
    uint32_t addr;                               /**< ENV node address */
};
typedef struct env_cache_node *env_cache_node_t;

struct sector_cache_node {
    uint32_t addr;                               /**< sector start address */
    uint32_t empty_addr;                         /**< sector empty address */
};
typedef struct sector_cache_node *sector_cache_node_t;

static EfErrCode gc_collect(void);
static EfErrCode checked_write_node_status(uint32_t addr, env_status_t status);

/* ENV start address in flash */
static uint32_t env_start_addr = 0;
/* default ENV set, must be initialized by user */
static ef_env const *default_env_set;
/* default ENV set size, must be initialized by user */
static size_t default_env_set_size = 0;
/* sector hdr gc_flag */
static uint32_t sector_hdr_gc_flag = DEFAULT_GC_FLAG;
/* initialize OK flag */
static bool init_ok = false;
/* request a GC check */
static bool gc_request = false;
/* is in recovery check status when first reboot */
static bool in_recovery_check = false;

#ifdef EF_ENV_USING_CACHE
/* ENV cache table */
struct env_cache_node env_cache_table[EF_ENV_CACHE_TABLE_SIZE] = { 0 };
/* sector cache table, it caching the sector info which status is current using */
struct sector_cache_node sector_cache_table[EF_SECTOR_CACHE_TABLE_SIZE] = { 0 };
#endif /* EF_ENV_USING_CACHE */

static size_t set_status(uint8_t status_table[], size_t status_num, size_t status_index)
{
    size_t byte_index = ~0UL;
    /*
     * | write garn |       status0       |       status1       |      status2         |
     * ---------------------------------------------------------------------------------
     * |    1bit    | 0xFF                | 0x7F                |  0x3F                |
     * |    8bit    | 0xFFFF              | 0x00FF              |  0x0000              |
     * |   32bit    | 0xFFFFFFFF FFFFFFFF | 0x00FFFFFF FFFFFFFF |  0x00FFFFFF 00FFFFFF |
     */
    memset(status_table, 0xFF, STATUS_TABLE_SIZE(status_num));
    if (status_index > 0) {
#if (EF_WRITE_GRAN == 1)
        byte_index = (status_index - 1) / 8;
        status_table[byte_index] &= ~(0x80 >> ((status_index - 1) % 8));
#else
        byte_index = (status_index - 1) * (EF_WRITE_GRAN / 8);
        status_table[byte_index] = 0x00;
#endif /* EF_WRITE_GRAN == 1 */
    }

    return byte_index;
}

static size_t get_status(const uint8_t status_table[], size_t status_num)
{
    size_t i = 0, status_num_bak = --status_num;

    while (status_num --) {
        /* get the first 0 position from end address to start address */
#if (EF_WRITE_GRAN == 1)
        if ((status_table[status_num / 8] & (0x80 >> (status_num % 8))) == 0x00) {
            break;
        }
#else /*  (EF_WRITE_GRAN == 8) ||  (EF_WRITE_GRAN == 32) ||  (EF_WRITE_GRAN == 64) */
        if (status_table[status_num * EF_WRITE_GRAN / 8] == 0x00) {
            break;
        }
#endif /* EF_WRITE_GRAN == 1 */
        i++;
    }

    return status_num_bak - i;
}

static EfErrCode write_status(uint32_t addr, uint8_t status_table[], size_t status_num, size_t status_index)
{
    EfErrCode result = EF_NO_ERR;
    size_t byte_index;

    EF_ASSERT(status_index < status_num);
    EF_ASSERT(status_table);

    /* set the status first */
    byte_index = set_status(status_table, status_num, status_index);

    /* the first status table value is all 1, so no need to write flash */
    if (byte_index == ~0UL) {
        return EF_NO_ERR;
    }
#if (EF_WRITE_GRAN == 1)
    result = ef_port_write(addr + byte_index, (uint32_t *)&status_table[byte_index], 1);
#else /*  (EF_WRITE_GRAN == 8) ||  (EF_WRITE_GRAN == 32) ||  (EF_WRITE_GRAN == 64) */
    /* write the status by write granularity
     * some flash (like stm32 onchip) NOT supported repeated write before erase */
    result = ef_port_write(addr + byte_index, (uint32_t *) &status_table[byte_index], EF_WRITE_GRAN / 8);
#endif /* EF_WRITE_GRAN == 1 */

    return result;
}

#ifdef EF_ENV_USING_CACHE
/*
 * It's only caching the current using status sector's empty_addr
 */
static void update_sector_cache(uint32_t sec_addr, uint32_t empty_addr)
{
    size_t i, empty_index = EF_SECTOR_CACHE_TABLE_SIZE;

    for (i = 0; i < EF_SECTOR_CACHE_TABLE_SIZE; i++) {
        if ((empty_addr > sec_addr) && (empty_addr < sec_addr + SECTOR_SIZE)) {
            /* update the sector empty_addr in cache */
            if (sector_cache_table[i].addr == sec_addr) {
                sector_cache_table[i].addr = sec_addr;
                sector_cache_table[i].empty_addr = empty_addr;
                return;
            } else if ((sector_cache_table[i].addr == FAILED_ADDR) && (empty_index == EF_SECTOR_CACHE_TABLE_SIZE)) {
                empty_index = i;
            }
        } else if (sector_cache_table[i].addr == sec_addr) {
            /* delete the sector which status is not current using */
            sector_cache_table[i].addr = FAILED_ADDR;
            return;
        }
    }
    /* add the sector empty_addr to cache */
    if (empty_index < EF_SECTOR_CACHE_TABLE_SIZE) {
        sector_cache_table[empty_index].addr = sec_addr;
        sector_cache_table[empty_index].empty_addr = empty_addr;
    }
}

/*
 * Get sector info from cache. It's return true when cache is hit.
 */
static bool get_sector_from_cache(uint32_t sec_addr, uint32_t *empty_addr)
{
    size_t i;

    for (i = 0; i < EF_SECTOR_CACHE_TABLE_SIZE; i++) {
        if (sector_cache_table[i].addr == sec_addr) {
            if (empty_addr) {
                *empty_addr = sector_cache_table[i].empty_addr;
            }
            return true;
        }
    }

    return false;
}

static void update_env_cache(const char *name, size_t name_len, uint32_t addr)
{
    size_t i, empty_index = EF_ENV_CACHE_TABLE_SIZE, min_activity_index = EF_ENV_CACHE_TABLE_SIZE;
    uint16_t name_crc = (uint16_t) (ef_calc_crc32(0, name, name_len) >> 16), min_activity = 0xFFFF;

    for (i = 0; i < EF_ENV_CACHE_TABLE_SIZE; i++) {
        if (addr != FAILED_ADDR) {
            /* update the ENV address in cache */
            if (env_cache_table[i].name_crc == name_crc) {
                env_cache_table[i].addr = addr;
                return;
            } else if ((env_cache_table[i].addr == FAILED_ADDR) && (empty_index == EF_ENV_CACHE_TABLE_SIZE)) {
                empty_index = i;
            } else if (env_cache_table[i].addr != FAILED_ADDR) {
                if (env_cache_table[i].active > 0) {
                    env_cache_table[i].active--;
                }
                if (env_cache_table[i].active < min_activity) {
                    min_activity_index = i;
                    min_activity = env_cache_table[i].active;
                }
            }
        } else if (env_cache_table[i].name_crc == name_crc) {
            /* delete the ENV */
            env_cache_table[i].addr = FAILED_ADDR;
            env_cache_table[i].active = 0;
            return;
        }
    }
    /* add the ENV to cache, using LRU (Least Recently Used) like algorithm */
    if (empty_index < EF_ENV_CACHE_TABLE_SIZE) {
        env_cache_table[empty_index].addr = addr;
        env_cache_table[empty_index].name_crc = name_crc;
        env_cache_table[empty_index].active = 0;
    } else if (min_activity_index < EF_ENV_CACHE_TABLE_SIZE) {
        env_cache_table[min_activity_index].addr = addr;
        env_cache_table[min_activity_index].name_crc = name_crc;
        env_cache_table[min_activity_index].active = 0;
    }
}

/*
 * Get ENV info from cache. It's return true when cache is hit.
 */
static bool get_env_from_cache_with_error(const char *name, size_t name_len, uint32_t *addr,
                                          EfErrCode *io_result)
{
    size_t i;
    if (io_result) *io_result = EF_NO_ERR;
    uint16_t name_crc = (uint16_t) (ef_calc_crc32(0, name, name_len) >> 16);

    for (i = 0; i < EF_ENV_CACHE_TABLE_SIZE; i++) {
        if ((env_cache_table[i].addr != FAILED_ADDR) && (env_cache_table[i].name_crc == name_crc)) {
            char saved_name[EF_ENV_NAME_MAX];
            /* read the ENV name in flash */
            if (ef_port_read(env_cache_table[i].addr + ENV_HDR_DATA_SIZE,
                             (uint32_t *) saved_name, EF_ENV_NAME_MAX) != EF_NO_ERR) {
                if (io_result) *io_result = EF_READ_ERR;
                return false;
            }
            if (!strncmp(name, saved_name, name_len)) {
                *addr = env_cache_table[i].addr;
                if (env_cache_table[i].active >= 0xFFFF - EF_ENV_CACHE_TABLE_SIZE) {
                    env_cache_table[i].active = 0xFFFF;
                } else {
                    env_cache_table[i].active += EF_ENV_CACHE_TABLE_SIZE;
                }
                return true;
            }
        }
    }

    return false;
}

#endif /* EF_ENV_USING_CACHE */

/*
 * find the continue 0xFF flash address to end address
 */
static uint32_t continue_ff_addr(uint32_t start, uint32_t end, EfErrCode *io_result)
{
    uint8_t buf[32], last_data = 0x00;
    size_t i, addr = start, read_size;
    *io_result = EF_NO_ERR;

    for (; start < end; start += sizeof(buf)) {
        if (start + sizeof(buf) < end) {
            read_size = sizeof(buf);
        } else {
            read_size = end - start;
        }
        if (ef_port_read(start, (uint32_t *) buf, read_size) != EF_NO_ERR) {
            *io_result = EF_READ_ERR;
            return FAILED_ADDR;
        }
        for (i = 0; i < read_size; i++) {
            if (last_data != 0xFF && buf[i] == 0xFF) {
                addr = start + i;
            }
            last_data = buf[i];
        }
    }

    if (last_data == 0xFF) {
        return EF_WG_ALIGN(addr);
    } else {
        return end;
    }
}

/*
 * find the next ENV address by magic word on the flash
 */
static uint32_t find_next_env_addr(uint32_t start, uint32_t end, EfErrCode *io_result)
{
    uint8_t buf[32];
    uint32_t start_bak = start, i;
    uint32_t magic;

#ifdef EF_ENV_USING_CACHE
    uint32_t empty_env;

    if (get_sector_from_cache(EF_ALIGN_DOWN(start, SECTOR_SIZE), &empty_env) && start == empty_env) {
        return FAILED_ADDR;
    }
#endif /* EF_ENV_USING_CACHE */

    for (; start < end; start += (sizeof(buf) - sizeof(uint32_t))) {
        /* Keep the magic-word lookahead, but do not read a full buffer beyond
         * the final search window. end leaves a sector-header-sized margin. */
        size_t scan_bytes = end - start;
        if (scan_bytes > sizeof(buf) - sizeof(uint32_t) + 1)
            scan_bytes = sizeof(buf) - sizeof(uint32_t) + 1;
        EfErrCode result = ef_port_read(start, (uint32_t *) buf,
                                        scan_bytes + sizeof(uint32_t) - 1);
        if (result != EF_NO_ERR && io_result) {
            *io_result = EF_READ_ERR;
            return FAILED_ADDR;
        }
        for (i = 0; i < sizeof(buf) - sizeof(uint32_t) && start + i < end; i++) {
#ifndef EF_BIG_ENDIAN            /* Little Endian Order */
            magic = buf[i] + ((uint32_t)buf[i + 1] << 8) + ((uint32_t)buf[i + 2] << 16) + ((uint32_t)buf[i + 3] << 24);
#else                       /* Big Endian Order */
            magic = buf[i + 3] + ((uint32_t)buf[i + 2] << 8) + ((uint32_t)buf[i + 1] << 16) + ((uint32_t)buf[i] << 24);
#endif
            if (magic == ENV_MAGIC_WORD && (start + i - ENV_MAGIC_OFFSET) >= start_bak) {
                return start + i - ENV_MAGIC_OFFSET;
            }
        }
    }

    return FAILED_ADDR;
}

static uint32_t get_next_env_addr_with_error(sector_meta_data_t sector, env_node_obj_t pre_env,
                                             EfErrCode *io_result)
{
    uint32_t addr = FAILED_ADDR;
    if (io_result) *io_result = EF_NO_ERR;

    if (sector->status.store == SECTOR_STORE_EMPTY) {
        return FAILED_ADDR;
    }

    if (pre_env->addr.start == FAILED_ADDR) {
        /* the first ENV address */
        addr = sector->addr + SECTOR_HDR_DATA_SIZE;
    } else {
        if (pre_env->addr.start <= sector->addr + SECTOR_SIZE) {
            if (pre_env->crc_is_ok) {
                addr = pre_env->addr.start + pre_env->len;
            } else {
                /* when pre_env CRC check failed, maybe the flash has error data
                 * find_next_env_addr after pre_env address */
                addr = pre_env->addr.start + EF_WG_ALIGN(1);
            }
            /* check and find next ENV address */
            addr = find_next_env_addr(addr, sector->addr + SECTOR_SIZE - SECTOR_HDR_DATA_SIZE, io_result);

            if (addr > sector->addr + SECTOR_SIZE || pre_env->len == 0) {
                //TODO 扇区连续模式
                return FAILED_ADDR;
            }
        } else {
            /* no ENV */
            return FAILED_ADDR;
        }
    }

    return addr;
}

static EfErrCode read_env_with_error(env_node_obj_t env, EfErrCode *io_result)
{
    struct env_hdr_data env_hdr;
    uint8_t buf[32];
    uint32_t calc_crc32 = 0, crc_data_len, env_name_addr;
    EfErrCode result = EF_NO_ERR;
    size_t len, size;
    if (io_result) *io_result = EF_NO_ERR;
    /* read ENV header raw data */
    if (ef_port_read(env->addr.start, (uint32_t *)&env_hdr, sizeof(struct env_hdr_data)) != EF_NO_ERR)
        goto io_failed;
    env->status = (env_status_t) get_status(env_hdr.status_table, ENV_STATUS_NUM);
    env->len = env_hdr.len;

    if (env->len == ~0UL || env->len > ENV_AREA_SIZE || env->len < ENV_NAME_LEN_OFFSET) {
        /* the ENV length was not write, so reserved the meta data for current ENV */
        env->len = ENV_HDR_DATA_SIZE;
        if (env->status != ENV_ERR_HDR) {
            env->status = ENV_ERR_HDR;
            EF_DEBUG("Error: The ENV @0x%08X length has an error.\r\n", env->addr.start);
            write_status(env->addr.start, env_hdr.status_table, ENV_STATUS_NUM, ENV_ERR_HDR);
        }
        env->crc_is_ok = false;
        return EF_READ_ERR;
    } else if (env->len > SECTOR_SIZE - SECTOR_HDR_DATA_SIZE && env->len < ENV_AREA_SIZE) {
        //TODO 扇区连续模式，或者写入长度没有写入完整
        EF_ASSERT(0);
    }

    /* CRC32 data len(header.name_len + header.value_len + name + value) */
    crc_data_len = env->len - ENV_NAME_LEN_OFFSET;
    /* calculate the CRC32 value */
    for (len = 0, size = 0; len < crc_data_len; len += size) {
        if (len + sizeof(buf) < crc_data_len) {
            size = sizeof(buf);
        } else {
            size = crc_data_len - len;
        }

        if (ef_port_read(env->addr.start + ENV_NAME_LEN_OFFSET + len, (uint32_t *) buf,
                         EF_WG_ALIGN(size)) != EF_NO_ERR) goto io_failed;
        calc_crc32 = ef_calc_crc32(calc_crc32, buf, size);
    }
    /* check CRC32 */
    if (calc_crc32 != env_hdr.crc32) {
        env->crc_is_ok = false;
        result = EF_READ_ERR;
    } else {
        /* the name is behind aligned ENV header */
        env_name_addr = env->addr.start + ENV_HDR_DATA_SIZE;
        if (ef_port_read(env_name_addr, (uint32_t *) env->name,
                         EF_WG_ALIGN(env_hdr.name_len)) != EF_NO_ERR) goto io_failed;
        env->crc_is_ok = true;
        /* the value is behind aligned name */
        env->addr.value = env_name_addr + EF_WG_ALIGN(env_hdr.name_len);
        env->value_len = env_hdr.value_len;
        env->name_len = env_hdr.name_len;
    }

    return result;

io_failed:
    env->crc_is_ok = false;
    env->name_len = env->value_len = 0;
    env->addr.value = FAILED_ADDR;
    if (io_result) *io_result = EF_READ_ERR;
    return EF_READ_ERR;
}

static EfErrCode read_env(env_node_obj_t env)
{
    return read_env_with_error(env, NULL);
}

static EfErrCode read_sector_meta_data(uint32_t addr, sector_meta_data_t sector, bool traversal)
{
    EfErrCode result = EF_NO_ERR;
    struct sector_hdr_data sec_hdr;

    EF_ASSERT(addr % SECTOR_SIZE == 0);
    EF_ASSERT(sector);

    /* read sector header raw data */
    result = ef_port_read(addr, (uint32_t *)&sec_hdr, sizeof(struct sector_hdr_data));
    if (result != EF_NO_ERR) {
        /* Failed I/O is not a corrupt, formattable header. */
        memset(sector, 0, sizeof(*sector));
        sector->addr = addr;
        sector->combined = SECTOR_NOT_COMBINED;
        sector->empty_env = FAILED_ADDR;
        return EF_READ_ERR;
    }

    sector->addr = addr;
    sector->magic = sec_hdr.magic;
    /* check magic word */
    if (sector->magic != SECTOR_MAGIC_WORD) {
        sector->check_ok = false;
        sector->combined = SECTOR_NOT_COMBINED;
        return EF_ENV_INIT_FAILED;
    }
    sector->check_ok = true;
    /* get other sector meta data */
    sector->combined = sec_hdr.combined;
    sector->status.store = (sector_store_status_t) get_status(sec_hdr.status_table.store, SECTOR_STORE_STATUS_NUM);
    sector->status.dirty = (sector_dirty_status_t) get_status(sec_hdr.status_table.dirty, SECTOR_DIRTY_STATUS_NUM);
    sector->gc_flag = sec_hdr.gc_flag;
    /* traversal all ENV and calculate the remain space size */
    if (traversal) {
        sector->remain = 0;
        sector->empty_env = sector->addr + SECTOR_HDR_DATA_SIZE;
        if (sector->status.store == SECTOR_STORE_EMPTY) {
            sector->remain = SECTOR_SIZE - SECTOR_HDR_DATA_SIZE;
        } else if (sector->status.store == SECTOR_STORE_USING) {
            struct env_node_obj env_meta;

#ifdef EF_ENV_USING_CACHE
            if (get_sector_from_cache(addr, &sector->empty_env)) {
                sector->remain = SECTOR_SIZE - (sector->empty_env - sector->addr);
                return result;
            }
#endif /* EF_ENV_USING_CACHE */

            sector->remain = SECTOR_SIZE - SECTOR_HDR_DATA_SIZE;
            env_meta.addr.start = FAILED_ADDR;
            EfErrCode io_result = EF_NO_ERR;
            while ((env_meta.addr.start = get_next_env_addr_with_error(sector, &env_meta, &io_result)) != FAILED_ADDR) {
                read_env_with_error(&env_meta, &io_result);
                if (io_result != EF_NO_ERR) return io_result;
                if (!env_meta.crc_is_ok) {
                    if (env_meta.status != ENV_PRE_WRITE && env_meta.status!= ENV_ERR_HDR) {
                        EF_INFO("Error: The ENV (@0x%08X) CRC32 check failed!\r\n", env_meta.addr.start);
                        sector->remain = 0;
                        result = EF_READ_ERR;
                        break;
                    }
                }
                sector->empty_env += env_meta.len;
                sector->remain -= env_meta.len;
            }
            if (io_result != EF_NO_ERR) return io_result;
            /* check the empty ENV address by read continue 0xFF on flash  */
            {
                uint32_t ff_addr;

                ff_addr = continue_ff_addr(sector->empty_env, sector->addr + SECTOR_SIZE, &io_result);
                if (io_result != EF_NO_ERR) return io_result;
                /* check the flash data is clean */
                if (sector->empty_env != ff_addr) {
                    /* update the sector information */
                    sector->empty_env = ff_addr;
                    sector->remain = SECTOR_SIZE - (ff_addr - sector->addr);
                }
            }

#ifdef EF_ENV_USING_CACHE
            update_sector_cache(sector->addr, sector->empty_env);
#endif
        }
    }

    return result;
}

static uint32_t get_next_sector_addr(sector_meta_data_t pre_sec)
{
    uint32_t next_addr;

    if (pre_sec->addr == FAILED_ADDR) {
        return env_start_addr;
    } else {
        /* check ENV sector combined */
        if (pre_sec->combined == SECTOR_NOT_COMBINED) {
            next_addr = pre_sec->addr + SECTOR_SIZE;
        } else {
            next_addr = pre_sec->addr + pre_sec->combined * SECTOR_SIZE;
        }
        /* check range */
        if (next_addr < env_start_addr + ENV_AREA_SIZE) {
            return next_addr;
        } else {
            /* no sector */
            return FAILED_ADDR;
        }
    }
}

static EfErrCode env_iterator(env_node_obj_t env, void *arg1, void *arg2,
        bool (*callback)(env_node_obj_t env, void *arg1, void *arg2))
{
    struct sector_meta_data sector;
    uint32_t sec_addr;

    sector.addr = FAILED_ADDR;
    /* search all sectors */
    while ((sec_addr = get_next_sector_addr(&sector)) != FAILED_ADDR) {
        EfErrCode result = read_sector_meta_data(sec_addr, &sector, false);
        if (result == EF_READ_ERR) return result;
        if (result != EF_NO_ERR) {
            continue;
        }
        if (callback == NULL) {
            continue;
        }
        /* sector has ENV */
        if (sector.status.store == SECTOR_STORE_USING || sector.status.store == SECTOR_STORE_FULL) {
            env->addr.start = FAILED_ADDR;
            /* search all ENV */
            while ((env->addr.start = get_next_env_addr_with_error(&sector, env, &result)) != FAILED_ADDR) {
                /* Preserve legacy CRC/recovery semantics, but never invoke a
                 * repair callback after failed I/O, even if bytes were filled. */
                read_env_with_error(env, &result);
                if (result != EF_NO_ERR) return result;
                /* iterator is interrupted when callback return true */
                if (callback(env, arg1, arg2)) {
                    return EF_NO_ERR;
                }
            }
            if (result != EF_NO_ERR) return result;
        }
    }
    return EF_NO_ERR;
}

static bool find_env_cb(env_node_obj_t env, void *arg1, void *arg2)
{
    const char *key = arg1;
    bool *find_ok = arg2;
    size_t key_len = strlen(key);

    if (key_len != env->name_len) {
        return false;
    }
    /* check ENV */
    if (env->crc_is_ok && env->status == ENV_WRITE && !strncmp(env->name, key, key_len)) {
        *find_ok = true;
        return true;
    }
    return false;
}

static EfErrCode lookup_env_no_cache(const char *key, env_node_obj_t env)
{
    bool find_ok = false;

    EfErrCode result = env_iterator(env, (void *)key, &find_ok, find_env_cb);
    return result != EF_NO_ERR ? result : (find_ok ? EF_NO_ERR : EF_ENV_NOT_FOUND);
}

static EfErrCode lookup_env(const char *key, env_node_obj_t env)
{
    EfErrCode result = EF_NO_ERR;

#ifdef EF_ENV_USING_CACHE
    size_t key_len = strlen(key);

    if (get_env_from_cache_with_error(key, key_len, &env->addr.start, &result)) {
        result = read_env(env);
        if (result != EF_NO_ERR) return result;
        if (env->crc_is_ok && env->status == ENV_WRITE) return EF_NO_ERR;
    }
    if (result != EF_NO_ERR) return result;
#endif /* EF_ENV_USING_CACHE */

    result = lookup_env_no_cache(key, env);

#ifdef EF_ENV_USING_CACHE
    if (result == EF_NO_ERR) {
        update_env_cache(key, key_len, env->addr.start);
    }
#endif /* EF_ENV_USING_CACHE */

    return result;
}

static bool find_env(const char *key, env_node_obj_t env)
{
    return lookup_env(key, env) == EF_NO_ERR;
}

/* Checked reads deliberately do not use the legacy cache/iterator: those
 * interfaces cannot distinguish I/O failure from an absent record. */
static EfErrCode checked_erased_range(uint32_t addr, size_t size)
{
    /* The flash port enters/exits XIP for every read. Batch blank-tail checks
     * without skipping bytes, trusting cached contents, or growing a single
     * read beyond 256 bytes. Keep the buffer on the caller's stack. */
    uint32_t words[64];
    const uint8_t *bytes = (const uint8_t *)words;
    while (size) {
        size_t chunk = size < sizeof(words) ? size : sizeof(words);
        if (ef_port_read(addr, words, chunk) != EF_NO_ERR) return EF_READ_ERR;
        for (size_t i = 0; i < chunk; ++i) {
            if (bytes[i] != 0xff) return EF_ENV_CORRUPT;
        }
        addr += chunk;
        size -= chunk;
    }
    return EF_NO_ERR;
}

/* Consider only a tail whose entire erased range has already been checked.
 * Prefer USING sectors to avoid consuming the GC reserve unnecessarily. */
static bool checked_sector_can_allocate(const struct sector_hdr_data *header)
{
    size_t store = get_status(header->status_table.store, SECTOR_STORE_STATUS_NUM);
    size_t dirty = get_status(header->status_table.dirty, SECTOR_DIRTY_STATUS_NUM);
    return header->gc_flag == DEFAULT_GC_FLAG &&
        (store == SECTOR_STORE_EMPTY || store == SECTOR_STORE_USING) &&
        (dirty == SECTOR_DIRTY_FALSE || dirty == SECTOR_DIRTY_TRUE);
}

static void checked_offer_space(sector_meta_data_t candidate, const struct sector_hdr_data *header,
                                 uint32_t sector, uint32_t addr, size_t requested)
{
    size_t store = get_status(header->status_table.store, SECTOR_STORE_STATUS_NUM);
    size_t dirty = get_status(header->status_table.dirty, SECTOR_DIRTY_STATUS_NUM);
    if (requested < ENV_HDR_DATA_SIZE || requested > sector + SECTOR_SIZE - addr ||
            !checked_sector_can_allocate(header)) return;
    if (candidate->empty_env != FAILED_ADDR &&
            !(candidate->status.store == SECTOR_STORE_EMPTY && store == SECTOR_STORE_USING)) return;
    candidate->addr = sector;
    candidate->empty_env = addr;
    candidate->remain = sector + SECTOR_SIZE - addr;
    candidate->check_ok = true;
    candidate->magic = header->magic;
    candidate->combined = header->combined;
    candidate->gc_flag = header->gc_flag;
    candidate->status.store = (sector_store_status_t)store;
    candidate->status.dirty = (sector_dirty_status_t)dirty;
}

/* An optional space result is a plan, not an allocation. Callers must retain
 * the ENV lock through use and enforce GC reserve/reclamation policy. */
struct checked_gc_info {
    bool settled;
    size_t empty_sectors;
    size_t garbage_bytes;
    bool queued_gc;
};

struct checked_prefix_match {
    const char *prefix;
    size_t length;
    char key[EF_ENV_NAME_MAX + 1];
};

/* A read-only scan visits many adjacent headers, names and CRC chunks. Keep
 * one page only for this invocation under the ENV lock. Every subsequent scan
 * (including every post-write verification) starts with an empty window. */
struct checked_scan_reader {
    uint32_t words[64];
    uint32_t base;
    bool valid;
};

static EfErrCode checked_scan_read(struct checked_scan_reader *reader, uint32_t addr,
                                  void *destination, size_t size)
{
    uint8_t *out = destination;
    while (size) {
        const uint32_t base = EF_ALIGN_DOWN(addr, sizeof(reader->words));
        if (!reader->valid || reader->base != base) {
            reader->valid = false;
            if (ef_port_read(base, reader->words, sizeof(reader->words)) != EF_NO_ERR)
                return EF_READ_ERR;
            reader->base = base;
            reader->valid = true;
        }
        size_t offset = addr - base;
        size_t chunk = sizeof(reader->words) - offset;
        if (chunk > size) chunk = size;
        memcpy(out, (const uint8_t *)reader->words + offset, chunk);
        out += chunk;
        addr += chunk;
        size -= chunk;
    }
    return EF_NO_ERR;
}

static EfErrCode checked_scan_env_internal(const char *key, size_t key_len, env_node_obj_t found,
                                env_node_obj_t predecessor, sector_meta_data_t space, size_t requested,
                                struct checked_gc_info *gc_info, struct checked_prefix_match *prefix_match)
{
    struct checked_scan_reader reader = { .valid = false };
    struct checked_gc_info gc = {true, 0, 0, false};
    if (prefix_match) prefix_match->key[0] = '\0';
    if (gc_info) memset(gc_info, 0, sizeof(*gc_info));
    struct sector_meta_data candidate = {0};
    candidate.empty_env = FAILED_ADDR;
    if (space) {
        memset(space, 0, sizeof(*space));
        space->empty_env = FAILED_ADDR;
    }
    unsigned live_matches = 0, recovery_matches = 0;
    bool deleted_match = false;
    if (predecessor) predecessor->addr.start = FAILED_ADDR;
    if (SECTOR_SIZE % sizeof(reader.words) ||
            !ENV_AREA_SIZE || ENV_AREA_SIZE % SECTOR_SIZE ||
            env_start_addr % SECTOR_SIZE || env_start_addr > UINT32_MAX - ENV_AREA_SIZE) {
        return EF_ENV_INIT_FAILED;
    }
    for (uint32_t sector = env_start_addr; sector < env_start_addr + ENV_AREA_SIZE; sector += SECTOR_SIZE) {
        struct sector_hdr_data sh;
        if (checked_scan_read(&reader, sector, (uint32_t *)&sh, sizeof(sh)) != EF_NO_ERR) return EF_READ_ERR;
        size_t store = get_status(sh.status_table.store, SECTOR_STORE_STATUS_NUM);
        size_t dirty = get_status(sh.status_table.dirty, SECTOR_DIRTY_STATUS_NUM);
        /* A bounded GC pass marks all dirty sectors but collects only part of
         * that worklist. A marked, still-DIRTY sector is queued work, not an
         * interrupted move. Actual GC-in-progress and staged records remain
         * ineligible for checked reclamation. The full scan still validates
         * every record's bounds and CRC before any collection can begin. */
        const bool queued_gc = sh.gc_flag == 0 && dirty == SECTOR_DIRTY_TRUE;
        if ((dirty != SECTOR_DIRTY_FALSE && dirty != SECTOR_DIRTY_TRUE) ||
                (sh.gc_flag != DEFAULT_GC_FLAG && !queued_gc))
            gc.settled = false;
        if (queued_gc) gc.queued_gc = true;
        if (sh.magic != SECTOR_MAGIC_WORD || sh.combined != SECTOR_NOT_COMBINED ||
                (store != SECTOR_STORE_EMPTY && store != SECTOR_STORE_USING && store != SECTOR_STORE_FULL)) {
            return EF_ENV_CORRUPT;
        }
        uint32_t addr = sector + SECTOR_HDR_DATA_SIZE, end = sector + SECTOR_SIZE;
        if (store == SECTOR_STORE_EMPTY) {
            EfErrCode result = checked_erased_range(addr, end - addr);
            if (result != EF_NO_ERR) return result;
            if (space) checked_offer_space(&candidate, &sh, sector, addr, requested);
            if (checked_sector_can_allocate(&sh)) ++gc.empty_sectors;
            continue;
        }
        while (addr < end) {
            struct env_hdr_data eh;
            if (end - addr < ENV_HDR_DATA_SIZE) {
                EfErrCode result = checked_erased_range(addr, end - addr);
                if (result != EF_NO_ERR) return result;
                break;
            }
            if (checked_scan_read(&reader, addr, (uint32_t *)&eh, sizeof(eh)) != EF_NO_ERR) return EF_READ_ERR;
            size_t status = get_status(eh.status_table, ENV_STATUS_NUM);
            if (status == ENV_UNUSED) {
                EfErrCode result = checked_erased_range(addr, end - addr);
                if (result != EF_NO_ERR) return result;
                break;
            }
            /* No resynchronization on guessed magic after a damaged header:
             * reporting integrity failure is safer than skipping a live key. */
            if (eh.magic != ENV_MAGIC_WORD || eh.len < ENV_HDR_DATA_SIZE || eh.len > end - addr ||
                    eh.len != EF_WG_ALIGN(eh.len) || !eh.name_len || eh.name_len > EF_ENV_NAME_MAX ||
                    eh.value_len > SECTOR_SIZE || status < ENV_PRE_WRITE ||
                    (status > ENV_DELETED && status != ENV_ERR_HDR) ||
                    eh.len != ENV_HDR_DATA_SIZE + EF_WG_ALIGN(eh.name_len) + EF_WG_ALIGN(eh.value_len)) {
                return EF_ENV_CORRUPT;
            }
            uint32_t words[8], crc = 0;
            size_t offset = ENV_NAME_LEN_OFFSET;
            while (offset < eh.len) {
                size_t chunk = eh.len - offset;
                if (chunk > sizeof(words)) chunk = sizeof(words);
                if (checked_scan_read(&reader, addr + offset, words, chunk) != EF_NO_ERR) return EF_READ_ERR;
                crc = ef_calc_crc32(crc, words, chunk);
                offset += chunk;
            }
            if (crc != eh.crc32) return EF_ENV_CORRUPT;
            if (status == ENV_PRE_WRITE || status == ENV_PRE_DELETE) gc.settled = false;
            if (dirty == SECTOR_DIRTY_TRUE && (status == ENV_DELETED || status == ENV_ERR_HDR))
                gc.garbage_bytes += eh.len;
            /* Select one key only after validating its complete record. The
             * rest of the store is still scanned before any caller may write. */
            if (prefix_match && !prefix_match->key[0] && status == ENV_WRITE &&
                    eh.name_len >= prefix_match->length) {
                uint32_t name_words[(EF_ENV_NAME_MAX + 3) / 4];
                if (checked_scan_read(&reader, addr + ENV_HDR_DATA_SIZE, name_words, eh.name_len) != EF_NO_ERR)
                    return EF_READ_ERR;
                if (memcmp(name_words, prefix_match->prefix, prefix_match->length) == 0) {
                    if (memchr(name_words, '\0', eh.name_len)) return EF_ENV_CORRUPT;
                    memcpy(prefix_match->key, name_words, eh.name_len);
                    prefix_match->key[eh.name_len] = '\0';
                }
            }
            /* A fully formed staged record can be marked ERR_HDR on reboot.
             * Ignore it only after the same bounds and CRC checks as live
             * records; a partial header/payload must still fail closed. */
            if ((status == ENV_WRITE || status == ENV_PRE_DELETE || status == ENV_DELETED) && eh.name_len == key_len) {
                uint32_t name_words[(EF_ENV_NAME_MAX + 3) / 4];
                if (checked_scan_read(&reader, addr + ENV_HDR_DATA_SIZE, name_words, key_len) != EF_NO_ERR) return EF_READ_ERR;
                if (memcmp(name_words, key, key_len) == 0) {
                    if (status == ENV_DELETED) {
                        deleted_match = true;
                        addr += eh.len;
                        continue;
                    }
                    if (status == ENV_WRITE) ++live_matches;
                    else {
                        ++recovery_matches;
                        if (predecessor) predecessor->addr.start = addr;
                    }
                    if (live_matches > 1) return EF_ENV_CORRUPT;
                    if (status == ENV_WRITE || live_matches == 0) {
                        found->addr.start = addr;
                        found->addr.value = addr + ENV_HDR_DATA_SIZE + EF_WG_ALIGN(eh.name_len);
                        found->value_len = eh.value_len;
                        found->len = eh.len;
                        found->status = (env_status_t)status;
                    }
                }
            }
            addr += eh.len;
        }
        if (space) checked_offer_space(&candidate, &sh, sector, addr, requested);
    }
    /* A deleted replacement may coexist with its interrupted predecessor.
     * Without record generations it is unsafe to resurrect that old value. */
    if (!live_matches && (recovery_matches > 1 || (recovery_matches && deleted_match))) return EF_ENV_CORRUPT;
    if (space) *space = candidate;
    if (gc_info) *gc_info = gc;
    if (prefix_match) {
        /* Do not guess which staged or GC records will survive recovery. */
        if (!gc.settled) return EF_ENV_CORRUPT;
        return prefix_match->key[0] ? EF_NO_ERR : EF_ENV_NOT_FOUND;
    }
    return live_matches || recovery_matches ? EF_NO_ERR : EF_ENV_NOT_FOUND;
}

static EfErrCode checked_scan_env(const char *key, size_t key_len, env_node_obj_t found,
                                env_node_obj_t predecessor, sector_meta_data_t space, size_t requested)
{
    return checked_scan_env_internal(key, key_len, found, predecessor, space, requested, NULL, NULL);
}

static EfErrCode checked_find_env(const char *key, size_t key_len, env_node_obj_t found,
                                  env_node_obj_t predecessor)
{
    return checked_scan_env(key, key_len, found, predecessor, NULL, 0);
}

EfErrCode ef_env_get_checked(const char *key, size_t offset, void *buf, size_t cap,
                             size_t *copied, size_t *total)
{
    size_t key_len = 0;
    if (copied) *copied = 0;
    if (total) *total = 0;
    if (!key || (!buf && cap)) return EF_ENV_ARG_ERR;
    while (key_len <= EF_ENV_NAME_MAX && key[key_len]) ++key_len;
    if (!key_len || key_len > EF_ENV_NAME_MAX) return EF_ENV_ARG_ERR;
    if (!init_ok) return EF_ENV_INIT_FAILED;
    ef_port_env_lock();
    struct env_node_obj found = {0};
    EfErrCode result = init_ok ? checked_find_env(key, key_len, &found, NULL) : EF_ENV_INIT_FAILED;
    if (result == EF_NO_ERR && offset > found.value_len) result = EF_ENV_ARG_ERR;
    if (result == EF_NO_ERR) {
        size_t size = found.value_len - offset, done = 0;
        if (size > cap) size = cap;
        while (done < size) {
            uint32_t words[8];
            size_t chunk = size - done;
            if (chunk > sizeof(words)) chunk = sizeof(words);
            if (ef_port_read(found.addr.value + offset + done, words, chunk) != EF_NO_ERR) {
                result = EF_READ_ERR;
                break;
            }
            memcpy((uint8_t *)buf + done, words, chunk);
            done += chunk;
        }
        if (result == EF_NO_ERR) {
            if (copied) *copied = size;
            if (total) *total = found.value_len;
        }
    }
    ef_port_env_unlock();
    return result;
}

/* Caller holds the ENV lock and has validated the record with a full scan. */
static EfErrCode checked_delete_record(uint32_t addr)
{
    uint32_t sector = EF_ALIGN_DOWN(addr, SECTOR_SIZE);
    struct sector_hdr_data sh;
    if (ef_port_read(sector, (uint32_t *)&sh, sizeof(sh)) != EF_NO_ERR) return EF_READ_ERR;
    size_t dirty = get_status(sh.status_table.dirty, SECTOR_DIRTY_STATUS_NUM);
    if (sh.magic != SECTOR_MAGIC_WORD ||
            (dirty != SECTOR_DIRTY_FALSE && dirty != SECTOR_DIRTY_TRUE && dirty != SECTOR_DIRTY_GC)) {
        return EF_ENV_CORRUPT;
    }
    /* Mark reclaimable before touching the value; a dirty-mark failure must
     * not delete a value whose sector could then become unreclaimable. */
    if (dirty == SECTOR_DIRTY_FALSE) {
        EfErrCode result = write_status(sector + SECTOR_DIRTY_OFFSET, sh.status_table.dirty,
                                        SECTOR_DIRTY_STATUS_NUM, SECTOR_DIRTY_TRUE);
        if (result != EF_NO_ERR) return result;
        if (ef_port_read(sector, (uint32_t *)&sh, sizeof(sh)) != EF_NO_ERR) return EF_READ_ERR;
        if (get_status(sh.status_table.dirty, SECTOR_DIRTY_STATUS_NUM) != SECTOR_DIRTY_TRUE) return EF_WRITE_ERR;
    }
    uint32_t status_words[(ENV_STATUS_TABLE_SIZE + 3) / 4];
    uint8_t *status = (uint8_t *)status_words;
    EfErrCode result = write_status(addr, status, ENV_STATUS_NUM, ENV_DELETED);
#ifdef EF_ENV_USING_CACHE
    /* Even an error may follow successful programming. Never retain a cache
     * entry that could expose the deleted record through a legacy getter. */
    for (size_t i = 0; i < EF_ENV_CACHE_TABLE_SIZE; ++i) {
        if (env_cache_table[i].addr == addr) {
            env_cache_table[i].addr = FAILED_ADDR;
            env_cache_table[i].active = 0;
        }
    }
#endif
    if (result != EF_NO_ERR) return result;
    if (ef_port_read(addr, status_words, ENV_STATUS_TABLE_SIZE) != EF_NO_ERR) return EF_READ_ERR;
    return get_status(status, ENV_STATUS_NUM) == ENV_DELETED ? EF_NO_ERR : EF_WRITE_ERR;
}

EfErrCode ef_env_del_checked(const char *key)
{
    size_t key_len = 0;
    if (!key) return EF_ENV_ARG_ERR;
    while (key_len <= EF_ENV_NAME_MAX && key[key_len]) ++key_len;
    if (!key_len || key_len > EF_ENV_NAME_MAX) return EF_ENV_ARG_ERR;
    if (!init_ok) return EF_ENV_INIT_FAILED;
    ef_port_env_lock();
    EfErrCode result = EF_ENV_INIT_FAILED;
    if (init_ok) {
        struct env_node_obj found = {0}, predecessor = {0};
        /* Remove predecessors first, while the current WRITE remains readable.
         * Otherwise a later recovery could resurrect a deleted replacement. */
        for (;;) {
            result = checked_find_env(key, key_len, &found, &predecessor);
            if (result != EF_NO_ERR) break;
            bool remove_predecessor = found.status == ENV_WRITE && predecessor.addr.start != FAILED_ADDR;
            result = checked_delete_record(remove_predecessor ? predecessor.addr.start : found.addr.start);
            if (result != EF_NO_ERR) break;
            if (!remove_predecessor) {
                result = checked_find_env(key, key_len, &found, NULL);
                if (result == EF_ENV_NOT_FOUND) result = EF_NO_ERR;
                else if (result == EF_NO_ERR) result = EF_WRITE_ERR;
                break;
            }
        }
    }
    ef_port_env_unlock();
    return result;
}

EfErrCode ef_env_del_prefix_checked(const char *prefix)
{
    size_t prefix_len = 0;
    if (!prefix) return EF_ENV_ARG_ERR;
    while (prefix_len <= EF_ENV_NAME_MAX && prefix[prefix_len]) ++prefix_len;
    if (!prefix_len || prefix_len > EF_ENV_NAME_MAX) return EF_ENV_ARG_ERR;
    if (!init_ok) return EF_ENV_INIT_FAILED;

    ef_port_env_lock();
    EfErrCode result = EF_ENV_INIT_FAILED;
    if (init_ok) {
        struct checked_prefix_match match = {.prefix = prefix, .length = prefix_len};
        struct env_node_obj found = {0};
        for (;;) {
            result = checked_scan_env_internal("", 0, &found, NULL, NULL, 0, NULL, &match);
            if (result == EF_ENV_NOT_FOUND) {
                result = EF_NO_ERR;
                break;
            }
            if (result != EF_NO_ERR) break;
            /* Exact lookup rejects ambiguous duplicate live records. Recovery
             * records have already been excluded by the settled-store scan. */
            result = checked_find_env(match.key, strlen(match.key), &found, NULL);
            if (result != EF_NO_ERR) break;
            result = checked_delete_record(found.addr.start);
            if (result != EF_NO_ERR) break;
        }
    }
    ef_port_env_unlock();
    return result;
}

static bool ef_is_str(uint8_t *value, size_t len)
{
#define __is_print(ch)       ((unsigned int)((ch) - ' ') < 127u - ' ')
    size_t i;

    for (i = 0; i < len; i++) {
        if (!__is_print(value[i])) {
            return false;
        }
    }
    return true;
}

static size_t get_env(const char *key, void *value_buf, size_t buf_len, size_t *value_len)
{
    struct env_node_obj env;
    size_t read_len = 0;

    if (find_env(key, &env)) {
        if (value_len) {
            *value_len = env.value_len;
        }
        if (buf_len > env.value_len) {
            read_len = env.value_len;
        } else {
            read_len = buf_len;
        }
        if (value_buf){
            if (ef_port_read(env.addr.value, (uint32_t *) value_buf, read_len) != EF_NO_ERR) {
                if (value_len) *value_len = 0;
                return 0;
            }
        } else if (value_len) {
            *value_len = 0;
        }
    }

    return read_len;
}

/**
 * Get a ENV object by key name
 *
 * @param key ENV name
 * @param env ENV object
 *
 * @return TRUE: find the ENV is OK, else return false
 */
bool ef_get_env_obj(const char *key, env_node_obj_t env)
{
    bool find_ok = false;

    if (!init_ok) {
        EF_INFO("ENV isn't initialize OK.\r\n");
        return 0;
    }

    /* lock the ENV cache */
    ef_port_env_lock();

    find_ok = find_env(key, env);

    /* unlock the ENV cache */
    ef_port_env_unlock();

    return find_ok;
}

/**
 * Get a blob ENV value by key name.
 *
 * @param key ENV name
 * @param value_buf ENV blob buffer
 * @param buf_len ENV blob buffer length
 * @param saved_value_len return the length of the value saved on the flash, 0: NOT found
 *
 * @return the actually get size on successful
 */
size_t ef_get_env_blob(const char *key, void *value_buf, size_t buf_len, size_t *saved_value_len)
{
    size_t read_len = 0;

    if (!init_ok) {
        EF_INFO("ENV isn't initialize OK.\r\n");
        return 0;
    }

    if (!key || !value_buf) {
        log_warn("key = %p, value_buf = %p\r\n", key, value_buf);
        return EF_ENV_ARG_ERR;
    }

    if (strlen(key) > EF_ENV_NAME_MAX) {
        log_warn("key err. %d > %d\r\n", strlen(key), EF_ENV_NAME_MAX);
        return EF_ENV_ARG_ERR;
    }
    /* lock the ENV cache */
    ef_port_env_lock();

    read_len = get_env(key, value_buf, buf_len, saved_value_len);

    /* unlock the ENV cache */
    ef_port_env_unlock();

    return read_len;
}

/**
 * Get an ENV value by key name.
 *
 * @note this function is NOT supported reentrant
 * @note this function is DEPRECATED
 *
 * @param key ENV name
 *
 * @return value
 */
char *ef_get_env(const char *key)
{
    static char value[EF_STR_ENV_VALUE_MAX_SIZE + 1];
    size_t get_size;

    if ((get_size = ef_get_env_blob(key, value, EF_STR_ENV_VALUE_MAX_SIZE, NULL)) > 0) {
        /* the return value must be string */
        if (ef_is_str((uint8_t *)value, get_size)) {
            value[get_size] = '\0';
            return value;
        } else {
            EF_INFO("Warning: The ENV value isn't string. Could not be returned\r\n");
            return NULL;
        }
    }

    return NULL;
}

/**
 * read the ENV value by ENV object
 *
 * @param env ENV object
 * @param value_buf the buffer for store ENV value
 * @param buf_len buffer length
 *
 * @return the actually read size on successful
 */
size_t ef_read_env_value(env_node_obj_t env, uint8_t *value_buf, size_t buf_len)
{
    size_t read_len = 0;

    EF_ASSERT(env);
    EF_ASSERT(value_buf);

    if (!init_ok) {
        EF_INFO("ENV isn't initialize OK.\r\n");
        return 0;
    }

    if (env->crc_is_ok) {
        /* lock the ENV cache */
        ef_port_env_lock();

        if (buf_len > env->value_len) {
            read_len = env->value_len;
        } else {
            read_len = buf_len;
        }

        ef_port_read(env->addr.value, (uint32_t *) value_buf, read_len);
        /* unlock the ENV cache */
        ef_port_env_unlock();
    }

    return read_len;
}

static EfErrCode write_env_hdr(uint32_t addr, env_hdr_data_t env_hdr) {
    EfErrCode result = EF_NO_ERR;
    /* write the status will by write granularity */
    result = write_status(addr, env_hdr->status_table, ENV_STATUS_NUM, ENV_PRE_WRITE);
    if (result != EF_NO_ERR) {
        return result;
    }
    /* write other header data */
    result = ef_port_write(addr + ENV_MAGIC_OFFSET, &env_hdr->magic, sizeof(struct env_hdr_data) - ENV_MAGIC_OFFSET);

    return result;
}

static EfErrCode format_sector(uint32_t addr, uint32_t combined_value)
{
    EfErrCode result = EF_NO_ERR;
    struct sector_hdr_data sec_hdr;

    EF_ASSERT(addr % SECTOR_SIZE == 0);

#ifdef EF_ENV_USING_CACHE
    /* An erase may change media even when it reports failure. */
    update_sector_cache(addr, addr + SECTOR_SIZE);
    for (size_t i = 0; i < EF_ENV_CACHE_TABLE_SIZE; ++i) {
        if (env_cache_table[i].addr >= addr && env_cache_table[i].addr - addr < SECTOR_SIZE) {
            env_cache_table[i].addr = FAILED_ADDR;
            env_cache_table[i].active = 0;
        }
    }
#endif
    result = ef_port_erase(addr, SECTOR_SIZE);
    if (result == EF_NO_ERR) {
        result = checked_erased_range(addr, SECTOR_SIZE);
        if (result != EF_NO_ERR) return result == EF_ENV_CORRUPT ? EF_ERASE_ERR : result;
        /* initialize the header data */
        memset(&sec_hdr, 0xFF, sizeof(struct sector_hdr_data));
        set_status(sec_hdr.status_table.store, SECTOR_STORE_STATUS_NUM, SECTOR_STORE_EMPTY);
        set_status(sec_hdr.status_table.dirty, SECTOR_DIRTY_STATUS_NUM, SECTOR_DIRTY_FALSE);
        sec_hdr.magic = SECTOR_MAGIC_WORD;
        sec_hdr.combined = combined_value;
        sec_hdr.gc_flag = 0xFFFFFFFF;
        /* save the header */
        result = ef_port_write(addr, (uint32_t *)&sec_hdr, sizeof(struct sector_hdr_data));
        if (result == EF_NO_ERR) {
            struct sector_hdr_data verified;
            if (ef_port_read(addr, (uint32_t *)&verified, sizeof(verified)) != EF_NO_ERR) return EF_READ_ERR;
            if (memcmp(&sec_hdr, &verified, sizeof(verified)) != 0) return EF_WRITE_ERR;
        }
    }

    return result;
}

static EfErrCode update_sec_status(sector_meta_data_t sector, size_t new_env_len, bool *is_full)
{
    uint8_t status_table[STORE_STATUS_TABLE_SIZE];
    EfErrCode result = EF_NO_ERR;
    /* change the current sector status */
    if (sector->status.store == SECTOR_STORE_EMPTY) {
        /* change the sector status to using */
        result = write_status(sector->addr, status_table, SECTOR_STORE_STATUS_NUM, SECTOR_STORE_USING);
    } else if (sector->status.store == SECTOR_STORE_USING) {
        /* check remain size */
        if (sector->remain < EF_SEC_REMAIN_THRESHOLD || sector->remain - new_env_len < EF_SEC_REMAIN_THRESHOLD) {
            /* change the sector status to full */
            result = write_status(sector->addr, status_table, SECTOR_STORE_STATUS_NUM, SECTOR_STORE_FULL);

#ifdef EF_ENV_USING_CACHE
            /* delete the sector cache */
            update_sector_cache(sector->addr, sector->addr + SECTOR_SIZE);
#endif /* EF_ENV_USING_CACHE */

            if (is_full) {
                *is_full = true;
            }
        } else if (is_full) {
            *is_full = false;
        }
    }

    return result;
}

static EfErrCode sector_iterator(sector_meta_data_t sector, sector_store_status_t status, void *arg1, void *arg2,
        bool (*callback)(sector_meta_data_t sector, void *arg1, void *arg2), bool traversal_env) {
    uint32_t sec_addr;

    /* search all sectors */
    sector->addr = FAILED_ADDR;
    while ((sec_addr = get_next_sector_addr(sector)) != FAILED_ADDR) {
        EfErrCode result = read_sector_meta_data(sec_addr, sector, false);
        if (result == EF_READ_ERR) return result;
        if (status == SECTOR_STORE_UNUSED || status == sector->status.store) {
            if (traversal_env) {
                result = read_sector_meta_data(sec_addr, sector, traversal_env);
                if (result == EF_READ_ERR) return result;
            }
            /* iterator is interrupted when callback return true */
            if (callback && callback(sector, arg1, arg2)) {
                return EF_NO_ERR;
            }
        }
    }
    return EF_NO_ERR;
}

static bool sector_statistics_cb(sector_meta_data_t sector, void *arg1, void *arg2)
{
    size_t *empty_sector = arg1, *using_sector = arg2;

    if (sector->check_ok && sector->status.store == SECTOR_STORE_EMPTY) {
        (*empty_sector)++;
    } else if (sector->check_ok && sector->status.store == SECTOR_STORE_USING) {
        (*using_sector)++;
    }

    return false;
}

static bool alloc_env_cb(sector_meta_data_t sector, void *arg1, void *arg2)
{
    size_t *env_size = arg1;
    uint32_t *empty_env = arg2;

    /* 1. sector has space
     * 2. the NO dirty sector
     * 3. the dirty sector only when the gc_request is false */
    if (sector->check_ok && sector->remain > *env_size
            && ((sector->status.dirty == SECTOR_DIRTY_FALSE)
                    || (sector->status.dirty == SECTOR_DIRTY_TRUE && !gc_request))) {
        *empty_env = sector->empty_env;
        return true;
    }

    return false;
}

static uint32_t alloc_env(sector_meta_data_t sector, size_t env_size)
{
    uint32_t empty_env = FAILED_ADDR;
    size_t empty_sector = 0, using_sector = 0;

    /* sector status statistics */
    if (sector_iterator(sector, SECTOR_STORE_UNUSED, &empty_sector, &using_sector,
                        sector_statistics_cb, false) != EF_NO_ERR) return FAILED_ADDR;
    if (using_sector > 0) {
        /* alloc the ENV from the using status sector first */
        if (sector_iterator(sector, SECTOR_STORE_USING, &env_size, &empty_env,
                            alloc_env_cb, true) != EF_NO_ERR) return FAILED_ADDR;
    }
    if (empty_sector > 0 && empty_env == FAILED_ADDR) {
        if (empty_sector > EF_GC_EMPTY_SEC_THRESHOLD || gc_request) {
            if (sector_iterator(sector, SECTOR_STORE_EMPTY, &env_size, &empty_env,
                                alloc_env_cb, true) != EF_NO_ERR) return FAILED_ADDR;
        } else {
            /* no space for new ENV now will GC and retry */
            EF_DEBUG("Trigger a GC check after alloc ENV failed.\r\n");
            gc_request = true;
        }
    }

    return empty_env;
}

static EfErrCode del_env(const char *key, env_node_obj_t old_env, bool complete_del) {
    EfErrCode result = EF_NO_ERR;
    uint32_t dirty_status_addr;

//#if (ENV_STATUS_TABLE_SIZE >= DIRTY_STATUS_TABLE_SIZE)
/**
 * enum value is invalid to determine during preprocess phase.
 * Previous implementation makes determination to be always true and makes build error if -Wundef and -Werror are enabled.
 * Now hard-code it to #if 1.
 * 
 * Any change on ENV_STATUS_NUM or SECTOR_DIRTY_STATUS_NUM should re-evaluate the size of status_table.
 * */
#if 1
    uint8_t status_table[ENV_STATUS_TABLE_SIZE];
#else
    uint8_t status_table[DIRTY_STATUS_TABLE_SIZE];
#endif

    struct env_node_obj env;

    /* need find ENV */
    if (!old_env) {
        //struct env_node_obj env;
        /* find ENV */
        result = lookup_env(key, &env);
        if (result == EF_NO_ERR) {
            old_env = &env;
        } else {
            EF_DEBUG("Not found '%s' in ENV.\r\n", key);
            return result == EF_ENV_NOT_FOUND ? EF_ENV_NAME_ERR : result;
        }
    }
    /* change and save the new status */
    if (!complete_del) {
        result = write_status(old_env->addr.start, status_table, ENV_STATUS_NUM, ENV_PRE_DELETE);
    } else {
        result = write_status(old_env->addr.start, status_table, ENV_STATUS_NUM, ENV_DELETED);

        if (result == EF_NO_ERR) {
#ifdef EF_ENV_USING_CACHE
            /* Invalidate this record, not a newer replacement with the same
             * key. Do not retain state across separate delete operations. */
            for (size_t i = 0; i < EF_ENV_CACHE_TABLE_SIZE; ++i) {
                if (env_cache_table[i].addr == old_env->addr.start) {
                    env_cache_table[i].addr = FAILED_ADDR;
                    env_cache_table[i].active = 0;
                }
            }
#endif /* EF_ENV_USING_CACHE */
        }
    }

    dirty_status_addr = EF_ALIGN_DOWN(old_env->addr.start, SECTOR_SIZE) + SECTOR_DIRTY_OFFSET;
    /* read and change the sector dirty status */
    if (result == EF_NO_ERR) {
        uint32_t words[(DIRTY_STATUS_TABLE_SIZE + 3) / 4];
        if (ef_port_read(dirty_status_addr, words, DIRTY_STATUS_TABLE_SIZE) != EF_NO_ERR) return EF_READ_ERR;
        if (get_status((uint8_t *)words, SECTOR_DIRTY_STATUS_NUM) == SECTOR_DIRTY_FALSE) {
            result = write_status(dirty_status_addr, status_table, SECTOR_DIRTY_STATUS_NUM, SECTOR_DIRTY_TRUE);
        }
    }

    return result;
}

/*
 * move the ENV to new space
 */
static EfErrCode move_env(env_node_obj_t env)
{
    EfErrCode result = EF_NO_ERR;
    uint32_t env_addr;
    struct sector_meta_data sector;

    /* prepare to delete the current ENV */
    if (env->status == ENV_WRITE) {
        result = del_env(NULL, env, false);
        if (result != EF_NO_ERR) {
            return result;
        }
    }

    if ((env_addr = alloc_env(&sector, env->len)) != FAILED_ADDR) {
        if (in_recovery_check) {
            struct env_node_obj env_bak;
            char name[EF_ENV_NAME_MAX + 1] = { 0 };
            strncpy(name, env->name, env->name_len);
            /* check the ENV in flash is already create success */
            result = lookup_env_no_cache(name, &env_bak);
            if (result == EF_NO_ERR) {
                /* already create success, don't need to duplicate */
                result = EF_NO_ERR;
                goto __exit;
            }
            if (result != EF_ENV_NOT_FOUND) return result;
            result = EF_NO_ERR;
        }
    } else {
        return EF_ENV_FULL;
    }
    /* start move the ENV */
    {
        uint32_t buf[8], verify[8];
        size_t len, size, env_len = env->len;

#ifdef EF_ENV_USING_CACHE
        /* A failed program may still change Flash. Drop the old free pointer
         * before writing so the next allocation scans any partial record.
         * Publish the new free pointer only after the move is committed. */
        update_sector_cache(sector.addr, sector.addr + SECTOR_SIZE);
#endif

        /* update the new ENV sector status first */
        result = update_sec_status(&sector, env->len, NULL);
        if (result != EF_NO_ERR) {
            return result;
        }
        struct sector_meta_data committed_sector;
        result = read_sector_meta_data(sector.addr, &committed_sector, false);
        if (result != EF_NO_ERR) return result;
        if (committed_sector.status.store != SECTOR_STORE_USING &&
                committed_sector.status.store != SECTOR_STORE_FULL) return EF_WRITE_ERR;

        result = checked_write_node_status(env_addr, ENV_PRE_WRITE);
        if (result != EF_NO_ERR) {
            return result;
        }
        env_len -= ENV_MAGIC_OFFSET;
        for (len = 0, size = 0; len < env_len; len += size) {
            if (len + sizeof(buf) < env_len) {
                size = sizeof(buf);
            } else {
                size = env_len - len;
            }
            result = ef_port_read(env->addr.start + ENV_MAGIC_OFFSET + len, (uint32_t *) buf, EF_WG_ALIGN(size));
            if (result != EF_NO_ERR) {
                return result;
            }
            result = ef_port_write(env_addr + ENV_MAGIC_OFFSET + len, (uint32_t *) buf, size);
            if (result != EF_NO_ERR) {
                return result;
            }
            /* Do not commit/delete the source based only on program ACK. */
            result = ef_port_read(env_addr + ENV_MAGIC_OFFSET + len, verify, size);
            if (result != EF_NO_ERR) return result;
            if (memcmp(buf, verify, size) != 0) return EF_WRITE_ERR;
        }
        result = checked_write_node_status(env_addr, ENV_WRITE);
        if (result != EF_NO_ERR) {
            return result;
        }

#ifdef EF_ENV_USING_CACHE
        update_sector_cache(EF_ALIGN_DOWN(env_addr, SECTOR_SIZE),
                env_addr + ENV_HDR_DATA_SIZE + EF_WG_ALIGN(env->name_len) + EF_WG_ALIGN(env->value_len));
        update_env_cache(env->name, env->name_len, env_addr);
#endif /* EF_ENV_USING_CACHE */
    }

    EF_DEBUG("Moved the ENV (%.*s) from 0x%08X to 0x%08X.\r\n", env->name_len, env->name, env->addr.start, env_addr);

__exit:
    return del_env(NULL, env, true);
}

static uint32_t new_env(sector_meta_data_t sector, size_t env_size)
{
    bool already_gc = false;
    uint32_t empty_env = FAILED_ADDR;

__retry:

    if ((empty_env = alloc_env(sector, env_size)) == FAILED_ADDR) {
        if (gc_request && !already_gc) {
            EF_DEBUG("Warning: Alloc an ENV (size %d) failed when new ENV. Now will GC then retry.\n", env_size);
            if (gc_collect() != EF_NO_ERR) return FAILED_ADDR;
            already_gc = true;
            goto __retry;
        } else {
            EF_DEBUG("Error: Alloc an ENV (size %d) failed after GC. ENV full.\n", env_size);
            gc_request = false;
        }
    }

    return empty_env;
}

static uint32_t new_env_by_kv(sector_meta_data_t sector, size_t key_len, size_t buf_len)
{
    size_t env_len = ENV_HDR_DATA_SIZE + EF_WG_ALIGN(key_len) + EF_WG_ALIGN(buf_len);

    return new_env(sector, env_len);
}

static bool gc_check_cb(sector_meta_data_t sector, void *arg1, void *arg2)
{
    size_t *empty_sec = arg1;

    if (sector->check_ok) {
        *empty_sec = *empty_sec + 1;
    }

    return false;

}

static bool do_gc(sector_meta_data_t sector, void *arg1, void *arg2)
{
    struct env_node_obj env;
    uint32_t *first_gc = arg1;
    EfErrCode local_result = EF_NO_ERR;
    EfErrCode *result = arg2 ? arg2 : &local_result;

    if (sector->check_ok && 
       (sector->status.dirty == SECTOR_DIRTY_TRUE || sector->status.dirty == SECTOR_DIRTY_GC) &&
       (sector->gc_flag != DEFAULT_GC_FLAG)) {
        uint8_t status_table[DIRTY_STATUS_TABLE_SIZE];
        /* change the sector status to GC */
        *result = write_status(sector->addr + SECTOR_DIRTY_OFFSET, status_table, SECTOR_DIRTY_STATUS_NUM, SECTOR_DIRTY_GC);
        if (*result != EF_NO_ERR) return true;
        /* search all ENV */
        env.addr.start = FAILED_ADDR;
        while ((env.addr.start = get_next_env_addr_with_error(sector, &env, result)) != FAILED_ADDR) {
            *result = read_env(&env);
            if (*result != EF_NO_ERR) {
                /* Do not erase a source sector whose contents could not be
                 * verified. Returning true stops the collection iterator. */
                return true;
            }
            if (env.crc_is_ok && (env.status == ENV_WRITE || env.status == ENV_PRE_DELETE)) {
                /* move the ENV to new space */
                *result = move_env(&env);
                if (*result != EF_NO_ERR) {
                    EF_DEBUG("Error: Moved the ENV (%.*s) for GC failed.\r\n", env.name_len, env.name);
                    /* A reported move failure must not erase the source sector. */
                    return true;
                }
            }
        }
        if (*result != EF_NO_ERR) return true;
        *result = format_sector(sector->addr, SECTOR_NOT_COMBINED);
        if (*result != EF_NO_ERR) return true;
        EF_DEBUG("Collect a sector @0x%08X\r\n", sector->addr);
        if(*first_gc == 1) {
            *first_gc = 0;
        } else {
            return true;
        }
    }

    return false;
}

static bool read_hdr_gc(sector_meta_data_t sector, void *arg1, void *arg2){
    uint32_t flag;
    EfErrCode result = ef_port_read(sector->addr+SECTOR_HDR_GC_FLAG_OFFSET, &flag, sizeof(flag));
    if (result != EF_NO_ERR) {
        if (arg2) *(EfErrCode *)arg2 = EF_READ_ERR;
        return true;
    }
    sector_hdr_gc_flag = flag;
    if(sector_hdr_gc_flag == DEFAULT_GC_FLAG){
        return false;
    }
    else{
        EF_DEBUG("Have sector to gc @0x%08x\r\n", sector->addr);
        return true;
    }
}

static bool write_hdr_gc(sector_meta_data_t sector, void *arg1, void *arg2){
    uint32_t data = 0;

    if (sector->check_ok && 
       (sector->status.dirty == SECTOR_DIRTY_TRUE || sector->status.dirty == SECTOR_DIRTY_GC)) {
        EF_DEBUG("Write sector hdr gc flag @0x%08x\r\n", sector->addr);
        EfErrCode result = ef_port_write(sector->addr+SECTOR_HDR_GC_FLAG_OFFSET, &data, sizeof(data));
        if (result != EF_NO_ERR) {
            if (arg2) *(EfErrCode *)arg2 = result;
            return true;
        }
    }

    return false;
}
/*
 * The GC will be triggered on the following scene:
 * 1. alloc an ENV when the flash not has enough space
 * 2. write an ENV then the flash not has enough space
 */
static EfErrCode gc_collect_internal(void *arg)
{
    struct sector_meta_data sector;
    size_t empty_sec = 0;
    uint32_t first_gc = 0;
    EfErrCode result, callback_result = EF_NO_ERR;

    /* GC check the empty sector number */
    result = sector_iterator(&sector, SECTOR_STORE_EMPTY, &empty_sec, NULL, gc_check_cb, false);
    if (result != EF_NO_ERR) goto done;

    /* do GC collect */
    EF_DEBUG("The remain empty sector is %d, GC threshold is %d.\r\n", empty_sec, EF_GC_EMPTY_SEC_THRESHOLD);
    if (empty_sec <= EF_GC_EMPTY_SEC_THRESHOLD || sector_hdr_gc_flag != DEFAULT_GC_FLAG) {
        if((empty_sec <= EF_GC_EMPTY_SEC_THRESHOLD) && sector_hdr_gc_flag == DEFAULT_GC_FLAG) {
            result = sector_iterator(&sector, SECTOR_STORE_UNUSED, NULL, &callback_result, write_hdr_gc, false);
            if (result == EF_NO_ERR) result = callback_result;
            if (result != EF_NO_ERR) goto done;
            first_gc = 1;
        }
            
        result = sector_iterator(&sector, SECTOR_STORE_UNUSED, &first_gc, &callback_result, do_gc, false);
        if (result == EF_NO_ERR) result = callback_result;

    }

done:
    gc_request = false;
    return result;
}

static EfErrCode gc_collect(void)
{
    return gc_collect_internal(NULL);
}

static EfErrCode align_write(uint32_t addr, const uint32_t *buf, size_t size)
{
    EfErrCode result = EF_NO_ERR;
    size_t align_remain;

#if (EF_WRITE_GRAN / 8 > 0)
    uint8_t align_data[EF_WRITE_GRAN / 8];
    size_t align_data_size = sizeof(align_data);
#else
    /* For compatibility with C89 */
    uint8_t align_data_u8, *align_data = &align_data_u8;
    size_t align_data_size = 1;
#endif

    memset(align_data, 0xFF, align_data_size);
    result = ef_port_write(addr, buf, EF_WG_ALIGN_DOWN(size));

    align_remain = size - EF_WG_ALIGN_DOWN(size);
    if (result == EF_NO_ERR && align_remain) {
        memcpy(align_data, (uint8_t *)buf + EF_WG_ALIGN_DOWN(size), align_remain);
        result = ef_port_write(addr + EF_WG_ALIGN_DOWN(size), (uint32_t *) align_data, align_data_size);
    }

    return result;
}

static EfErrCode create_env_blob_internal(sector_meta_data_t sector, const char *key, const void *value,
                                         size_t len, bool commit)
{
    EfErrCode result = EF_NO_ERR;
    struct env_hdr_data env_hdr;
    bool is_full = false;
    uint32_t env_addr = sector->empty_env;

    if (strlen(key) > EF_ENV_NAME_MAX) {
        EF_INFO("Error: The ENV name length is more than %d\r\n", EF_ENV_NAME_MAX);
        return EF_ENV_NAME_ERR;
    }

    memset(&env_hdr, 0xFF, sizeof(struct env_hdr_data));
    env_hdr.magic = ENV_MAGIC_WORD;
    env_hdr.name_len = strlen(key);
    env_hdr.value_len = len;
    env_hdr.len = ENV_HDR_DATA_SIZE + EF_WG_ALIGN(env_hdr.name_len) + EF_WG_ALIGN(env_hdr.value_len);

    if (env_hdr.len > SECTOR_SIZE - SECTOR_HDR_DATA_SIZE) {
        EF_INFO("Error: The ENV size is too big\r\n");
        return EF_ENV_FULL;
    }

    if (env_addr != FAILED_ADDR || (env_addr = new_env(sector, env_hdr.len)) != FAILED_ADDR) {
        size_t align_remain;
#ifdef EF_ENV_USING_CACHE
        /* A failed header/program operation may consume this free slot. */
        update_sector_cache(sector->addr, sector->addr + SECTOR_SIZE);
#endif
        /* update the sector status */
        if (result == EF_NO_ERR) {
            result = update_sec_status(sector, env_hdr.len, &is_full);
        }
        if (result == EF_NO_ERR) {
            uint8_t ff = 0xFF;
            /* start calculate CRC32 */
            env_hdr.crc32 = ef_calc_crc32(0, &env_hdr.name_len, ENV_HDR_DATA_SIZE - ENV_NAME_LEN_OFFSET);
            env_hdr.crc32 = ef_calc_crc32(env_hdr.crc32, key, env_hdr.name_len);
            align_remain = EF_WG_ALIGN(env_hdr.name_len) - env_hdr.name_len;
            while (align_remain--) {
                env_hdr.crc32 = ef_calc_crc32(env_hdr.crc32, &ff, 1);
            }
            env_hdr.crc32 = ef_calc_crc32(env_hdr.crc32, value, env_hdr.value_len);
            align_remain = EF_WG_ALIGN(env_hdr.value_len) - env_hdr.value_len;
            while (align_remain--) {
                env_hdr.crc32 = ef_calc_crc32(env_hdr.crc32, &ff, 1);
            }
            /* write ENV header data */
            result = write_env_hdr(env_addr, &env_hdr);

        }
        /* write key name */
        if (result == EF_NO_ERR) {
            result = align_write(env_addr + ENV_HDR_DATA_SIZE, (uint32_t *) key, env_hdr.name_len);
        }
        /* write value */
        if (result == EF_NO_ERR) {
            result = align_write(env_addr + ENV_HDR_DATA_SIZE + EF_WG_ALIGN(env_hdr.name_len), value,
                    env_hdr.value_len);
        }
        /* change the ENV status to ENV_WRITE */
        if (!commit) return result;
        if (result == EF_NO_ERR) {
            result = write_status(env_addr, env_hdr.status_table, ENV_STATUS_NUM, ENV_WRITE);
        }
#ifdef EF_ENV_USING_CACHE
        /* Only committed values may become visible through the key cache. */
        if (result == EF_NO_ERR) {
            if (!is_full) {
                update_sector_cache(sector->addr, env_addr + env_hdr.len);
            }
            update_env_cache(key, env_hdr.name_len, env_addr);
        }
#endif /* EF_ENV_USING_CACHE */
        /* trigger GC collect when current sector is full */
        if (result == EF_NO_ERR && is_full) {
            EF_DEBUG("Trigger a GC check after created ENV.\r\n");
            gc_request = true;
        }
    } else {
        result = EF_ENV_FULL;
    }

    return result;
}

static EfErrCode create_env_blob(sector_meta_data_t sector, const char *key, const void *value, size_t len)
{
    return create_env_blob_internal(sector, key, value, len, true);
}

static EfErrCode checked_compare_bytes(uint32_t addr, const void *expected, size_t size)
{
    const uint8_t *bytes = expected;
    while (size) {
        uint32_t words[8];
        size_t chunk = size < sizeof(words) ? size : sizeof(words);
        if (ef_port_read(addr, words, chunk) != EF_NO_ERR) return EF_READ_ERR;
        if (memcmp(words, bytes, chunk)) return EF_ENV_CORRUPT;
        addr += chunk;
        bytes += chunk;
        size -= chunk;
    }
    return EF_NO_ERR;
}

static EfErrCode checked_verify_value(uint32_t addr, const char *key, size_t key_len,
                                      const void *value, size_t len, env_status_t status)
{
    struct env_hdr_data header;
    if (ef_port_read(addr, (uint32_t *)&header, sizeof(header)) != EF_NO_ERR) return EF_READ_ERR;
    if (header.magic != ENV_MAGIC_WORD || header.name_len != key_len || header.value_len != len ||
            header.len != ENV_HDR_DATA_SIZE + EF_WG_ALIGN(key_len) + EF_WG_ALIGN(len) ||
            get_status(header.status_table, ENV_STATUS_NUM) != status) return EF_ENV_CORRUPT;
    EfErrCode result = checked_compare_bytes(addr + ENV_HDR_DATA_SIZE, key, key_len);
    if (result == EF_NO_ERR) {
        result = checked_compare_bytes(addr + ENV_HDR_DATA_SIZE + EF_WG_ALIGN(key_len), value, len);
    }
    return result;
}

static EfErrCode checked_write_node_status(uint32_t addr, env_status_t status)
{
    uint32_t words[(ENV_STATUS_TABLE_SIZE + 3) / 4];
    EfErrCode result = write_status(addr, (uint8_t *)words, ENV_STATUS_NUM, status);
#ifdef EF_ENV_USING_CACHE
    for (size_t i = 0; i < EF_ENV_CACHE_TABLE_SIZE; ++i) {
        if (env_cache_table[i].addr == addr) {
            env_cache_table[i].addr = FAILED_ADDR;
            env_cache_table[i].active = 0;
        }
    }
#endif
    if (result != EF_NO_ERR) return result;
    if (ef_port_read(addr, words, ENV_STATUS_TABLE_SIZE) != EF_NO_ERR) return EF_READ_ERR;
    return get_status((uint8_t *)words, ENV_STATUS_NUM) == status ? EF_NO_ERR : EF_WRITE_ERR;
}

EfErrCode ef_env_set_checked(const char *key, const void *value, size_t len)
{
    size_t key_len = 0;
    const uint8_t empty = 0;
    if (!key || (!value && len) || len > EF_STR_ENV_VALUE_MAX_SIZE) return EF_ENV_ARG_ERR;
    while (key_len <= EF_ENV_NAME_MAX && key[key_len]) ++key_len;
    if (!key_len || key_len > EF_ENV_NAME_MAX) return EF_ENV_ARG_ERR;
    size_t record_len = ENV_HDR_DATA_SIZE + EF_WG_ALIGN(key_len) + EF_WG_ALIGN(len);
    if (record_len > SECTOR_SIZE - SECTOR_HDR_DATA_SIZE) return EF_ENV_ARG_ERR;
    if (!init_ok) return EF_ENV_INIT_FAILED;
    if (!value) value = &empty;
    ef_port_env_lock();
    EfErrCode result = EF_ENV_INIT_FAILED;
    struct env_node_obj old = {0}, predecessor = {0}, verify = {0};
    struct sector_meta_data space;
    bool had_old = false;
    bool collected = false;
    struct checked_gc_info gc_info;
    if (!init_ok) goto done;
rescan:
    for (;;) {
        result = checked_scan_env_internal(key, key_len, &old, &predecessor, &space, record_len, &gc_info, NULL);
        if (result != EF_NO_ERR && result != EF_ENV_NOT_FOUND) goto done;
        had_old = result == EF_NO_ERR;
        if (!had_old || old.status != ENV_WRITE || predecessor.addr.start == FAILED_ADDR) break;
        result = checked_delete_record(predecessor.addr.start);
        if (result != EF_NO_ERR) goto done;
    }
    if (space.empty_env == FAILED_ADDR) goto reclaim;
    /* Preserve the final empty reserve for relocating live records. */
    if (space.status.store == SECTOR_STORE_EMPTY) {
        size_t empty_sectors = 0;
        for (uint32_t addr = env_start_addr; addr < env_start_addr + ENV_AREA_SIZE; addr += SECTOR_SIZE) {
            struct sector_hdr_data header;
            if (ef_port_read(addr, (uint32_t *)&header, sizeof(header)) != EF_NO_ERR) {
                result = EF_READ_ERR; goto done;
            }
            if (get_status(header.status_table.store, SECTOR_STORE_STATUS_NUM) == SECTOR_STORE_EMPTY &&
                    checked_sector_can_allocate(&header)) ++empty_sectors;
        }
        if (empty_sectors <= EF_GC_EMPTY_SEC_THRESHOLD) goto reclaim;
    }
    result = create_env_blob_internal(&space, key, value, len, false);
    if (result != EF_NO_ERR) goto done;
    result = checked_find_env(key, key_len, &verify, NULL); /* Includes staged CRC validation. */
    if (result != EF_NO_ERR && result != EF_ENV_NOT_FOUND) goto done;
    result = checked_verify_value(space.empty_env, key, key_len, value, len, ENV_PRE_WRITE);
    if (result != EF_NO_ERR) goto done;
    if (had_old && old.status == ENV_WRITE) {
        result = checked_write_node_status(old.addr.start, ENV_PRE_DELETE);
        if (result != EF_NO_ERR) goto done;
    }
    result = checked_write_node_status(space.empty_env, ENV_WRITE);
    if (result != EF_NO_ERR) goto done;
    result = checked_find_env(key, key_len, &verify, NULL);
    if (result != EF_NO_ERR) goto done;
    result = checked_verify_value(space.empty_env, key, key_len, value, len, ENV_WRITE);
    if (result != EF_NO_ERR) goto done;
    if (had_old) {
        result = checked_delete_record(old.addr.start);
        if (result != EF_NO_ERR) goto done;
    }
    result = checked_find_env(key, key_len, &verify, NULL);
    if (result == EF_NO_ERR && verify.addr.start != space.empty_env) result = EF_ENV_CORRUPT;
#ifdef EF_ENV_USING_CACHE
    if (result == EF_NO_ERR) update_env_cache(key, key_len, space.empty_env);
#endif
    goto done;
reclaim:
    /* Reclaim only a completely validated, settled store with actual garbage
     * and a relocation reserve. Pending recovery must not enter this path. */
    result = EF_ENV_FULL;
    if (collected || !gc_info.settled || !gc_info.empty_sectors || !gc_info.garbage_bytes) goto done;
    collected = true;
    gc_request = true;
    sector_hdr_gc_flag = gc_info.queued_gc ? 0 : DEFAULT_GC_FLAG;
    result = gc_collect();
    if (result != EF_NO_ERR) goto done;
    /* GC may relocate any key: discard previous addresses and space plans. */
    goto rescan;
done:
    ef_port_env_unlock();
    return result;
}

/**
 * Delete an ENV.
 *
 * @param key ENV name
 *
 * @return result
 */
EfErrCode ef_del_env(const char *key)
{
    EfErrCode result = EF_NO_ERR;

    if (!init_ok) {
        EF_INFO("Error: ENV isn't initialize OK.\r\n");
        return EF_ENV_INIT_FAILED;
    }

    if (!key || (strlen(key) > EF_ENV_NAME_MAX)) {
        log_warn("key = %p\r\n", key);
        return EF_NO_ERR;
    }

    /* lock the ENV cache */
    ef_port_env_lock();

    result = del_env(key, NULL, true);

    /* unlock the ENV cache */
    ef_port_env_unlock();

    return result;
}

/**
 * The same to ef_del_env on this mode
 * It's compatibility with older versions (less then V4.0).
 *
 * @note this function is DEPRECATED
 *
 * @param key ENV name
 *
 * @return result
 */
EfErrCode ef_del_and_save_env(const char *key)
{
    return ef_del_env(key);
}

static EfErrCode set_env(const char *key, const void *value_buf, size_t buf_len)
{
    EfErrCode result = EF_NO_ERR;
    static struct env_node_obj env;
    static struct sector_meta_data sector;
    bool env_is_found = false;

    if (value_buf == NULL) {
        result = del_env(key, NULL, true);
    } else {
        result = lookup_env(key, &env);
        if (result != EF_NO_ERR && result != EF_ENV_NOT_FOUND) return result;
        env_is_found = result == EF_NO_ERR;
        result = EF_NO_ERR;
        /* make sure the flash has enough space */
        if (new_env_by_kv(&sector, strlen(key), buf_len) == FAILED_ADDR) {
            return EF_ENV_FULL;
        }
        /* Allocation can collect garbage and relocate the old record. Never
         * use the preflight lookup address after that operation. */
        result = lookup_env(key, &env);
        if (result != EF_NO_ERR && result != EF_ENV_NOT_FOUND) return result;
        env_is_found = result == EF_NO_ERR;
        result = EF_NO_ERR;
        /* prepare to delete the old ENV */
        if (env_is_found) {
            result = del_env(key, &env, false);
        }
        /* create the new ENV */
        if (result == EF_NO_ERR) {
            result = create_env_blob(&sector, key, value_buf, buf_len);
        }
        /* delete the old ENV */
        if (env_is_found && result == EF_NO_ERR) {
            result = del_env(key, &env, true);
        }
        if (result != EF_NO_ERR) return result;
        EfErrCode callback_result = EF_NO_ERR;
        result = sector_iterator(&sector, SECTOR_STORE_UNUSED, NULL, &callback_result, read_hdr_gc, false);
        if (result != EF_NO_ERR) return result;
        if (callback_result != EF_NO_ERR) return callback_result;
        /* process the GC after set ENV */
        if (gc_request || (sector_hdr_gc_flag != DEFAULT_GC_FLAG)) {
            result = gc_collect();
        }
    }

    return result;
}

/**
 * Set a blob ENV. If it value is NULL, delete it.
 * If not find it in flash, then create it.
 *
 * @param key ENV name
 * @param value ENV value
 * @param len ENV value length
 *
 * @return result
 */
EfErrCode ef_set_env_blob(const char *key, const void *value_buf, size_t buf_len)
{
    EfErrCode result = EF_NO_ERR;


    if (!init_ok) {
        EF_INFO("ENV isn't initialize OK.\r\n");
        return EF_ENV_INIT_FAILED;
    }

    if (!key || !value_buf) {
        log_warn("key = %p, value_buf = %p\r\n", key, value_buf);
        return EF_ENV_ARG_ERR;
    }

    if (strlen(key) > EF_ENV_NAME_MAX) {
        log_warn("key err. %d > %d\r\n", strlen(key), EF_ENV_NAME_MAX);
        return EF_ENV_ARG_ERR;
    }

    if (buf_len > EF_STR_ENV_VALUE_MAX_SIZE) {
        log_warn("buf_len err. %d > %d\r\n", buf_len, EF_STR_ENV_VALUE_MAX_SIZE);
        return EF_ENV_ARG_ERR;
    }

    /* lock the ENV cache */
    ef_port_env_lock();

    result = set_env(key, value_buf, buf_len);

    /* unlock the ENV cache */
    ef_port_env_unlock();

    return result;
}

/**
 * Set a string ENV. If it value is NULL, delete it.
 * If not find it in flash, then create it.
 *
 * @param key ENV name
 * @param value ENV value
 *
 * @return result
 */
EfErrCode ef_set_env(const char *key, const char *value)
{
    return ef_set_env_blob(key, value, strlen(value));
}

/**
 * The same to ef_set_env on this mode.
 * It's compatibility with older versions (less then V4.0).
 *
 * @note this function is DEPRECATED
 *
 * @param key ENV name
 * @param value ENV value
 *
 * @return result
 */
EfErrCode ef_set_and_save_env(const char *key, const char *value)
{
    return ef_set_env_blob(key, value, strlen(value));
}

/**
 * Save ENV to flash.
 *
 * @note this function is DEPRECATED
 */
EfErrCode ef_save_env(void)
{
    /* do nothing not cur mode */
    return EF_NO_ERR;
}

/**
 * ENV set default.
 *
 * @return result
 */
EfErrCode ef_env_set_default(void)
{
    EfErrCode result = EF_NO_ERR;
    uint32_t addr, i, value_len;
    struct sector_meta_data sector;

    EF_ASSERT(default_env_set);
    EF_ASSERT(default_env_set_size);

    /* lock the ENV cache */
    ef_port_env_lock();
    /* format all sectors */
    for (addr = env_start_addr; addr < env_start_addr + ENV_AREA_SIZE; addr += SECTOR_SIZE) {
        result = format_sector(addr, SECTOR_NOT_COMBINED);
        if (result != EF_NO_ERR) {
            goto __exit;
        }
    }
    /* create default ENV */
    for (i = 0; i < default_env_set_size; i++) {
        /* It seems to be a string when value length is 0.
         * This mechanism is for compatibility with older versions (less then V4.0). */
        if (default_env_set[i].value_len == 0) {
            value_len = strlen(default_env_set[i].value);
        } else {
            value_len = default_env_set[i].value_len;
        }
        sector.empty_env = FAILED_ADDR;
        result = create_env_blob(&sector, default_env_set[i].key, default_env_set[i].value, value_len);
        if (result != EF_NO_ERR) goto __exit;
        /* A successful program acknowledgement alone does not establish that
         * the default exists as a complete committed record. */
        struct env_node_obj verified;
        size_t key_len = strlen(default_env_set[i].key);
        result = checked_find_env(default_env_set[i].key, key_len, &verified, NULL);
        if (result == EF_ENV_NOT_FOUND) result = EF_WRITE_ERR;
        if (result != EF_NO_ERR) goto __exit;
        result = checked_verify_value(verified.addr.start, default_env_set[i].key, key_len,
                                      default_env_set[i].value, value_len, ENV_WRITE);
        if (result != EF_NO_ERR) goto __exit;
    }

__exit:
    /* unlock the ENV cache */
    ef_port_env_unlock();

    return result;
}

static bool print_env_cb(env_node_obj_t env, void *arg1, void *arg2)
{
    bool value_is_str = true, print_value = false;
    size_t *using_size = arg1;

    if (env->crc_is_ok) {
        /* calculate the total using flash size */
        *using_size += env->len;
        /* check ENV */
        if (env->status == ENV_WRITE) {
            ef_print("%.*s=", env->name_len, env->name);

            if (env->value_len < EF_STR_ENV_VALUE_MAX_SIZE ) {
                uint8_t buf[32];
                size_t len, size;
__reload:
                /* check the value is string */
                for (len = 0, size = 0; len < env->value_len; len += size) {
                    if (len + sizeof(buf) < env->value_len) {
                        size = sizeof(buf);
                    } else {
                        size = env->value_len - len;
                    }
                    ef_port_read(env->addr.value + len, (uint32_t *) buf, EF_WG_ALIGN(size));
                    if (print_value) {
                        ef_print("%.*s", size, buf);
                    } else if (!ef_is_str(buf, size)) {
                        value_is_str = false;
                        break;
                    }
                }
            } else {
                value_is_str = false;
            }
            if (value_is_str && !print_value) {
                print_value = true;
                goto __reload;
            } else if (!value_is_str) {
                ef_print("blob @0x%08X %dbytes", env->addr.value, env->value_len);
            }
            ef_print("\r\n");
        }
    }

    return false;
}


/**
 * Print ENV.
 */
void ef_print_env(void)
{
    struct env_node_obj env;
    size_t using_size = 0;

    if (!init_ok) {
        EF_INFO("ENV isn't initialize OK.\r\n");
        return;
    }

    /* lock the ENV cache */
    ef_port_env_lock();

    env_iterator(&env, &using_size, NULL, print_env_cb);

    ef_print("\r\nmode: next generation\r\n");
    ef_print("size: %lu/%lu bytes.\r\n", using_size + (SECTOR_NUM - EF_GC_EMPTY_SEC_THRESHOLD) * SECTOR_HDR_DATA_SIZE,
            ENV_AREA_SIZE - SECTOR_SIZE * EF_GC_EMPTY_SEC_THRESHOLD);

    /* unlock the ENV cache */
    ef_port_env_unlock();
}

/* Extended API */
void ef_print_env_cb(print_env_cb_t cb)
{
    struct env_node_obj env;
    size_t using_size = 0;

    if (!init_ok) {
        EF_INFO("ENV isn't initialize OK.\r\n");
        return;
    }

    /* lock the ENV cache */
    ef_port_env_lock();

    env_iterator(&env, &using_size, NULL, cb);

    ef_print("\r\nmode: next generation\r\n");
    ef_print("size: %lu/%lu bytes.\r\n", using_size + (SECTOR_NUM - EF_GC_EMPTY_SEC_THRESHOLD) * SECTOR_HDR_DATA_SIZE,
            ENV_AREA_SIZE - SECTOR_SIZE * EF_GC_EMPTY_SEC_THRESHOLD);

    /* unlock the ENV cache */
    ef_port_env_unlock();
}

#ifdef EF_ENV_AUTO_UPDATE
/*
 * Auto update ENV to latest default when current EF_ENV_VER_NUM is changed.
 */
static void env_auto_update(void)
{
    size_t saved_ver_num, setting_ver_num = EF_ENV_VER_NUM;

    if (get_env(VER_NUM_ENV_NAME, &saved_ver_num, sizeof(size_t), NULL) > 0) {
        /* check version number */
        if (saved_ver_num != setting_ver_num) {
            struct env_node_obj env;
            size_t i, value_len;
            struct sector_meta_data sector;
            EF_DEBUG("Update the ENV from version %d to %d.\r\n", saved_ver_num, setting_ver_num);
            for (i = 0; i < default_env_set_size; i++) {
                /* add a new ENV when it's not found */
                EfErrCode lookup_result = lookup_env(default_env_set[i].key, &env);
                if (lookup_result != EF_NO_ERR && lookup_result != EF_ENV_NOT_FOUND) return;
                if (lookup_result == EF_ENV_NOT_FOUND) {
                    /* It seems to be a string when value length is 0.
                     * This mechanism is for compatibility with older versions (less then V4.0). */
                    if (default_env_set[i].value_len == 0) {
                        value_len = strlen(default_env_set[i].value);
                    } else {
                        value_len = default_env_set[i].value_len;
                    }
                    sector.empty_env = FAILED_ADDR;
                    create_env_blob(&sector, default_env_set[i].key, default_env_set[i].value, value_len);
                }
            }
        } else {
            /* version number not changed now return */
            return;
        }
    }

    set_env(VER_NUM_ENV_NAME, &setting_ver_num, sizeof(size_t));
}
#endif /* EF_ENV_AUTO_UPDATE */

static bool check_sec_hdr_cb(sector_meta_data_t sector, void *arg1, void *arg2)
{
    if (!sector->check_ok) {
        size_t *failed_count = arg1;

        EF_INFO("Warning: Sector header check failed. Format this sector (0x%08x).\r\n", sector->addr);
        (*failed_count) ++;
        *(EfErrCode *)arg2 = format_sector(sector->addr, SECTOR_NOT_COMBINED);
        return *(EfErrCode *)arg2 != EF_NO_ERR;
    }

    return false;
}

static bool check_and_recovery_gc_cb(sector_meta_data_t sector, void *arg1, void *arg2)
{
    if (sector->check_ok && sector->status.dirty == SECTOR_DIRTY_GC) {
        /* make sure the GC request flag to true */
        gc_request = true;
        /* resume the GC operate */
        *(EfErrCode *)arg1 = gc_collect();
        return *(EfErrCode *)arg1 != EF_NO_ERR;
    }

    return false;
}

static bool check_and_recovery_env_cb(env_node_obj_t env, void *arg1, void *arg2)
{
    EfErrCode *recovery_result = arg1;
    /* recovery the prepare deleted ENV */
    if (env->crc_is_ok && env->status == ENV_PRE_DELETE) {
        EF_INFO("Found an ENV (%.*s) which has changed value failed. Now will recovery it.\r\n", env->name_len, env->name);
        /* recovery the old ENV */
        *recovery_result = move_env(env);
        if (*recovery_result == EF_NO_ERR) {
            EF_DEBUG("Recovery the ENV successful.\r\n");
        } else {
            EF_DEBUG("Warning: ENV recovery failed (size %d).\r\n", env->len);
            return true;
        }
    } else if (env->status == ENV_PRE_WRITE) {
        /* the ENV has not write finish, change the status to error */
        *recovery_result = checked_write_node_status(env->addr.start, ENV_ERR_HDR);
        return *recovery_result != EF_NO_ERR;
    }

    return false;
}

/**
 * Check and load the flash ENV meta data.
 *
 * @return result
 */
EfErrCode ef_load_env(void)
{
    EfErrCode result = EF_NO_ERR;
    EfErrCode recovery_result = EF_NO_ERR;
    struct env_node_obj env;
    struct sector_meta_data sector;
    size_t check_failed_count = 0;

    in_recovery_check = true;
    /* check all sector header */
    result = sector_iterator(&sector, SECTOR_STORE_UNUSED, &check_failed_count, &recovery_result, check_sec_hdr_cb, false);
    if (result == EF_NO_ERR) result = recovery_result;
    if (result != EF_NO_ERR) {
        in_recovery_check = false;
        return result;
    }
    /* all sector header check failed */
    if (check_failed_count == SECTOR_NUM) {
        EF_INFO("Warning: All sector header check failed. Set it to default.\r\n");
        result = ef_env_set_default();
        if (result != EF_NO_ERR) {
            in_recovery_check = false;
            return result;
        }
    }

    /* lock the ENV cache */
    ef_port_env_lock();
    /* check all sector header for recovery GC */
    result = sector_iterator(&sector, SECTOR_STORE_UNUSED, &recovery_result, NULL, check_and_recovery_gc_cb, false);
    if (result != EF_NO_ERR) goto unlock;
    if (recovery_result != EF_NO_ERR) {
        result = recovery_result;
        goto unlock;
    }

__retry:
    /* check all ENV for recovery */
    recovery_result = EF_NO_ERR;
    result = env_iterator(&env, &recovery_result, NULL, check_and_recovery_env_cb);
    if (result != EF_NO_ERR) goto unlock;
    if (recovery_result != EF_NO_ERR) {
        result = recovery_result;
        goto unlock;
    }
    if (gc_request) {
        result = gc_collect();
        if (result != EF_NO_ERR) goto unlock;
        goto __retry;
    }

unlock:
    in_recovery_check = false;

    /* unlock the ENV cache */
    ef_port_env_unlock();

    return result;
}

#ifdef EF_ENV_USING_CACHE
bool env_key_possibly_exist(const char *name, size_t name_len)
{
    size_t i = 0, is_cache_avaiable = false;
    uint16_t name_crc = (uint16_t) (ef_calc_crc32(0, name, name_len) >> 16);

    for (i = 0; i < EF_ENV_CACHE_TABLE_SIZE; i++) {
        if ((env_cache_table[i].addr != FAILED_ADDR) && (env_cache_table[i].name_crc == name_crc)) {
            /** crc matched, means that it has high possibility in cache */
            return true;
        }

        if (env_cache_table[i].addr == FAILED_ADDR) {
            is_cache_avaiable = true;
        }
    }

    if (false == is_cache_avaiable) {
        return true;
    }

    return false;
}

static bool env_cache_cb (env_node_obj_t env, void *arg1, void *arg2) 
{
    if (ENV_WRITE == env->status) {
        env->name[env->name_len] = '\0';

        update_env_cache(env->name, env->name_len, env->addr.start);   
    }
    return false;
}

void ef_load_env_cache(void) 
{
    memset(env_cache_table, 0 ,sizeof(env_cache_table));
    ef_print_env_cb(env_cache_cb);
}
#endif

/**
 * Flash ENV initialize.
 *
 * @param default_env default ENV set for user
 * @param default_env_size default ENV set size
 *
 * @return result
 */
EfErrCode ef_env_init(ef_env const *default_env, size_t default_env_size) {
    EfErrCode result = EF_NO_ERR;

#ifdef EF_ENV_USING_CACHE
    size_t i;
#endif

    EF_ASSERT(default_env);
    EF_ASSERT(ENV_AREA_SIZE);
    /* must be aligned with erase_min_size */
    EF_ASSERT(ENV_AREA_SIZE % EF_ERASE_MIN_SIZE == 0);
    /* sector number must be greater than or equal to 2 */
    EF_ASSERT(SECTOR_NUM >= 2);
    /* must be aligned with write granularity */
    EF_ASSERT((EF_STR_ENV_VALUE_MAX_SIZE * 8) % EF_WRITE_GRAN == 0);

    if (init_ok) {
        return EF_NO_ERR;
    }

#ifdef EF_ENV_USING_CACHE
    for (i = 0; i < EF_SECTOR_CACHE_TABLE_SIZE; i++) {
        sector_cache_table[i].addr = FAILED_ADDR;
    }
    for (i = 0; i < EF_ENV_CACHE_TABLE_SIZE; i++) {
        env_cache_table[i].addr = FAILED_ADDR;
    }
#endif /* EF_ENV_USING_CACHE */

    env_start_addr = EF_START_ADDR;
    default_env_set = default_env;
    default_env_set_size = default_env_size;

    EF_DEBUG("ENV start address is 0x%08X, size is %d bytes.\r\n", EF_START_ADDR, ENV_AREA_SIZE);

    result = ef_load_env();

#ifdef EF_ENV_AUTO_UPDATE
    if (result == EF_NO_ERR) {
        env_auto_update();
    }
#endif

    if (result == EF_NO_ERR) {
        init_ok = true;
    }

    return result;
}

#endif /* defined(EF_USING_ENV) && !defined(EF_ENV_USING_LEGACY_MODE) */
