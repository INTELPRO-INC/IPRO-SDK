/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/


#include "atbm_hal.h"

atbm_spinlock_t skb_ref_lock;
#if ATBM_SKB_DEBUG
atbm_spinlock_t skb_debug_lock;
static int atbm_skb_cnt = 0;
static int atbm_skb_free = 0;
static int atbm_skb_reuse_cnt = 0;
static int atbm_skb_reuse_free = 0;
static int atbm_skb_alloc_cnt = 0;
static int atbm_skb_free_cnt = 0;
static atbm_uint32 last_print_time = 0;
atbm_LIST_HEAD(skb_list);
atbm_LIST_HEAD(reuse_list);
static __INLINE void atbm_add_skb_to_debug_list(struct atbm_buff* skb){
	atbm_spin_lock(&skb_debug_lock);
	atbm_skb_cnt++;
	atbm_list_add_tail(&skb->skb_list, &skb_list);
	atbm_spin_unlock(&skb_debug_lock);
}
static __INLINE void atbm_add_skb_to_reuse_list(struct atbm_buff* skb){
	atbm_spin_lock(&skb_debug_lock);
	atbm_skb_reuse_cnt++;
	atbm_list_add_tail(&skb->reuse_list, &reuse_list);
	atbm_spin_unlock(&skb_debug_lock);
}
static __INLINE void atbm_del_skb_from_debug_list(struct atbm_buff* skb){
	atbm_spin_lock(&skb_debug_lock);
	atbm_skb_free++;
	atbm_list_del(&skb->skb_list);
	atbm_spin_unlock(&skb_debug_lock);
}
static __INLINE void atbm_del_skb_from_reuse_list(struct atbm_buff* skb){
	atbm_spin_lock(&skb_debug_lock);
	atbm_skb_reuse_free++;
	atbm_list_del(&skb->reuse_list);
	atbm_spin_unlock(&skb_debug_lock);
}

static __INLINE void atbm_add_skb_alloc_cnt(){
	atbm_spin_lock(&skb_debug_lock);
	atbm_skb_alloc_cnt++;
	atbm_spin_unlock(&skb_debug_lock);
}

static __INLINE void atbm_add_skb_free_cnt(){
	atbm_spin_lock(&skb_debug_lock);
	atbm_skb_free_cnt++;
	atbm_spin_unlock(&skb_debug_lock);
}

void dump_skb_status(){
	struct atbm_buff* skb;

	if(atbm_TimeAfter(last_print_time, 5*ATBM_HZ)){
		return;
	}

	atbm_spin_lock(&skb_debug_lock);
	wifi_printk(WIFI_ALWAYS, "skb cnt %d %d %d %d %d %d\n", atbm_skb_cnt, atbm_skb_free, atbm_skb_reuse_cnt,
			atbm_skb_reuse_free, atbm_skb_alloc_cnt, atbm_skb_free_cnt);
	atbm_list_for_each_entry(skb, &skb_list, skb_list){
		wifi_printk(WIFI_ALWAYS, "skb:%x len:%x ref:%d\n", skb, skb->totalLen, skb->ref);
	}
	atbm_list_for_each_entry(skb, &reuse_list, reuse_list){
		wifi_printk(WIFI_ALWAYS, "reuse skb:%x len:%x ref:%d\n", skb, skb->totalLen, skb->ref);
	}

	atbm_spin_unlock(&skb_debug_lock);
	last_print_time = atbm_GetOsTime();
}
#else
static __INLINE void atbm_add_skb_to_debug_list(struct atbm_buff* skb){}
static __INLINE void atbm_add_skb_to_reuse_list(struct atbm_buff* skb){}
static __INLINE void atbm_del_skb_from_debug_list(struct atbm_buff* skb){}
static __INLINE void atbm_del_skb_from_reuse_list(struct atbm_buff* skb){}
static __INLINE void atbm_add_skb_alloc_cnt(){}
static __INLINE void atbm_add_skb_free_cnt(){}
#endif

static struct atbm_buff * __atbm_pbuf_alloc(atbm_uint16 length)
{
	struct atbm_buff *p;
	atbm_uint32 size;

	size = ATBM_MEM_ALIGN_SIZE(sizeof(struct atbm_buff)  + ATBM_HWBUF_EXTERN_HEADROM_LEN + ATBM_HWBUF_EXTERN_TAILROM_LEN+ length);
	/* If atbm_buff is to be allocated in RAM, allocate memory for it. */
	p = (struct atbm_buff*) atbm_kmalloc(size,GFP_KERNEL);

