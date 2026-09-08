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

#include <reent.h>
#include <errno.h>
#include <stdio.h>
#include <reent.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <FreeRTOS.h>
#include "task.h"

#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_POSIX)
#include "evfs_posix.h"
#define SYS_VFS_ENABLE 1
#endif

#ifdef SYS_TRACE_MEM_ENABLE
typedef struct
{
    void *ptr;
    size_t size;
    void *caller;  // actually the return address of malloc/free, used to trace where malloc/free is called
}malloc_entry_t;

typedef struct
{
    int table_full;
    uint32_t entry_num;
    uint32_t entry_num_max;
}malloc_table_info_t;

typedef struct 
{
    void* caller;
    uint32_t totalSize;
    uint32_t mallocTimes;
}mem_stats_t;

malloc_entry_t malloc_entry[SYS_TRACE_MEM_ENTRY_NUM];
malloc_table_info_t malloc_table_info;
#define SYS_TRACE_MEM_STATS_ENTRY_NUM 100
mem_stats_t mem_stats[SYS_TRACE_MEM_STATS_ENTRY_NUM];

bool set_monitor;
uint32_t callerAddr;
uint32_t mallocCnt = 0;
uint32_t freeCnt = 0;

void trace_malloc(void *ptr, size_t size, void *caller)
{
    int i;

    if(ptr == NULL){
        return;
    }

    if(!malloc_table_info.table_full){
        for(i = 0; i < SYS_TRACE_MEM_ENTRY_NUM; i++){
            if(malloc_entry[i].ptr == NULL){
                malloc_entry[i].ptr = ptr;
                malloc_entry[i].size = size;
                malloc_entry[i].caller = caller;
                if(set_monitor && callerAddr == (uint32_t)caller)
                {
                    mallocCnt++;
                    printf("trace_malloc, caller=%08lx, ptr=%08lx, mallocCnt=%lu\r\n", callerAddr, (uint32_t)malloc_entry[i].ptr, mallocCnt);
                }
                break;
            }
        }
        if(i == SYS_TRACE_MEM_ENTRY_NUM){
            malloc_table_info.table_full = 1;
        }
    }

    malloc_table_info.entry_num++;
    if(malloc_table_info.entry_num > malloc_table_info.entry_num_max){
        malloc_table_info.entry_num_max = malloc_table_info.entry_num;
    }
}

void trace_free(void *ptr, void *caller)
{
    int i;

    if(ptr == NULL){
        return;
    }

    if(!malloc_table_info.table_full){
        for(i = 0; i < SYS_TRACE_MEM_ENTRY_NUM; i++){
            if(malloc_entry[i].ptr == ptr){
                if(set_monitor && callerAddr == (uint32_t)malloc_entry[i].caller)
                {
                    freeCnt++;
                    printf("trace_free, caller=%08lx, ptr=%08lx, freeCnt=%lu\r\n", callerAddr, (uint32_t)malloc_entry[i].ptr, freeCnt);
                }
                malloc_entry[i].ptr = NULL;
                break;
            }
        }
    }

    malloc_table_info.entry_num--;
}

void trace_realloc(void *ptr_new, void *ptr_old, size_t size, void *caller)
{
    int i;

    if(ptr_old == NULL){
        trace_malloc(ptr_new, size, caller);
    }else if(ptr_new == NULL){
        trace_free(ptr_old, caller);
    }else{
        if(!malloc_table_info.table_full){
            for(i = 0; i < SYS_TRACE_MEM_ENTRY_NUM; i++){
                if(malloc_entry[i].ptr == ptr_old){
                    malloc_entry[i].ptr = ptr_new;
                    malloc_entry[i].size = size;
                    malloc_entry[i].caller = caller;
                    if(set_monitor && callerAddr == (uint32_t)malloc_entry[i].caller)
                    {
                        printf("trace_realloc, caller=%08lx, ptr=%08lx\r\n", callerAddr, (uint32_t)malloc_entry[i].ptr);
                    }
                    break;
                }
            }
            if(i == SYS_TRACE_MEM_ENTRY_NUM){
                trace_malloc(ptr_new, size, caller);
            }
        }
    }
}

