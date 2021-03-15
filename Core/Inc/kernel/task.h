/*
 * Created by mikePPeng.
 * This file declares the task related APIs of myOS.
 * Change Logs:
 * Date           Notes
 * Feb 23, 2021   the first version
 * Mar  3, 2021   add priority to task
 * Mar 11, 2021   add ipc support to task
 */

#ifndef __TASK_H__
#define __TASK_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "kernel/common.h"
#include "kernel/interrupt.h"
#include "kernel/soft_timer.h"

#define IDLE_STACK_SIZE 200

typedef enum task_state {
    TASK_READY,
    TASK_RUNNING,
    TASK_PENDING,
} task_state;

typedef struct task_control_block {
    char             name[NAME_MAX_LEN];
    uint32_t        *sp;
    void            *entry;
    void            *parameter;
    task_state       state;

    void            *stack_addr;
    uint32_t         stack_size;

    //used for task delay
    uint32_t         delay_tick;
    uint32_t         delay_tick_left;

    //used for time slice
    uint32_t         init_tick;
    uint32_t         init_tick_left;

    uint8_t          prio;

    //software timer for ipc
    soft_timer_t     soft_timer;
    err_t            error;

    uint32_t         event;
    uint32_t         event_flag;

    //list for scheduler
    struct list_head list;
} tcb_t, *p_tcb_t;

typedef struct priority_list {
    uint8_t          prio;
    struct list_head task_list_head;   //task list head for each priority
    struct list_head list;
}prio_list_t, *p_prio_list_t;

/*
 * This function is used to create a task with given task stack.
 * Input:
 * task_handler: task control block of task
 * name:         name of the task
 * entry:        task body
 * parameter:    parameter of task body
 * stack_addr:   task stack start address
 * stack_size:   task stack size in byte
 * init_tick:    task time slice in tick
 * Output:
 * create result: 0 - ok
 *                1 - fail
 */
err_t task_create_static(p_tcb_t task_handler,
                         const char *name,
                         void (*entry) (void *parameter),
                         void *parameter,
                         uint8_t prio,
                         void *stack_addr,
                         uint32_t stack_size,
                         uint32_t init_tick);

/*
 * This function is used to create a task without given task stack.
 * Input:
 * task_handler: task control block of task
 * name:         name of the task
 * entry:        task body
 * parameter:    parameter of task body
 * stack_size:   task stack size in byte
 * init_tick:    task time slice in tick
 * Output:
 * create result: 0 - ok
 *                1 - fail
 */
err_t task_create(p_tcb_t task_handler,
                  const char *name,
                  void (*entry) (void *parameter),
                  void *parameter,
                  uint8_t prio,
                  uint32_t stack_size,
                  uint32_t init_tick);

/*
 * This function is used to insert the given task into task schedule list.
 * Input:
 * task_handler: handler of task
 * Output:
 * none
 */
void insert_task_to_list(p_tcb_t task_handler);

/*
 * This function is used to update task state
 * Input:
 * none
 * Output:
 * none
 */
void update_task_state(void);

/*
 * This function is used to schedule task.
 * Input:
 * none
 * Output:
 * none
 */
void task_schedule(void);

/*
 * This function is used to delay a task for given ticks.
 * Input:
 * tick: tick count to delay
 * Output:
 * none
 */
void task_delay(uint32_t tick);

/*
 * This function is used to show task info.
 * Input:
 * task_handler: task control block of task
 * Output:
 * none
 */
void show_task_info(p_tcb_t task_handler);

/*
 * This function is used to start os schedule
 * Input:
 * none
 * Output:
 * none
 */
void os_start_schedule(void);

/*
 * This function is used to get handler of current task.
 * Input:
 * none
 * Output:
 * current task handler
 */
p_tcb_t task_get_self(void);

#endif
