/* TinyMaix port configuration for IPRO7 (rv32imafc, FreeRTOS) */

#ifndef __TM_PORT_H
#define __TM_PORT_H

#define TM_ARCH_CPU         (0)
#define TM_ARCH_ARM_SIMD    (1)
#define TM_ARCH_ARM_NEON    (2)
#define TM_ARCH_ARM_MVEI    (3)
#define TM_ARCH_RV32P       (4)
#define TM_ARCH_RV64V       (5)
#define TM_ARCH_CSKYV2      (6)
#define TM_ARCH_X86_SSE2    (7)

#define TM_OPT0             (0)
#define TM_OPT1             (1)
#define TM_OPT2             (2)

/******************************* PORT CONFIG  ************************************/
#define TM_ARCH         TM_ARCH_CPU     /* rv32imafc: no packed SIMD, use scalar C */
#define TM_OPT_LEVEL    TM_OPT0        /* Least code and buffer usage */
#define TM_MDL_TYPE     TM_MDL_INT8    /* INT8 quantized models */
#define TM_FASTSCALE    (1)            /* Enable: rv32imafc has FPU but INT8 benefits from fast scaling */
#define TM_LOCAL_MATH   (0)            /* Use standard libm (FPU available) */
#define TM_ENABLE_STAT  (1)            /* Enable model statistics */
#define TM_MAX_CSIZE    (1000)         /* Max channel num */
#define TM_MAX_KSIZE    (5*5)          /* Max kernel size */
#define TM_MAX_KCSIZE   (3*3*256)      /* Max kernel_size * channels */

#define TM_INLINE       __attribute__((always_inline)) static inline
#define TM_WEAK         __attribute__((weak))

/* Memory allocation via FreeRTOS */
#include <FreeRTOS.h>
#define tm_malloc(x)    pvPortMalloc(x)
#define tm_free(x)      vPortFree(x)

/* Debug output */
#include <stdio.h>
#define TM_PRINTF(...)  printf(__VA_ARGS__)
#define TM_DBG(...)     TM_PRINTF("###L%d: ",__LINE__);TM_PRINTF(__VA_ARGS__);
#define TM_DBGL()       TM_PRINTF("###L%d\n",__LINE__);

/******************************* DBG TIME CONFIG  ************************************/
/* Use RISC-V mcycle CSR for timing */
static inline uint32_t _tm_get_us(void) {
    uint32_t cycles;
    __asm__ volatile("csrr %0, mcycle" : "=r"(cycles));
    return cycles / 320;  /* Assuming 320MHz clock */
}

#define TM_GET_US()        _tm_get_us()

#define TM_DBGT_INIT()     uint32_t _start,_finish;float _time;_start=TM_GET_US();
#define TM_DBGT_START()    _start=TM_GET_US();
#define TM_DBGT(x)         {_finish=TM_GET_US();\
                            _time = (float)(_finish-_start)/1000.0;\
                            TM_PRINTF("===%s use %.3f ms\n", (x), _time);\
                            _start=TM_GET_US();}

/******************************* DBG PERFORMANCE CONFIG  ************************************/
#define TM_EN_PERF 0

#if TM_EN_PERF
    #define  TM_GET_TICK(x)     __asm__ volatile("csrr %0, mcycle" : "=r"(x));
    #define  TM_TICK_PERUS    (320)
    #define  TM_PERF_REG(x)    uint64_t x=0;
    #define  TM_PERF_EXTREG(x) extern uint64_t x;
    #define  TM_PERF_INIT(x)   uint64_t _##x##_t0, _##x##_t1;
    #define  TM_PERF_START(x)  TM_GET_TICK(_##x##_t0);
    #define  TM_PERF_ADD(x)   {TM_GET_TICK(_##x##_t1);(x)+=(_##x##_t1-_##x##_t0);TM_GET_TICK(_##x##_t0);};
    #define  TM_PERF_PRINT(x) TM_PRINTF("PERF "#x": %ld us\r\n", (x)/TM_TICK_PERUS)
#else
    #define  TM_GET_TICK(x)
    #define  TM_TICK_PERUS
    #define  TM_PERF_REG(x)
    #define  TM_PERF_EXTREG(x)
    #define  TM_PERF_INIT(x)
    #define  TM_PERF_START(x)
    #define  TM_PERF_ADD(x)
    #define  TM_PERF_PRINT(x)
#endif

/******************************* OPS CONFIG  ************************************/

#endif /* __TM_PORT_H */
