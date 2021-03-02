/*
 * Created by mikePPeng.
 * This file declares the task related APIs of myOS.
 * Change Logs:
 * Date           Notes
 * Feb 23, 2021   the first version
 */

#ifndef __TASK_H__
#define __TASK_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "kernel/common.h"

#define NAME_MAX_LEN 20
#define IDLE_STACK_SIZE 200

typedef enum task_state {
    TASK_READY,
    TASK_RUNNING,
    TASK_PENDING,
} task_state;

typedef struct task_control_block {
    char           name[NAME_MAX_LEN];
    uint32_t      *sp;
    void          *entry;
    void          *parameter;
    task_state     state;

    void          *stack_addr;
    uint32_t       stack_size;

    //used for task delay
    uint32_t       delay_tick;
    uint32_t       delay_tick_left;

    //used for time slice
    uint32_t       init_tick;
    uint32_t       init_tick_left;

    struct list_head list;
} tcb_t, *p_tcb_t;

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
                  uint32_t stack_size,
                  uint32_t init_tick);

/*
 * This function is used to get the next task to schedule.
 * Input:
 * none
 * Output:
 * none
 */
void get_next_task(void);

/*
 * This function is used to get psp of next task.
 * Input:
 * none
 * Output:
 * next psp
 */
uint32_t *get_next_psp(void);

/*
 * This function is used to save psp of current task.
 * Input:
 * sp: current psp
 * Output:
 * none
 */
void save_current_psp(uint32_t *sp);

/*
 * This function is used to update current task with next task after context switching.
 * Input:
 * none
 * Output:
 * none
 */
void update_cur_with_next(void);

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
 * This function is used to create the idle task.
 * Input:
 * none
 * Output:
 * none
 */
err_t idle_task_create(void);

/*
 * This function is used to update task state
 * Input:
 * none
 * Output:
 * none
 */
void update_task_state(void);

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
