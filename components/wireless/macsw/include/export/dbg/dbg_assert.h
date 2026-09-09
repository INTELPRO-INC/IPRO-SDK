/**
 ****************************************************************************************
 *
 * @file dbg_assert.h
 *
 * @brief File containing the definitions of the assertion macros.
 *
 * Copyright (C) RivieraWaves 2011-2021
 *
 ****************************************************************************************
 */

#ifndef _DBG_ASSERT_H_
#define _DBG_ASSERT_H_

#ifdef __cplusplus
extern "C" {
#endif
/**
 ****************************************************************************************
 * @defgroup ASSERT ASSERT
 * @ingroup DEBUG
 * @brief Assertion management module
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "export/rwnx_config.h"
#include "export/compiler.h"

/*
 * DEFINES
 ****************************************************************************************
 */
#if NX_TRACE
/// Line number: Include file id if trace is enabled
#define LINE_NB ((TRACE_FILE_ID << 20) + (__LINE__ & 0xfffff))
#else
/// Line number
#define LINE_NB __LINE__
#endif

#if WIFI_STATISTIC_ENABLE
typedef struct __fw_dbg_desc {
    const char *const name;
    uint32_t line;
} fw_dbg_t;

#define STATIC_FWDBG_ATTR         __attribute__((used, section(".static_fw_dbg_entry")))
#define STATIC_FWDBG_COUNTER_ATTR __attribute__((used, section(".static_fw_dbg_count")))

#define __STRINGIZE(x)            #x
#define __LINE_STRINGIZE(x)       __STRINGIZE(x)
#define __FILENAMELINE__          (__FILENAME__ "_" __LINE_STRINGIZE(__LINE__))

#define __REFF_LEVEL(a, c)        _fsymf_level_##a ## c
#define __AREFF_LEVEL(a,c)        __REFF_LEVEL(a,c)
#define __REFF_COUNT(a, c)        _fsymf_fwcount_##a ## c
#define __AREFF_COUNT(a,c)        __REFF_COUNT(a,c)
#define __DEFF_LEVEL(a, b, c)     {static uint32_t __AREFF_COUNT(a, __LINE__) STATIC_FWDBG_COUNTER_ATTR;\
                                  const static fw_dbg_t __AREFF_LEVEL(a, __LINE__)           \
                                  STATIC_FWDBG_ATTR = {.name= b, .line = c};        \
                                  __AREFF_COUNT(a, __LINE__) += 1;}
#else
#define __DEFF_LEVEL(a, b, c)
#endif

/*
 * ASSERTION CHECK
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Force the trigger of the logic analyzer
 *
 * @param[in] msg Error condition that caused the trigger to be forced.
 *
 ****************************************************************************************
 */
void dbg_force_trigger(const char *msg);

/**
 ****************************************************************************************
 * @brief Print the assertion error reason and trigger the recovery procedure.
 *
 * @param[in] condition C string containing the condition.
 * @param[in] file C string containing file where the assertion is located.
 * @param[in] line Line number in the file where the assertion is located.
 ****************************************************************************************
 */
void dbg_assert_rec(const char * file, int line);

/**
 ****************************************************************************************
 * @brief Print the assertion error reason and loop forever.
 *
 * @param[in] condition C string containing the condition.
 * @param[in] file C string containing file where the assertion is located.
 * @param[in] line Line number in the file where the assertion is located.
 ****************************************************************************************
 */
void dbg_assert_err(const char * file, int line);

/**
 ****************************************************************************************
 * @brief Print the assertion warning reason.
 *
 * @param[in] condition C string containing the condition.
 * @param[in] file C string containing file where the assertion is located.
 * @param[in] line Line number in the file where the assertion is located.
 ****************************************************************************************
 */
void dbg_assert_warn(const char * file, int line);

/// Macro defining the format of the assertion calls
#if (NX_DEBUG > 1)
#define ASSERT(type, cond) dbg_assert_##type(__MODULE__, LINE_NB)
#elif (NX_DEBUG > 0)
#define ASSERT(type, cond) dbg_assert_##type("", __MODULE__, LINE_NB)
#else
#define ASSERT(type, cond) dbg_assert_##type("", "", 0)
#endif