void mem_trace_stats()
{
    for(int i=0;i<SYS_TRACE_MEM_STATS_ENTRY_NUM;i++)
    {
        mem_stats[i].caller = NULL;
        mem_stats[i].totalSize = 0;
        mem_stats[i].mallocTimes = 0;
    }
    
    for(int i=0; i<SYS_TRACE_MEM_ENTRY_NUM; i++)
    {
        if(malloc_entry[i].ptr == NULL)
        {
            continue;
        }

        uint8_t fExist = 0;
        uint16_t firstEmpty = 0xFFFF;
        for(int j=0; j<SYS_TRACE_MEM_STATS_ENTRY_NUM;j++)
        {
            if(malloc_entry[i].caller == mem_stats[j].caller)
            {
                fExist = 1;
                mem_stats[j].mallocTimes++;
                mem_stats[j].totalSize += malloc_entry[i].size;
                break;
            }

            if(firstEmpty==0xFFFF && mem_stats[j].caller == NULL)
            {
                firstEmpty = j;
            }
        }

        if(!fExist && firstEmpty != 0xFFFF)
        {
            mem_stats[firstEmpty].caller = malloc_entry[i].caller;
            mem_stats[firstEmpty].totalSize = malloc_entry[i].size;
            mem_stats[firstEmpty].mallocTimes = 1;
        }
    }
    
    for(int i=0;i<SYS_TRACE_MEM_STATS_ENTRY_NUM;i++)
    {
        if(mem_stats[i].caller)
        {
            printf("%d, caller:0x%08lx, totalSize:%lu, mallocTimes:%lu\r\n", i, (uint32_t)mem_stats[i].caller, mem_stats[i].totalSize, mem_stats[i].mallocTimes);
        }
    }
    printf("Current left size is %d bytes\r\n", xPortGetFreeHeapSize());
}

#endif

/* Reentrant versions of system calls.  */

/* global errno in RT-Thread */
static volatile int _sys_errno = 0;

#ifndef _REENT_ONLY
int *__errno ()
{
    #if( configUSE_POSIX_ERRNO == 1 )
	{
        extern int FreeRTOS_errno;

        return &FreeRTOS_errno;
	}
	#endif
    return &_REENT->_errno;;
}
#endif

struct _reent *__getreent(void)
{
  return _impure_ptr;
}

int _getpid_r(struct _reent *ptr)
{
    return 0;
}

int _execve_r(struct _reent *ptr, const char * name, char *const *argv, char *const *env)
{
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
}

int _fcntl_r(struct _reent *ptr, int fd, int cmd, int arg)
{
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
}

int _fork_r(struct _reent *ptr)
{
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
}

int _fstat_r(struct _reent *ptr, int fd, struct stat *pstat)
{
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
}

int _isatty_r(struct _reent *ptr, int fd)
{
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
}

int _kill_r(struct _reent *ptr, int pid, int sig)
{
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
}

int _link_r(struct _reent *ptr, const char *old, const char *new)
{
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
}

_off_t _lseek_r(struct _reent *ptr, int fd, _off_t pos, int whence)
{
#ifdef SYS_VFS_ENABLE
    return (_off_t)evfs_posix_lseek(fd, pos, whence);
#else
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
#endif
}

#if 0
int	mkdir (const char *_path, mode_t __mode )
{
#ifndef SYS_VFS_ENABLE
    return -1;
#else
    int rc;

    rc = aos_mkdir(_path);
    return rc;
#endif
}
#endif

int _open_r(struct _reent *ptr, const char *file, int flags, int mode)
{
#ifdef SYS_VFS_ENABLE
    return evfs_posix_open(file, flags);
#else
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
#endif
}

int _close_r(struct _reent *ptr, int fd)
{
#ifdef SYS_VFS_ENABLE
    return evfs_posix_close(fd);
#else
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
#endif
}

