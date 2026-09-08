/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/

#include "atbm_hal.h"

#ifndef LINUX_OS
#define num_present_cpus() 2
#endif

 static  atbm_void __atbmwifi_queue_lock(struct atbmwifi_queue *queue,struct atbmwifi_vif *priv, int bit)
{
	atbm_uint8 if_id;
	ATBM_BOOL stop = ATBM_TRUE;

	if (!priv){
		return ;
	}

	//wifi_printk(WIFI_ALWAYS, "lock:%d %llx\n", bit, queue->tx_locked_cnt);
	if(queue->tx_locked_cnt == 0){
		atbm_for_each_vif(priv->hw_priv, priv, if_id){
			if(!priv){
				continue;
			}
			tcp_opt->net_stop_queue(priv->ndev,queue->queue_id);
		}
	}

	atbm_set_bit(bit, (atbm_uint32*)&queue->tx_locked_cnt);
}

static  atbm_void __atbmwifi_queue_unlock(struct atbmwifi_queue *queue,struct atbmwifi_vif *priv, int bit)
{
	atbm_uint8 if_id;

	//wifi_printk(WIFI_ALWAYS, "unlock:%d %llx\n", bit, queue->tx_locked_cnt);
	if ((!priv) || (queue->tx_locked_cnt==0)){
		return ;
	}	
	atbm_clear_bit(bit, (atbm_uint32*)&queue->tx_locked_cnt);
	//ATBM_BUG_ON(!queue->tx_locked_cnt);
	if (queue->tx_locked_cnt == 0) {
		wifi_printk(WIFI_QUEUE, "[TX] Queue[%d] unlock\n",queue->queue_id);
		atbm_for_each_vif(priv->hw_priv, priv, if_id){
			if(!priv){
				continue;
			}
			tcp_opt->net_start_queue(priv->ndev,queue->queue_id);
		}
	}
}

 static  atbm_void atbmwifi_queue_parse_id(atbm_uint32 packetID, atbm_uint8 *queue_generation,
						atbm_uint8 *queue_id,
						atbm_uint8 *item_generation,
						atbm_uint8 *item_id,
						atbm_uint8 *if_id,
						atbm_uint8 *link_id)
{
	*item_id		= (packetID >>  0) & 0xFF;
	*item_generation	= (packetID >>  8) & 0xFF;
	*queue_id		= (packetID >> 16) & 0xF;
	*if_id			= (packetID >> 20) & 0x7;
	*link_id		= (packetID >> 23) & 0x1F;
	*queue_generation	= (packetID >> 28) & 0xF;
}

 static  atbm_uint32 atbmwifi_queue_make_packet_id(atbm_uint8 queue_generation, atbm_uint8 queue_id,
						atbm_uint8 item_generation, atbm_uint8 item_id,
						atbm_uint8 if_id, atbm_uint8 link_id)
{
	/*TODO:COMBO: Add interfaceID to the packetID */
	return ((atbm_uint32)item_id << 0) |
		((atbm_uint32)item_generation << 8) |
		((atbm_uint32)queue_id << 16) |
		((atbm_uint32)if_id << 20) |
		((atbm_uint32)link_id << 23) |
		((atbm_uint32)queue_generation << 28);
}
 int atbmwifi_queue_stats_init(struct atbmwifi_queue_stats *stats,
			    atbm_uint32 map_capacity,
			    struct atbmwifi_common *hw_priv)
{
	int i;

