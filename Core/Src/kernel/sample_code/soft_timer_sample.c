/*
 * Created by mikePPeng.
 * This is sample code of software timer.
 * Change Logs:
 * Date           Notes
 * Mar 9, 2021   the first version
 */

#include "kernel/soft_timer.h"
#include "kernel/task.h"

static int t1 = 1;
static int t2 = 2;
static int t3 = 3;

soft_timer_t timer1;
soft_timer_t timer2;
soft_timer_t timer3;

void timeout_func(void *parameter)
{
    int num = *(int *)parameter;
    if (num == 1) {
        printf("this is timer1!\r\n");
        soft_timer_create(&timer2, "timer2", timeout_func, &t2, 1000, TYPE_REPEAT);
        soft_timer_create(&timer3, "timer3", timeout_func, &t3, 2000, TYPE_REPEAT);
        soft_timer_start(&timer2);
        soft_timer_start(&timer3);
    }
    if (num == 2) {
        printf("this is timer2!\r\n");
    }
    if (num == 3) {
        printf("this is timer3!\r\n");
    }
}
void soft_timer_sample_entry(void)
{
    soft_timer_create(&timer1, "timer1", timeout_func, &t1, 1000, TYPE_ONESHOT);
    soft_timer_start(&timer1);

    os_start_schedule();
}
