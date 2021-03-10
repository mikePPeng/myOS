/*
 * Created by mikePPeng.
 * This file contains task related APIs of myOS.
 * Change Logs:
 * Date           Notes
 * Feb 24, 2021   the first version
 * Mar  3, 2021   add priority to task
 */

#include "kernel/task.h"

static p_tcb_t g_cur_task = NULL;
static p_tcb_t g_next_task = NULL;
static tcb_t g_idle_handle;

list_head_init(g_prio_list_head);

p_prio_list_t create_prio_list_entry(p_tcb_t task_handler)
{
    p_prio_list_t prio_list = (p_prio_list_t)malloc(sizeof(prio_list_t));

    prio_list->prio = task_handler->prio;

    //initialize task list head
    prio_list->task_list_head.next = &prio_list->task_list_head;
    prio_list->task_list_head.prev = &prio_list->task_list_head;

    return prio_list;
}

void insert_task_to_list(p_tcb_t task_handler)
{
    if (list_empty(&g_prio_list_head)) {
        p_prio_list_t prio_list = NULL;
        prio_list = create_prio_list_entry(task_handler);

        list_add_before(&prio_list->list, &g_prio_list_head);               //deal priority list
        list_add_before(&task_handler->list, &prio_list->task_list_head);   //deal task list of this priority

    } else {     //list not empty
        p_prio_list_t itr, itr_next;

        list_for_each_entry_safe(itr, itr_next, &g_prio_list_head, list) {

            if (task_handler->prio == itr->prio) {
                list_add_before(&task_handler->list, &itr->task_list_head);   //deal task list of this priority
                break;
            } else if (task_handler->prio < itr->prio) {
                p_prio_list_t prio_list = NULL;
                prio_list = create_prio_list_entry(task_handler);

                list_add_after(&prio_list->list, &g_prio_list_head);                //deal priority list
                list_add_before(&task_handler->list, &prio_list->task_list_head);   //deal task list of this priority
                break;
            } else {  //current priority value is greater than @itr->prio

                if (&itr_next->list != &g_prio_list_head) {   //@itr is not the last entry of the list

                    if (task_handler->prio < itr_next->prio) {
                        p_prio_list_t prio_list = NULL;
                        prio_list = create_prio_list_entry(task_handler);

                        list_add_between(&prio_list->list, &itr->list, &itr_next->list);    //deal priority list
                        list_add_before(&task_handler->list, &prio_list->task_list_head);   //deal task list of this priority
                        break;
                    } else {
                        continue;
                    }
                 } else {   //@itr is the last entry of list
                     p_prio_list_t prio_list = NULL;
                     prio_list = create_prio_list_entry(task_handler);

                     list_add_before(&prio_list->list, &g_prio_list_head);             //deal priority list
                     list_add_before(&task_handler->list, &prio_list->task_list_head); //deal task list of this priority
                     break;
                 }
            }
        }
    }
}

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
                         uint32_t init_tick)
{
    //initialize tcb
    strncpy(task_handler->name, name, NAME_MAX_LEN);
    task_handler->entry = (void *)entry;
    task_handler->parameter = parameter;
    task_handler->prio = prio;
    task_handler->stack_addr = stack_addr;
    task_handler->stack_size = stack_size;
    task_handler->init_tick = init_tick;
    task_handler->init_tick_left = init_tick;
    task_handler->state = TASK_READY;

    uint32_t level = interrupt_disable();
    insert_task_to_list(task_handler);
    interrupt_enable(level);

    task_handler->sp = (uint32_t *)((uint32_t)stack_addr + stack_size);

    //initialize task stack, which is organized in Full Descending manner in cortex m3/m4
    //xpsr
    (task_handler->sp)--;
    *(task_handler->sp) = 0x01000000;//T bit always equals 1

    //pc
    (task_handler->sp)--;
    *(task_handler->sp) = (uint32_t)task_handler->entry;

    //lr
    (task_handler->sp)--;
    *(task_handler->sp) = 0xFFFFFFFD;//return to thread mode using psp, so that another task can be schedule

    //r12, r3 ~ r0, r11 ~ r4
    int i;
    for (i = 0; i < 13; i++) {
        (task_handler->sp)--;
        if (i == 4) {
            //put parameter into r0
            *(task_handler->sp) = (uint32_t)parameter;
        } else {
            *(task_handler->sp) = 0;
        }

    }

    return ERR_OK;
}

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
                  uint32_t init_tick)
{
    void *stack_addr = (void *)malloc(stack_size);
    return task_create_static(task_handler,
                              name,
                              entry,
                              parameter,
                              prio,
                              stack_addr,
                              stack_size,
                              init_tick);
}

void idle_entry(void *para)
{
    while (1);
}

/*
 * This function is used to create the idle task.
 * Input:
 * none
 * Output:
 * none
 */
err_t idle_task_create(void)
{
    void *idle_stack = (void *)malloc(IDLE_STACK_SIZE);
    g_cur_task = &g_idle_handle;
    return task_create_static(&g_idle_handle,
                              "idle_task",
                              idle_entry,
                              NULL,
                              255,
                              idle_stack,
                              IDLE_STACK_SIZE,
                              1);
}

/*
 * This function is used to get psp of next task.
 * Input:
 * none
 * Output:
 * next psp
 */
uint32_t *get_next_psp(void)
{
    return g_next_task->sp;
}

/*
 * This function is used to get psp of current task.
 * Input:
 * none
 * Output:
 * next psp
 */