	atbm_memset(stats, 0, sizeof(*stats));
	stats->map_capacity = map_capacity;
	stats->hw_priv = hw_priv;
	atbm_spin_lock_init(&stats->lock);
	//init_waitqueue_head(&stats->wait_link_id_empty);
	for (i = 0; i < ATBM_WIFI_MAX_VIFS; i++) { 
		stats->link_map_cache[i] = (int *)atbm_kzalloc(sizeof(int[WLAN_LINK_ID_MAX]),GFP_KERNEL);
		if (!stats->link_map_cache[i]) {
			for (; i >= 0; i--)
				atbm_kfree(stats->link_map_cache[i]);
			return -ATBM_ENOMEM;
		}
	}
	return 0;
}
atbm_void atbmwifi_queued_timeout(atbm_void *data1,atbm_void *data2)
{
	struct atbmwifi_queue *queue =(struct atbmwifi_queue *)data1;
	unsigned long flags;
	struct atbmwifi_queue_stats *stats = queue->stats;
	struct atbmwifi_queue_item *item = ATBM_NULL;
	struct atbmwifi_vif *priv=ATBM_NULL;
	struct atbm_buff *skb=ATBM_NULL;
	int if_id;	
	queue->queuedFlag=ATBM_FALSE;
	atbm_spin_lock_irqsave(&queue->lock, &flags);
	while (!atbm_list_empty(&queue->queue)) {
		struct atbmwifi_txpriv *txpriv;
		item = atbm_list_first_entry(
			&queue->queue, struct atbmwifi_queue_item, head);
		if (atbm_TimeAfter(item->queue_timestamp, queue->ttl)){
			wifi_printk(WIFI_QUEUE,"queued_to not remove..%d..%d\n",atbm_GetOsTimeMs(), (int)item->queue_timestamp);
			break;
		}
		txpriv = &item->txpriv;
		skb=item->skb;
		item->skb=ATBM_NULL;
		if_id = txpriv->if_id;
		--queue->num_queued;
		--queue->num_queued_vif[if_id];
		--queue->link_map_cache[if_id][txpriv->link_id];
		atbm_spin_lock(&stats->lock);
		--stats->num_queued[if_id];
		--stats->link_map_cache[if_id][txpriv->link_id];
		atbm_spin_unlock(&stats->lock);
		priv = _atbmwifi_hwpriv_to_vifpriv(stats->hw_priv, if_id);
		ATBM_BUG_ON(!priv);
		atbm_list_move_tail(&item->head, &queue->free_pool);
		atbmwifi_skb_dtor(priv->hw_priv, skb,txpriv);
	}
	if (queue->overfull) {
		//wifi_printk(WIFI_ALWAYS,"atbmwifi_removed_queued_timeout\n");
		if (queue->num_queued <= (stats->hw_priv->vif0_throttle/2)) {
			queue->overfull = ATBM_FALSE;
			atbmwifi_queue_unlock_direct(queue,priv,0,ATBM_LOCK_REASON_QUEUE_OVERFULL);
		} 
	}else{
		queue->queuedFlag=ATBM_FALSE;
	}
	
	if (!atbm_list_empty(&queue->queue)) {
		atbmwifi_eloop_register_timeout(0,5*ATBM_HZ,atbmwifi_queued_timeout,(atbm_void *)queue,ATBM_NULL);
		queue->queuedFlag=ATBM_TRUE;
	}
	atbm_spin_unlock_irqrestore(&queue->lock,flags);
}
 int atbmwifi_queue_init(struct atbmwifi_queue *queue,
		      struct atbmwifi_queue_stats *stats,
		      atbm_uint8 queue_id,
		      atbm_size_t capacity)
{
	int i;

	atbm_memset(queue, 0, sizeof(*queue));
	queue->stats = stats;
	queue->capacity = capacity;
	queue->queue_id = queue_id;
	queue->queuedFlag=ATBM_FALSE;
	queue->ttl = 1*ATBM_HZ;
	ATBM_INIT_LIST_HEAD(&queue->queue);
	ATBM_INIT_LIST_HEAD(&queue->pending);
	ATBM_INIT_LIST_HEAD(&queue->free_pool);
	atbm_spin_lock_init(&queue->lock);
	//atbm_InitTimer(&queue->timeout,atbmwifi_queued_timeout,(atbm_void*)queue);
	queue->pool = (struct atbmwifi_queue_item *)atbm_kzalloc(sizeof(struct atbmwifi_queue_item) * capacity /*pool*/
						+ (sizeof(int[WLAN_LINK_ID_MAX])* ATBM_WIFI_MAX_VIFS)/*link_map_cache*/,GFP_KERNEL);
	
	if (!queue->pool)
		return -ATBM_ENOMEM;

	for (i = 0; i < ATBM_WIFI_MAX_VIFS; i++) {
		queue->link_map_cache[i] =(atbm_void *) (((atbm_uint8 *)queue->pool)
									+sizeof(struct atbmwifi_queue_item) * capacity
									+(sizeof(int[WLAN_LINK_ID_MAX])*i));
	}
	for (i = 0; i < capacity; ++i)
		atbm_list_add_tail(&queue->pool[i].head, &queue->free_pool);

	
	wifi_printk(WIFI_CONNECT,"atbmwifi_queue_init,cap(%d)\n",queue->capacity);
	return 0;
}

