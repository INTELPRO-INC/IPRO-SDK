/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/
#include "atbm_hal.h"
#include "../../include/svn_version.h"
#include "atbm_sdio_hwio.h"

static atbm_os_msgq atbm_sdio_tx_msgQ;
static ATBM_INF_DESC *atbm_urb_tx_msqQbuf[TX_DESC_NUM+1];
static atbm_os_msgq atbm_sdio_rx_msgQ ;
static ATBM_INF_DESC *atbm_urb_rx_msqQbuf[RX_DESC_NUM+1];

extern atbm_void atbm_core_release(struct atbmwifi_common *hw_priv);
extern int atbm_hmac_receive_data(struct sbus_priv *self,unsigned int addr,atbm_void  *dst, int count);
extern int atbm_hmac_xmit_data(struct sbus_priv *self);
extern int atbm_reg_read_16(struct atbmwifi_common *hw_priv, atbm_uint16 addr, atbm_uint16 *val);
int atbm_sdio_set_block_size(struct sbus_priv *self, int type, atbm_uint32 size);

extern struct atbmwifi_common g_hw_prv;
static int atbm_wifi_inif = 0;

void atbm_wifi_set_init_flag(int value)
{
	atbm_wifi_inif = value;
}

int atbm_wifi_get_init_flag(void)
{
	return atbm_wifi_inif;
}

#define DPLL_CLOCK 24
struct build_info{
	int ver;
	int dpll;
	char driver_info[64];
};
const char DRIVER_INFO[]={"[===SDIO-ATHENAB=="};
static int driver_build_info(atbm_void)
{
	struct build_info build;
	build.ver=SVN_VERSION;
	build.dpll=DPLL_CLOCK;
	atbm_memcpy(build.driver_info,(atbm_void*)DRIVER_INFO,sizeof(DRIVER_INFO));
	wifi_printk(WIFI_DBG_ANY,"SVN_VER=%d,DPLL_CLOCK=%d,BUILD_TIME=%s\n",build.ver,build.dpll,build.driver_info);
	return 0;
}

