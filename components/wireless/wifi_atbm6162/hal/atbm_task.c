/**************************************************************************************************************
 * altobeam RTOS
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/

#include "atbm_hal.h"
#include "atbm_task.h"

#if QUEUE_LIST
static atbm_void atbm_workqueue_thread(atbm_void *arg, int wakeup_state)
{
	atbm_uint32 ret;
	atbm_uint32 irq_flag;
	struct atbmwifi_common *hw_priv = (struct atbmwifi_common *)arg;
		
	irq_flag = atbm_local_irq_save();
	while (!atbm_list_empty(&hw_priv->work_Link))
	{
		struct atbmwifi_work_struct *work = NULL;
		struct atbmwifi_work_struct pending;
		work = atbm_list_first_entry(&hw_priv->work_Link,struct atbmwifi_work_struct,workLink);
		pending.valid = work->valid;
		pending.fun = work->fun;
		pending.data = work->data;
		atbm_list_del(&work->workLink);
		atbm_local_irq_restore(irq_flag);
		if(pending.valid){
			if(pending.fun){
				pending.fun(pending.data);
			}
		}else{
			break;
		}
		irq_flag = atbm_local_irq_save();
	}
}

/**
 * @brief 初始化工作队列
 *
 * @param hw_priv 硬件私有数据指针
 * @param fun 工作函数指针
 * @param data 工作函数参数
 * @return atbm_work 工作队列句柄
 *
 * @note 该函数分配并初始化一个工作队列结构体，用于后续的任务调度
 *
 * @see atbm_cancel_work() - 取消工作队列
 * @see atbm_queue_work() - 将工作加入队列
 */
atbm_work  atbm_init_work(struct atbmwifi_common *hw_priv,atbm_work_fun fun, atbm_void *data)
{
	struct atbmwifi_work_struct *work = NULL;
	work = (struct atbmwifi_work_struct *)atbm_kmalloc(sizeof(struct atbmwifi_work_struct),GFP_KERNEL);	
	if(work){
		work->fun = (atbm_void (*)(atbm_void *))fun;
		work->data = data;
		work->valid=1;
		work->workLink.next != ATBM_LIST_POISON1
	}
	return work;
}

/**
 * @brief 取消工作队列（链表版本）
 *
 * @param hw_priv 硬件私有数据指针
 * @param work 工作队列结构体指针
 *
 * @note 该函数取消并释放一个工作队列：
 *       1. 检查工作队列是否有效
 *       2. 标记为无效
 *       3. 释放工作队列内存
 *
 * 适用场景:
 * - 停止 STA/AP 模式时取消工作队列
 * - 扫描完成时取消扫描工作队列
 * - Join 完成时取消 Join 工作队列
 *
 * @see atbm_init_work() - 初始化工作队列
 * @see atbm_queue_work() - 将工作加入队列
 * @see atbmwifi_stop_sta() - 调用该函数取消工作队列
 * @see atbmwifi_stop_ap() - 调用该函数取消工作队列
 */
atbm_void  atbm_cancel_work(struct atbmwifi_common *hw_priv,struct atbmwifi_work_struct *work)
{
	atbm_uint32 irq_flag;

	irq_flag = atbm_local_irq_save();
	if(work->valid){
		work->valid = 0;
		if(work->workLink.next != ATBM_LIST_POISON1){
			atbm_list_del(&hw_priv->work_Link);
		}
		atbm_kfree(work);
	}
	atbm_local_irq_restore(irq_flag);
}

atbm_uint32 atbm_queue_work(struct atbmwifi_common *hw_priv, struct atbmwifi_work_struct *work)
{
	atbm_uint32 irq_flg;
	irq_flg = atbm_local_irq_save();
	atbm_list_add_tail(&work->workLink, &hw_priv->work_Link);
	atbm_local_irq_restore(irq_flg);
	atbm_wakeupThreadInternal(hw_priv->work_queue_thread);
	return 0;
}
int atbm_create_workqueue(struct atbmwifi_common *hw_priv)
{
	atbm_uint32 ret;

	ATBM_INIT_LIST_HEAD(&hw_priv->work_Link);
	hw_priv->work_queue_thread = atbm_createThreadInternal("atbm_work",
											atbm_workqueue_thread,
											(atbm_void*)hw_priv,
											WORK_TASK_PRIO,
											10*ATBM_HZ);
	if (!hw_priv->work_queue_thread){
		wifi_printk(WIFI_IF,"work_queue_thread Failed\n");
		ret = WIFI_ERROR;
	}else{
		ret = WIFI_OK;
	}

	return ret;
}

int atbm_destory_task_work(struct atbmwifi_common *hw_priv)
{
	int ret = 0;
	//wifi_printk(WIFI_ALWAYS,"atbm_destory_task_work\n");
	hw_priv->bh_term=1;
	atbm_stopThreadInternal(hw_priv->work_queue_thread);
	return ret;
}
#else