/* TODO:COMBO: Flush only a particular interface specific parts */
 int atbmwifi_queue_clear(struct atbmwifi_queue *queue, int if_id)
{
	int i, cnt, iter;
	struct atbmwifi_queue_stats *stats = queue->stats;
	unsigned long flags;
	
	//atbm_LIST_HEAD(gc_list);

	cnt = 0;
	atbm_spin_lock_irqsave(&queue->lock, &flags);
	queue->generation++;
	queue->generation &= 0xf;
	atbm_list_splice_tail_init(&queue->queue, &queue->pending);
	while (!atbm_list_empty(&queue->pending)) {
		struct atbmwifi_queue_item *item = atbm_list_first_entry(
			&queue->pending, struct atbmwifi_queue_item, head);
		ATBM_WARN_ON_FUNC(!item->skb);
		if (ATBM_WIFI_ALL_IFS == if_id || item->txpriv.if_id == if_id) {
			//atbmwifi_queue_register_post_gc(&gc_list, item);
			atbmwifi_skb_dtor(stats->hw_priv, item->skb, &item->txpriv);
			item->skb = ATBM_NULL;
			atbm_list_move_tail(&item->head, &queue->free_pool);
			cnt++;
		}
	}
	queue->num_queued -= cnt;
	queue->num_pending -= cnt;
	if (ATBM_WIFI_ALL_IFS != if_id) {
		queue->num_queued_vif[if_id] = 0;
		queue->num_pending_vif[if_id] = 0;
	} else {
		for (iter = 0; iter < ATBM_WIFI_MAX_VIFS; iter++) {
			queue->num_queued_vif[iter] = 0;
			queue->num_pending_vif[iter] = 0;
		}
	}
	atbm_spin_lock(&stats->lock);
	if (ATBM_WIFI_ALL_IFS != if_id) {
		for (i = 0; i < stats->map_capacity; ++i) {
			stats->num_queued[if_id] -=
				queue->link_map_cache[if_id][i];
			stats->link_map_cache[if_id][i] -=
				queue->link_map_cache[if_id][i];
			queue->link_map_cache[if_id][i] = 0;
		}
	} else {
		for (iter = 0; iter < ATBM_WIFI_MAX_VIFS; iter++) {
			for (i = 0; i < stats->map_capacity; ++i) {
				stats->num_queued[iter] -=
					queue->link_map_cache[iter][i];
				stats->link_map_cache[iter][i] -=
					queue->link_map_cache[iter][i];
				queue->link_map_cache[iter][i] = 0;
			}
		}
	}
	atbm_spin_unlock(&stats->lock);
	if (atbm_unlikely(queue->overfull)) {
		queue->overfull = ATBM_FALSE;
		atbmwifi_queue_unlock_direct(queue,_atbmwifi_hwpriv_to_vifpriv(stats->hw_priv,if_id),0,ATBM_LOCK_REASON_QUEUE_OVERFULL);
	}
	atbm_spin_unlock_irqrestore(&queue->lock,flags);
	//wake_up(stats->wait_link_id_empty);
	//atbmwifi_queue_post_gc(stats, &gc_list);

	return 0;
}

 /* TODO:COMBO: Flush only a particular interface specific parts */
  int atbmwifi_queue_clear_map(struct atbmwifi_queue *queue, int if_id, atbm_uint32 link_id_map)
 {
	 struct atbmwifi_queue_stats *stats = queue->stats;
	 unsigned long flags;
	 struct atbmwifi_queue_item *item, *tmp;
	 ATBM_BOOL unlock_queue = ATBM_FALSE;

	 atbm_spin_lock_irqsave(&queue->lock, &flags);

	 atbm_list_for_each_entry_safe(item, tmp, &queue->queue, head, struct atbmwifi_queue_item ){
		 ATBM_WARN_ON_FUNC(!item->skb);
		 if (item->txpriv.if_id == if_id && (link_id_map & ATBM_BIT(item->txpriv.link_id))){
			 //atbmwifi_queue_register_post_gc(&gc_list, item);
			 atbmwifi_skb_dtor(stats->hw_priv, item->skb, &item->txpriv);
			 item->skb = ATBM_NULL;
			 atbm_list_move_tail(&item->head, &queue->free_pool);
			 queue->num_queued--;
			 queue->num_queued_vif[if_id]--;
			 atbm_spin_lock(&stats->lock);
			 stats->num_queued[if_id]--;
			 stats->link_map_cache[if_id][item->txpriv.link_id]--;
			 atbm_spin_unlock(&stats->lock);
			 queue->link_map_cache[if_id][item->txpriv.link_id]--;
			 unlock_queue = ATBM_TRUE;
		 }
	 }

	 if (atbm_unlikely(queue->overfull)) {
	 	 if(unlock_queue && (queue->num_queued <= (stats->hw_priv->vif0_throttle/2))){
			 queue->overfull = ATBM_FALSE;
			 atbmwifi_queue_unlock_direct(queue,_atbmwifi_hwpriv_to_vifpriv(stats->hw_priv,if_id),0,ATBM_LOCK_REASON_QUEUE_OVERFULL);
	 	 }
	 }
	
	 atbm_spin_unlock_irqrestore(&queue->lock,flags);
 
	 return 0;
 }

