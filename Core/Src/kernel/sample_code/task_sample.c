/*
 * Created by mikePPeng.
 * This is a sample code of task.
 * Change Logs:
 * Date           Notes
 * Mar 2, 2021   the first version
 */

#include "kernel/task.h"

static int t1 = 1;
static int t2 = 2;
static int t3 = 3;

void task_entry(void *para)
{
    int num = *(int *)para;
    while (1) {
        printf("this is task %d\r\n", num);
        show_task_info(task_get_self());
        task_delay(num * 1000);
    }
}

void dummy_entry(void *para)
{
    unsigned int i = 0;
    while (1) {
        if (i++ % 0x200000 == 0) {
            printf("holding cpu...\r\n");
        }
    }
}

void task_sample_entry(void)
{
    p_tcb_t task1_handler = (p_tcb_t)malloc(sizeof(tcb_t));
    p_tcb_t task2_handler = (p_tcb_t)malloc(sizeof(tcb_t));
    p_tcb_t task3_handler = (p_tcb_t)malloc(sizeof(tcb_t));

    task_create(task1_handler, "task1", task_entry, &t1, 1, 0x200, 0xffffffff);
    task_create(task2_handler, "task2", task_entry, &t2, 2, 0x200, 0xffffffff);
    task_create(task3_handler, "task3", task_entry, &t3, 3, 0x200, 0xffffffff);

    os_start_schedule();
}
