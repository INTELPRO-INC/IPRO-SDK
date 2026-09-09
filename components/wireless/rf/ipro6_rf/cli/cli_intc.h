/**
 ****************************************************************************************
 *
 * @file intc.h
 *
 * @brief Declaration of the Reference Interrupt Controller (INTC) API.
 *
 * Copyright (C) RivieraWaves 2011-2021
 *
 ****************************************************************************************
 */

#ifndef _INTC_H_
#define _INTC_H_

/**
 ****************************************************************************************
 * @defgroup INTC INTC
 * @ingroup PLATFORM_DRIVERS
 * @brief Declaration of the Reference Interrupt Controller API
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"                // standard integer functions

/*
 * DEFINES
 ****************************************************************************************
 */
/// MAC interrupt stack memory pattern
#define MAC_IRQ_STACK_PATTERN   0x12345678

/// MAC interrupt stack end of memory pattern
#define MAC_IRQ_STACK_END   0xC0DEABCD

/// Length of MAC interrupt stack
#define MAC_IRQ_STACK_LEN   256

// Mapping of the peripherals interrupts in the INTC.

// Inter Processor Communication
/// @name IPC interrupt indexes
/// @{
/// IPC interrupt for the TX descriptors pushed
#define INTC_IPC3          63
/// IPC interrupt for the free RX descriptors pushed
#define INTC_IPC2          62
/// IPC interrupt for the messages pushed
#define INTC_IPC1          61
/// IPC interrupt not used
#define INTC_IPC0          60
/// @}

/// @name MAC HW interrupt indexes
/// @{

/// MAC HW Protocol trigger interrupt
#define INTC_MACPROT       55
/// MAC HW generic interrupts
#define INTC_MACINTGEN     54
/// MAC HW TX interrupts
#define INTC_MACTX         53
/// MAC HW RX interrupts
#define INTC_MACRX         52
/// MAC HW other interrupts
#define INTC_MACOTHER      51
/// MAC HW timer interrupts
#define INTC_MACTIMER      50

/// @}

/// @name HSU interrupt indexes
/// @{
#define INTC_HSU           48
/// @}

/// @name DMA interrupt indexes
/// @{

/// DMA bus error interrupt
#define INTC_DMA_BUSERR    40
/// DMA LLI15 interrupt
#define INTC_LLI15         39
/// DMA LLI14 interrupt
#define INTC_LLI14         38
/// DMA LLI13 interrupt
#define INTC_LLI13         37
/// DMA LLI12 interrupt
#define INTC_LLI12         36
/// DMA LLI11 interrupt
#define INTC_LLI11         35
/// DMA LLI10 interrupt
#define INTC_LLI10         34
/// DMA LLI9 interrupt
#define INTC_LLI9          33
/// DMA LLI8 interrupt
#define INTC_LLI8          32
/// DMA LLI7 interrupt
#define INTC_LLI7          31
/// DMA LLI6 interrupt
#define INTC_LLI6          30
/// DMA LLI5 interrupt
#define INTC_LLI5          29
/// DMA LLI4 interrupt
#define INTC_LLI4          28
/// DMA LLI3 interrupt
#define INTC_LLI3          27
/// DMA LLI2 interrupt
#define INTC_LLI2          26
/// DMA LLI1 interrupt
#define INTC_LLI1          25
/// DMA LLI0 interrupt
#define INTC_LLI0          24
/// End Of Transfer interrupt: ch3
#define INTC_DMA_CH3_EOT   23
/// End Of Transfer interrupt: ch2
#define INTC_DMA_CH2_EOT   22
/// End Of Transfer interrupt: ch1
#define INTC_DMA_CH1_EOT   21
/// End Of Transfer interrupt: ch0
#define INTC_DMA_CH0_EOT   20

/// @}
/// @name External interrupt
/// @{

/// External interrupt
#define INTC_EXTINT        12
/// @}

/// @name PHY interrupt indexes
/// @{

/// Radio Controller interrupt
#define INTC_RC            11
/// Modem interrupt
#define INTC_MODEM         10
/// @}

/*
 * ENUMERATION DEFINITIONS
 *****************************************************************************************
 */
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize and configure the reference INTC.
 * This function configures the INTC according to the system needs.
 ****************************************************************************************
 */
void phycli_intc_init(void);
void phycli_intc_irq(void);
void phycli_intc_enable(void);
void phycli_intc_disable(void);
void phycli_intc_takeover(void);
/// @} INTC

#endif // _INTC_H_