/**
 * @brief 清除发送队列的 Link ID 映射
 *
 * @param hw_priv 硬件私有数据指针
 * @param if_id 虚拟接口 ID
 * @param link_id_map Link ID 位图，指定要清除的链路
 * @return int 0:成功
 *
 * @note 该函数清除所有 4 个 WMM AC 发送队列中指定 Link ID 的映射关系：
 *       1. 遍历所有 4 个发送队列
 *       2. 调用 atbmwifi_queue_clear_map() 清除指定 Link ID 的映射
 *
 * 适用场景:
 * - STA 断开连接时清除队列映射
 * - AP 关闭时清除所有 STA 的队列映射
 * - 模式切换时清除队列映射
 *
 * @see atbmwifi_queue_clear_map() - 单个队列清除映射
 * @see atbm_sta_deauth() - 调用该函数清除 STA 队列映射
 * @see atbmwifi_stop_ap() - 调用该函数清除 AP 队列映射
 */
 int atbmwifi_clear_queues_map(struct atbmwifi_common *hw_priv, int if_id, atbm_uint32 link_id_map){
 	int i;
	for(i = 0; i < 4; i++){
		atbmwifi_queue_clear_map(&hw_priv->tx_queue[i], if_id, link_id_map);
	}
	return 0;
 }

 atbm_void atbmwifi_queue_deinit(struct atbmwifi_queue *queue)
{
	int i;
	
	atbmwifi_queue_clear(queue, ATBM_WIFI_ALL_IFS);
	ATBM_INIT_LIST_HEAD(&queue->free_pool);
	atbm_kfree(queue->pool);
	for (i = 0; i < ATBM_WIFI_MAX_VIFS; i++) {
		queue->link_map_cache[i] = ATBM_NULL;
	}
	queue->pool = ATBM_NULL;
	queue->capacity = 0;
}

 atbm_size_t atbmwifi_queue_get_num_queued(struct atbmwifi_vif *priv,
				   struct atbmwifi_queue *queue,
				   atbm_uint32 link_id_map)
{
	atbm_size_t ret;
	int i, bit;
	atbm_size_t map_capacity = queue->stats->map_capacity;
	unsigned long flags;

	if (!link_id_map)
		return 0;

	atbm_spin_lock_irqsave(&queue->lock, &flags);
	if (atbm_likely(link_id_map == (atbm_uint32) -1)) {
		ret = queue->num_queued_vif[priv->if_id] -
			queue->num_pending_vif[priv->if_id];

	} else {
		ret = 0;
		for (i = 0, bit = 1; i < map_capacity; ++i, bit <<= 1) {
			if (link_id_map & bit)
				ret +=
				queue->link_map_cache[priv->if_id][i];
		}
	}
	atbm_spin_unlock_irqrestore(&queue->lock,flags);
	return ret;
}

