
/**
 ****************************************************************************************
 *
 * @file co_bool.h
 * Copyright (C) IPRO 2024
 *
 ****************************************************************************************
 */


#ifndef _CO_BOOL_H_
#define _CO_BOOL_H_


/**
 ****************************************************************************************
 * @addtogroup CO_BOOL
 * @ingroup COMMON
 * @brief Common boolean standard types (removes use of stdbool).
 *
 * @{
 ****************************************************************************************
 */


/*
 * DEFINES
 ****************************************************************************************
 */


//STDBOOL---------------------------------------------------------------------------------
#if defined(CFG_RWTL)
///Boolean type
typedef unsigned char bool;
///True value
#define true    1
///False value
#define false   0
#else
#include <stdbool.h>
#endif

/// @} CO_BOOL
#endif // _CO_BOOL_H_
