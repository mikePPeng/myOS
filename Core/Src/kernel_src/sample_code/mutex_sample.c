/*
 * Created by mikePPeng.
 * This is a sample code for mutex.
 * Change Logs:
 * Date           Notes
 * Mar 15, 2021   the first version
 */

#include "kernel_inc/ipc.h"
#include "kernel_inc/task.h"

mutex_t lock;
int num1 = 0;
int num2 = 0;

void high_entry(void *parameter)
{
    while (1) {
        printf("high priority is %u.\r\n", task_get_self()->prio);
        //let low priority task schedule first
        task_delay(1);

        mutex_take(&lock, WAIT_FOREVER);

        if (num1 == num2) {
            printf("num1 = num2 = %d.\r\n", num1);
            printf("num1 and num2 is successfully protected by mutex.\r\n");
        } else {
            printf("mutex protect fail.\r\n");
        }
        mutex_release(&lock);

        task_delay(2000);
    }
}

void low_entry(void *parameter)
{

    while (1) {
        unsigned int i = 0;
        //volatile uint8_t cur_prio = task_get_self()->prio;
        printf("low priority is %u before taking mutex.\r\n", (volatile uint8_t)task_get_self()->prio);

        mutex_take(&lock, WAIT_FOREVER);

        num1++;
        //hold cpu for a while
        while (i++ < 0x500000);
        num2++;

        printf("low priority is %u after taking mutex.\r\n", (volatile uint8_t)task_get_self()->prio);

        mutex_release(&lock);

        printf("low priority is %u after releasing mutex.\r\n", (volatile uint8_t)task_get_self()->prio);

        task_delay(1000);

    }
}

void mutex_sample_entry(void)
{
    if (heap_init() != ERR_OK) {
        printf("heap init failed!\r\n");
        return;
    }

    p_tcb_t task_high = (p_tcb_t)os_malloc(sizeof(tcb_t));
    p_tcb_t task_low = (p_tcb_t)os_malloc(sizeof(tcb_t));

    task_create(task_high, "task_high", high_entry, NULL, 2, 0x500, 0xffffffff);
    task_create(task_low, "task_low", low_entry, NULL, 3, 0x500, 0xffffffff);

    mutex_create(&lock);

    os_start_schedule();
}