	if (p == ATBM_NULL) {
		dump_skb_status();
		ATBM_WARN("__atbm_pbuf_alloc fail \n");
		return ATBM_NULL;
	}
	atbm_memset(p, 0, sizeof(struct atbm_buff));
	/* Set up internal structure of the atbm_buff. */
	ATBM_OS_SKB_DATA(p) = ATBM_MEM_ALIGN((atbm_void *)((atbm_uint8 *)p + sizeof(struct atbm_buff) + ATBM_HWBUF_EXTERN_HEADROM_LEN));
	ATBM_OS_SKB_HEAD(p) = ATBM_MEM_ALIGN((atbm_uint8 *)(p+1));
	//p->dlen =length+ HWBUF_EXTERN_TAILROM_LEN;

	p->ref = 1;
	p->totalLen = ATBM_MEM_ALIGN_SIZE(ATBM_HWBUF_EXTERN_HEADROM_LEN + ATBM_HWBUF_EXTERN_TAILROM_LEN + length);
	p->bufferLen = ATBM_MEM_ALIGN_SIZE(ATBM_HWBUF_EXTERN_HEADROM_LEN + ATBM_HWBUF_EXTERN_TAILROM_LEN +length);
	//wifi_printk(WIFI_ALWAYS,"****************size*****************=%d:%d:%d\n",size,p->totalLen,p->bufferLen);

	/*add for wifi*/
	ATBM_OS_SKB_LEN(p) = 0;
	p->Tail = ATBM_OS_SKB_DATA(p);
	atbm_add_skb_to_debug_list(p);
	return p;
}

atbm_void  atbm_skb_reinit(struct atbm_buff *skb)
{	
	//clear skb struct to 0
	
	ATBM_OS_SKB_DATA(skb) = ATBM_MEM_ALIGN((atbm_void  *)((atbm_uint8 *)skb + sizeof(struct atbm_buff) + ATBM_HWBUF_EXTERN_HEADROM_LEN));
	ATBM_OS_SKB_HEAD(skb) = (atbm_uint8 *)(skb+1);
	skb->bufferLen = skb->totalLen;

	//skb->ref = 1;
	//skb->totalLen = (ATBM_MEM_ALIGN_SIZE(sizeof(struct atbm_buff)  + HWBUF_EXTERN_HEADROM_LEN + HWBUF_EXTERN_TAILROM_LEN) + ATBM_MEM_ALIGN_SIZE(length));
	

	/*add for wifi*/
	ATBM_OS_SKB_LEN(skb) = 0;
	skb->Tail = ATBM_OS_SKB_DATA(skb);
}

#if ATBM_RX_REUSE_PACKET
static atbm_void __atbm_pbuf_free(struct atbm_buff *p)
{
	atbm_uint8 ref;
	unsigned long flags;

	//skb may be freed in diffrent thread
	atbm_spin_lock_irqsave(&skb_ref_lock, &flags);
	ATBM_ASSERT(p->ref >= 1);
	p->ref--;
	ref = p->ref;
	atbm_spin_unlock_irqrestore(&skb_ref_lock, flags);

	if(ref == 0){
		if(p->parent){
			__atbm_pbuf_free(p->parent);
			atbm_del_skb_from_reuse_list(p);
		}
		atbm_del_skb_from_debug_list(p);
		atbm_kfree(p);
	}
}

#else
static atbm_void __atbm_pbuf_free(struct atbm_buff *p)
{
	ATBM_ASSERT(p->ref == 1);
	p->ref--;

	atbm_del_skb_from_debug_list(p);
	atbm_kfree(p);
}
#endif

void atbm_init_skb_lock(){
#if ATBM_RX_REUSE_PACKET
	atbm_spin_lock_init(&skb_ref_lock);
#endif
#if ATBM_SKB_DEBUG
	atbm_spin_lock_init(&skb_debug_lock);
#endif
}

struct atbm_buff * atbm_dev_alloc_skb(atbm_int32 len) 
{
	struct atbm_buff * skb;
	skb =  __atbm_pbuf_alloc(len);
	if(skb == ATBM_NULL){
		ATBM_BUG_ON(1);
		//return  ATBM_NULL;
	}
	atbm_add_skb_alloc_cnt();
	return skb;
}

#if ATBM_RX_REUSE_PACKET
struct atbm_buff * atbm_dev_reuse_skb(struct atbm_buff * parent, atbm_uint8 *data, int len){
	struct atbm_buff *p;
	unsigned long flags;

