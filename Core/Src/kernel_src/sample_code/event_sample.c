/*
 * Created by mikePPeng.
 * This is sample code for event.
 * Change Logs:
 * Date           Notes
 * Mar 15, 2021   the first version
 */

#include "kernel_inc/ipc.h"
#include "kernel_inc/task.h"

event_t event;
uint32_t event1 = (1 << 3);
uint32_t event2 = (1 << 7);
uint32_t event3 = (1 << 10);

void send_entry(void *parameter)
{
    while (1) {
        event_send(&event, event1);
        task_delay(1000);
        event_send(&event, event2);
        task_delay(1000);
        event_send(&event, event3);
        task_delay(1000);
    }
}

void recv_entry(void *parameter)
{
    event_add(event1);
    event_add(event2);

    while (1) {
        int ret = event_wait(&event, EVENT_FLAG_AND | EVENT_FLAG_CLEAR, 1500);
        if (ret == ERR_OK) {
            printf("task wait for event success!\r\n");
            event_del(event1);
        } else {
            printf("task wait for event timeout!\r\n");
        }
    }
}

void event_sample_entry(void)
{
    if (heap_init() != ERR_OK) {
        printf("heap init failed!\r\n");
        return;
    }

    p_tcb_t task_send = (p_tcb_t)os_malloc(sizeof(tcb_t));
    p_tcb_t task_recv = (p_tcb_t)os_malloc(sizeof(tcb_t));

    task_create(task_send, "task_send", send_entry, NULL, 2, 0x500, 0xffffffff);
    task_create(task_recv, "task_recv", recv_entry, NULL, 1, 0x500, 0xffffffff);

    event_create(&event);

    os_start_schedule();
}
