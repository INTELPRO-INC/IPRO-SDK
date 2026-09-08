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

#include "atbm_hal.h"
/******** Functions below is Wlan API **********/
#include "fh_sdio.h"

#define FH_SIDO_DEBUG_LEVEL	WIFI_DBG_MSG
#define FH_SIDO_ERR_LEVEL	WIFI_DBG_ERROR
typedef void (*irq_handle)(struct atbm_sdio_func *func);
static struct atbm_sdio_func func0;
static struct atbm_sdio_func func1;
struct atbm_sdio_cccr cccr;
static struct atbm_sdio_func *irq_func = ATBM_NULL;
static atbm_mutex sdio_lock;
static irq_handle func_irqhanlde = ATBM_NULL;
static int mmc_bus_interface_control(int enable);
static int mmc_sdio_switch_hs(int enable);
static int atbm_sdio_read_cccr(void);

#define ATBM_CALL_IRQHANDLE() 					if(func_irqhanlde&&irq_func) func_irqhanlde(irq_func)
#define ATBM_SET_IRQ(__func,__handle)			irq_func = __func;func_irqhanlde = __handle

#define SDIO_FUNC_SET_EN(__func,__en)			(__func)->en = __en
#define SDIO_FUNC_SET_NUM(__func,__num)			(__func)->func = __num
#define SDIO_FUNC_SET_PRIV(__func,__priv)		(__func)->priv = __priv
#define SDIO_FUNC_SET_BLKSIZE(__func,__blks)	(__func)->blocksize = __blks
#define SDIO_FUNC_GET_EN(__func)				((__func)->en)
#define SDIO_FUNC_GET_NUM(__func)				((__func)->func)
#define SDIO_FUNC_GET_PRIV(__func)				((__func)->priv)
#define SDIO_FUNC_GET_BLKSIZE(__func)			((__func)->blocksize)	

#define SDIO_FUNC0_INT()						SDIO_FUNC_SET_NUM(&func0,0) ;SDIO_FUNC_SET_EN(&func0,1);	\
												SDIO_FUNC_SET_PRIV(&func0,ATBM_NULL);SDIO_FUNC_SET_BLKSIZE(&func0,288)
#define SDIO_FUNC1_INT()						SDIO_FUNC_SET_NUM(&func1,1) ;SDIO_FUNC_SET_EN(&func1,0);	\
												SDIO_FUNC_SET_PRIV(&func1,ATBM_NULL);SDIO_FUNC_SET_BLKSIZE(&func1,288)
#define SDIO_FUNC1()							func1
#define SDIO_FUNC0()							func0
#define SDIO_LOCK_INIT()						atbm_os_mutexLockInit(&sdio_lock)
#define SDIO_LOCK()								atbm_os_mutexLock(&sdio_lock,0)
#define SDIO_UNLOCK()							atbm_os_mutexUnLock(&sdio_lock)
#define __ATBM_SDIO_ALIGNSIZE(___blks,___size)	((___size)+(___blks)-((___size)%(___blks)))
#define ATBM_SDIO_ALIGNSIZE(_blks,_size)		(((_size)%(_blks)==0)?(_size):__ATBM_SDIO_ALIGNSIZE(_blks,_size))
//#define ATBM_SDIO_ENABLE_HS(X) mmc_sdio_switch_hs(X)
//#define ATBM_SDIO_ENABLE_HS(X)
#if 1
#define ATBM_SDIO_ENABLE_HS(X) do { \
			sdio_high_speed_mode(ATBM_GET_HANEL(), sdio_bus_width_rtl, ATBM_SDIO_CLK); \
			}while(0)
#endif
#define ATBM_SET_BUS mmc_bus_interface_control
#define ATBM_READ_CCCR() atbm_sdio_read_cccr()
#define SDIO_FBR_BASE(f)	((f) * 0x100) /* base of function f's FBRs */