	p = (struct atbm_buff*) atbm_kzalloc(sizeof(struct atbm_buff),GFP_KERNEL);
	if (p == ATBM_NULL) {
		ATBM_WARN("__atbm_pbuf_alloc fail \n");
		return ATBM_NULL;
	}
	p->parent = parent;

	/* Set up internal structure of the atbm_buff. */
	ATBM_OS_SKB_DATA(p) = data;
	ATBM_OS_SKB_HEAD(p) = data;
	//p->dlen =length+ HWBUF_EXTERN_TAILROM_LEN;

	atbm_spin_lock_irqsave(&skb_ref_lock, &flags);
	parent->ref++;
	p->ref = 1;
	atbm_spin_unlock_irqrestore(&skb_ref_lock, flags);

	p->totalLen = len;
	p->bufferLen = len;
	//wifi_printk(WIFI_ALWAYS,"****************size*****************=%d:%d:%d\n",size,p->totalLen,p->bufferLen);

	/*add for wifi*/
	ATBM_OS_SKB_LEN(p) = len;
	p->Tail = ATBM_OS_SKB_DATA(p) + len;
	atbm_add_skb_to_reuse_list(p);
	atbm_add_skb_to_debug_list(p);
	atbm_add_skb_alloc_cnt();
	return p;
}
#endif

/**************************************************************************************************************
 * 函数名: atbm_dev_alloc_skbhdr
 *
 * 功能描述:
 *   分配 Socket Buffer 头部结构体（不分配数据缓冲区）。这是零拷贝模式（WLAN_ZERO_COPY1）中
 *   使用的函数，只分配 atbm_buff 结构体本身，数据缓冲区由协议栈的 pbuf 提供。这样可以避免
 *   数据复制，提高性能。
 *
 * 在 TX 流程中的位置:
 *   阶段 1: OS 适配层（零拷贝模式）
 *   在 atbm_wifi_tx_pkt() 中，当启用零拷贝且 pbuf 是单个连续缓冲区时调用
 *
 * 参数说明:
 *   无
 *
 * 返回值:
 *   成功: Socket Buffer 指针（atbm_buff *）
 *   失败: NULL
 *
 * 使用场景:
 *   - 零拷贝模式（WLAN_ZERO_COPY1）
 *   - pbuf 是单个连续缓冲区（q->len == q->tot_len）
 *   - 需要避免数据复制，提高性能
 *
 * 注意事项:
 *   - 只分配结构体本身，不分配数据缓冲区
 *   - 数据缓冲区由协议栈的 pbuf 提供
 *   - 需要配合 atbm_dev_decriptor_os_skb() 使用
 *   - 释放时需要使用 atbm_dev_free_os_skb() 释放协议栈的 pbuf
 *
 * 相关函数:
 *   - atbm_dev_decriptor_os_skb(): 设置 pbuf 描述符信息
 *   - atbm_dev_free_os_skb(): 释放零拷贝模式下的 OS 缓冲区
 *   - __atbm_pbuf_alloc(): 底层内存分配函数
 *
 **************************************************************************************************************/
struct atbm_buff * atbm_dev_alloc_skbhdr(void) 
{
	struct atbm_buff *p;

	/* 步骤 1: 分配 atbm_buff 结构体 */
	/* 只分配结构体本身，不分配数据缓冲区（length = 0） */
	/* If atbm_buff is to be allocated in RAM, allocate memory for it. */
	p = (struct atbm_buff*) __atbm_pbuf_alloc(0);
	if (p == ATBM_NULL) {
		ATBM_WARN("atbm_dev_alloc_skbhdr fail \n");
		return ATBM_NULL;
	}
  
    return p;
}


atbm_int32 atbm_dev_kfree_skb(struct atbm_buff * skb)
{
#if defined (WLAN_ZERO_COPY1)
	atbm_dev_free_os_skb(skb);
#endif //#if defined (WLAN_ZERO_COPY1)
	__atbm_pbuf_free(skb);
	atbm_add_skb_free_cnt();
	return 1;
}


/*
 *	Add data to an atbm_buff
 */
atbm_uint8 *atbm_skb_put(struct atbm_buff *skb, atbm_uint32 len)
{
	atbm_uint8 *tmp = skb->Tail;
	skb->Tail += len;
	ATBM_OS_SKB_LEN(skb)  += len;
	return tmp;
}

atbm_uint8 *atbm_skb_push(struct atbm_buff *skb, atbm_uint32 len)
{
 	ATBM_OS_SKB_DATA(skb) -= len;
 	ATBM_OS_SKB_LEN(skb)  += len;
	return ATBM_OS_SKB_DATA(skb);
}