int atbm_task_work(atbm_void *arg, int wakeup_state);
int atbm_create_workqueue(struct atbmwifi_common *hw_priv)
{
	int ret;
	atbm_memset(&hw_priv->work_queue_table[0],0,sizeof(struct atbmwifi_work_struct)*ATBM_WIFI_MAX_WORKQUEUE);
	hw_priv->work_map =0;
	//sema_init(&hw_priv->work_wq,0);
	hw_priv->work_queue_thread=atbm_createThreadInternal("atbm_work",
												atbm_task_work,
												(atbm_void*)hw_priv,
												WORK_TASK_PRIO,
												10*ATBM_HZ);
	if (!hw_priv->work_queue_thread){
		wifi_printk(WIFI_IF,"work_queue_thread Failed\n");
		ret = WIFI_ERROR;
	}else{
		ret = WIFI_OK;
	}

	return ret;
}
int atbm_destory_task_work(struct atbmwifi_common *hw_priv)
{
	int ret = 0;
	//wifi_printk(WIFI_ALWAYS,"atbm_destory_task_work\n");
	hw_priv->bh_term=1;
	atbm_stopThreadInternal(hw_priv->work_queue_thread);
	return ret;
}

int atbm_task_work(atbm_void *arg, int wakeup_state)
{

	struct atbmwifi_common *hw_priv = (struct atbmwifi_common *)arg;
	int id=0;
	struct atbmwifi_work_struct *work;
	atbm_work_fun fun;

	for(id=0;id<ATBM_WIFI_MAX_WORKQUEUE;id++){		
		work = &hw_priv->work_queue_table[id];
		fun = work->fun;
		if(atbm_test_bit(id,&hw_priv->work_map)){		
			atbm_clear_bit(id,&hw_priv->work_map);
			wifi_printk(WIFI_TASK,"work->fun-id %d-\n",id);
			fun(work->data);				
		}
		if(hw_priv->work_map ==0)
			break;
	}

	//atbm_stopThreadInternal(hw_priv->work_queue_thread);
	return 0;
}

atbm_void atbm_queue_work(struct atbmwifi_common *hw_priv,atbm_work workid)
{
	struct atbmwifi_work_struct *work;

	
	wifi_printk(WIFI_IF,"atbm_queue_work1++ workid %d\n",workid); 
	work = &hw_priv->work_queue_table[workid];
	if(work->valid ==0){
		wifi_printk(WIFI_ALWAYS,"atbm_queue_work error !\n");
		return;
	}
	if(atbm_atomic_read(&work->pending)==0){
		wifi_printk(WIFI_TASK,"atbm_queue_work _wakeup_\n");
		//atbm_atomic_set(&work->pending,1);
		atbm_set_bit(workid,&hw_priv->work_map);
		atbm_wakeupThreadInternal(hw_priv->work_queue_thread);
	}
}

/**
 * @brief 初始化工作队列（ID 版本）
 *
 * @param hw_priv 硬件私有数据指针
 * @param fun 工作函数指针
 * @param data 工作函数参数
 * @return atbm_work 工作队列 ID
 *
 * @note 该函数从工作队列表中分配一个空闲的工作队列：
 *       1. 遍历工作队列表，查找空闲项
 *       2. 初始化工作队列参数
 *       3. 返回工作队列 ID
 *
 * @see atbm_cancel_work() - 取消工作队列
 * @see atbm_queue_work() - 将工作加入队列
 */
atbm_work atbm_init_work(struct atbmwifi_common *hw_priv,atbm_work_fun fun, atbm_void *data)
{
	int id = -1;
	struct atbmwifi_work_struct *work;
	atbm_uint32 irq_flg;

	/* 关中断保护 */
	irq_flg = atbm_local_irq_save();

	/* 遍历工作队列表，查找空闲项 */
	for(id=0;id<ATBM_WIFI_MAX_WORKQUEUE;id++){
		work = &hw_priv->work_queue_table[id];
		if(work->valid ==0){
			work->fun = fun;
			work->data = data;
			work->index= id;
			work->valid= 1;
			break;
		}
	}
	atbm_local_irq_restore(irq_flg);

	/* 断言检查是否找到空闲项 */
	ATBM_ASSERT(id != -1);

	return id;
}

/**
 * @brief 取消工作队列（ID 版本）
 *
 * @param hw_priv 硬件私有数据指针
 * @param work_id 工作队列 ID
 *
 * @note 该函数取消指定 ID 的工作队列：
 *       1. 清除工作队列映射位
 *       2. 标记工作队列为无效
 *       3. 清空工作队列参数
 *
 * 适用场景:
 * - 停止 STA/AP 模式时取消工作队列
 * - 扫描完成时取消扫描工作队列
 * - Join 完成时取消 Join 工作队列
 *
 * @see atbm_init_work() - 初始化工作队列
 * @see atbm_queue_work() - 将工作加入队列
 * @see atbmwifi_stop_sta() - 调用该函数取消工作队列
 * @see atbmwifi_stop_ap() - 调用该函数取消工作队列
 */
