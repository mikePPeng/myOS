/*
 * Created by mikePPeng.
 * This file implements system level exceptions.
 * Change Logs:
 * Date           Notes
 * Feb 27, 2021   the first version
 */

#include "kernel/system_exception.h"

__attribute__((naked)) void PendSV_Handler(void)
{
    //1. save the context of current task
    __asm volatile("MRS R0, PSP");
    __asm volatile("STMDB R0!, {R4-R11}");

    //save lr before call other function
    __asm volatile("PUSH {LR}");

    //2. save the current value of psp
    __asm volatile("BL save_current_psp");

    //3. retrieve the context of next task
    __asm volatile("BL get_next_psp");

    __asm volatile("LDMIA R0!, {R4-R11}");

    //update psp
    __asm volatile("MSR PSP,R0");

    //update current task with next task and exit
    __asm volatile("BL update_cur_with_next");
    __asm volatile("POP {LR}");
    __asm volatile("BX LR");
}

void SysTick_Handler(void)
{
    //HAL_IncTick();
    soft_timer_check();

    update_task_state();

    task_schedule();
}