atbm_uint8 * atbm_skb_pull(struct atbm_buff *skb, atbm_uint32 len)
{
	ATBM_OS_SKB_LEN(skb) -= len;
	return ATBM_OS_SKB_DATA(skb) += len;
}


/**
 *	atbm_skb_headroom - bytes at buffer head
 *	@skb: buffer to check
 *
 *	Return the number of bytes of free space at the head of an &atbm_buff.
 */
atbm_uint32 atbm_skb_headroom(const struct atbm_buff *skb)
{
	return ATBM_OS_SKB_DATA(skb) - (atbm_uint8 *)ATBM_OS_SKB_HEAD(skb);
}

/**
 *	skb_tailroom - bytes at buffer end
 *	@skb: buffer to check
 *
 *	Return the number of bytes of free space at the tail of an atbm_buff
 */
atbm_int32 atbm_skb_tailroom(const struct atbm_buff *skb)
{
	return ((atbm_uint8 *)ATBM_OS_SKB_HEAD(skb)+ skb->bufferLen) - (atbm_uint8 *)skb->Tail;
}

/**
 *	skb_reserve - adjust headroom
 *	@skb: buffer to alter
 *	@len: bytes to move
 *
 *	Increase the headroom of an empty &atbm_buff by reducing the tail
 *	room. This is only allowed for an empty buffer.
 */
atbm_int32 atbm_skb_reserve(struct atbm_buff *skb, atbm_int32 len)
{
	//atbm_uint8 * buf = OS_SKB_DATA(skb);
	//ATBM_WARN_ON_FUNC(OS_SKB_LEN(skb) > 0);
	//if(skb->dlen < len){
	//	return -1;
	//}
	ATBM_OS_SKB_DATA(skb) += len;
	skb->Tail = ATBM_OS_SKB_DATA(skb);
	//skb->tot_len -= len;
	//skb->dlen -= len;	
	return 0;
}


atbm_void atbm_skb_set_tail_pointer(struct atbm_buff *skb, const atbm_int32 offset)
{
	skb->Tail = ATBM_OS_SKB_DATA(skb) + offset;
}

atbm_void atbm_skb_trim(struct atbm_buff *skb, atbm_uint32 len)
{

	ATBM_OS_SKB_LEN(skb) = len;
	atbm_skb_set_tail_pointer(skb, len);
}

static atbm_void __atbm_skb_insert(struct atbm_buff *newsk,
				struct atbm_buff *prev, struct atbm_buff *next,
				struct atbm_buff_head *list)
{
	newsk->next = next;
	newsk->prev = prev;
	next->prev  = prev->next = newsk;
	list->qlen++;
}

/**
 *	__skb_queue_after_ - queue a buffer at the list head
 *	@list: list to use
 *	@prev: place after this buffer
 *	@newsk: buffer to queue
 *
 *	Queue a buffer int the middle of a list. This function takes no locks
 *	and you must therefore hold required locks before calling it.
 *
 *	A buffer cannot be placed on two lists at the same time.
 */
 /*
static atbm_void __skb_queue_after_(struct atbm_buff_head *list,
				     struct atbm_buff *prev,
				     struct atbm_buff *newsk)
{
	__atbm_skb_insert(newsk, prev, prev->next, list);
}
*/
 static  atbm_void __skb_queue_before_(struct atbm_buff_head *list,
				      struct atbm_buff *next,
				      struct atbm_buff *newsk)
{
	__atbm_skb_insert(newsk, next->prev, next, list);
}
/*
static  atbm_void __skb_queue_head_(struct atbm_buff_head *list,
				    struct atbm_buff *newsk)
{
	__skb_queue_after_(list, (struct atbm_buff *)list, newsk);
}
*/
/**
 *	__skb_queue_tail_ - queue a buffer at the list tail
 *	@list: list to use
 *	@newsk: buffer to queue
 *
 *	Queue a buffer at the end of a list. This function takes no locks
 *	and you must therefore hold required locks before calling it.
 *
 *	A buffer cannot be placed on two lists at the same time.
 */
static  atbm_void __skb_queue_tail_(struct atbm_buff_head *list,
				   struct atbm_buff *newsk)
{
	__skb_queue_before_(list, (struct atbm_buff *)list, newsk);
}
static  atbm_void __skb_unlink_(struct atbm_buff *skb, struct atbm_buff_head *list)
{
	struct atbm_buff *next, *prev;

	list->qlen--;
	next	   = skb->next;
	prev	   = skb->prev;
	skb->next  = skb->prev = ATBM_NULL;
	next->prev = prev;
	prev->next = next;
}

