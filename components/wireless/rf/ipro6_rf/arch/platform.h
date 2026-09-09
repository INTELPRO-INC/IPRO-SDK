/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

#ifndef __PLATFORM__
#define __PLATFORM__

#include "rf/asic/inc/aon_reg.h"
#include "rf/asic/inc/glb_reg.h"
#include "rf/asic/inc/hbn_reg.h"
#include "rf/asic/inc/pds_reg.h"
#define HBN_BASE    ((uint32_t)0x2008F000)
#define GLB_BASE    ((uint32_t)0x20000000)
#define AON_BASE    ((uint32_t)0x2008F000)
#define PDS_BASE    ((uint32_t)0x2008e000)

#define BL_RD_WORD(addr)                             (*((volatile uint32_t*)(addr)))
#define BL_WR_WORD(addr,val)                         ((*(volatile uint32_t*)(addr))=(val))
#define BL_RD_SHORT(addr)                            (*((volatile uint16_t*)(addr)))
#define BL_WR_SHORT(addr,val)                        ((*(volatile uint16_t*)(addr))=(val))
#define BL_RD_BYTE(addr)                             (*((volatile uint8_t*)(addr)))
#define BL_WR_BYTE(addr,val)                         ((*(volatile uint8_t*)(addr))=(val))
#define BL_RDWD_FRM_BYTEP(p)                         ((p[3]<<24)|(p[2]<<16)|(p[1]<<8)|(p[0]))
#define BL_WRWD_TO_BYTEP(p,val)                      {p[0]=val&0xff;p[1]=(val>>8)&0xff;p[2]=(val>>16)&0xff;p[3]=(val>>24)&0xff;}

#define BL_RD_REG16(addr,regname)                    BL_RD_SHORT(addr+regname##_OFFSET)
#define BL_WR_REG16(addr,regname,val)                BL_WR_SHORT(addr+regname##_OFFSET,val)
#define BL_RD_REG(addr,regname)                      BL_RD_WORD(addr+regname##_OFFSET)
#define BL_WR_REG(addr,regname,val)                  BL_WR_WORD(addr+regname##_OFFSET,val)
#define BL_SET_REG_BIT(val,bitname)                  ( (val) |(1U<<bitname##_POS))
#define BL_CLR_REG_BIT(val,bitname)                  ( (val) & bitname##_UMSK )
#define BL_GET_REG_BITS_VAL(val,bitname)             ( ((val) & bitname##_MSK) >> bitname##_POS )
#define BL_SET_REG_BITS_VAL(val,bitname,bitval)      ( ((val)&bitname##_UMSK) | (((uint32_t)(bitval)<<bitname##_POS) & bitname##_MSK) )
#define BL_IS_REG_BIT_SET(val,bitname)               ( ((val)&(1U<<(bitname##_POS))) !=0 )
#define BL_DRV_DUMMY                                 {__NOP();__NOP();__NOP();__NOP();}

#endif