int atbm_sdio_suspend(struct atbmwifi_common *hw_priv)
{
	return 0;
}
int atbm_sdio_resume(struct atbmwifi_common *hw_priv)
{
	return 0;
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_memcpy_fromio
 *
 * 功能描述:
 *   从 SDIO 设备读取数据到内存（队列模式）。这是 sbus_ops.sbus_memcpy_fromio 的实现，
 *   用于批量接收 WiFi 数据包。函数会启动多个 RX 描述符来接收数据。
 *
 * 使用场景:
 *   - 在 HAL 层初始化时，启动 RX 接收
 *   - 当需要从硬件接收数据时调用
 *   - 由 sbus_ops 结构体中的 sbus_memcpy_fromio 指针引用
 *
 * 参数说明:
 *   @self:  sbus_priv 结构体指针，包含 SDIO 功能指针和驱动私有数据
 *   @addr:  SDIO 地址（在队列模式下通常不使用）
 *   @dst:   目标缓冲区指针（在队列模式下通常不使用）
 *   @count: 数据长度（在队列模式下通常不使用）
 *
 * 返回值:
 *   0:  成功
 *   -1: 失败（RX 锁被占用）
 *
 * 实现机制:
 *   1. 检查 RX 锁是否被占用
 *   2. 循环启动 RX_DESC_NUM 个接收描述符
 *   3. 调用 atbm_hmac_receive_data() 执行实际的数据接收
 *
 * 注意事项:
 *   - 这是队列模式的实现，参数 addr/dst/count 在此模式下不直接使用
 *   - 实际的数据接收由 atbm_hmac_receive_data() 完成
 *   - 需要确保 RX 锁未被占用
 *
 **************************************************************************************************************/
static int atbm_sdio_memcpy_fromio(struct sbus_priv *self,
				     unsigned int addr,
				     void *dst, int count)
{
	int i=0,ret = 0;
	/* 检查 RX 锁是否被占用，如果被占用则返回错误 */
	if(atbm_atomic_add_return(0, &self->rx_lock)){
		wifi_printk(WIFI_ALWAYS,"%s %d \n",__func__,__LINE__);
		return -1;
	}

	/* 循环启动 RX_DESC_NUM 个接收描述符 */
	for(i=0;i<RX_DESC_NUM;i++){
		/* 调用底层接收函数，启动数据接收 */
	 	ret = atbm_hmac_receive_data(self,addr,dst,count);
		if(ret < 0){
			wifi_printk(WIFI_ALWAYS,"%s %d atbm_hmac_receive_data err\n",__func__,__LINE__);
		}
	}
	return 0;
}
/**************************************************************************************************************
 * 函数名: atbm_sdio_memcpy_toio
 *
 * 功能描述:
 *   向 SDIO 设备写入数据（队列模式）。这是 sbus_ops.sbus_memcpy_toio 的实现，
 *   用于批量发送 WiFi 数据包。
 *
 * 使用场景:
 *   - TX 数据发送
 *   - 由 sbus_ops 结构体中的 sbus_memcpy_toio 指针引用
 *
 * 参数说明:
 *   @self:  sbus_priv 结构体指针
 *   @addr:  SDIO 地址
 *   @src:   源数据缓冲区指针
 *   @count: 数据长度（字节）
 *
 * 返回值:
 *   0:  成功
 *   <0: 失败
 *
 **************************************************************************************************************/
static int atbm_sdio_memcpy_toio(struct sbus_priv *self,
				   unsigned int addr,
				   const void *src, int count)
{
	return __atbm_sdio_memcpy_toio(self->func, addr, (void *)src, count);
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_read_sync
 *
 * 功能描述:
 *   从 SDIO 设备同步读取数据。这是 sbus_ops.sbus_read_sync 的实现，
 *   用于读取寄存器和固件下载时的数据读取。
 *
 * 使用场景:
 *   - 寄存器读取
 *   - 固件下载过程中的数据验证
 *   - 由 sbus_ops 结构体中的 sbus_read_sync 指针引用
 *
 * 参数说明:
 *   @self:  sbus_priv 结构体指针
 *   @addr:  SDIO 地址（需要 4 字节对齐）
 *   @dst:   目标缓冲区指针
 *   @count: 数据长度（字节）
 *
 * 返回值:
 *   0:  成功
 *   <0: 失败
 *
 * 注意事项:
 *   - 这是同步阻塞函数，会等待传输完成
 *   - 数据缓冲区需要满足对齐要求
 *
 **************************************************************************************************************/
static int atbm_sdio_read_sync(struct sbus_priv *self,
				     unsigned int addr,
				     void *dst, int count)
{
	return __atbm_sdio_memcpy_fromio(self->func, dst, addr, count);
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_lock
 *
 * 功能描述:
 *   锁定 SDIO 主机，防止并发访问。这是 sbus_ops.lock 的实现。
 *   在进行 SDIO 数据传输前必须调用此函数获取主机锁。
 *
 * 使用场景:
 *   - 在进行 SDIO 数据读写前调用
 *   - 需要独占 SDIO 总线时调用
 *   - 由 sbus_ops 结构体中的 lock 指针引用
 *
 * 参数说明:
 *   @self: sbus_priv 结构体指针
 *
 * 注意事项:
 *   - 必须与 atbm_sdio_unlock() 配对使用
 *   - 支持嵌套锁定（同一任务可多次调用）
 *   - 在中断上下文中不能调用
 *
 **************************************************************************************************************/
static void atbm_sdio_lock(struct sbus_priv *self)
{
	atbm_sdio_claim_host(self->func);
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_unlock
 *
 * 功能描述:
 *   释放 SDIO 主机锁。这是 sbus_ops.unlock 的实现。
 *   在完成 SDIO 数据传输后必须调用此函数释放主机锁。
 *
 * 使用场景:
 *   - 在完成 SDIO 数据读写后调用
 *   - 由 sbus_ops 结构体中的 unlock 指针引用
 *
 * 参数说明:
 *   @self: sbus_priv 结构体指针
 *
 * 注意事项:
 *   - 必须与 atbm_sdio_lock() 配对使用
 *   - 调用次数必须与 lock 调用次数相同
 *
 **************************************************************************************************************/
static void atbm_sdio_unlock(struct sbus_priv *self)
{
	atbm_sdio_release_host(self->func);
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_abort
 *
 * 功能描述:
 *   中止当前 SDIO 传输。这是 sbus_ops.abort 的实现。
 *   用于在异常情况下中止正在进行的数据传输。
 *
 * 使用场景:
 *   - 传输超时时调用
 *   - 发生错误需要中止传输时调用
 *   - 由 sbus_ops 结构体中的 abort 指针引用
 *
 * 参数说明:
 *   @self: sbus_priv 结构体指针
 *
 * 返回值:
 *   0: 成功
 *
 **************************************************************************************************************/
static int atbm_sdio_abort(struct sbus_priv *self)
{
	wifi_printk(WIFI_ALWAYS,"%s %d \n",__func__,__LINE__);
	return 0;
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_irq_handler
 *
 * 功能描述:
 *   SDIO 中断处理函数。当 SDIO 设备产生中断时，由底层 SDIO 驱动调用此函数。
 *   此函数会调用上层注册的中断处理回调函数。
 *
 * 使用场景:
 *   - 当 WiFi 芯片有数据需要接收时产生中断
 *   - 当 WiFi 芯片完成某些操作需要通知主机时产生中断
 *   - 由 atbm_sdio_claim_irq() 注册到底层 SDIO 驱动
 *
 * 参数说明:
 *   @func: SDIO 功能结构体指针
 *
 * 实现机制:
 *   1. 从 SDIO 功能结构体获取驱动私有数据
 *   2. 调用上层注册的中断处理函数
 *
 * 注意事项:
 *   - 此函数在中断上下文中执行，不能睡眠
 *   - 应该尽快返回，将耗时操作放到工作队列
 *
 **************************************************************************************************************/
static void atbm_sdio_irq_handler(struct atbm_sdio_func *func)
{
	/* 从 SDIO 功能结构体获取驱动私有数据 */
	struct sbus_priv *self = atbm_sdio_get_drvdata(func);

	ATBM_BUG_ON(!self);

	/* 调用上层注册的中断处理函数 */
	if (self->irq_handler)
		self->irq_handler(self->irq_priv);

}

/**************************************************************************************************************
 * 函数名: atbm_sdio_irq_subscribe
 *
 * 功能描述:
 *   注册 SDIO 中断处理函数。这是 sbus_ops.irq_subscribe 的实现。
 *   在 HAL 层初始化时调用，用于注册中断处理回调函数。
 *
 * 使用场景:
 *   - 在 Atbmwifi_halEntry() 中调用，注册中断处理函数
 *   - 由 sbus_ops 结构体中的 irq_subscribe 指针引用
 *
 * 参数说明:
 *   @self:    sbus_priv 结构体指针
 *   @handler: 中断处理回调函数指针
 *   @priv:    传递给中断处理函数的私有数据指针
 *
 * 返回值:
 *   0:  成功
 *   <0: 失败
 *
 * 实现机制:
 *   1. 保存中断处理函数和私有数据到 sbus_priv 结构体
 *   2. 调用 atbm_sdio_claim_irq() 向底层 SDIO 驱动注册中断
 *
 * 注意事项:
 *   - handler 不能为 NULL
 *   - 使用自旋锁保护中断处理函数指针的设置
 *
 **************************************************************************************************************/
static int atbm_sdio_irq_subscribe(struct sbus_priv *self,
				     sbus_irq_handler handler,
				     void *priv)
{
	int ret;
	unsigned long flags;

	/* 检查 handler 是否有效 */
	if (!handler)
		return -ATBM_EINVAL;

	/* 使用自旋锁保护，保存中断处理函数和私有数据 */
	atbm_spin_lock_irqsave(&self->lock, &flags);
	self->irq_priv = priv;
	self->irq_handler = handler;
	atbm_spin_unlock_irqrestore(&self->lock, flags);
	
	/* 向底层 SDIO 驱动注册中断处理函数 */
	atbm_sdio_claim_host(self->func);
	ret = atbm_sdio_claim_irq(self->func, atbm_sdio_irq_handler);
	if (ret)
		wifi_printk(WIFI_IF,"Failed to claim sdio Irq :%d\n",ret);
	atbm_sdio_release_host(self->func);
	return ret;
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_irq_en
 *
 * 功能描述:
 *   使能或禁用 SDIO 中断。这是 sbus_ops.sdio_irq_en 的实现。
 *   用于动态控制 SDIO 中断的使能状态。
 *
 * 使用场景:
 *   - 在需要临时禁用中断时调用
 *   - 由 sbus_ops 结构体中的 sdio_irq_en 指针引用
 *
 * 参数说明:
 *   @self: sbus_priv 结构体指针
 *   @en:   使能标志（1: 使能, 0: 禁用）
 *
 * 注意事项:
 *   - 当前实现为空，根据需要可添加具体实现
 *
 **************************************************************************************************************/
static void atbm_sdio_irq_en(struct sbus_priv *self,atbm_uint8 en)
{

}
static int atbm_sdio_irq_unsubscribe(struct sbus_priv *self)
{
	int ret = 0;
	unsigned long flags;
	//const struct resource *irq = self->pdata->irq;

	ATBM_WARN_ON_FUNC(!self->irq_handler);
	if (!self->irq_handler)
		return 0;

	atbm_sdio_claim_host(self->func);
	ret = atbm_sdio_release_irq(self->func);
	atbm_sdio_release_host(self->func);
	atbm_spin_lock_irqsave(&self->lock, &flags);
	self->irq_priv = ATBM_NULL;
	self->irq_handler = ATBM_NULL;
	atbm_spin_unlock_irqrestore(&self->lock, flags);

	return ret;
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_reset
 *
 * 功能描述:
 *   复位 SDIO 设备。这是 sbus_ops.reset 的实现。
 *   用于在异常情况下复位 WiFi 芯片，重新初始化 SDIO 连接。
 *
 * 使用场景:
 *   - 当 SDIO 通信出现严重错误时调用
 *   - 需要重新初始化 WiFi 芯片时调用
 *   - 由 sbus_ops 结构体中的 reset 指针引用
 *
 * 参数说明:
 *   @self: sbus_priv 结构体指针
 *
 * 返回值:
 *   0: 成功
 *
 * 实现机制:
 *   1. 通过 CCCR_ABORT 寄存器复位 SDIO 卡
 *   2. 配置高速模式（CCCR_SPEED）
 *   3. 配置总线宽度（CCCR_IF）
 *   4. 使能 SDIO 功能（CCCR_IOEx）
 *   5. 设置块大小
 *
 * 注意事项:
 *   - 当前实现直接返回 0，复位逻辑被注释掉
 *   - 如需启用复位功能，需要移除 return 0 语句
 *
 **************************************************************************************************************/
static int atbm_sdio_reset(struct sbus_priv *self)
{
	int ret;
	int regdata;
	int func_num;

	return 0;  /* 当前直接返回，复位逻辑未启用 */
	wifi_printk(WIFI_IF,"atbm_sdio_reset++\n");
	atbm_sdio_claim_host(self->func);

	/**********************/
	wifi_printk(WIFI_IF,"SDIO_RESET++\n");
	/* SDIO Simplified Specification V2.0, 4.4 Reset for SDIO */
	regdata = atbm_sdio_f0_readb(self->func, ATBM_SDIO_CCCR_ABORT, &ret);
	if (ret)
		regdata = 0x08;
	else
		regdata |= 0x08;
	atbm_sdio_f0_writeb(self->func, regdata, ATBM_SDIO_CCCR_ABORT, &ret);
	if (ATBM_WARN_ON(ret))
		goto set_func0_err;
	atbm_mdelay(1500);
	regdata = atbm_sdio_f0_readb(self->func, ATBM_SDIO_CCCR_ABORT, &ret);
	wifi_printk(WIFI_IF,"SDIO_RESET-- 0x%x\n",regdata);

	/**********************/
	wifi_printk(WIFI_IF,"ATBM_SDIO_SPEED_EHS++\n");
	regdata = atbm_sdio_f0_readb(self->func, ATBM_SDIO_CCCR_SPEED, &ret);
	if (ATBM_WARN_ON(ret))
		goto set_func0_err;

	regdata |= ATBM_SDIO_SPEED_EHS;
	atbm_sdio_f0_writeb(self->func, regdata, ATBM_SDIO_CCCR_SPEED, &ret);
	if (ATBM_WARN_ON(ret))
		goto set_func0_err;

	regdata = atbm_sdio_f0_readb(self->func, ATBM_SDIO_CCCR_SPEED, &ret);
	wifi_printk(WIFI_IF,"ATBM_SDIO_SPEED_EHS -- 0x%x:0x%x\n",regdata,ATBM_SDIO_SPEED_EHS);

	/**********************/
	wifi_printk(WIFI_IF,"ATBM_SDIO_BUS_WIDTH_4BIT++\n");
	regdata = atbm_sdio_f0_readb(self->func, ATBM_SDIO_CCCR_IF, &ret);
	if (ATBM_WARN_ON(ret))
		goto set_func0_err;

	//regdata |= ATBM_SDIO_BUS_WIDTH_4BIT;
	regdata = 0xff;
	atbm_sdio_f0_writeb(self->func, regdata, ATBM_SDIO_CCCR_IF, &ret);
	if (ATBM_WARN_ON(ret))
		goto set_func0_err;
	regdata = atbm_sdio_f0_readb(self->func, ATBM_SDIO_CCCR_IF, &ret);
	wifi_printk(WIFI_IF,"ATBM_SDIO_BUS_WIDTH_4BIT -- 0x%x:0x%x\n",regdata,ATBM_SDIO_BUS_WIDTH_4BIT);
	/**********************/
	wifi_printk(WIFI_IF,"SDIO_BUS_ENABLE_FUNC++\n");
	regdata = atbm_sdio_f0_readb(self->func, ATBM_SDIO_CCCR_IOEx, &ret);
	if (ATBM_WARN_ON(ret))
		goto set_func0_err;
	regdata |= ATBM_BIT(func_num);
	wifi_printk(WIFI_IF,"SDIO_BUS_ENABLE_FUNC regdata %x\n",regdata);
	atbm_sdio_f0_writeb(self->func, regdata, ATBM_SDIO_CCCR_IOEx, &ret);
	if (ATBM_WARN_ON(ret))
		goto set_func0_err;
	regdata = atbm_sdio_f0_readb(self->func, ATBM_SDIO_CCCR_IOEx, &ret);
	wifi_printk(WIFI_IF,"SDIO_BUS_ENABLE_FUNC -- 0x%x\n",regdata);
	/**********************/
set_func0_err:
	atbm_sdio_set_block_size(self,0,ATBM_SDIO_BLOCK_SIZE);
	/* Restore the WLAN function number */
	atbm_sdio_release_host(self->func);
	return 0;
}
/**************************************************************************************************************
 * 函数名: atbm_sdio_align_size
 *
 * 功能描述:
 *   计算对齐后的数据大小。这是 sbus_ops.align_size 的实现。
 *   SDIO 传输通常需要按块大小对齐，此函数计算实际需要传输的数据大小。
 *
 * 使用场景:
 *   - 在进行 SDIO 数据传输前，计算对齐后的传输大小
 *   - 由 sbus_ops 结构体中的 align_size 指针引用
 *
 * 参数说明:
 *   @self: sbus_priv 结构体指针
 *   @size: 原始数据大小（字节）
 *
 * 返回值:
 *   对齐后的数据大小（字节），通常是块大小的整数倍
 *
 * 注意事项:
 *   - 对齐大小取决于 SDIO 块大小设置（通常为 256 字节）
 *
 **************************************************************************************************************/
static atbm_uint32 atbm_sdio_align_size(struct sbus_priv *self, atbm_uint32 size)
{
	atbm_uint32 aligned = atbm_sdio_alignsize(self->func, size);
	return aligned;
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_set_block_size
 *
 * 功能描述:
 *   设置 SDIO 块大小。这是 sbus_ops.set_block_size 的实现。
 *   块大小影响数据传输效率，通常设置为 256 字节。
 *
 * 使用场景:
 *   - 在 SDIO 功能使能后调用
 *   - 在固件下载前设置下载块大小
 *   - 在固件下载后设置传输块大小
 *   - 由 sbus_ops 结构体中的 set_block_size 指针引用
 *
 * 参数说明:
 *   @self: sbus_priv 结构体指针
 *   @type: 功能类型（0: 数据功能, 非0: 命令功能）
 *   @size: 块大小（字节），通常为 ATBM_SDIO_BLOCK_SIZE (256)
 *
 * 返回值:
 *   0:  成功
 *   <0: 失败
 *
 * 注意事项:
 *   - 必须在使能功能后设置
 *   - 不同功能可能有不同的块大小
 *
 **************************************************************************************************************/
int atbm_sdio_set_block_size(struct sbus_priv *self, int type, atbm_uint32 size)
{
	if(type && self->cmd_func){
		/* 设置命令功能的块大小 */
		return atbm_sdio_set_blocksize(self->cmd_func, size);
	}else{
		/* 设置数据功能的块大小 */
		return atbm_sdio_set_blocksize(self->func, size);
	}
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_pm
 *
 * 功能描述:
 *   SDIO 电源管理函数。这是 sbus_ops.power_mgmt 的实现。
 *   用于控制 SDIO 设备的挂起和恢复。
 *
 * 使用场景:
 *   - 系统进入低功耗模式时调用（suspend = ATBM_TRUE）
 *   - 系统从低功耗模式恢复时调用（suspend = ATBM_FALSE）
 *   - 由 sbus_ops 结构体中的 power_mgmt 指针引用
 *
 * 参数说明:
 *   @self:    sbus_priv 结构体指针
 *   @suspend: 挂起标志（ATBM_TRUE: 挂起, ATBM_FALSE: 恢复）
 *
 * 返回值:
 *   0: 成功
 *
 * 注意事项:
 *   - 当前实现为空，根据需要可添加具体的电源管理逻辑
 *
 **************************************************************************************************************/
static int atbm_sdio_pm(struct sbus_priv *self, ATBM_BOOL  suspend)
{
	int ret = 0;
	return ret;
}

/**************************************************************************************************************
 * 函数名: sdio_dvobj_init
 *
 * 功能描述:
 *   初始化 SDIO 设备对象结构体。分配并初始化 dvobj_priv 结构体，
 *   用于存储 SDIO 设备相关的私有数据。
 *
 * 使用场景:
 *   - 在 atbm_sdio_probe() 中调用，初始化设备对象
 *
 * 返回值:
 *   成功: 返回 dvobj_priv 结构体指针
 *   失败: 返回 ATBM_NULL
 *
 * 实现机制:
 *   1. 分配 dvobj_priv 结构体内存
 *   2. 初始化 TX/RX 描述符数量
 *
 **************************************************************************************************************/
static struct dvobj_priv *sdio_dvobj_init()
{
	struct dvobj_priv *pdvobjpriv=ATBM_NULL;
	
	/* 分配设备对象结构体 */
	pdvobjpriv = (struct dvobj_priv *)atbm_kzalloc(sizeof(*pdvobjpriv),GFP_KERNEL);
	if (!pdvobjpriv){
		wifi_printk(WIFI_IF, "Can't allocate USB dvobj.");
		goto exit;
	}
	/* 初始化 TX/RX 描述符数量 */
	pdvobjpriv->tx_desc_num_put = TX_DESC_NUM;
	pdvobjpriv->rx_desc_num_put = RX_DESC_NUM;
exit:
	return pdvobjpriv;
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_desc_free
 *
 * 功能描述:
 *   释放 SDIO 描述符资源。释放 TX/RX 描述符数组中的所有资源，
 *   包括描述符结构体和关联的 SKB 缓冲区。
 *
 * 使用场景:
 *   - 在设备断开时调用，释放 RX/TX 描述符
 *   - 在初始化失败时调用，清理已分配的资源
 *
 * 参数说明:
 *   @self:    sbus_priv 结构体指针
 *   @pDesc:   描述符数组指针
 *   @max_num: 描述符数量
 *
 * 实现机制:
 *   1. 释放 DMA 缓冲区（如果启用了聚合 TX）
 *   2. 循环释放每个描述符的资源
 *
 **************************************************************************************************************/
atbm_void atbm_sdio_desc_free(struct sbus_priv *self,struct sbus_desc *pDesc,int max_num)
{
	int i=0;
#if CONFIG_INF_AGGR_TX
	/* 释放 DMA 聚合缓冲区 */
	if(self->tx_dma_addr_buffer){
		atbm_kfree(self->tx_dma_addr_buffer);
		self->tx_dma_addr_buffer = ATBM_NULL;
	}
#endif //CONFIG_INF_AGGR_TX
	/* 循环释放每个描述符的资源 */
	for(i=0;i<max_num;i++){
		atbm_kfree(pDesc[i].test_desc);
		pDesc[i].test_desc = ATBM_NULL;
		if(pDesc[i].test_skb){
			atbm_dev_kfree_skb(pDesc[i].test_skb);
			pDesc[i].test_skb = ATBM_NULL;
		}
	}
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_desc_malloc
 *
 * 功能描述:
 *   分配 SDIO 描述符资源。为 TX/RX 描述符数组分配内存，
 *   初始化每个描述符的基本信息。
 *
 * 使用场景:
 *   - 在 atbm_sdio_probe() 中调用，分配 RX/TX 描述符
 *
 * 参数说明:
 *   @self:    sbus_priv 结构体指针
 *   @pDesc:   描述符数组指针
 *   @max_num: 描述符数量
 *   @len:     缓冲区长度（当前未使用）
 *
 * 返回值:
 *   0:         成功
 *   -ATBM_ENOMEM: 内存分配失败
 *
 * 实现机制:
 *   1. 循环分配每个描述符的结构体
 *   2. 初始化描述符的基本信息（ID、对象指针等）
 *   3. 如果分配失败，释放已分配的资源
 *
 **************************************************************************************************************/
int atbm_sdio_desc_malloc(struct sbus_priv *self,struct sbus_desc *pDesc,int max_num,int len)
{
	int i=0;
	/* 循环分配每个描述符 */
	for(i=0;i<max_num;i++){
		/* 分配描述符结构体 */
		pDesc[i].test_desc = (ATBM_INF_DESC*)atbm_kzalloc(sizeof(ATBM_INF_DESC),0);
		if (!pDesc[i].test_desc){
			wifi_printk(WIFI_DBG_ERROR, "Can't allocate test_urb.");
			goto __free_urb;
		}
		/* 初始化描述符基本信息 */
		pDesc[i].test_skb = ATBM_NULL;
		pDesc[i].desc_id = i;
		pDesc[i].obj =self;
		
		pDesc[i].link =0;
	}
	return 0;
__free_urb:
	/* 分配失败，释放已分配的资源 */
	for( ;i>=0;--i){
		atbm_kfree(pDesc[i].test_desc);
	}

	return -ATBM_ENOMEM;
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_probe
 *
 * 功能描述:
 *   SDIO 设备探测函数。当 SDIO 设备被系统检测到时，由底层 SDIO 驱动调用此函数。
 *   此函数完成 SDIO 设备的初始化，包括分配资源、使能功能、初始化 HAL 层等。
 *
 * 使用场景:
 *   - 当 WiFi 模块插入或系统启动时，SDIO 子系统检测到设备后自动调用
 *   - 由 atbm_sdio_driver 结构体中的 probe 指针引用
 *
 * 参数说明:
 *   @func: SDIO 功能结构体指针，包含 SDIO 设备信息
 *   @id:   SDIO 设备 ID 结构体指针，用于设备匹配
 *
 * 返回值:
 *   0:  成功
 *   <0: 失败
 *
 * 实现机制:
 *   1. 检查功能编号是否匹配
 *   2. 分配 sbus_priv 结构体
 *   3. 初始化自旋锁和设置功能指针
 *   4. 使能 SDIO 功能
 *   5. 分配 RX/TX 描述符
 *   6. 调用 Atbmwifi_halEntry() 初始化 HAL 层
 *
 * 注意事项:
 *   - 只处理指定功能编号（ATBM_SDIO_CARE_NUM）的设备
 *   - 如果初始化失败，需要释放所有已分配的资源
 *
 **************************************************************************************************************/
int atbm_sdio_probe(struct atbm_sdio_func *func,
			      const struct atbm_sdio_device_id *id)
{
	struct sbus_priv *self;
	int ret;

	wifi_printk(WIFI_IF,"Probe called\n");

	/* 步骤 1: 检查功能编号是否匹配 (IPRO7 port: struct member is `func`, not `num`) */
	if(func->func != ATBM_SDIO_CARE_NUM)
		return 0;	
	//atbm_atomic_set(&g_wtd.wtd_probe, 0);
#ifdef LINUX_OS
	/* Linux 特定的 SDIO 卡特性设置 */
	func->card->quirks|=MMC_QUIRK_LENIENT_FN0;
	func->card->quirks |= MMC_QUIRK_BLKSZ_FOR_BYTE_MODE;
#endif

	/* 步骤 2: 分配 sbus_priv 结构体 */
	self = (struct sbus_priv *)atbm_kzalloc(sizeof(*self), GFP_KERNEL);
	if (!self) {
		wifi_printk(WIFI_DBG_ERROR, "Can't allocate SDIO sbus_priv.");
		return -1;
	}
	
	/* 步骤 3: 初始化自旋锁和设置功能指针 */
	atbm_spin_lock_init(&self->lock);
	self->func = func;
	self->drvobj = sdio_dvobj_init();
	atbm_sdio_set_drvdata(func, self);
	
	/* 步骤 4: 使能 SDIO 功能 */
	atbm_sdio_claim_host(func);
	ret=atbm_sdio_enable_func(func);
	if(ret){
		atbm_sdio_disable_func(func);
		atbm_sdio_release_host(func);
		atbm_kfree(self);
		return -1;
	}
	atbm_sdio_release_host(func);
	ret = atbm_sdio_desc_malloc(self,self->rx_desc,RX_DESC_NUM,RX_BUFFER_SIZE);
	if (ret != 0){
		wifi_printk(WIFI_DBG_ERROR, "Can't allocate rx_desc.");
		return ret;
	}
	/*3---alloc tx_urb*/
	ret = atbm_sdio_desc_malloc(self,self->tx_desc,TX_DESC_NUM,TX_BUFFER_SIZE);
	if (ret != 0){
		wifi_printk(WIFI_DBG_ERROR, "Can't allocate tx_desc.");
		return ret;
	}
	ret=Atbmwifi_halEntry(self);
	if (ret) {
		atbm_kfree(self);
		atbm_sdio_claim_host(func);
		atbm_sdio_disable_func(func);
		atbm_sdio_release_host(func);
		atbm_sdio_set_drvdata(func, NULL);
		//atbm_atomic_set(&g_wtd.wtd_probe, -1);
		atbm_wifi_set_init_flag(0);
	}
	else {
		//atbm_atomic_set(&g_wtd.wtd_probe, 1);
		wifi_printk(WIFI_IF,"[atbm_wtd]:set wtd_probe = 1\n");
		atbm_wifi_set_init_flag(1);
	}
	return ret;
}

void atbm_build_wsm_header_v0(struct atbmwifi_common *hw_priv, struct wsm_hdr_tx *wsm_tx, int *tx_len){
	atbm_uint16 wsm_len_u16[2];
	atbm_uint16 wsm_len_sum;
	wsm_len_u16[0] = wsm_tx->u.common.total_len & 0xff;
	wsm_len_u16[1] = (wsm_tx->u.common.total_len >> 8) & 0xff;
	wsm_len_sum = wsm_len_u16[0] + wsm_len_u16[1];
	if (wsm_len_sum & ATBM_BIT(8)){
	  wsm_tx->u.common.mark = atbm_cpu_to_le16(((wsm_len_sum + 1) & 0xff) << 8);
	}else{
	  wsm_tx->u.common.mark = atbm_cpu_to_le16((wsm_len_sum & 0xff) << 8);
	}
}

int atbm_sdio_tx_submit(ATBM_INF_DESC *desc)
{
	atbm_uint32 status;

	status = atbm_os_MsgQ_Send(&atbm_sdio_tx_msgQ, &desc,sizeof(ATBM_INF_DESC*),ATBM_TX_WAIT_FOREVER);
	if (0 != status){
		wifi_printk(WIFI_ALWAYS, "atbm_sdio_tx_msgQ send failed 0x%x\n", status);
	}

#if PROCESS_TXRX_IN_ONE_THREAD
	extern struct atbmwifi_common g_hw_prv;
#if PROCESS_RX_IN_IRQ_THREAD
	atbm_sdio_wakeup_irq_thread(g_hw_prv.sbus_priv->func);
#else
	atbm_wakeupThreadInternal(g_hw_prv.atbm_sdio_rx_thread);
#endif
#endif
	return status;
}

int atbm_sdio_rx_submit(ATBM_INF_DESC *desc)
{
	atbm_uint32 status;

	status = atbm_os_MsgQ_Send(&atbm_sdio_rx_msgQ, &desc,sizeof(ATBM_INF_DESC*),ATBM_TX_WAIT_FOREVER);
	if (0 != status){
		wifi_printk(WIFI_ALWAYS, "atbm_sdio_rx_msgQ send failed 0x%x\n", status);
	}
	
	return status;
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_wait_enough_space
 *
 * 功能描述:
 *   等待硬件 TX 缓冲区有足够的空间来发送指定数量的数据帧。这是一个阻塞函数，会循环检查硬件
 *   缓冲区状态，直到有足够空间或超时。与 atbm_sdio_have_enough_space() 的区别是，此函数会
 *   主动等待硬件释放缓冲区，而不是立即返回。
 *
 * 使用场景:
 *   - 在 SDIO TX 队列处理中，当硬件缓冲区不足时，需要等待硬件处理完数据并释放缓冲区
 *   - 用于实现硬件流控，防止硬件缓冲区溢出
 *   - 在 atbm_sdio_tx_queue_process() 中，当 wait == ATBM_TRUE 时调用
 *
 * 参数说明:
 *   @hw_priv: WiFi 硬件私有数据结构指针，包含硬件状态信息（如 hw_bufs_free, n_xmits 等）
 *   @n_needs: 需要检查的缓冲区数量，通常等于要发送的帧数量（frame_cnt）
 *
 * 返回值:
 *   ATBM_TRUE:  硬件有足够的缓冲区空间（等待成功）
 *   ATBM_FALSE: 等待超时或任务终止（等待失败）
 *
 * 实现机制:
 *   1. 首先检查缓存的硬件缓冲区数量
 *   2. 如果不足，循环等待：
 *      a. 从硬件读取最新的空闲缓冲区数量
 *      b. 检查是否有足够空间
 *      c. 如果仍不足，等待 5ms 后重试
 *      d. 最多循环 MAX_LOOP_POLL_CNT 次（约 6 秒）
 *   3. 在等待过程中检查任务终止标志
 *
 * 注意事项:
 *   - 这是一个阻塞函数，会占用当前任务/线程
 *   - 超时时间约为 6 秒（MAX_LOOP_POLL_CNT = 2*3000，每次等待 5ms）
 *   - 如果硬件处理速度慢，可能导致长时间等待
 *   - 在任务终止时会立即返回 ATBM_FALSE
 *
 * 相关函数:
 *   - atbm_sdio_have_enough_space(): 快速检查，不等待
 *   - atbm_sdio_get_tx_free_buffers(): 从硬件读取空闲缓冲区数量
 *   - atbm_sdio_tx_queue_process(): 调用此函数的队列处理函数
 *
 * 性能考虑:
 *   - 每次循环都会读取硬件寄存器，有一定开销
 *   - 等待间隔为 5ms，避免过于频繁的硬件访问
 *   - 超时后返回失败，避免无限等待
 *
 **************************************************************************************************************/
ATBM_BOOL atbm_sdio_wait_enough_space(struct atbmwifi_common	*hw_priv, atbm_uint32 n_needs)
{
#define MAX_LOOP_POLL_CNT  (2*3000)  /* 最大循环次数：2*3000 = 6000 次，每次等待 5ms，总超时时间约 30 秒 */
	atbm_uint32 hw_xmited = 0;        /* 硬件已发送完成的帧数量（当前未使用，保留用于调试） */
	ATBM_BOOL enough = ATBM_FALSE;   /* 是否有足够缓冲区的标志 */
	int ret = 0;                      /* 返回值（当前未使用，保留用于错误处理） */
	int loop = 0;                     /* 循环计数器，用于超时控制 */
	atbm_uint32 print = 0;            /* 调试信息打印标志，避免重复打印 */

	/* 步骤 1: 快速检查缓存的硬件缓冲区数量 */
	/* 使用缓存值进行初始检查，避免不必要的等待 */
	enough = hw_priv->hw_bufs_free >= n_needs ? ATBM_TRUE : ATBM_FALSE;
	
	/* 步骤 2: 如果缓存值不足，循环等待硬件释放缓冲区 */
	while(enough == ATBM_FALSE){
		/* 步骤 2.1: 检查任务终止标志 */
		/* 如果底层处理任务已终止，立即返回失败，避免无限等待 */
		if(atbm_bh_is_term(hw_priv)){
			wifi_printk(WIFI_DBG_ERROR, "%s:bh term\n",__func__);
			return ATBM_FALSE;
		}

		/* 步骤 2.2: 从硬件读取最新的空闲缓冲区数量 */
		/* 硬件可能已经处理完一些数据包，释放了缓冲区，需要读取最新状态 */
		hw_priv->hw_bufs_free = atbm_sdio_get_tx_free_buffers(hw_priv);
		
		/* 步骤 2.3: 检查读取结果（当前未使用，保留用于错误处理） */
		if(ret){
			enough = ATBM_FALSE;
			break;
		}
		
		/* 步骤 2.4: 使用最新值重新检查是否有足够空间 */
		enough = hw_priv->hw_bufs_free >= n_needs ? ATBM_TRUE : ATBM_FALSE;
		
		/* 步骤 2.5: 更新硬件缓冲区足够标志（用于其他模块查询） */
		/* 使用自旋锁保护，确保并发安全 */
		atbm_spin_lock_bh(&hw_priv->tx_com_lock);
		hw_priv->hw_buf_enough = enough;
		atbm_spin_unlock_bh(&hw_priv->tx_com_lock);

		/* 步骤 2.6: 如果仍不足，等待后重试 */
		if(enough == ATBM_FALSE){
			loop ++;  /* 增加循环计数 */
			
			/* 步骤 2.6.1: 检查是否超时 */
			/* 如果循环次数超过最大值，退出等待，返回失败 */
			if(loop>=MAX_LOOP_POLL_CNT)
				break;
			
			/* 步骤 2.6.2: 打印调试信息（仅打印一次，避免日志过多） */
			/* 在循环 3 次后打印一次，帮助诊断问题 */
			if((loop >= 3)&&(print == 0)){			
				wifi_printk(WIFI_DBG_MSG, "%s:n_xmits(%d),hw_xmited(%d),need(%d)\n",__func__,
					hw_priv->n_xmits,    /* 驱动已提交到硬件的帧数量 */
					hw_xmited,           /* 硬件已发送完成的帧数量（当前未使用） */
					n_needs);           /* 需要检查的缓冲区数量 */
				print = 1;  /* 设置打印标志，避免重复打印 */
			}
			
			/* 步骤 2.6.3: 等待硬件释放缓冲区 */
			/* 等待 5ms 后重试，给硬件时间处理数据并释放缓冲区 */
			/* 使用事件等待机制，可以被硬件完成事件唤醒 */
			atbm_os_wait_event_timeout(&hw_priv->wake_up_wq, 5);
		}
	}

	/* 步骤 3: 返回检查结果 */
	/* ATBM_TRUE: 有足够空间（等待成功） */
	/* ATBM_FALSE: 等待超时或任务终止（等待失败） */
	return enough;
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_have_enough_space
 *
 * 功能描述:
 *   快速检查硬件 TX 缓冲区是否有足够的空间。与 atbm_sdio_wait_enough_space() 的区别是，
 *   此函数不会等待，只是快速检查当前状态并立即返回。
 *
 * 使用场景:
 *   - 在 TX 队列处理中，快速检查是否可以发送数据
 *   - 当 wait == ATBM_FALSE 时，在 atbm_sdio_tx_queue_process() 中调用
 *   - 用于非阻塞的缓冲区检查
 *
 * 参数说明:
 *   @hw_priv: WiFi 硬件私有数据结构指针
 *   @n_needs: 需要检查的缓冲区数量
 *
 * 返回值:
 *   ATBM_TRUE:  硬件有足够的缓冲区空间
 *   ATBM_FALSE: 硬件缓冲区不足
 *
 * 实现机制:
 *   1. 首先检查缓存的硬件缓冲区数量
 *   2. 如果不足，从硬件读取最新的空闲缓冲区数量
 *   3. 再次检查是否有足够空间
 *
 * 注意事项:
 *   - 这是非阻塞函数，会立即返回
 *   - 如果需要等待，应使用 atbm_sdio_wait_enough_space()
 *
 **************************************************************************************************************/
ATBM_BOOL atbm_sdio_have_enough_space(struct atbmwifi_common	*hw_priv, atbm_uint32 n_needs)
{
	ATBM_BOOL enough = ATBM_FALSE;
	int ret = 0;

	/* 步骤 1: 使用缓存值快速检查 */
	enough = hw_priv->hw_bufs_free >= n_needs ? ATBM_TRUE : ATBM_FALSE;
	if(enough == ATBM_FALSE){
		/* 步骤 2: 从硬件读取最新的空闲缓冲区数量 */
		hw_priv->hw_bufs_free = atbm_sdio_get_tx_free_buffers(hw_priv);
		/* 步骤 3: 再次检查是否有足够空间 */
		enough = hw_priv->hw_bufs_free >= n_needs ? ATBM_TRUE : ATBM_FALSE;
	}
	return enough;
}

#if SUPPORT_LIGHT_SLEEP
/**************************************************************************************************************
 * 函数名: atbm_device_wakeup
 *
 * 功能描述:
 *   唤醒处于休眠状态的 WiFi 设备。当设备处于轻度休眠（Light Sleep）模式时，
 *   主机需要通过此函数唤醒设备，然后才能进行数据传输。
 *
 * 使用场景:
 *   - 在 TX 发送前，如果设备处于休眠状态，需要先唤醒
 *   - 在 RX 接收前，如果设备处于休眠状态，需要先唤醒
 *   - 由 _atbm_light_sleep_try_to_wakeup() 调用
 *
 * 参数说明:
 *   @hw_priv: WiFi 硬件私有数据结构指针
 *
 * 返回值:
 *   1:  成功唤醒
 *   -1: 唤醒失败（超时）
 *   <0: 其他错误
 *
 * 实现机制:
 *   1. 设置 WLAN_UP 位为 1，请求唤醒设备
 *   2. 循环读取控制寄存器，等待 WLAN_RDY 位变为 1
 *   3. 如果超时（约 2 秒），返回失败
 *   4. 唤醒成功后，如果有待接收数据，唤醒 RX 处理线程
 *
 * 注意事项:
 *   - 超时时间约为 2 秒（1000 次循环，每次等待 2ms）
 *   - 需要在 sleep_mutex 保护下调用
 *
 **************************************************************************************************************/
static int atbm_device_wakeup(struct atbmwifi_common *hw_priv)
{
	atbm_uint16 ctrl_reg;
	int ret=0;
	int loop = 1;

	/* 步骤 1: 设置 WLAN_UP 位为 1，请求唤醒设备 */
	/* To force the device to be always-on, the host sets WLAN_UP to 1 */
	ret = atbm_reg_write_16(hw_priv, ATBM_HIFREG_CONTROL_REG_ID,
			ATBM_HIFREG_CONT_WUP_BIT);
	if (ATBM_WARN_ON(ret))
		return ret;

	/* 步骤 2: 循环等待设备返回 WLAN_RDY = 1 */
	while(1){
		ret = atbm_bh_read_ctrl_reg(hw_priv, &ctrl_reg);
		if (ATBM_WARN_ON(ret)){
			wifi_printk(WIFI_ALWAYS,"[BH] wake up err\n");
			goto __loop_continue;
		}
		/* If the device returns WLAN_RDY as 1, the device is active and will
		 * remain active. */
		/* 检查 WLAN_RDY 位，如果为 1 表示设备已唤醒 */
		if (ctrl_reg & ATBM_HIFREG_CONT_RDY_BIT) {
			wifi_printk(WIFI_BH,"[BH] Device awake.<%d>\n",loop);
			ret= 1;
			goto __wup_exit;
		}
__loop_continue:
		/* 步骤 3: 超时检查（约 2 秒） */
		//wait 1S,else fail
		if(loop++ > 1000){
			ret = atbm_bh_read_ctrl_reg(hw_priv, &ctrl_reg);
			wifi_printk(WIFI_ALWAYS,"[BH] Device wakeup Fail<%d> %x\n",loop,ctrl_reg);
			ret= -1;
			goto __wup_exit;
		}
		atbm_mdelay(2);  /* 等待 2ms 后重试 */
	}
	wifi_printk(WIFI_BH,"[BH]  wakeup err\n");
__wup_exit:
	/* 步骤 4: 唤醒 RX 处理线程（如果有待接收数据） */
#if PROCESS_RX_IN_IRQ_THREAD
	//#error "Add code to wake up sdio irq thread here"
	atbm_sdio_wakeup_irq_thread(hw_priv->sbus_priv->func);
#else
	if(ctrl_reg & ATBM_HIFREG_CONT_NEXT_LEN_MASK){
		atbm_wakeupThreadInternal(hw_priv->atbm_sdio_rx_thread);
	}
#endif
	return ret;
}

#if LIGHT_SLEEP_DELAY
/**************************************************************************************************************
 * 函数名: atbm_light_sleep_delay_expire
 *
 * 功能描述:
 *   延迟休眠定时器回调函数。当启用延迟休眠（LIGHT_SLEEP_DELAY）时，
 *   此函数在定时器到期后被调用，使设备进入休眠状态。
 *
 * 使用场景:
 *   - 由 _atbm_light_sleep_try_to_sleep() 注册的定时器回调
 *   - 用于避免频繁的休眠唤醒切换
 *
 * 参数说明:
 *   @data1: WiFi 硬件私有数据结构指针
 *   @data2: 未使用
 *
 * 实现机制:
 *   1. 获取休眠互斥锁
 *   2. 再次检查是否可以进入休眠
 *   3. 如果可以，写入 0 到控制寄存器，使设备进入休眠
 *
 **************************************************************************************************************/
static void atbm_light_sleep_delay_expire(atbm_void *data1,atbm_void *data2){
	struct atbmwifi_common	*hw_priv = data1;
	atbm_os_mutexLock(&hw_priv->sleep_mutex, 0);
	/* 再次检查是否可以进入休眠 */
	if((hw_priv->device_can_sleep == ATBM_FALSE) && (hw_priv->ps_flag == 0)){
		wifi_printk(WIFI_DBG_MSG, "tx Enter light sleep!!\n");
		/* 写入 0 到控制寄存器，使设备进入休眠 */
		ATBM_WARN_ON(atbm_reg_write_16(hw_priv, ATBM_HIFREG_CONTROL_REG_ID, 0));
		hw_priv->device_can_sleep = ATBM_TRUE;
	}
	atbm_os_mutexUnLock(&hw_priv->sleep_mutex);
}
#endif

/**************************************************************************************************************
 * 函数名: _atbm_light_sleep_try_to_wakeup
 *
 * 功能描述:
 *   尝试唤醒处于休眠状态的设备。如果设备处于休眠状态，执行唤醒操作；
 *   同时设置对应的唤醒原因位，防止设备再次休眠。
 *
 * 使用场景:
 *   - 在 TX 发送前调用，确保设备处于唤醒状态
 *   - 在 RX 接收前调用，确保设备处于唤醒状态
 *   - 通过宏 atbm_light_sleep_try_to_wakeup() 调用
 *
 * 参数说明:
 *   @hw_priv: WiFi 硬件私有数据结构指针
 *   @reason:  唤醒原因（ATBM_SDIO_WAKEUP_REASON_TX_PROGRESS 或
 *             ATBM_SDIO_WAKEUP_REASON_RX_PROGRESS）
 *
 * 实现机制:
 *   1. 获取休眠互斥锁
 *   2. 如果设备处于休眠状态，调用 atbm_device_wakeup() 唤醒
 *   3. 设置对应的唤醒原因位
 *
 **************************************************************************************************************/
void _atbm_light_sleep_try_to_wakeup(struct atbmwifi_common *hw_priv, int reason){
	atbm_os_mutexLock(&hw_priv->sleep_mutex, 0);
	/* 如果设备处于休眠状态，执行唤醒 */
	if(hw_priv->device_can_sleep){
		ATBM_WARN_ON(atbm_device_wakeup(hw_priv) < 0);
		hw_priv->device_can_sleep = ATBM_FALSE;
	}
	/* 设置对应的唤醒原因位，防止设备再次休眠 */
	hw_priv->ps_flag |= ATBM_BIT(reason);
	atbm_os_mutexUnLock(&hw_priv->sleep_mutex);
}

/**************************************************************************************************************
 * 函数名: _atbm_light_sleep_try_to_keepalive
 *
 * 功能描述:
 *   保持设备唤醒状态。设置对应的唤醒原因位，防止设备进入休眠。
 *   与 _atbm_light_sleep_try_to_wakeup() 的区别是，此函数不执行唤醒操作，
 *   只是设置标志位。
 *
 * 使用场景:
 *   - 在 RX 接收过程中，保持设备唤醒
 *   - 当设备已经唤醒，只需要防止休眠时使用
 *   - 通过宏 atbm_light_sleep_try_to_keepalive() 调用
 *
 * 参数说明:
 *   @hw_priv: WiFi 硬件私有数据结构指针
 *   @reason:  唤醒原因
 *
 **************************************************************************************************************/
void _atbm_light_sleep_try_to_keepalive(struct atbmwifi_common *hw_priv, int reason){
	atbm_os_mutexLock(&hw_priv->sleep_mutex, 0);
	/* 设置对应的唤醒原因位，保持设备唤醒状态 */
	hw_priv->ps_flag |= ATBM_BIT(reason);
	atbm_os_mutexUnLock(&hw_priv->sleep_mutex);
}

/**************************************************************************************************************
 * 函数名: _atbm_light_sleep_try_to_sleep
 *
 * 功能描述:
 *   尝试使设备进入休眠状态。清除对应的唤醒原因位，如果所有唤醒原因都已清除，
 *   则使设备进入休眠。
 *
 * 使用场景:
 *   - 在 TX 发送完成后调用
 *   - 在 RX 接收完成后调用
 *   - 通过宏 atbm_light_sleep_try_to_sleep() 调用
 *
 * 参数说明:
 *   @hw_priv: WiFi 硬件私有数据结构指针
 *   @reason:  唤醒原因
 *
 * 实现机制:
 *   1. 获取休眠互斥锁
 *   2. 清除对应的唤醒原因位
 *   3. 如果所有唤醒原因都已清除（ps_flag == 0），使设备进入休眠
 *   4. 如果启用延迟休眠，注册定时器延迟后再休眠
 *
 **************************************************************************************************************/
void _atbm_light_sleep_try_to_sleep(struct atbmwifi_common *hw_priv, int reason){
	atbm_os_mutexLock(&hw_priv->sleep_mutex, 0);
	/* 清除对应的唤醒原因位 */
	hw_priv->ps_flag &= ~ATBM_BIT(reason);
	/* 检查是否可以进入休眠 */
	if((hw_priv->device_can_sleep == ATBM_FALSE) && (hw_priv->ps_flag == 0)){
#if LIGHT_SLEEP_DELAY
		/* 启用延迟休眠：注册定时器，延迟后再进入休眠 */
		hw_priv->last_active_time = atbm_GetOsTime();
		atbmwifi_eloop_cancel_timeout(atbm_light_sleep_delay_expire, hw_priv, ATBM_NULL);
		atbmwifi_eloop_register_timeout(0, LIGHT_SLEEP_DELAY, atbm_light_sleep_delay_expire, hw_priv, ATBM_NULL);
#else
		/* 立即进入休眠 */
		wifi_printk(WIFI_DBG_MSG, "tx Enter light sleep!!\n");
		ATBM_WARN_ON(atbm_reg_write_16(hw_priv, ATBM_HIFREG_CONTROL_REG_ID, 0));
		hw_priv->device_can_sleep = ATBM_TRUE;
#endif	
	}
	atbm_os_mutexUnLock(&hw_priv->sleep_mutex);	
}
#endif

static int atbm_sdio_process_tx(struct atbmwifi_common *hw_priv, ATBM_INF_DESC *pDesc){
	struct sbus_desc *tx_desc = (struct sbus_desc *)pDesc->context;
	int tx_count, tx_sg, frame_cnt;
	int status = 0;

#if (ATBM_SUPPORT_SG || CONFIG_INF_AGGR_TX)
	tx_count = tx_desc->frame_cnt;
#else
	tx_count = 1;
#endif
	tx_sg = 0;
	frame_cnt = tx_desc->frame_cnt;	
	do{
		hw_priv->n_xmits += tx_count;
		hw_priv->hw_bufs_free -= tx_count;
		hw_priv->buf_id_tx_offset = tx_count;

#if ATBM_SUPPORT_SG
		if(tx_count > 1){
			if (ATBM_WARN_ON(atbm_data_write_sgs(hw_priv,(const void **)pDesc->buf, pDesc->len))) { 			
				wifi_printk(WIFI_DBG_ERROR, "%s: xmit data err\n",__func__);
				status = -1;
				break;
			}
		}else
#endif
		if (ATBM_WARN_ON(atbm_data_write(hw_priv,pDesc->buf[tx_sg], pDesc->len[tx_sg]))) {		
			wifi_printk(WIFI_DBG_ERROR, "%s: xmit data err\n",__func__);
			status = -1;
			break;
		}
		frame_cnt -= tx_count;
		tx_sg++;
	}while(frame_cnt > 0);
	pDesc->status = status;
	/*Tx Urb complete*/
	pDesc->complete(pDesc);
	return status;
}

/*
 *1. No desc return 0
 *2. Have desc need wait 0x10000
 *3. Have desc write bytes return total len + 0x10000
 *4. No desc write bytes return total len
*/
int atbm_sdio_tx_queue_process(struct atbmwifi_common *hw_priv, atbm_uint32 timeout, ATBM_BOOL wait){
	int status;
	static ATBM_INF_DESC *pDesc = ATBM_NULL;
	struct sbus_desc *tx_desc;
	ATBM_BOOL enough;
	int total_len = 0;

	if(!pDesc){
		status = atbm_os_MsgQ_Recv(&atbm_sdio_tx_msgQ,&pDesc,sizeof(ATBM_INF_DESC *),timeout);
		if(status != 0){
			pDesc = ATBM_NULL;
			return total_len;
		}
	}
	atbm_light_sleep_try_to_wakeup(hw_priv, ATBM_SDIO_WAKEUP_REASON_TX_PROGRESS);
TxAction:
	tx_desc = (struct sbus_desc *)pDesc->context;
	enough = atbm_sdio_have_enough_space(hw_priv,tx_desc->frame_cnt);
	if(enough == ATBM_FALSE){
		goto TxWait;
	}
TxContinue:
	total_len += 1;
	status = atbm_sdio_process_tx(hw_priv, pDesc);
	if(status){
		goto TxErr;
	}
	status = atbm_os_MsgQ_Recv(&atbm_sdio_tx_msgQ,&pDesc,sizeof(ATBM_INF_DESC *),timeout);
	if(status != 0){
		goto TxFinished;
	}
	goto TxAction;
TxWait:
	if(wait == ATBM_FALSE){
		total_len += 0x10000;
		return total_len;
	}
	if((enough == ATBM_FALSE)&&(atbm_sdio_wait_enough_space(hw_priv,tx_desc->frame_cnt) == ATBM_FALSE)){
		wifi_printk(WIFI_DBG_ERROR, "%s: wait space timeout\n",__func__);
		goto TxErr;
	}
	goto TxContinue;
TxErr:
	pDesc->status = -1;
	/*Tx Urb complete*/
	pDesc->complete(pDesc);
TxFinished:
	pDesc = ATBM_NULL;
	atbm_light_sleep_try_to_sleep(hw_priv, ATBM_SDIO_WAKEUP_REASON_TX_PROGRESS);	
	return total_len;
}

#if ATBM_TX_WRITE_TASK
int atbm_sdio_tx_queue_task(void *arg, int wakeup_state)
{
	struct atbmwifi_common *hw_priv = (struct atbmwifi_common *)arg;

	while(!hw_priv->bh_term){
		atbm_sdio_tx_queue_process(hw_priv, ATBM_TX_WAIT_FOREVER, ATBM_TRUE);
	}
	return 0;
}
#endif

static int atbm_sdio_process_rx(struct atbmwifi_common *hw_priv, int read_len, ATBM_INF_DESC *pDesc){
	atbm_uint32 read_len_lsb = 0;
	atbm_uint32 read_len_msb = 0;
	atbm_uint32 alloc_len = 0;
	int status = 0;
	struct sbus_desc *rx_desc;

	/* Add SIZE of PIGGYBACK reg (CONTROL Reg)
	 * to the NEXT Message length + 2 Bytes for SKB */
	alloc_len = read_len;
	if (alloc_len % ATBM_SDIO_BLOCK_SIZE ) {
		alloc_len -= (alloc_len % ATBM_SDIO_BLOCK_SIZE );
		alloc_len += ATBM_SDIO_BLOCK_SIZE;
	}

	/* Check if not exceeding CW1200 capabilities */
	if (ATBM_WARN_ON(alloc_len > EFFECTIVE_BUF_SIZE)) {	
		wifi_printk(WIFI_ALWAYS,"Read aligned len: %d\n",
			alloc_len);
	}

	rx_desc = (struct sbus_desc *)pDesc->context;
	rx_desc->test_skb = atbm_dev_alloc_skb(alloc_len);
	if (ATBM_WARN_ON(atbm_data_read(hw_priv,ATBM_OS_SKB_DATA(rx_desc->test_skb), alloc_len))) {	
		wifi_printk(WIFI_ALWAYS, "%s: rx data err\n",__func__);
		status = -1;
	}
	//pDesc->actual_length = ((struct wsm_hdr *)(pDesc->buf[0]))->len;
	pDesc->actual_length = alloc_len;
	pDesc->status = status;

	read_len = atbm_sdio_get_rx_len(hw_priv, __atbm_le16_to_cpu(
			((atbm_uint16*)ATBM_OS_SKB_DATA(rx_desc->test_skb))[alloc_len / 2 - 1]));
	pDesc->complete(pDesc);
	return read_len;
}

/*
 *1. No desc return 0x10000
 *2. Have desc no read len return 0 
 *3. Have desc read bytes return total len
 *4. Have desc read bytes -> no desc 0x10000 + total_len
*/
int atbm_sdio_rx_queue_process(struct atbmwifi_common *hw_priv, int wakeup_state, atbm_uint32 timeout){
	atbm_uint16 rx_len = 0;
	int total_len = 0;
	static ATBM_INF_DESC *pDesc = ATBM_NULL;
	int status;

	if(!pDesc){
		status = atbm_os_MsgQ_Recv(&atbm_sdio_rx_msgQ,&pDesc,sizeof(ATBM_INF_DESC*), timeout);
		if(status != 0){
			pDesc = ATBM_NULL;
			total_len += 0x10000;
			goto RxExit;
		}
	}

	rx_len = atbm_sdio_get_rx_len(hw_priv, rx_len);
	if(rx_len > 0){
		if(wakeup_state == 0){
			wifi_printk(WIFI_ALWAYS, "BH miss %x\n", rx_len);
		}
		atbm_light_sleep_try_to_keepalive(hw_priv, ATBM_SDIO_WAKEUP_REASON_RX_PROGRESS);
	}else{
		goto RxExit;
	}
RxAction:
	total_len += 1;
	rx_len = atbm_sdio_process_rx(hw_priv, rx_len, pDesc);
	if(rx_len > 0){
		status = atbm_os_MsgQ_Recv(&atbm_sdio_rx_msgQ,&pDesc,sizeof(ATBM_INF_DESC*), timeout);
		if(status != 0){
			pDesc = ATBM_NULL; //Not sure if packet remains
			total_len += 0x10000;
			goto RxExit;
		}
		goto RxAction;
	}
	pDesc = ATBM_NULL;
	atbm_light_sleep_try_to_sleep(hw_priv, ATBM_SDIO_WAKEUP_REASON_RX_PROGRESS);
RxExit:
	return total_len;
}

int atbm_sdio_rx_queue_task(void *arg, int wakeup_state)
{
	struct atbmwifi_common *hw_priv = (struct atbmwifi_common *)arg;
#if PROCESS_TXRX_IN_ONE_THREAD
	int tx_len, rx_len;

	do{
		tx_len = atbm_sdio_tx_queue_process(hw_priv, 0, ATBM_FALSE);
		rx_len = atbm_sdio_rx_queue_process(hw_priv, 1, 0);
		if(rx_len || tx_len)
			continue;
	}while(atbm_atomic_xchg(&hw_priv->atbm_sdio_rx_thread->count, 0) != 0);
#else
	atbm_sdio_rx_queue_process(hw_priv, wakeup_state, ATBM_TX_WAIT_FOREVER);
#endif
	atbm_sdio_host_enable_irq(1); //Enable irq at first rxframe?
	return 0;
}

void atbm_sdio_queue_init(struct atbmwifi_common *hw_priv)
{
	atbm_uint32 status;

	wifi_printk(WIFI_ALWAYS, "atbm_urb_queue_init ==>\n");

	atbm_os_init_waitevent(&hw_priv->wake_up_wq);
	hw_priv->hw_buf_enough = ATBM_TRUE;
#if SUPPORT_LIGHT_SLEEP
	atbm_os_mutexLockInit(&hw_priv->sleep_mutex);
#endif

	// Init TX MessageQ
	status=atbm_os_MsgQ_Create(&atbm_sdio_tx_msgQ,(atbm_uint32 *)&atbm_urb_tx_msqQbuf[0],sizeof(atbm_uint32),TX_DESC_NUM);
	if(status != 0){
		wifi_printk(WIFI_ALWAYS, "atbm_sdio_tx_msgQ create failed 0x%x\n", status);
		return;
	}

	// Init RX MessageQ
	status = atbm_os_MsgQ_Create(&atbm_sdio_rx_msgQ,(atbm_uint32 *)&atbm_urb_rx_msqQbuf,sizeof(atbm_uint32*),RX_DESC_NUM);
	if(status != 0){
		wifi_printk(WIFI_ALWAYS, "atbm_sdio_rx_msgQ create failed 0x%x\n", status);
		return;
	}
#if ATBM_TX_WRITE_TASK && (PROCESS_TXRX_IN_ONE_THREAD == 0)
	//Init TX Task
	hw_priv->atbm_sdio_tx_thread = atbm_createThreadInternal("Atbm_TX",
															atbm_sdio_tx_queue_task,
															hw_priv,
															TX_BH_TASK_PRIO,
															0);
	if (!hw_priv->atbm_sdio_tx_thread){
		wifi_printk(WIFI_DBG_ERROR,"bh_thread Failed\n");
		return;
	}
#endif

#if (PROCESS_RX_IN_IRQ_THREAD == 0)
	//Init RX Task
	hw_priv->atbm_sdio_rx_thread=atbm_createThreadInternal("Atbm_RX",
															atbm_sdio_rx_queue_task,
															hw_priv,
															RX_BH_TASK_PRIO,
															100*ATBM_HZ);
	if (!hw_priv->atbm_sdio_rx_thread){
		wifi_printk(WIFI_DBG_ERROR,"bh_thread Failed\n");
		return;
	}
#endif
	//Urb Init Ready
	wifi_printk(WIFI_ALWAYS, "atbm_urb_queue_init <==\n");
	return ;
}

void atbm_sdio_queue_exit(struct atbmwifi_common *hw_priv)
{
	atbm_uint32 status;
	
	wifi_printk(WIFI_ALWAYS, "atbm_urb_queue_exit ==>\n");

#if ATBM_TX_WRITE_TASK && (PROCESS_TXRX_IN_ONE_THREAD == 0)
	//Delete TX Task
	status = atbm_stopThreadInternal(hw_priv->atbm_sdio_tx_thread);
	if(status != 0){
		wifi_printk(WIFI_ALWAYS, "atbm_urb_tx_thread delete failed 0x%x\n", status);
	}
#endif

#if (PROCESS_RX_IN_IRQ_THREAD == 0)
	//Delete RX Task
	status = atbm_stopThreadInternal(hw_priv->atbm_sdio_rx_thread);
	if(status != 0){
		wifi_printk(WIFI_ALWAYS, "atbm_urb_rx_thread delete failed 0x%x\n", status);
	}
#endif

	//Delete TX MessageQ
	status = atbm_os_MsgQ_Delete(&atbm_sdio_tx_msgQ);
	if(status != 0){
		wifi_printk(WIFI_ALWAYS, "atbm_sdio_tx_msgQ delete failed 0x%x\n", status);
	}

	//Delete RX MessageQ
	status = atbm_os_MsgQ_Delete(&atbm_sdio_rx_msgQ);
	if(status != 0){
		wifi_printk(WIFI_ALWAYS, "atbm_sdio_rx_msgQ delete failed 0x%x\n", status);
	}

	atbm_os_delete_waitevent(&hw_priv->wake_up_wq);
#if SUPPORT_LIGHT_SLEEP
	atbm_os_DeleteMutex(&hw_priv->sleep_mutex);
#endif

	wifi_printk(WIFI_ALWAYS, "atbm_urb_queue_exit <==\n");
	return;
}

#if PROCESS_RX_IN_IRQ_THREAD
/**************************************************************************************************************
 * 函数名: atbm_irq_handler
 *
 * 功能描述:
 *   SDIO 中断处理函数（在中断线程中处理 RX 的版本）。当 WiFi 芯片产生中断时，
 *   由底层 SDIO 驱动调用此函数。此函数直接在中断上下文中处理 RX 数据。
 *
 * 使用场景:
 *   - 当 PROCESS_RX_IN_IRQ_THREAD 宏启用时使用此版本
 *   - WiFi 芯片有数据需要接收时产生中断
 *   - 由 atbm_sdio_irq_subscribe() 注册到底层 SDIO 驱动
 *
 * 参数说明:
 *   @hw_priv: WiFi 硬件私有数据结构指针
 *
 * 实现机制:
 *   1. 检查初始化状态和错误状态
 *   2. 如果启用 PROCESS_TXRX_IN_ONE_THREAD，同时处理 TX 和 RX
 *   3. 否则，唤醒 TX 等待事件，然后处理 RX
 *   4. 重新使能中断
 *
 * 注意事项:
 *   - 此函数在中断上下文中执行
 *   - 需要尽快处理，避免长时间占用中断
 *
 **************************************************************************************************************/
void atbm_irq_handler(struct atbmwifi_common *hw_priv)
{
	int rx_len;
#if PROCESS_TXRX_IN_ONE_THREAD
	int tx_len;
#endif

	/* 步骤 1: 检查初始化状态 */
	/* To force the device to be always-on, the host sets WLAN_UP to 1 */
	if(!hw_priv->init_done){
		wifi_printk(WIFI_ALWAYS,"[BH] irq. init_done =0 drop\n");
		return;
	}
	/* 检查错误状态 */
	if (/* WARN_ON */(hw_priv->bh_error))
		return;

#if PROCESS_TXRX_IN_ONE_THREAD
	/* 步骤 2: 同时处理 TX 和 RX（在同一线程中） */
	do{
		tx_len = atbm_sdio_tx_queue_process(hw_priv, 0, ATBM_FALSE);
		rx_len = atbm_sdio_rx_queue_process(hw_priv, 1, 0);
	}while(rx_len || tx_len);
#else
	/* 步骤 2: 唤醒 TX 等待事件（如果硬件缓冲区不足） */
	atbm_spin_lock_bh(&hw_priv->tx_com_lock);
	if(hw_priv->hw_buf_enough == ATBM_FALSE){
		atbm_os_wakeup_event(&hw_priv->wake_up_wq);
	}
	atbm_spin_unlock_bh(&hw_priv->tx_com_lock);

	/* 步骤 3: 处理 RX 数据 */
	atbm_sdio_rx_queue_process(hw_priv, 1, ATBM_TX_WAIT_FOREVER);
#endif
	/* 步骤 4: 重新使能中断 */
	atbm_sdio_host_enable_irq(1); //Enable irq at first rxframe?
}
#else
/**************************************************************************************************************
 * 函数名: atbm_irq_handler
 *
 * 功能描述:
 *   SDIO 中断处理函数（唤醒 RX 线程的版本）。当 WiFi 芯片产生中断时，
 *   由底层 SDIO 驱动调用此函数。此函数只唤醒 RX 处理线程，不直接处理数据。
 *
 * 使用场景:
 *   - 当 PROCESS_RX_IN_IRQ_THREAD 宏未启用时使用此版本
 *   - WiFi 芯片有数据需要接收时产生中断
 *   - 由 atbm_sdio_irq_subscribe() 注册到底层 SDIO 驱动
 *
 * 参数说明:
 *   @hw_priv: WiFi 硬件私有数据结构指针
 *
 * 实现机制:
 *   1. 检查初始化状态和错误状态
 *   2. 唤醒 TX 等待事件（如果硬件缓冲区不足）
 *   3. 唤醒 RX 处理线程
 *
 * 注意事项:
 *   - 此函数在中断上下文中执行，应该尽快返回
 *   - 实际的 RX 处理在 RX 线程中完成
 *
 **************************************************************************************************************/
void atbm_irq_handler(struct atbmwifi_common *hw_priv)
{

	/* 步骤 1: 检查初始化状态 */
	/* To force the device to be always-on, the host sets WLAN_UP to 1 */
	if(!hw_priv->init_done){
		wifi_printk(WIFI_ALWAYS,"[BH] irq. init_done =0 drop\n");
		return;
	}
	/* 检查错误状态 */
	if (/* WARN_ON */(hw_priv->bh_error))
		return;

	/* 步骤 2: 唤醒 TX 等待事件（如果硬件缓冲区不足） */
	atbm_spin_lock_bh(&hw_priv->tx_com_lock);
	if(hw_priv->hw_buf_enough == ATBM_FALSE){
		atbm_os_wakeup_event(&hw_priv->wake_up_wq);
	}
	atbm_spin_unlock_bh(&hw_priv->tx_com_lock);
	
	/* 步骤 3: 唤醒 RX 处理线程 */
	atbm_wakeupThreadInternal(hw_priv->atbm_sdio_rx_thread);
}
#endif

/**************************************************************************************************************
 * 函数名: atbm_sdio_disconnect
 *
 * 功能描述:
 *   SDIO 设备断开函数。当 SDIO 设备被移除或系统关闭时，由底层 SDIO 驱动调用此函数。
 *   此函数完成 SDIO 设备的清理工作，包括释放资源、禁用功能等。
 *
 * 使用场景:
 *   - 当 WiFi 模块被移除时自动调用
 *   - 系统关闭时调用
 *   - 由 atbm_sdio_driver 结构体中的 remove 指针引用
 *
 * 参数说明:
 *   @func: SDIO 功能结构体指针
 *
 * 实现机制:
 *   1. 释放 WiFi 核心资源
 *   2. 注销中断处理函数
 *   3. 禁用 SDIO 功能
 *   4. 释放 RX/TX 描述符
 *   5. 释放 sbus_priv 结构体
 *
 **************************************************************************************************************/
void atbm_sdio_disconnect(struct atbm_sdio_func *func)
{
	struct sbus_priv *self = atbm_sdio_get_drvdata(func);
	wifi_printk(WIFI_IF,"atbm_sdio_disconnect");
	if (self) {
		//atbm_atomic_set(&g_wtd.wtd_probe, 0);
		/* 步骤 1: 释放 WiFi 核心资源 */
		if (self->core) {
			atbm_core_release(self->core);
			/* 步骤 2: 注销中断处理函数 */
			self->core->sbus_ops->irq_unsubscribe(self->core->sbus_priv);
			//self->core = ATBM_NULL;
		}
		/* 步骤 3: 禁用 SDIO 功能 */
		atbm_sdio_claim_host(func);

		if (self->core) {
			self->core = ATBM_NULL;
		}
		atbm_sdio_disable_func(func);
		atbm_sdio_release_host(func);
		atbm_sdio_set_drvdata(func, ATBM_NULL);
		/* 步骤 4: 释放 RX/TX 描述符 */
		atbm_sdio_desc_free(self,self->rx_desc,RX_DESC_NUM);
		atbm_sdio_desc_free(self,self->tx_desc,TX_DESC_NUM);
		/* 步骤 5: 释放 sbus_priv 结构体 */
		if(self->drvobj)
			atbm_kfree(self->drvobj);
		atbm_kfree(self);
		atbm_wifi_set_init_flag(0);
	}
}

/**************************************************************************************************************
 * 全局变量: atbm_sdio_sbus_ops
 *
 * 功能描述:
 *   SDIO 总线操作结构体实例。包含所有 SDIO 总线操作函数的指针，
 *   由 HAL 层通过 hw_priv->sbus_ops 访问。
 *
 **************************************************************************************************************/
struct sbus_ops atbm_sdio_sbus_ops;

/**************************************************************************************************************
 * 函数名: atbm_sdio_init
 *
 * 功能描述:
 *   SDIO 驱动初始化函数。初始化 sbus_ops 结构体，设置所有 SDIO 操作函数指针，
 *   然后注册 SDIO 驱动到系统。
 *
 * 使用场景:
 *   - 由 atbm_sdio_module_init() 调用
 *   - 在驱动加载时执行
 *
 * 返回值:
 *   0:  成功
 *   <0: 失败
 *
 * 实现机制:
 *   1. 初始化 sbus_ops 结构体的所有函数指针
 *   2. 打印驱动版本信息
 *   3. 注册 SDIO 驱动到系统
 *
 **************************************************************************************************************/
static int  atbm_sdio_init(atbm_void)
{
	/* 初始化数据读写接口 */
	atbm_sdio_sbus_ops.sbus_memcpy_fromio	= atbm_sdio_memcpy_fromio;
	atbm_sdio_sbus_ops.sbus_memcpy_toio	= atbm_sdio_memcpy_toio;
	atbm_sdio_sbus_ops.sbus_read_sync		= atbm_sdio_read_sync;
	atbm_sdio_sbus_ops.sbus_write_sync	= atbm_sdio_memcpy_toio;
	
	/* 初始化主机锁定接口 */
	atbm_sdio_sbus_ops.lock				= atbm_sdio_lock;
	atbm_sdio_sbus_ops.unlock				= atbm_sdio_unlock;
	
	/* 初始化其他接口 */
	atbm_sdio_sbus_ops.reset				= atbm_sdio_reset;
	atbm_sdio_sbus_ops.align_size			= atbm_sdio_align_size;
	atbm_sdio_sbus_ops.power_mgmt			= atbm_sdio_pm;
	atbm_sdio_sbus_ops.set_block_size		= atbm_sdio_set_block_size;
	
	/* 初始化中断管理接口 */
	atbm_sdio_sbus_ops.irq_unsubscribe	= atbm_sdio_irq_unsubscribe;
	atbm_sdio_sbus_ops.irq_subscribe	= atbm_sdio_irq_subscribe;
	atbm_sdio_sbus_ops.abort	= atbm_sdio_abort;
	atbm_sdio_sbus_ops.sdio_irq_en = atbm_sdio_irq_en;

	wifi_printk(WIFI_IF, "atbm_sdio_init\n");
	/* 打印驱动版本信息 */
	driver_build_info();

	/* 注册 SDIO 驱动到系统 */
	return atbm_sdio_register_init();
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_exit
 *
 * 功能描述:
 *   SDIO 驱动退出函数。从系统注销 SDIO 驱动。
 *
 * 使用场景:
 *   - 由 atbm_sdio_module_exit() 调用
 *   - 在驱动卸载时执行
 *
 **************************************************************************************************************/
static atbm_void  atbm_sdio_exit(atbm_void)
{
	/* 从系统注销 SDIO 驱动 */
	atbm_sdio_register_deinit();
	wifi_printk(WIFI_IF,"atbm_usb_exit:atbm_sdio_exit\n");
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_module_init
 *
 * 功能描述:
 *   SDIO 模块初始化入口函数。这是 SDIO 驱动的主入口点，
 *   完成固件初始化和 SDIO 驱动初始化。
 *
 * 使用场景:
 *   - 在系统启动时由应用层调用
 *   - 作为 WiFi 驱动的入口函数
 *
 * 实现机制:
 *   1. 初始化固件数据（将固件加载到内存）
 *   2. 调用 atbm_sdio_init() 初始化 SDIO 驱动
 *
 * 调用流程:
 *   atbm_sdio_module_init()
 *       ├── atbm_init_firmware()      // 初始化固件数据
 *       └── atbm_sdio_init()          // SDIO 驱动初始化
 *           ├── 初始化 sbus_ops       // 设置操作函数指针
 *           └── atbm_sdio_register_init() // 注册驱动
 *               └── atbm_sdio_probe() // 设备探测（自动调用）
 *                   └── Atbmwifi_halEntry() // HAL 层入口
 *
 **************************************************************************************************************/
atbm_void atbm_sdio_module_init(atbm_void)
{
	wifi_printk(WIFI_ALWAYS, "atbm_sdio_module_init\n");
	/* 步骤 1: 初始化固件数据 */
	atbm_init_firmware();
	wifi_printk(WIFI_ALWAYS,"[Wifi] Enter %s \n", __func__);
	/* 步骤 2: SDIO 驱动初始化 */
	atbm_sdio_init();
}

/**************************************************************************************************************
 * 函数名: atbm_sdio_module_exit
 *
 * 功能描述:
 *   SDIO 模块退出函数。完成 SDIO 驱动的清理工作，
 *   包括注销驱动和释放固件资源。
 *
 * 使用场景:
 *   - 在系统关闭时调用
 *   - 在需要卸载 WiFi 驱动时调用
 *
 * 实现机制:
 *   1. 调用 atbm_sdio_exit() 注销 SDIO 驱动
 *   2. 释放固件资源
 *
 **************************************************************************************************************/
atbm_void atbm_sdio_module_exit(atbm_void)
{
	wifi_printk(WIFI_IF,"atbm_sdio_module_exit\n");
	/* 步骤 1: 注销 SDIO 驱动 */
	atbm_sdio_exit();
	/* 步骤 2: 释放固件资源 */
	atbm_release_firmware();
	return ;
}