static HSDC atbm_sdio_handle;
static void *hisr_stack = ATBM_NULL;
static long isr_gpio = 0;
#define ATBM_HISR_STACK_SIZE	(2048)
#define ATBM_SET_HANLE(__hanle) atbm_sdio_handle = __hanle
#define ATBM_GET_HANEL()		atbm_sdio_handle

static unsigned char* g_sdio_swap_buffer;
#define SDIO_SWAP_BUF_SIZE (4096-32)
static int sdio_bus_width_rtl = 4;
#define ATBM_SDIO_CLK 25000000

int mmc_io_rw_direct(atbm_uint32 write, unsigned fn, unsigned addr, atbm_uint8 in, atbm_uint8 *out)
{
	atbm_uint32 resp;
	int ret;

	if(write){
		ret = sdio_drv_creg_write(ATBM_GET_HANEL(), addr, fn, in, &resp);
	}else{
		ret = sdio_drv_creg_read(ATBM_GET_HANEL(), addr, fn, &resp);
		*out = (resp & 0xff);
	}

	return ret;
}
int mmc_bus_interface_control(int enable)
{
	int ret;
	unsigned char data;
	ret = mmc_io_rw_direct(0, 0, ATBM_SDIO_CCCR_IF, 0, &data);
	if (ret != 0)
		goto out;

	//data |= 0X20;
	if(enable){
		data &= 0x3;
		data |= 0x2;
	}else{
		data &= 0x3;
		data |= 0x1;
	}
	ret = mmc_io_rw_direct(1, 0, ATBM_SDIO_CCCR_IF, data, &data);
	if (ret != 0)
		goto out;
out:
	if (ret != 0)
	wifi_printk(WIFI_ALWAYS,"sdio_read_cccr err\n");
	return ret;
}

int mmc_sdio_switch_hs(int enable)
{
	int ret;
	atbm_uint8 speed;

	ret = mmc_io_rw_direct(0, 0, ATBM_SDIO_CCCR_SPEED, 0, &speed);
	if (ret != 0)
	{
		wifi_printk(WIFI_SDIO,"mmc_sdio_switch_hs err:%d\n", ret);
		return ret;
	}
	if (enable)
		speed |= ATBM_SDIO_SPEED_EHS;
	else
		speed &= ~ATBM_SDIO_SPEED_EHS;

	ret = mmc_io_rw_direct(1, 0, ATBM_SDIO_CCCR_SPEED, speed, NULL);
	if (ret != 0)
		wifi_printk(WIFI_SDIO,"mmc_sdio_switch_hs err1:%d\n", ret);
	else
		wifi_printk(WIFI_SDIO,"mmc_sdio_switch_hs ok\n");

	return ret;
}