atbm_void  atbm_cancel_work(struct atbmwifi_common *hw_priv,atbm_work work_id)
{
	struct atbmwifi_work_struct *work;
	atbm_uint32 irq_flg;

	/* 关中断保护 */
	irq_flg = atbm_local_irq_save();

	work = &hw_priv->work_queue_table[work_id];

	/* 清除工作队列映射位 */
	atbm_clear_bit(work_id,&hw_priv->work_map);

	/* 清空工作队列参数 */
	if(work->valid == 1){
		work->valid = 0;
		work->fun = ATBM_NULL;
		work->data = ATBM_NULL;
		work->index= 0;
	}

	atbm_local_irq_restore(irq_flg);
	return;
}

#endif

int atbm_hmac_xmit_data(struct sbus_priv *self);

/**************************************************************************************************************
 * 函数名: atbm_bh_schedule_tx
 *
 * 功能描述:
 *   调度 TX 发送任务。该函数是 TX 流程中的关键调度函数，负责触发数据包的实际发送。
 *   根据配置，可以选择同步模式（直接发送）或异步模式（唤醒 BH 任务）。
 *
 * 在 TX 流程中的位置:
 *   阶段 3 → 阶段 4 的触发点
 *   在 atbmwifi_tx() 或 atbmwifi_tx_queue() 中调用，数据包加入队列后触发发送
 *
 * 参数说明:
 *   @hw_priv: 硬件私有数据指针，包含 WiFi 硬件的通用信息和状态
 *
 * 返回值:
 *   无
 *
 * 处理流程:
 *   根据 ATBM_TX_WRITE_TASK 配置：
 *   - 同步模式 (ATBM_TX_WRITE_TASK == 1):
 *     直接调用 atbm_hmac_xmit_data() 在当前上下文中发送数据
 *   - 异步模式 (ATBM_TX_WRITE_TASK == 0):
 *     1. 设置 bh_tx 原子标志为 1
 *     2. 唤醒 BH 任务线程
 *     3. BH 任务检测到 bh_tx 标志后调用 atbm_tx_task()
 *
 * 同步 vs 异步模式:
 *   - 同步模式: 延迟低，但可能阻塞当前任务
 *   - 异步模式: 不阻塞当前任务，但延迟稍高
 *
 * 调用时机:
 *   - 数据包加入发送队列后
 *   - 发送完成回调中（继续发送下一个数据包）
 *   - 流控解除后（队列有空间）
 *
 * 注意事项:
 *   - 同步模式下，函数会阻塞直到数据发送完成
 *   - 异步模式下，函数立即返回，发送在 BH 任务中进行
 *   - 使用原子操作设置标志，确保并发安全
 *
 * 相关函数:
 *   - atbm_hmac_xmit_data(): 实际的数据发送函数
 *   - atbm_tx_task(): TX 任务处理函数
 *   - atbm_hmac_bh(): BH 任务主函数
 *   - atbm_bh_wakeup(): 通用的 BH 唤醒函数
 *
 **************************************************************************************************************/
atbm_void atbm_bh_schedule_tx(struct atbmwifi_common	*hw_priv)
{
#if (ATBM_TX_WRITE_TASK == 1)
	atbm_hmac_xmit_data(hw_priv->sbus_priv);
#else
	atbm_atomic_set(&hw_priv->bh_tx, 1);
	atbm_wakeupThreadInternal(hw_priv->bh_thread);
#endif
}

/**************************************************************************************************************
 * 函数名: atbm_bh_wakeup
 *
 * 功能描述:
 *   通用的 BH 任务唤醒函数，用于唤醒 BH 任务进行 TX 处理。
 *   该函数在唤醒前会检查错误状态，避免在错误状态下继续发送。
 *
 * 在 TX 流程中的位置:
 *   阶段 3 → 阶段 4 的触发点
 *   作为 atbm_bh_schedule_tx() 的封装，增加了错误检查
 *
 * 参数说明:
 *   @hw_priv: 硬件私有数据指针，包含 WiFi 硬件的通用信息和状态
 *
 * 返回值:
 *   无
 *
 * 处理流程:
 *   1. 检查错误状态：如果 bh_error 或 bh_term 被设置，打印警告并返回
 *   2. 调用 atbm_bh_schedule_tx() 唤醒 TX 任务
 *
 * 注意事项:
 *   - 在错误状态下不会唤醒 BH 任务
 *   - 用于需要错误检查的场景
 *
 * 相关函数:
 *   - atbm_bh_schedule_tx(): 实际的调度函数
 *   - atbm_bh_is_term(): 检查 BH 任务是否终止
 *
 **************************************************************************************************************/
atbm_void atbm_bh_wakeup(struct atbmwifi_common *hw_priv)
{
	if ((hw_priv->bh_error)||atbm_bh_is_term(hw_priv)){
		wifi_printk(WIFI_IF,"atbm_wifi [BH] err drop\n");
		return;
	}
	/*wakeup TxThread to transmit.........*/
	
	atbm_bh_schedule_tx(hw_priv);
}

