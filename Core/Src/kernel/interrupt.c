/*
 * Created by mikePPeng.
 * This file contains interrupt related APIs of myOS.
 * Change Logs:
 * Date           Notes
 * Feb 23, 2021   the first version
 */

#include "kernel/interrupt.h"

__attribute__((naked)) uint32_t interrupt_disable(void)
{
    __asm volatile("MRS R0, PRIMASK");
    __asm volatile("CPSID I");
    __asm volatile("BX LR");
}

__attribute__((naked)) void interrupt_enable(uint32_t prev_state)
{
    __asm volatile("MSR PRIMASK, R0");
    __asm volatile("BX LR");
}