static int atbm_sdio_read_cccr(void)
{
	int ret;
	int cccr_vsn;
	unsigned char data;

	atbm_memset(&cccr, 0, sizeof(struct atbm_sdio_cccr));

	ret = mmc_io_rw_direct(0, 0, ATBM_SDIO_CCCR_CCCR, 0, &data);
	if (ret != 0)
		goto out;

	cccr_vsn = data & 0x0f;
	wifi_printk(WIFI_SDIO,"SDIO_CCCR_CCCR:%x\n", cccr_vsn);

	if (cccr_vsn > SDIO_CCCR_REV_1_20)
	{
		wifi_printk(WIFI_SDIO,"unrecognised CCCR structure version %d\n", cccr_vsn);
		return -1;
	}

	cccr.sdio_vsn = (data & 0xf0) >> 4;

	ret = mmc_io_rw_direct(0, 0, ATBM_SDIO_CCCR_CAPS, 0, &data);
	if (ret != 0)
		goto out;

	wifi_printk(WIFI_SDIO,"SDIO_CCCR_CAPS:%x\n", data);
	if (data & SDIO_CCCR_CAP_SMB)
		cccr.multi_block = 1;
	if (data & SDIO_CCCR_CAP_LSC)
		cccr.low_speed = 1;
	if (data & SDIO_CCCR_CAP_4BLS)
		cccr.wide_bus = 1;

	if (cccr_vsn >= SDIO_CCCR_REV_1_10)
	{
		ret = mmc_io_rw_direct(0, 0, ATBM_SDIO_CCCR_POWER, 0, &data);

		wifi_printk(WIFI_SDIO,"SDIO_CCCR_POWER:%x\n", data);
		if (ret != 0)
			goto out;

		if (data & SDIO_POWER_SMPC)
			cccr.high_power = 1;
	}

	if (cccr_vsn >= SDIO_CCCR_REV_1_20)
	{
		ret = mmc_io_rw_direct(0, 0, ATBM_SDIO_CCCR_SPEED, 0, &data);

		wifi_printk(WIFI_SDIO,"SDIO_CCCR_SPEED:%x\n", data);
		if (ret != 0)
			goto out;

		if (data & ATBM_SDIO_SPEED_SHS)
			cccr.high_speed = 1;
	}

	ret = mmc_io_rw_direct(0, 0, 7, 0, &data);
	if (ret != 0)
		goto out;

	data |= 0X20;
	ret = mmc_io_rw_direct(1, 0, 7, data, &data);
	if (ret != 0)
		goto out;
	
out:
	if (ret != 0)
		wifi_printk(WIFI_SDIO,"sdio_read_cccr err\n");
	return ret;
}

int __atbm_sdio_enable_func(int num)
{
	int ret;
	unsigned char reg;
	wifi_printk(WIFI_SDIO,"enable func for sdio\n");
	ret = mmc_io_rw_direct(0, 0, ATBM_SDIO_CCCR_IOEx, 0, &reg);
	wifi_printk(WIFI_SDIO,"enable func for sdio  0--->%x %x\n",ret,num);
	if (ret  != 0)
		goto err;

	reg |= 1 <<num;
	wifi_printk(WIFI_SDIO,"enable func for sdio  1\n");

	ret = mmc_io_rw_direct(1, 0, ATBM_SDIO_CCCR_IOEx, reg, NULL);
	if (ret  != 0)
		goto err;

	wifi_printk(WIFI_SDIO,"enable func for sdio  2\n");

	while (1) {
		ret = mmc_io_rw_direct(0, 0, ATBM_SDIO_CCCR_IOEx, 0, &reg);
		if (ret  != 0)
			goto err;
		if (reg & (1 <<num))
			break;
	}
	wifi_printk(WIFI_SDIO,"enable func for sdio  3\n");

	wifi_printk(WIFI_SDIO,"SDIO: Enabled func %d\n", reg);

	return 0;

err:
	wifi_printk(WIFI_SDIO,"SDIO: Failed to enable device\n");
	return ret;
}

int wifi_sdio_init(){
	HSDC handle;
    unsigned int resp;
    unsigned int wkmod = 0;
    unsigned int which = 0;

	/* Initialize SDIO driver */
	if (WIFI_SDIO == 0)
	{
		which = 0;
	}
	else if (WIFI_SDIO == 1)
	{
		which = 1;
	}

#ifdef USING_1BIT_MODE
	wkmod = SDC_WKMOD_1WIRE|SDC_WKMOD_25M_STAND_SPEED;
	sdio_bus_width_rtl = 1;
#else
	wkmod = SDC_WKMOD_4WIRE|SDC_WKMOD_25M_STAND_SPEED;
#endif

	resp = sdio_init(which, wkmod, NULL, &handle);
	if (resp || (handle == NULL))
	{
		printf("sdio_init(WIFI_SDIO:%d) ret %u!\n", WIFI_SDIO, resp);
		return -1;
	}
	ATBM_SET_HANLE(handle);
	return 0;
}


void atbm_sdio_isr_enable(atbm_uint8 enable);

