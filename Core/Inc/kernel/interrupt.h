/*
 * Created by mikePPeng.
 * This file declares APIs in interrupt.c
 * Change Logs:
 * Date           Notes
 * Feb 23, 2021   the first version
 */

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <stdint.h>

__attribute__((naked)) uint32_t interrupt_disable(void);
__attribute__((naked)) void interrupt_enable(uint32_t prev_state);

#endif