/**************************************************************************************************************
 * 函数名: atbmwifi_queue_put
 *
 * 功能描述:
 *   将数据包放入发送队列。这是 TX 流程中的队列管理函数，负责将处理好的数据包加入对应的
 *   WMM AC 队列，并管理队列状态、流控、统计信息等。队列采用空闲池（free_pool）和队列
 *   （queue）的双链表结构，提高内存使用效率。
 *
 * 在 TX 流程中的位置:
 *   阶段 3: HAL 层处理 → 队列管理
 *   在 atbmwifi_tx() 和 atbmwifi_tx_queue() 中调用，将数据包加入队列
 *
 * 参数说明:
 *   @queue: 队列指针，指向要加入的 WMM AC 队列（0-3 对应 AC_BE, AC_BK, AC_VI, AC_VO）
 *   @skb: Socket Buffer 指针，包含要发送的数据包
 *   @txpriv: 发送私有数据指针，包含接口 ID、链路 ID、TID、速率 ID 等信息
 *
 * 返回值:
 *   0: 成功
 *   -ATBM_EINVAL: 链路 ID 无效
 *   -ATBM_ENOENT: 队列空闲池为空（队列已满）
 *
 * 处理流程:
 *   1. 参数验证：检查链路 ID 是否有效
 *   2. 锁定队列：使用自旋锁保护队列操作
 *   3. 从空闲池获取队列项：如果空闲池不为空，获取一个队列项
 *   4. 设置队列项信息：设置数据包、发送私有数据、生成 packetID
 *   5. 更新统计信息：更新队列计数、接口计数、链路计数
 *   6. 流控检查：如果队列接近满，锁定队列并通知协议栈暂停发送
 *   7. 注册超时：注册队列超时处理（如果未注册）
 *   8. 解锁队列：释放自旋锁
 *
 * 队列结构:
 *   - free_pool: 空闲队列项池，存放可用的队列项
 *   - queue: 实际队列，存放待发送的数据包
 *   - pool: 队列项数组，预分配的队列项内存
 *
 * 流控机制:
 *   - 当队列项数量达到阈值（vif0_throttle - num_present_cpus()）时，标记队列为过满
 *   - 调用 net_stop_queue() 通知协议栈暂停发送
 *   - 当队列有空间时，调用 net_start_queue() 通知协议栈恢复发送
 *
 * 注意事项:
 *   - 函数使用自旋锁保护，可以在中断上下文中调用
 *   - 队列项从空闲池获取，避免频繁的内存分配
 *   - packetID 用于标识数据包，用于发送完成后的匹配
 *   - 队列超时处理用于检测和处理队列阻塞情况
 *
 * 相关函数:
 *   - atbmwifi_queue_get(): 从队列取出数据包
 *   - atbmwifi_queue_lock_direct(): 锁定队列
 *   - atbmwifi_queued_timeout(): 队列超时处理
 *   - atbmwifi_queue_make_packet_id(): 生成 packetID
 *
 **************************************************************************************************************/
 int atbmwifi_queue_put(struct atbmwifi_queue *queue,
		     struct atbm_buff *skb,
		     struct atbmwifi_txpriv *txpriv)
{
	int ret = 0;
	//atbm_LIST_HEAD(gc_list);
	struct atbmwifi_queue_stats *stats = queue->stats;
	unsigned long flags;
	/* TODO:COMBO: Add interface ID info to queue item */

	/* 步骤 1: 参数验证 */
	/* 检查链路 ID 是否在有效范围内 */
	if (txpriv->link_id >= queue->stats->map_capacity)
		return -ATBM_EINVAL;

	/* 步骤 2: 锁定队列 */
	/* 使用自旋锁保护队列操作，确保并发安全 */
	atbm_spin_lock_irqsave(&queue->lock, &flags);
	