static void atbm_sdio_gpio_handle(unsigned long *pin, unsigned long polarity)
{
//	atbm_sdio_isr_enable(0);
	//wifi_printk(FH_SIDO_DEBUG_LEVEL,"atbm_sdio_gpio_handle\n");

	AK_Activate_HISR(ATBM_GET_HANEL());
}
int atbm_sdio_register(struct atbm_sdio_driver *sdio_driver)
{
	wifi_printk(WIFI_SDIO,"atbm_sdio_register\n");
	g_sdio_swap_buffer = (unsigned char *)fh_dma_mem_malloc_align(SDIO_SWAP_BUF_SIZE, 128,  "SDIO-SWAP");
    if (!g_sdio_swap_buffer)
    {
        wifi_printk(WIFI_DBG_ERROR, "ERR in fh_dma_mem_malloc_align()!\n");
        return -1;
    }
	wifi_sdio_init();
	ATBM_SET_BUS(sdio_bus_width_rtl == 4);
	ATBM_SDIO_ENABLE_HS(1);
	ATBM_READ_CCCR();
	SDIO_FUNC0_INT();
	SDIO_FUNC1_INT();
	SDIO_LOCK_INIT();
	return sdio_driver->probe_func(&SDIO_FUNC1(),sdio_driver->match_id_table);
} 
void atbm_sdio_deregister(struct atbm_sdio_driver *sdio_driver)
{
	sdio_driver->discon_func(&SDIO_FUNC1());
}
void atbm_sdio_claim_host(struct atbm_sdio_func *func)
{
	func = func;
	SDIO_LOCK();
}
void atbm_sdio_release_host(struct atbm_sdio_func *func)
{
	func = func;
	SDIO_UNLOCK();
}

atbm_int32 atbm_sdio_enable_func(struct atbm_sdio_func *func)
{
    int ret;
    unsigned int resp;
    unsigned char reg;

	wifi_printk(FH_SIDO_DEBUG_LEVEL,"atbm_sdio_enable_func(%d)\n",func->func);
	SDIO_FUNC_SET_EN(func,1);
	//sdio_enable_func(func->func);
	/* rt_kprintf("atbm_sdio_enable_func: called with func%d.\n", func->func); */

	ret = sdio_drv_creg_read(ATBM_GET_HANEL(), ATBM_SDIO_CCCR_IOEx, 0, &resp);
	if (ret)
		return -1;

	reg = resp & 0xff;
	reg |= (1 << func->func);

	ret = sdio_drv_creg_write(ATBM_GET_HANEL(), ATBM_SDIO_CCCR_IOEx, 0, reg, &resp);
	if (ret)
		return -1;


	while (1)
	{
		ret = sdio_drv_creg_read(ATBM_GET_HANEL(), ATBM_SDIO_CCCR_IORx, 0, &resp);
		if (ret)
			return -1;

		if (resp & (1 << func->func))
		{
			break;
		}
		rt_thread_delay(1);
	}

	/* rt_kprintf("rtl_sdio_enable_func: function%d enabled.\n", func->num); */

	return 0;
}

void atbm_sdio_disable_func(struct atbm_sdio_func *func)
{
	SDIO_FUNC_SET_EN(func,0);
}

void atbm_sdio_set_drvdata(struct atbm_sdio_func *func,void *priv)
{
	SDIO_FUNC_SET_PRIV(func,priv);
}

void *atbm_sdio_get_drvdata(struct atbm_sdio_func *func)
{
	return SDIO_FUNC_GET_PRIV(func);
}

#define SDIO_MESSAGE 1
OS_TIMER  bh_wake;
extern struct atbmwifi_common g_hw_prv;
atbm_void atbmwifi_bh_wake(TIMER_1ST_PARAM atbm_void *arg)
{
	if (atbm_atomic_add_return(1, &g_hw_prv.bh_rx) == 1){
		atbm_os_wakeup_event(&g_hw_prv.bh_wq);
	}
}

void sdio_interrupt_int_callback()
{
	wifi_printk(WIFI_SDIO,"sdio_interrupt_int_callback \n");
	ATBM_CALL_IRQHANDLE();
}

