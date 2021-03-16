/*
 * Created by mikePPeng.
 * This file declares inter-process communication related APIs.
 * Change Logs:
 * Date           Notes
 * Mar 10, 2021   the first version
 * Mar 16, 2021   add message queue
 */

#ifndef __IPC_H__
#define __IPC_H__

#include <stdint.h>
#include "kernel/common.h"
#include "kernel/task.h"
#include "kernel/soft_timer.h"
#include "kernel/interrupt.h"

typedef struct semaphore {
    uint32_t         value;
    struct list_head pend_list;
} sem_t, *p_sem_t;

typedef struct mutex {
    uint8_t      origin_prio;
    p_tcb_t      owner;
    uint32_t     recursive_time;
    sem_t        sem;
} mutex_t, *p_mutex_t;

typedef struct event {
    uint32_t         bit_table;
    struct list_head pend_list;
} event_t, *p_event_t;

typedef struct msg {
    void            *data;
    uint32_t         size;
    struct list_head list;
} msg_t, *p_msg_t;

typedef struct msg_queue {
    struct list_head msg_list;
    struct list_head pend_list;
} mq_t, *p_mq_t;

typedef enum ipc_wait_time {
    WAIT_NONE = 0,
    WAIT_FOREVER = 0xFFFFFFFFU,
} wait_t;

typedef enum event_flag {
    EVENT_FLAG_AND = 0x01,
    EVENT_FLAG_OR = 0x02,
    EVENT_FLAG_CLEAR = 0x04,
} event_flag_t;

typedef enum msg_urgency {
    MSG_NORMAL = 0x0,
    MSG_URGENT,
} urgency_t;

/*
 * This function is used to create a semaphore.
 * Input:
 * sem_handler:   handler of semaphore
 * val:           init value of semaphore
 * Output:
 * create result: 0 - ok
 *                1 - fail
 */
err_t semaphore_create(p_sem_t sem_handler,
                       uint32_t val);

/*
 * This function is used to take the given semaphore.
 * Input:
 * sem_handler: semaphore handler
 * time:        time in tick to wait for the semaphore
 * Output:
 * result:      0 - ok
 *              1 - fail
 *              2 - timeout
 */
err_t semaphore_take(p_sem_t sem_handler,
                    uint32_t time);

/*
 * This function is used to release the given semaphore.
 * Input:
 * sem_handler: semaphore handler
 * Output:
 * result:      0 - ok
 *              1 - fail
 */
err_t semaphore_release(p_sem_t sem_handler);

/*
 * This function is used to create a mutex.
 * Input:
 * mutex_handler: handler of mutex
 * Output:
 * create result: 0 - ok
 *                1 - fail
 */
err_t mutex_create(p_mutex_t mutex_handler);

/*
 * This function is used to take the given mutex.
 * Input:
 * mutex_handler: mutex handler
 * time:          time in tick to wait for the mutex
 * Output:
 * result:      0 - ok
 *              1 - fail
 *              2 - timeout
 */
err_t mutex_take(p_mutex_t mutex_handler,
                 uint32_t time);

/*
 * This function is used to release the given mutex.
 * Input:
 * mutex_handler: mutex handler
 * Output:
 * result:        0 - ok
 *                1 - fail
 */
err_t mutex_release(p_mutex_t mutex_handler);

/*
 * This function is used to create a event.
 * Input:
 * event_handler: handler of event
 * Output:
 * create result: 0 - ok
 *                1 - fail
 */
err_t event_create(p_event_t event_handler);

/*
 * This function is used to add event to task.
 * Input:
 * event: event set to wait
 * Output:
 * create result: 0 - ok
 *                1 - fail
 */
err_t event_add(uint32_t event);

/*
 * This function is used to delete event from task.
 * Input:
 * event: event set to delete
 * Output:
 * create result: 0 - ok
 *                1 - fail
 */
err_t event_del(uint32_t event);

/*
 * This function is used to receive event.
 * Input:
 * event_handler: event handler
 * flag:          event type
 * time:          time in tick to wait for the event
 * Output:
 * result:      0 - ok
 *              1 - fail
 *              2 - timeout
 */
err_t event_wait(p_event_t     event_handler,
                 event_flag_t  flag,
                 uint32_t      time);

/*
 * This function is used to send event.
 * Input:
 * event_handler: event handler
 * evet:          event to send
 * Output:
 * none
 */
void event_send(p_event_t event_handler,
                uint32_t  event);

/*
 * This function is used to create a message queue.
 * Input:
 * msg_handler: handler of message queue
 * Output:
 * create result: 0 - ok
 *                1 - fail
 */
err_t msg_queue_create(p_mq_t msg_handler);

/*
 * This function is used to send message at the end of message queue.
 * Input:
 * msg_handler: handler of message queue
 * buf:         address of send message
 * size:        size of send message
 * urgent:      urgency of message
 * Output:
 * result:      0 - ok
 *              1 - fail
 */
err_t msg_queue_send(p_mq_t msg_handler,
                     void *buf,
                     uint32_t size,
                     urgency_t urgent);

/*
 * This function is used to receive message from message queue.
 * Input:
 * msg_handler: handler of message queue
 * buf:         address of receive buffer
 * size:        size of receive buffer
 * time:        wait time if message queue is empty
 * Output:
 * result:      0 - ok
 *              1 - fail
 *              2 - timeout
 */
err_t msg_queue_recv(p_mq_t msg_handler,
                     void *buf,
                     uint16_t size,
                     uint32_t time);

#endif