	/* 步骤 3: 从空闲池获取队列项 */
	/* 如果空闲池不为空，获取一个队列项 */
	if (!(atbm_list_empty(&queue->free_pool))) {
		/* 从空闲池头部获取一个队列项 */
		struct atbmwifi_queue_item *item = atbm_list_first_entry(
			&queue->free_pool, struct atbmwifi_queue_item, head);
		ATBM_BUG_ON(item->skb);
		
		/* 步骤 4: 将队列项从空闲池移动到队列尾部 */
		atbm_list_move_tail(&item->head, &queue->queue);
		
		/* 步骤 5: 设置队列项信息 */
		/* 设置数据包指针 */
		item->skb = skb;
		/* 复制发送私有数据 */
		item->txpriv = *txpriv;
		/* 初始化生成号 */
		item->generation = 0;
		/* 生成 packetID，用于发送完成后的匹配 */
		item->packetID = atbmwifi_queue_make_packet_id(
			queue->generation, queue->queue_id,
			item->generation, item - queue->pool,
			txpriv->if_id, txpriv->raw_link_id);
		/* 记录入队时间戳，用于统计和调试 */
		item->queue_timestamp = atbm_GetOsTimeMs();

		/* 步骤 6: 更新队列统计信息 */
		/* 增加队列中的总数据包数 */
		++queue->num_queued;
		/* 增加对应接口的数据包数 */
		++queue->num_queued_vif[txpriv->if_id];
		/* 增加对应链路的数据包数 */
		++queue->link_map_cache[txpriv->if_id][txpriv->link_id];

		/* 更新全局统计信息 */
		atbm_spin_lock(&stats->lock);
		++stats->num_queued[txpriv->if_id];
		++stats->link_map_cache[txpriv->if_id][txpriv->link_id];
		atbm_spin_unlock(&stats->lock);

		/* 步骤 7: 流控检查 */
		/*
		 * TX may happen in parallel sometimes.
		 * Leave extra queue slots so we don't overflow.
		 */
		/* 如果队列接近满（达到阈值），锁定队列并通知协议栈暂停发送 */
		if ((queue->overfull == ATBM_FALSE) &&
				(queue->num_queued >=(stats->hw_priv->vif0_throttle)-num_present_cpus())) {
			queue->overfull = ATBM_TRUE;
			/* 锁定队列，通知协议栈暂停发送 */
			atbmwifi_queue_lock_direct(queue,_atbmwifi_hwpriv_to_vifpriv(stats->hw_priv,txpriv->if_id), 0, ATBM_LOCK_REASON_QUEUE_OVERFULL);
			//wifi_printk(WIFI_CONNECT,"queue->overfull %d,num_queued %d------%d>>>\n",queue->overfull,queue->num_queued,queue->queuedFlag);
		}
		
		/* 步骤 8: 注册队列超时处理 */
		/* 如果未注册超时处理，注册一个 5 秒的超时，用于检测队列阻塞 */
		if(queue->queuedFlag==ATBM_FALSE){
			queue->queuedFlag = ATBM_TRUE;
			atbm_spin_unlock_irqrestore(&queue->lock,flags);
			atbmwifi_eloop_register_timeout(0,5*ATBM_HZ,atbmwifi_queued_timeout,(atbm_void *)queue,ATBM_NULL);
			atbm_spin_lock_irqsave(&queue->lock, &flags);
		}
	} else {
		/* 步骤 9: 错误处理 */
		/* 空闲池为空，队列已满，无法加入新数据包 */
		extern struct atbmwifi_common g_hw_prv;
		struct atbmwifi_vif *priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv, 1);
		wifi_printk(WIFI_ALWAYS, "overfull:%d enable:%d\n", queue->overfull, priv->ndev->lwip_queue_enable);
		ret = -ATBM_ENOENT;
	}
	
	/* 步骤 10: 解锁队列 */
	atbm_spin_unlock_irqrestore(&queue->lock,flags);
	return ret;
}
/*
get frame from the queue->queue, then move it to queue->pending
*/
 int atbmwifi_queue_get(struct atbmwifi_queue *queue,
			int if_id,
		     atbm_uint32 link_id_map,
		     struct atbm_buff **skb,
		     atbm_uint32 *packetID,
		     const struct atbmwifi_txpriv **txpriv)
{
	int ret = -ATBM_ENOENT;
	struct atbmwifi_queue_item *item;
	struct atbmwifi_queue_stats *stats = queue->stats;
	ATBM_BOOL wakeup_stats = ATBM_FALSE;
	unsigned long flags;

	atbm_spin_lock_irqsave(&queue->lock, &flags);
	for(item = atbm_list_entry((&queue->queue)->next, struct atbmwifi_queue_item, head);
			&(item)->head != (&queue->queue); 	
	     item = atbm_list_entry((item)->head.next, struct atbmwifi_queue_item, head)){
		if ((item->txpriv.if_id == if_id) &&
			(link_id_map & ATBM_BIT(item->txpriv.link_id))) {
			ret = 0;
			break;
		}else{
			ret = -1;
		}
	}
	if (!/*ATBM_WARN_ON*/(ret)) {
		*skb = item->skb;
		*txpriv = &item->txpriv;
		*packetID = atbm_cpu_to_le32(item->packetID);
		atbm_list_move_tail(&item->head, &queue->pending);
		++queue->num_pending;
		++queue->num_pending_vif[item->txpriv.if_id];
		--queue->link_map_cache[item->txpriv.if_id]
				[item->txpriv.link_id];
		//item->xmit_timestamp = atbm_GetOsTimeMs;

		atbm_spin_lock(&stats->lock);
		--stats->num_queued[item->txpriv.if_id];
		if (!--stats->link_map_cache[item->txpriv.if_id]
					[item->txpriv.link_id])
			wakeup_stats = ATBM_TRUE;

		atbm_spin_unlock(&stats->lock);
	}
	atbm_spin_unlock_irqrestore(&queue->lock,flags);
	//if (wakeup_stats)
	//	wake_up(stats->wait_link_id_empty);
	return ret;
}


 int atbmwifi_queue_requeue(struct atbmwifi_queue *queue, atbm_uint32 packetID)
{
	int ret = 0;
	atbm_uint8 queue_generation, queue_id, item_generation, item_id, if_id, link_id;
	struct atbmwifi_queue_item *item;
	struct atbmwifi_queue_stats *stats = queue->stats;
	unsigned long flags;
	atbm_spin_lock_irqsave(&queue->lock, &flags);

	atbmwifi_queue_parse_id(packetID, &queue_generation, &queue_id,
				&item_generation, &item_id, &if_id, &link_id);

	item = &queue->pool[item_id];

	/*if_id = item->txpriv.if_id;*/

	ATBM_BUG_ON(queue_id != queue->queue_id);
	if (atbm_unlikely(queue_generation != queue->generation)) {
		ret = -ATBM_ENOENT;
	} else if (atbm_unlikely(item_id >= (unsigned) queue->capacity)) {
		ATBM_WARN_ON_FUNC(1);
		ret = -ATBM_EINVAL;
	} else if (atbm_unlikely(item->generation != item_generation)) {
		ATBM_WARN_ON_FUNC(1);
		ret = -ATBM_ENOENT;
	} else {
		--queue->num_pending;
		--queue->num_pending_vif[if_id];
		++queue->link_map_cache[if_id][item->txpriv.link_id];

		atbm_spin_lock(&stats->lock);
		++stats->num_queued[item->txpriv.if_id];
		++stats->link_map_cache[if_id][item->txpriv.link_id];
		atbm_spin_unlock(&stats->lock);

		item->generation = ++item_generation;
		item->packetID = atbmwifi_queue_make_packet_id(
			queue_generation, queue_id, item_generation, item_id,
			if_id, link_id);
		atbm_list_move(&item->head, &queue->queue);
	}
	atbm_spin_unlock_irqrestore(&queue->lock,flags);
	return ret;
}

 int atbmwifi_queue_requeue_all(struct atbmwifi_queue *queue)
{
	struct atbmwifi_queue_stats *stats = queue->stats;
	unsigned long flags;

	atbm_spin_lock_irqsave(&queue->lock, &flags);
	while (!atbm_list_empty(&queue->pending)) {
		struct atbmwifi_queue_item *item = atbm_list_entry(
			queue->pending.prev, struct atbmwifi_queue_item, head);

		--queue->num_pending;
		--queue->num_pending_vif[item->txpriv.if_id];
		++queue->link_map_cache[item->txpriv.if_id]
				[item->txpriv.link_id];

		atbm_spin_lock(&stats->lock);
		++stats->num_queued[item->txpriv.if_id];
		++stats->link_map_cache[item->txpriv.if_id]
				[item->txpriv.link_id];
		atbm_spin_unlock(&stats->lock);

		++item->generation;
		item->packetID = atbmwifi_queue_make_packet_id(
			queue->generation, queue->queue_id,
			item->generation, item - queue->pool,
			item->txpriv.if_id, item->txpriv.raw_link_id);
		atbm_list_move(&item->head, &queue->queue);
	}
	atbm_spin_unlock_irqrestore(&queue->lock,flags);

	return 0;
}

 int atbmwifi_queue_remove(struct atbmwifi_queue *queue, atbm_uint32 packetID)
{
	int ret = 0;
	atbm_uint8 queue_generation, queue_id, item_generation, item_id, if_id, link_id;
	struct atbmwifi_queue_item *item;
	struct atbmwifi_queue_stats *stats = queue->stats;
	struct atbm_buff *gc_skb = ATBM_NULL;
	struct atbmwifi_txpriv gc_txpriv;
	unsigned long flags;
	atbm_spin_lock_irqsave(&queue->lock, &flags);

	atbmwifi_queue_parse_id(packetID, &queue_generation, &queue_id,
				&item_generation, &item_id, &if_id, &link_id);
	

	item = &queue->pool[item_id];
	if(item==ATBM_NULL){
		atbm_spin_unlock_irqrestore(&queue->lock,flags);
		return 0;
	}
	ATBM_BUG_ON(queue_id != queue->queue_id);
	/*TODO:COMBO:Add check for interface ID also */
	if (atbm_unlikely(queue_generation != queue->generation)) {
		ATBM_WARN_ON_FUNC(1);
		ret = -ATBM_ENOENT;
	} else if (atbm_unlikely(item_id >= (unsigned) queue->capacity)) {
		ATBM_WARN_ON_FUNC(1);
		ret = -ATBM_EINVAL;
	} else if (atbm_unlikely(item->generation != item_generation)) {
		ATBM_WARN_ON_FUNC(1);
		ret = -ATBM_ENOENT;
	} else {
		gc_txpriv = item->txpriv;
		gc_skb = item->skb;
		item->skb = ATBM_NULL;
		--queue->num_pending;
		--queue->num_pending_vif[if_id];
		--queue->num_queued;
		--queue->num_queued_vif[if_id];
		//++queue->num_sent;
		++item->generation;
		/* Do not use atbm_list_move_tail here, but atbm_list_move:
		 * try to utilize cache row.
		 */
		atbm_list_move(&item->head, &queue->free_pool);
		//wifi_printk(WIFI_ALWAYS,"[TX] queue_rmove %d %d\n",queue->num_queued,queue->overfull);

		if (atbm_unlikely(queue->overfull) &&
		    (queue->num_queued <= (stats->hw_priv->vif0_throttle / 2))) {
			queue->overfull = ATBM_FALSE;
			atbmwifi_queue_unlock_direct(queue,_atbmwifi_hwpriv_to_vifpriv(stats->hw_priv,if_id),0,ATBM_LOCK_REASON_QUEUE_OVERFULL);
		}
	}

	atbm_spin_unlock_irqrestore(&queue->lock,flags);

	if (gc_skb){
	//	wifi_printk(WIFI_ALWAYS,"atbmwifi_queue_remove \n");
		atbmwifi_skb_dtor(stats->hw_priv, gc_skb, &gc_txpriv);
	}

	return ret;
}

 int atbmwifi_queue_get_skb(struct atbmwifi_queue *queue, atbm_uint32 packetID,
			 struct atbm_buff **skb,
			 const struct atbmwifi_txpriv **txpriv)
{
	int ret = 0;
	atbm_uint8 queue_generation, queue_id, item_generation, item_id, if_id, link_id;
	struct atbmwifi_queue_item *item;
	unsigned long flags;
	
	atbm_spin_lock_irqsave(&queue->lock, &flags);

	atbmwifi_queue_parse_id(packetID, &queue_generation, &queue_id,
				&item_generation, &item_id, &if_id, &link_id);

	item = &queue->pool[item_id];

	ATBM_BUG_ON(queue_id != queue->queue_id);
	/* TODO:COMBO: Add check for interface ID here */
	if (atbm_unlikely(queue_generation != queue->generation)) {
		ret = -ATBM_ENOENT;
	} else if (atbm_unlikely(item_id >= (unsigned) queue->capacity)) {
		ATBM_WARN_ON_FUNC(1);
		ret = -ATBM_EINVAL;
	} else if (atbm_unlikely(item->generation != item_generation)) {
		ATBM_WARN_ON_FUNC(1);
		ret = -ATBM_ENOENT;
	} else {
		*skb = item->skb;
		*txpriv = &item->txpriv;
	}
	atbm_spin_unlock_irqrestore(&queue->lock,flags);
	return ret;
}

 atbm_void atbmwifi_queue_lock(struct atbmwifi_queue *queue,struct atbmwifi_vif *priv, int link_id, enum atbm_lock_reason reason)
{
	unsigned long flags;
	atbm_spin_lock_irqsave(&queue->lock, &flags);
	__atbmwifi_queue_lock(queue,priv,link_id*ATBM_LOCK_REASON_MAX+reason);
	atbm_spin_unlock_irqrestore(&queue->lock,flags);
}

 atbm_void atbmwifi_queue_unlock(struct atbmwifi_queue *queue,struct atbmwifi_vif *priv, int link_id, enum atbm_lock_reason reason)
{
	unsigned long flags;
	atbm_spin_lock_irqsave(&queue->lock, &flags);
	__atbmwifi_queue_unlock(queue,priv,link_id*ATBM_LOCK_REASON_MAX+reason);
	atbm_spin_unlock_irqrestore(&queue->lock,flags);
}

  atbm_void atbmwifi_queue_lock_direct(struct atbmwifi_queue *queue,struct atbmwifi_vif *priv, int link_id, enum atbm_lock_reason reason)
 {
	 __atbmwifi_queue_lock(queue,priv,link_id*ATBM_LOCK_REASON_MAX+reason);
 }
 
  atbm_void atbmwifi_queue_unlock_direct(struct atbmwifi_queue *queue,struct atbmwifi_vif *priv, int link_id, enum atbm_lock_reason reason)
 {
	 __atbmwifi_queue_unlock(queue,priv,link_id*ATBM_LOCK_REASON_MAX+reason);
 }

 ATBM_BOOL atbmwifi_queue_stats_is_empty(struct atbmwifi_queue_stats *stats,
				 atbm_uint32 link_id_map, int if_id)
{
	ATBM_BOOL empty = ATBM_TRUE;

	atbm_spin_lock(&stats->lock);
	if (link_id_map == (atbm_uint32)-1)
		empty = stats->num_queued[if_id] == 0;
	else {
		int i, if_id;
		for (if_id = 0; if_id < ATBM_WIFI_MAX_VIFS; if_id++) {
			for (i = 0; i < stats->map_capacity; ++i) {
				if (link_id_map & ATBM_BIT(i)) {
					if (stats->link_map_cache[if_id][i]) {
						empty = ATBM_FALSE;
						break;
					}
				}
			}
		}
	}
	atbm_spin_unlock(&stats->lock);

	return empty;
}