/**
 *	atbm_skb_peek_tail - peek at the tail of an &atbm_buff_head
 *	@list_: list to peek at
 *
 *	Peek an &atbm_buff. Unlike most other operations you _MUST_
 *	be careful with this one. A peek leaves the buffer on the
 *	list and someone else may run off with it. You must hold
 *	the appropriate locks or have a private queue to do this.
 *
 *	Returns %NULL for an empty list or a pointer to the tail element.
 *	The reference count is not incremented and the reference is therefore
 *	volatile. Use with caution.
 */
 /*
static  struct atbm_buff *atbm_skb_peek_tail(struct atbm_buff_head *list_)
{
	struct atbm_buff *list = ((struct atbm_buff *)list_)->prev;
	if (list == (struct atbm_buff *)list_)
		list = ATBM_NULL;
	return list;
}
*/
/**
 *	atbm_skb_peek - peek at the head of an &atbm_buff_head
 *	@list_: list to peek at
 *
 *	Peek an &atbm_buff. Unlike most other operations you _MUST_
 *	be careful with this one. A peek leaves the buffer on the
 *	list and someone else may run off with it. You must hold
 *	the appropriate locks or have a private queue to do this.
 *
 *	Returns %NULL for an empty list or a pointer to the head element.
 *	The reference count is not incremented and the reference is therefore
 *	volatile. Use with caution.
 */
struct atbm_buff *atbm_skb_peek(struct atbm_buff_head *list_)
{
	struct atbm_buff *list = ((struct atbm_buff *)list_)->next;
	if (list == (struct atbm_buff *)list_)
		list = ATBM_NULL;
	return list;
}
/**
 *	__skb_dequeue_tail_ - remove from the tail of the queue
 *	@list: list to dequeue from
 *
 *	Remove the tail of the list. This function does not take any locks
 *	so must be used with appropriate locks held only. The tail item is
 *	returned or %NULL if the list is empty.
 */
 /*
static  struct atbm_buff *__skb_dequeue_tail_(struct atbm_buff_head *list)
{
	struct atbm_buff *skb = atbm_skb_peek_tail(list);
	if (skb)
		__skb_unlink_(skb, list);
	return skb;
}
*/
/**
 *	__skb_dequeue_ - remove from the head of the queue
 *	@list: list to dequeue from
 *
 *	Remove the head of the list. This function does not take any locks
 *	so must be used with appropriate locks held only. The head item is
 *	returned or %NULL if the list is empty.
 */
static  struct atbm_buff *__skb_dequeue_(struct atbm_buff_head *list)
{
	struct atbm_buff *skb = atbm_skb_peek(list);
	if (skb)
		__skb_unlink_(skb, list);
	return skb;
}

/**
 *	skb_dequeue - remove from the head of the queue
 *	@list: list to dequeue from
 *
 *	Remove the head of the list. The list lock is taken so the function
 *	may be used safely with other locking list functions. The head item is
 *	returned or %NULL if the list is empty.
 */
struct atbm_buff *atbm_skb_dequeue(struct atbm_buff_head *list)
{
	unsigned long flags;
	struct atbm_buff *result;

	atbm_spin_lock_irqsave(&list->lock, &flags);
	wifi_printk(WIFI_DBG_MSG,"atbm: atbm_skb_dequeue(), start.\n");
	result = __skb_dequeue_(list);
	wifi_printk(WIFI_DBG_MSG,"atbm: atbm_skb_dequeue(), end.\n");
	atbm_spin_unlock_irqrestore(&list->lock, flags);
	return result;
}

atbm_void atbm_skb_queue_tail(struct atbm_buff_head *list, struct atbm_buff *newsk)
{
	
	unsigned long flags;

	atbm_spin_lock_irqsave(&list->lock, &flags);
	wifi_printk(WIFI_DBG_MSG,"atbm: atbm_skb_queue_tail(), start.\n");
	__skb_queue_tail_(list, newsk);
	wifi_printk(WIFI_DBG_MSG,"atbm: atbm_skb_queue_tail(), end.\n");
	atbm_spin_unlock_irqrestore(&list->lock, flags);
}
/**
 *	skb_queue_purge - empty a list
 *	@list: list to empty
 *
 *	Delete all buffers on an &atbm_buff list. Each buffer is removed from
 *	the list and one reference dropped. This function takes the list
 *	lock and is atomic with respect to other list locking functions.
 */
 atbm_void atbm_skb_queue_purge(struct atbm_buff_head *list)
{
	struct atbm_buff *skb;
	while ((skb = atbm_skb_dequeue(list)) != ATBM_NULL)
		atbm_dev_kfree_skb(skb);
}

