/*
 * Created by mikePPeng.
 * This file implements software timer related APIs.
 * Change Logs:
 * Date           Notes
 * Mar 9, 2021   the first version
 */

#include "kernel/soft_timer.h"

list_head_init(g_timer_list_head);

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
                        timer_type_t type)
{
    //initialize timer handler
    strncpy(timer_handler->name, name, NAME_MAX_LEN);
    timer_handler->timeout_func = (void *)timeout_func;
    timer_handler->parameter = (void *)parameter;
    timer_handler->init_tick = init_tick;
    timer_handler->timeout_tick = init_tick;
    timer_handler->type = type;

    return ERR_OK;
}

/*
 * This function is used to start the given software timer.
 * Input:
 * timer_handler: handler of timer
 * Output:
 * none
 */
void soft_timer_start(p_soft_timer_t timer_handler)
{
    if (list_empty(&g_timer_list_head)) {
        //the first entry, add to list directly
        list_add_before(&timer_handler->list, &g_timer_list_head);
    } else {
        p_soft_timer_t itr, itr_next;
        list_for_each_entry_safe(itr, itr_next, &g_timer_list_head, list) {
            if (timer_handler->timeout_tick <= itr->timeout_tick) {   //new timer will be the first
                itr->timeout_tick -= timer_handler->timeout_tick;
                list_add_after(&timer_handler->list, &g_timer_list_head);
                break;
            } else {
                timer_handler->timeout_tick -= itr->timeout_tick;
                if (&itr_next->list != &g_timer_list_head) {  //@itr is not the last entry
                    if (timer_handler->timeout_tick < itr_next->timeout_tick) {
                        itr_next->timeout_tick -= timer_handler->timeout_tick;
                        list_add_between(&timer_handler->list, &itr->list, &itr_next->list);
                        break;
                    } else {
                        continue;
                    }
                } else {   //@itr is the last entry
                    list_add_before(&timer_handler->list, &g_timer_list_head);
                    break;
                }
            }
        }
    }
}

/*
 * This function is used to stop the given software timer.
 * Input:
 * timer_handler: handler of timer
 * Output:
 * none
 */
void soft_timer_stop(p_soft_timer_t timer_handler)
{
    //remove entry from list
    list_del(&timer_handler->list);
}

/*
 * This function is used to check timer timeout.
 * Input:
 * none
 * Output:
 * none
 */
void soft_timer_check(void)
{
    if (!list_empty(&g_timer_list_head)) {
        p_soft_timer_t timer = NULL;
        timer = list_entry((&g_timer_list_head)->next, typeof(soft_timer_t), list);
        if (timer->timeout_tick == 0) {
            //time is up
            ((void (*) (void *))timer->timeout_func)(timer->parameter);
            soft_timer_stop(timer);

            if (timer->type == TYPE_REPEAT) {
                //insert timer to timer list again
                timer->timeout_tick = timer->init_tick;
                soft_timer_start(timer);
            }
        }
        timer->timeout_tick--;
    }
}