_ssize_t _read_r(struct _reent *ptr, int fd, void *buf, size_t nbytes)
{
#ifdef SYS_VFS_ENABLE
    return (_ssize_t)evfs_posix_read(fd, buf, nbytes);
#else
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
#endif
}

int _rename_r(struct _reent *ptr, const char *old, const char *new)
{
#ifdef SYS_VFS_ENABLE
    return evfs_posix_rename(old, new);
#else
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
#endif
}

int _stat_r(struct _reent *ptr, const char *file, struct stat *pstat)
{
#ifdef SYS_VFS_ENABLE
    return evfs_posix_stat(file, pstat);
#else
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
#endif
}

int _unlink_r(struct _reent *ptr, const char *file)
{
#ifdef SYS_VFS_ENABLE
    return evfs_posix_unlink(file);
#else
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
#endif
}

int _wait_r(struct _reent *ptr, int *status)
{
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
}

_ssize_t _write_r(struct _reent *ptr, int fd, const void *buf, size_t nbytes)
{
#ifdef SYS_VFS_ENABLE
    return (_ssize_t)evfs_posix_write(fd, buf, nbytes);
#else
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
#endif
}

int fsync(int fd)
{
#ifdef SYS_VFS_ENABLE
    return evfs_posix_fsync(fd);
#else
    /* return "not supported" */
    errno = ENOSYS;
    return -1;
#endif
}

void *_malloc_r(struct _reent *ptr, size_t size)
{
    void* result;

    if (size == 0)
    {
        return NULL;
    }

	result = (void*)pvPortMalloc(size);

    if (result == NULL)
    {
        ptr->_errno = ENOMEM;
    }

#ifdef SYS_TRACE_MEM_ENABLE
    trace_malloc(result, size, (void *)__builtin_return_address(0));
#endif

    return result;
}

void *_realloc_r(struct _reent *ptr, void *old, size_t newlen)
{
    void* result;

	result = (void*)pvPortRealloc(old, newlen);

    if (result == NULL)
    {
        ptr->_errno = ENOMEM;
    }

#ifdef SYS_TRACE_MEM_ENABLE
    trace_realloc(result, old, newlen, (void *)__builtin_return_address(0));
#endif

    return result;
}

void *_calloc_r(struct _reent *ptr, size_t size, size_t len)
{
    void* result;

    if (size == 0)
    {
        return NULL;
    }

	result = (void*)pvPortCalloc(size, len);

    if (result == NULL)
    {
        ptr->_errno = ENOMEM;
    }

#ifdef SYS_TRACE_MEM_ENABLE
    trace_malloc(result, size, (void *)__builtin_return_address(0));
#endif

    return result;
}

void _free_r(struct _reent *ptr, void *addr)
{
	vPortFree(addr);

#ifdef SYS_TRACE_MEM_ENABLE
    trace_free(addr, (void *)__builtin_return_address(0));
#endif
}

void* _valloc_r(struct _reent *ptr, size_t size) __attribute__((alias("_malloc_r")));

void* _pvalloc_r(struct _reent *ptr, size_t size) __attribute__((alias("_malloc_r")));

void _cfree_r(struct _reent *ptr, void *addr) __attribute__((alias("_free_r")));

void *_sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
    void *ret;
    ptr->_errno = ENOMEM;
    ret = (void *)-1;
    return ret;
}

/* for exit() and abort() */
void __attribute__ ((noreturn))
_exit (int status)
{
    printf("\r\n!!! _exit(%d) called from task: %s !!!\r\n", 
           status, pcTaskGetName(NULL));
    
    /* Delete current task instead of crashing whole system */
    vTaskDelete(NULL);
    
    /* Should never reach here */
    while(1);
}

void _system(const char *s)
{
}

void __libc_init_array(void)
{
    /* we not use __libc init_aray to initialize C++ objects */
}

mode_t umask(mode_t mask)
{
    return 022;
}

int flock(int fd, int operation)
{
    return 0;
}

void newlibc_syscall_init(void)
{
    // For force sync symbol
}
