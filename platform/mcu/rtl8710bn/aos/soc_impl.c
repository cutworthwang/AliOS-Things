/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>
#include <sys/time.h>
#include "diag.h"

#include "ameba_soc.h"
#include "build_info.h"
//#include "strproc.h"
#include "system_8195a.h"


#if (RHINO_CONFIG_HW_COUNT > 0)
void soc_hw_timer_init(void)
{
}

hr_timer_t soc_hr_hw_cnt_get(void)
{
    return 0;
    //return *(volatile uint64_t *)0xc0000120;
}

lr_timer_t soc_lr_hw_cnt_get(void)
{
    return 0;
}
#endif /* RHINO_CONFIG_HW_COUNT */

#if (RHINO_CONFIG_INTRPT_GUARD > 0)
void soc_intrpt_guard(void)
{
}
#endif

#if (RHINO_CONFIG_INTRPT_STACK_REMAIN_GET > 0)
size_t soc_intrpt_stack_remain_get(void)
{
    return 0;
}
#endif

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
void soc_intrpt_stack_ovf_check(void)
{
}
#endif

#if (RHINO_CONFIG_DYNTICKLESS > 0)
void soc_tick_interrupt_set(tick_t next_ticks,tick_t elapsed_ticks)
{
}

tick_t soc_elapsed_ticks_get(void)
{
    return 0;
}
#endif

#if (RHINO_CONFIG_MM_TLF > 0)
#if !defined (__CC_ARM) /* Keil / armcc */
//extern void         *heap_start;
//extern void         *heap_end;
//extern void         *heap_len;

//extern void         *heap2_start;
//extern void         *heap2_len;
#endif

//#include "section_config.h"
//SRAM_BF_DATA_SECTION
//#endif
//static unsigned char ucHeap[ configTOTAL_HEAP_SIZE ];


#if defined (__CC_ARM) /* Keil / armcc */
#define HEAP_BUFFER_SIZE 1024*60
uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}, {(uint8_t *)0x10000000, 0x8000}};
#else

k_mm_region_t g_mm_region[] = {{(uint8_t*)&heap_start,(size_t)&heap_len}, 
                                                          {(uint8_t*)MM_ALIGN_UP(0x100014f9), MM_ALIGN_DOWN(0xb07)}, 
                                                          {(uint8_t*)MM_ALIGN_UP(0x10002475), MM_ALIGN_DOWN(0x2b8b)}};

#endif
int           g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);

#endif

#if (RHINO_CONFIG_MM_LEAKCHECK > 0 )

//extern int __bss_start__, __bss_end__, _sdata, _edata;
//u8* __bss_start__;
void aos_mm_leak_region_init(void)
{
#if (RHINO_CONFIG_MM_DEBUG > 0)
    krhino_mm_leak_region_init(__bss_start__, __bss_end__);
    //krhino_mm_leak_region_init(&_sdata, &_edata);
#endif
}

#endif


#if (RHINO_CONFIG_TASK_STACK_CUR_CHECK > 0)
size_t soc_get_cur_sp()
{
    size_t sp = 0;
#if defined (__GNUC__)&&!defined(__CC_ARM)
    asm volatile(
        "mov %0,sp\n"
        :"=r"(sp));
#endif
    return sp;
}
static void soc_print_stack()
{
    void    *cur, *end;
    int      i=0;
    int     *p;
    ktask_t * task;
    task = krhino_cur_task_get();
    printf("task name %s\r\n", task->task_name);
    end   = krhino_cur_task_get()->task_stack_base + krhino_cur_task_get()->stack_size;
    cur = (void *)soc_get_cur_sp();
    p = (int*)cur;
    while(p < (int*)end) {
        if(i%4==0) {
            printf("\r\n%08x:",(uint32_t)p);
        }
        printf("%08x ", *p);
        i++;
        p++;
    }
    printf("\r\n");
    return;
}
#endif

void soc_err_proc(kstat_t err)
{
    (void)err;
    printf("soc_err_proc : %d", err);
    #if (RHINO_CONFIG_TASK_STACK_CUR_CHECK > 0)
    soc_print_stack();
    #endif
    assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;