uint32_t *get_current_psp(void)
{
    return g_cur_task->sp;
}

/*
 * This function is used to save psp of current task.
 * Input:
 * sp: current psp
 * Output:
 * none
 */
void save_current_psp(uint32_t *sp)
{
    g_cur_task->sp = sp;
}

/*
 * This function is used to update current task with next task after context switching.
 * Input:
 * none
 * Output:
 * none
 */
void update_cur_with_next(void)
{
    g_cur_task = g_next_task;
}

/*
 * This function is used to get the next task to schedule.
 * Input:
 * none
 * Output:
 * none
 */
void get_next_task(void)
{
    //for now no priority is implemented
    p_prio_list_t prio_itr = NULL;

    list_for_each_entry(prio_itr, &g_prio_list_head, list) {
        //find the first non-pending task to schedule
        p_tcb_t itr = NULL;
        list_for_each_entry(itr, &prio_itr->task_list_head, list) {
            if (itr->state != TASK_PENDING) {
                if (itr != g_cur_task) {
                    itr->state = TASK_RUNNING;
                    g_next_task = itr;
                    if (g_cur_task->state == TASK_RUNNING) {   //current task is preempted
                        g_cur_task->state = TASK_READY;
                    }
                } else {
                    g_next_task = g_cur_task;
                }

                return;
            }
        }

    }
}

/*
 * This function is used to schedule task.
 * Input:
 * none
 * Output:
 * none
 */
void task_schedule(void)
{
    uint32_t level = interrupt_disable();
    get_next_task();
    interrupt_enable(level);

    if (g_next_task != g_cur_task) {
        //pend the pendSV exception
        uint32_t *pICSR = (uint32_t *)0xE000ED04; //address of ICSR
        *pICSR |= (1 << 28); //set the 28th bit, which is PENDSVSET
    }

}

/*
 * This function is used to delay a task for given ticks.
 * Input:
 * tick: tick count to delay
 * Output:
 * none
 */
void task_delay(uint32_t tick)
{
    g_cur_task->delay_tick = tick;
    g_cur_task->delay_tick_left = tick;
    g_cur_task->state = TASK_PENDING;
    task_schedule();
}

/*
 * This function is used to show task info.
 * Input:
 * task_handler: task control block of task
 * Output:
 * none
 */
void show_task_info(p_tcb_t task_handler)
{
    char str[10] = {0};
    if (task_handler->state == TASK_READY) {
        strcpy(str, "ready");
    } else if (task_handler->state == TASK_PENDING) {
        strcpy(str, "pending");
    } else if (task_handler->state == TASK_RUNNING) {
        strcpy(str, "running");
    } else {
        strcpy(str, "undefined");
    }
    printf("task info:\r\n");
    printf("task name: %-20s\r\n", task_handler->name);
    printf("task entry addr: %10p | task parameter addr: %10p\r\n", task_handler->entry, task_handler->parameter);
    printf("task stack addr: %10p | task stack size: %14lu\r\n", task_handler->stack_addr, task_handler->stack_size);
    printf("task state: %15s | task psp: %21p\r\n", str, task_handler->sp);
}

/*
 * This function is used to update task state
 * Input:
 * none
 * Output:
 * none
 */
void update_task_state(void)
{
    p_prio_list_t prio_itr = NULL;
    list_for_each_entry(prio_itr, &g_prio_list_head, list) {
        p_tcb_t itr = NULL;
        p_tcb_t t_itr = NULL;

        list_for_each_entry_safe(itr, t_itr, &prio_itr->task_list_head, list) {
            if (itr->state == TASK_RUNNING) {
                itr->init_tick_left--;

                if (itr->init_tick_left == 0) {
                    //time silce use up, reset time slice, remove to the end of list and ready to be scheduled
                    itr->init_tick_left = itr->init_tick;
                    itr->state = TASK_READY;
                    list_del(&itr->list);
                    list_add_before(&itr->list, &prio_itr->task_list_head);
                }
                continue;
            }

            if (itr->state == TASK_PENDING) {
                itr->delay_tick_left--;

                if (itr->delay_tick_left == 0) {
                    //set the task ready to be scheduled, @delay_tick_left will be updated in task_delay().
                    itr->state = TASK_READY;
                    list_del(&itr->list);
                    list_add_before(&itr->list, &prio_itr->task_list_head);
                }
                continue;
            }
        }
    }
}

__attribute__((naked)) void switch_msp_to_psp(void)
{
    //initialize psp
    __asm volatile("PUSH {LR}");
    __asm volatile("BL get_current_psp");
    __asm volatile("MSR PSP, R0");
    __asm volatile("POP {LR}");

    //switch from msp to psp
    __asm volatile("MOV R0, 0X02");
    __asm volatile("MSR CONTROL, R0"); //write CONTROL register, SPSEL = 1
    __asm volatile("BX LR");
}

/*
 * This function is used to start os schedule
 * Input:
 * none
 * Output:
 * none
 */
void os_start_schedule(void)
{
    //initialize
    idle_task_create();

    switch_msp_to_psp();

    //schedule
    g_cur_task->state = TASK_RUNNING;
    ((void (*) (void *))g_cur_task->entry)(g_cur_task->parameter);
}

/*
 * This function is used to get handler of current task.
 * Input:
 * none
 * Output:
 * current task handler
 */
p_tcb_t task_get_self(void)
{
    return g_cur_task;
}