#if NX_DEBUG
/// Assertions showing a critical error that could require a full system reset
#define ASSERT_ERR(cond)                                                                 \
    do {                                                                                 \
        if (!(cond)) {                                                                   \
            ASSERT(err, #cond);                                                          \
        }                                                                                \
    } while(0)

/// Assertions showing a critical error that could require a full system reset
#define ASSERT_ERR2(cond, param0, param1) ASSERT_ERR(cond)

/// Assertions showing a non-critical problem that has to be fixed by the SW
#define ASSERT_WARN(cond)                                                                \
    do {                                                                                 \
        if (!(cond)) {                                                                   \
            ASSERT(warn, #cond);                                                         \
        }                                                                                \
    } while(0)
#else
/// Assertions showing a critical error that could require a full system reset
#define ASSERT_ERR(cond)
/// Assertions showing a critical error that could require a full system reset
#define ASSERT_ERR2(cond, param0, param1)
/// Assertions showing a non-critical problem that has to be fixed by the SW
#define ASSERT_WARN(cond)
#endif

#if NX_RECOVERY
/// Assertions that trigger the automatic recovery mechanism and return void
#define ASSERT_REC(cond)                                                                 \
    ({                                                                                   \
        if (!(cond)) {                                                                   \
            __DEFF_LEVEL(__COMPONENT_FILE_NAME_DEQUOTED__, __FILENAME__, __LINE__);            \
            ASSERT(rec, #cond);                                                          \
            return;                                                                      \
        }                                                                                \
    })

/// Assertions that trigger the automatic recovery mechanism and return a value
#define ASSERT_REC_VAL(cond, ret)                                                        \
    ({                                                                                   \
        if (!(cond)) {                                                                   \
            ASSERT(rec, #cond);                                                          \
            return (ret);                                                                \
        }                                                                                \
    })

/// Assertions that trigger the automatic recovery mechanism and do not return
#define ASSERT_REC_NO_RET(cond)                                                          \
    ({                                                                                   \
        if (!(cond)) {                                                                   \
            ASSERT(rec, #cond);                                                          \
        }                                                                                \
    })
#else
/// Assertions that trigger the automatic recovery mechanism and return void
#define ASSERT_REC(cond)             ASSERT_ERR(cond)

/// Assertions that trigger the automatic recovery mechanism and return a value
#define ASSERT_REC_VAL(cond, ret)    ASSERT_ERR(cond)

/// Assertions that trigger the automatic recovery mechanism and do not return
#define ASSERT_REC_NO_RET(cond)      ASSERT_ERR(cond)
#endif

#ifdef CFG_LPM
/// Flag to enable or disable debug message
#define MACSW_DBG_ON            0x80U
#define MACSW_DBG_OFF           0x00U

#ifdef MACSW_DEBUG
#define MACSW_DEBUG(debug, message) do {\
                            if ( \
                                   ((debug) & MACSW_DBG_ON) && \
                                   ((s16_t)((debug) & MACSW_DBG_MASK_LEVEL) >= MACSW_DBG_MIN_LEVEL)) { \
                                 bl_fw_printf message; \
                            } \
                        } while(0)
#else
#define MACSW_DEBUG(debug, message)
#endif // MACSW_DEBUG

#if NX_RECOVERY
#define MACSW_ASSERT_REC(cond, message)                                                  \
    ({                                                                                   \
        if (!(cond)) {                                                                   \
            __DEFF_LEVEL(__COMPONENT_FILE_NAME_DEQUOTED__, __FILENAME__, __LINE__);      \
            GLOBAL_INT_DISABLE();                                                        \
            bl_fw_printf ("[MAC] %d: ", LINE_NB);                                        \
            bl_fw_printf message;                                                        \
            dbg_assert_rec_no_log();                                                     \
            GLOBAL_INT_RESTORE();                                                        \
            return;                                                                      \
        }                                                                                \
    })
#else
#define MACSW_ASSERT_REC(cond, message)        ASSERT_ERR(cond)
#endif
#endif // CFG_LPM

/// @}  // end of group ASSERT

#ifdef __cplusplus
}
#endif
#endif // _DBG_ASSERT_H_
