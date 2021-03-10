/*
 * Created by mikePPeng.
 * This file declares software timer related APIs.
 * Change Logs:
 * Date           Notes
 * Mar 9, 2021   the first version
 */

#ifndef __SOFTWARE_TIMER_H__
#define __SOFTWARE_TIMER_H__

#include <stdint.h>
#include <string.h>
#include "kernel/common.h"

typedef enum software_timer_type {
    TYPE_ONESHOT = 0,
    TYPE_REPEAT,
} timer_type_t;

typedef struct software_timer {
    char             name[NAME_MAX_LEN];
    void            *timeout_func;
    void            *parameter;
    uint32_t         init_tick;
    uint32_t         timeout_tick;   //relative tick count calculated using all previous timer list entry
    timer_type_t     type;
    struct list_head list;
} soft_timer_t, *p_soft_timer_t;

/*
 * This function is used to create a software timer.
 * Input:
 * soft_handler:  handler of timer
 * name:          name of timer
 * timeout_func:  function to call when time is up
 * parameter:     input of timeout function
 * init_tick:     timeout of timer in tick
 * type:          type of timer
 * Output:
 * create result: 0 - ok
 *                1 - fail
 */
err_t soft_timer_create(p_soft_timer_t timer_handler,
                       const char *name,
                       void (*timeout_func) (void *parameter),
                       void *parameter,
                       uint32_t init_tick,
                       timer_type_t type);

/*
 * This function is used to start the given software timer.
 * Input:
 * timer_handler: handler of timer
 * Output:
 * none
 */
void soft_timer_start(p_soft_timer_t timer_handler);

/*
 * This function is used to stop the given software timer.
 * Input:
 * timer_handler: handler of timer
 * Output:
 * none
 */
void soft_timer_stop(p_soft_timer_t timer_handler);

/*
 * This function is used to check timer timeout.
 * Input:
 * none
 * Output:
 * none
 */
void soft_timer_check(void);

#endif
