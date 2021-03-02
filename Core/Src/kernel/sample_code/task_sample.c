/*
 * Created by mikePPeng.
 * This is a sample code of task.
 * Change Logs:
 * Date           Notes
 * Mar 2, 2021   the first version
 */

#include "kernel/task.h"

void task_entry(void *para)
{
    int num = *(int *)para;
    while (1) {
        printf("this is task %d\r\n", num);
        //HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_10);
        show_task_info(task_get_self());
        task_delay(1000);
    }
}

void dummy_entry(void *para)
{
    while (1) {
        printf("holding cpu...\r\n");
    }
}

void task_sample_entry(void)
{
    tcb_t task1_handler;
    tcb_t task2_handler;
    tcb_t task3_handler;
    int t1 = 1;
    int t2 = 2;
    int t3 = 3;

    task_create(&task1_handler, "task1", task_entry, &t1, 0x500, 0xffffffff);
    task_create(&task2_handler, "task2", task_entry, &t2, 0x500, 0xffffffff);
    task_create(&task3_handler, "task3", task_entry, &t3, 0x500, 0xffffffff);
    idle_task_create();

    os_start_schedule();
}
