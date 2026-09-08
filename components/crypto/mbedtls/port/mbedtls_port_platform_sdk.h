#ifndef MBEDTLS_PORT_PLATFORM_SDK
#define MBEDTLS_PORT_PLATFORM_SDK

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//TODO: change to ZSP
//#include "mem.h"

#define MBEDTLS_PLATFORM_STD_FREE free
#define MBEDTLS_PLATFORM_STD_CALLOC calloc

#define MBEDTLS_PLATFORM_FPRINTF_MACRO fprintf
#define MBEDTLS_PLATFORM_PRINTF_MACRO printf
#define MBEDTLS_PLATFORM_SNPRINTF_MACRO snprintf
#define MBEDTLS_PLATFORM_EXIT_MACRO exit

#endif // MBEDTLS_PORT_PLATFORM_SDK