static int atbm_sdio_data1_irq(void)
{
	wifi_printk(WIFI_SDIO,"atbm_sdio_data1_irq \n");
	sdio_set_card_int_cb(ATBM_GET_HANEL(), sdio_interrupt_int_callback);
	sdio_enable_card_int(ATBM_GET_HANEL(), 1);
	return 0;

}

void atbm_sdio_host_enable_irq(int enable){
	sdio_enable_card_int(ATBM_GET_HANEL(), 1);
}

int atbm_sdio_claim_irq(struct atbm_sdio_func *func,void (*irq_handle)(struct atbm_sdio_func *func))
{
	pAtbm_thread_t bh_thread;
	struct atbmwifi_common *hw_priv=&g_hw_prv;
	int ret;
	atbm_uint8 reg = 0;
	wifi_printk(WIFI_SDIO,"atbm_sdio_claim_irq  %d\n",func->func);

	ATBM_SET_IRQ(func,irq_handle);
	ret = mmc_io_rw_direct(0, 0, ATBM_SDIO_CCCR_IENx, 0, &reg);
	if(ret != 0){
		wifi_printk(WIFI_SDIO,"read ATBM_SDIO_CCCR_IENx err\n");
		goto __fail;
	}
	/* Master interrupt enable ... */
	reg |= BIT(0);
	/* ... for our function */
	reg |= BIT(func->func);
	
	wifi_printk(WIFI_SDIO,"write ATBM_SDIO_CCCR_IENx %x\n",reg);
	ret = mmc_io_rw_direct(1, 0, ATBM_SDIO_CCCR_IENx, reg, NULL);
	if(ret != 0){
		wifi_printk(WIFI_SDIO,"write ATBM_SDIO_CCCR_IENx err\n");
		goto __fail;
	}
	ret = mmc_io_rw_direct(0, 0, ATBM_SDIO_CCCR_IENx, 0, &reg);
	
	wifi_printk(WIFI_SDIO,"atbm_sdio_claim_irq  %x\n",reg);
	/*Init sdio Interrupt,after the sdio Initial*/
	//interrupt_init();
	ret = atbm_sdio_data1_irq();
	if(ret != 0)
	{
		wifi_printk(WIFI_SDIO,"sdio register irq err1\n");
	}
	return 0;
__fail:	
	return -1;
}
int atbm_sdio_release_irq(struct atbm_sdio_func *func)
{
	int ret;
	unsigned char reg;
	ret = mmc_io_rw_direct(0, 0, ATBM_SDIO_CCCR_IENx, 0, &reg);
	if (ret)
		return ret;

	reg &= ~(1 << func->func);

	/* Disable master interrupt with the last function interrupt */
	if (!(reg & 0xFE))
		reg = 0;

	ret = mmc_io_rw_direct(1, 0, ATBM_SDIO_CCCR_IENx, reg, NULL);
	if (ret)
		return ret;
	
	ATBM_SET_IRQ(ATBM_NULL,ATBM_NULL);
	
	sdio_enable_card_int(ATBM_GET_HANEL(), 0);
	return 0;
}

int __atbm_sdio_memcpy_fromio(struct atbm_sdio_func *func,void *dst,unsigned int addr,int count)
{
	int ret;
	int blocks = 0;

	if(func->en == 0){
		wifi_printk(FH_SIDO_DEBUG_LEVEL,"%s:%d func not en \n",__func__,func->func);
		return -1;
	}
	if((((unsigned int)dst) &3)||(count &3)){
		wifi_printk(WIFI_ALWAYS,"Read Data & Len must be 4 align,dst=0x%08x,count=%d",(unsigned int )dst,count);
	}

    if (count > func->blocksize)
    {
        blocks = count / func->blocksize;
        ret = sdio_drv_read(ATBM_GET_HANEL(), addr, func->func,
            blocks, func->blocksize, g_sdio_swap_buffer);
        if (ret)
        {
            rt_kprintf("rtl_sdio_memcpy_fromio: failed.ret=%d.\n", ret);
            return -1;
        }
        count = count % func->blocksize;
		atbm_memcpy(dst, g_sdio_swap_buffer, blocks*func->blocksize);
    }

    if (count)
    {
        ret = sdio_drv_read(ATBM_GET_HANEL(), addr, func->func,
            1, count, g_sdio_swap_buffer);
        if (ret)
        {
            rt_kprintf("rtl_sdio_memcpy_fromio: failed.ret=%d.\n", ret);
            return -1;
        }
		atbm_memcpy(dst + blocks*func->blocksize, g_sdio_swap_buffer, count);
    }

	return 0;
}

