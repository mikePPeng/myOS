/*
 * Created by mikePPeng.
 * This is sample code for message queue.
 * Change Logs:
 * Date           Notes
 * Mar 16, 2021   the first version
 */

#include "kernel/ipc.h"
#include "kernel/task.h"

#define MSG_SIZE 64
mq_t msg_queue;

void msg_send_entry(void *parameter)
{
    unsigned int i = 1;
    while (1) {
        //after four normal message, send one urgent messsage
        if (i++ % 5 != 0) {
            char msg1[MSG_SIZE] = "This is a normal message.\r\n";
            msg_queue_send(&msg_queue, msg1, MSG_SIZE, MSG_NORMAL);
            task_delay(1000);
        } else {
            char msg2[MSG_SIZE] = "This is an urgent message!\r\n";
            msg_queue_send(&msg_queue, msg2, MSG_SIZE, MSG_URGENT);
            task_delay(1000);
        }
    }
}

void msg_recv_entry(void *parameter)
{
    unsigned int i = 1;
    while (1) {
        char recv_msg[MSG_SIZE] = {0};

        //receive message for every two seconds
        if (msg_queue_recv(&msg_queue, recv_msg, MSG_SIZE, 500) == ERR_OK) {
            printf("message No.%d is: %s\r\n", i++, recv_msg);
        } else {
            printf("message recv timeout!\r\n");
        }

        task_delay(2000);
    }
}

void msg_queue_sample_entry(void)
{
    p_tcb_t task_send = (p_tcb_t)malloc(sizeof(tcb_t));
    p_tcb_t task_recv = (p_tcb_t)malloc(sizeof(tcb_t));

    task_create(task_send, "task_send", msg_send_entry, NULL, 2, 0x500, 0xffffffff);
    task_create(task_recv, "task_recv", msg_recv_entry, NULL, 2, 0x500, 0xffffffff);

    msg_queue_create(&msg_queue);

    os_start_schedule();
}