#if 0
int __atbm_sdio_memcpy_toio(struct atbm_sdio_func *func,unsigned int addr,void *dst,int count)
{
	int ret;
	int blocks;
	unsigned char *pos = (unsigned char *)dst;
	if(func->en == 0){
		wifi_printk(FH_SIDO_DEBUG_LEVEL,"%s:%d func not en \n",__func__,func->func);
		return -1;
	}
	if((((unsigned int)dst) &3)||(count &3)){
		wifi_printk(WIFI_ALWAYS,"Write Data & Len must be 4 align,dst=0x%08x,count=%d",(unsigned int )dst,count);
	}

    if (count > func->blocksize)
    {
        blocks = count / func->blocksize;
		atbm_memcpy(g_sdio_swap_buffer, pos, blocks * func->blocksize);
		ret = sdio_drv_write(ATBM_GET_HANEL(), addr, func->func,
			    blocks, func->blocksize, g_sdio_swap_buffer);
        if (ret)
        {
            rt_kprintf("rtl_sdio_memcpy_toio: failed.ret=%d.\n", ret);
            return -1;
        }

		count = count % func->blocksize;
		pos += blocks * func->blocksize;
    }

    if (count)
    {
		atbm_memcpy(g_sdio_swap_buffer, pos, count);
        ret = sdio_drv_write(ATBM_GET_HANEL(), addr, func->func,
            1, count, g_sdio_swap_buffer);
        if (ret)
        {
            rt_kprintf("rtl_sdio_memcpy_toio: failed.ret=%d.\n", ret);
            return -1;
        }
    }

	return 0;
}
#else
int __atbm_sdio_memcpy_toio(struct atbm_sdio_func *func,unsigned int addr,void *dst,int count)
{
	int ret;
	int blocks;
	unsigned char *pos = (unsigned char *)dst;
	if(func->en == 0){
		wifi_printk(FH_SIDO_DEBUG_LEVEL,"%s:%d func not en \n",__func__,func->func);
		return -1;
	}
	if((((unsigned int)dst) &3)||(count &3)){
		wifi_printk(WIFI_ALWAYS,"Write Data & Len must be 4 align,dst=0x%08x,count=%d",(unsigned int )dst,count);
	}

    if (count > func->blocksize)
    {
        blocks = count / func->blocksize;
            ret = sdio_drv_write(ATBM_GET_HANEL(), addr, func->func,
            blocks, func->blocksize, pos);
        if (ret)
        {
            rt_kprintf("rtl_sdio_memcpy_toio: failed.ret=%d.\n", ret);
            return -1;
        }

        count = count % func->blocksize;
		pos += blocks * func->blocksize;
    }

    if (count)
    {
        ret = sdio_drv_write(ATBM_GET_HANEL(), addr, func->func,
            1, count, pos);
        if (ret)
        {
            rt_kprintf("rtl_sdio_memcpy_toio: failed.ret=%d.\n", ret);
            return -1;
        }
    }

	return 0;
}
#endif

unsigned char atbm_sdio_f0_readb(struct atbm_sdio_func *func,unsigned int addr,int *retval)
{
	atbm_uint32 resp;
	unsigned char data = 0;
	int ret;

	ret = sdio_drv_creg_read(ATBM_GET_HANEL(), addr, 0, &resp);

	data = resp & 0xff;

	*retval = 0;
	if(ret == 0)
		*retval = -1;
_return:
	return data;
}

void atbm_sdio_f0_writeb(struct atbm_sdio_func *func,unsigned char regdata,unsigned int addr,int *retval)
{
	atbm_uint32 resp;
	int ret;

	ret = sdio_drv_creg_write(ATBM_GET_HANEL(), addr, 0, regdata, &resp);
	
	*retval = 0;
	
	if(ret == 0)
		*retval = -1;
_return:
	return;
}

atbm_uint32 atbm_sdio_alignsize(struct atbm_sdio_func *func,atbm_uint32 size)
{
	atbm_uint32 block_size = SDIO_FUNC_GET_BLKSIZE(func);
	atbm_uint32 alignsize = size;
	if((block_size == 0)||(size == 0)){
		wifi_printk(FH_SIDO_DEBUG_LEVEL,"atbm_sdio_alignsize err\n");
		return alignsize;
	}

	alignsize = ATBM_SDIO_ALIGNSIZE(block_size,size);
	return alignsize;
}

int __atbm_sdio_set_blocksize(struct atbm_sdio_func *func,int blksz)
{
	int ret = 0;

	wifi_printk(WIFI_SDIO,"atbm_sdio_set_blocksize func->func =%d :%d\n",func->func,blksz);
	ret = mmc_io_rw_direct(1, 0,
		SDIO_FBR_BASE(func->func) + ATBM_SDIO_FBR_BLKSIZE,
		blksz & 0xff, NULL);
	if (ret)
		goto end;

	ret = mmc_io_rw_direct(1, 0,
		SDIO_FBR_BASE(func->func) + ATBM_SDIO_FBR_BLKSIZE + 1,
		(blksz >> 8) & 0xff, NULL);
	if (ret)
		goto end;
end:
	return ret;
}

int  atbm_sdio_set_blocksize(struct atbm_sdio_func *func,int blocksize)
{
	if(__atbm_sdio_set_blocksize(func,blocksize)){
		wifi_printk(FH_SIDO_DEBUG_LEVEL,"err:atbm_sdio_set_blocksize(%d)(%d) ret =%d\n",blocksize,func->blocksize);
		return -1;
	}
	SDIO_FUNC_SET_BLKSIZE(func,blocksize);
	wifi_printk(FH_SIDO_DEBUG_LEVEL,"atbm_sdio_set_blocksize(%d)(%d) ret =%d\n",blocksize,func->blocksize);
	return 0;
}

static struct atbm_sdio_driver atmbwifi_driver;
extern int atbm_sdio_probe(struct atbm_sdio_func *func,const struct atbm_sdio_device_id *id);
extern int atbm_sdio_disconnect(struct atbm_sdio_func *func);
static struct atbm_sdio_device_id atbm_sdio_ids[] = {
	//{ SDIO_DEVICE(SDIO_ANY_ID, SDIO_ANY_ID) },
	{ /* end: all zeroes */			},
};
int atbm_sdio_register_init()
{	
	int ret =0;
	atbm_memcpy(atmbwifi_driver.name, "atbm6021",sizeof("atbm6021"));;
	atmbwifi_driver.match_id_table	= atbm_sdio_ids;
	atmbwifi_driver.probe_func		= atbm_sdio_probe;
	atmbwifi_driver.discon_func		= atbm_sdio_disconnect;	
	wifi_printk(WIFI_ALWAYS, "atbm_sdio_register_init\r\n");
	ret = atbm_sdio_register(&atmbwifi_driver);
	if (ret){
		wifi_printk(WIFI_DBG_ERROR,"atbmwifi sdio driver register error\n");	
		return ret;
	}
	return 0;
}
int atbm_sdio_register_deinit()
{
	atbm_sdio_deregister(&atmbwifi_driver);
	return 0;
}



