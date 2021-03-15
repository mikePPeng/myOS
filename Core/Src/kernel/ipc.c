/*
 * Created by mikePPeng.
 * This file implements inter-process communication related APIs.
 * Change Logs:
 * Date           Notes
 * Mar 10, 2021   the first version
 */

#include "kernel/ipc.h"

/*
 * This function is used to add the given task into semaphore pending list.
 * Input:
 * sem_handler:  handler of semaphore
 * task_handler: handler of task
 * Output:
 * none
 */
void pend_list_add(p_sem_t sem_handler,
                   p_tcb_t task_handler)
{
    //first remove entry from schedule list
    list_del(&task_handler->list);
    task_handler->state = TASK_PENDING;

    //then add entry to pending list
    if (list_empty(&sem_handler->pend_list)) {
        list_add_before(&task_handler->list, &sem_handler->pend_list);
    } else {
        p_tcb_t itr, itr_next;
        list_for_each_entry_safe(itr, itr_next, &sem_handler->pend_list, list) {
            if (task_handler->prio <= itr->prio) {
                list_add_after(&task_handler->list, &sem_handler->pend_list);
                break;
            } else {
                if (&itr_next->list != &sem_handler->pend_list) {   //@itr is not the last entry
                    if (task_handler->prio < itr_next->prio) {
                        list_add_between(&task_handler->list, &itr->list, &itr_next->list);
                        break;
                    } else {
                        continue;
                    }
                } else {   //@itr is the last entry
                    list_add_before(&task_handler->list, &sem_handler->pend_list);
                    break;
                }
            }
        }
    }
}

void pend_list_del(p_tcb_t task_handler)
{
    //first remove entry from pending list
    list_del(&task_handler->list);

    //then add entry to scheduler list
    task_handler->state = TASK_READY;
    insert_task_to_list(task_handler);
}

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
                       uint32_t val)
{
    sem_handler->value = val;

    //initialize semaphore pending list
    sem_handler->pend_list.next = &sem_handler->pend_list;
    sem_handler->pend_list.prev = &sem_handler->pend_list;

    return ERR_OK;
}

void ipc_timer(void *parameter)
{
    //time is up, schedule current task anyway
    p_tcb_t cur_task = (p_tcb_t)parameter;
    cur_task->error = ERR_TIMEOUT;
    pend_list_del(cur_task);

    //do schedule
    task_schedule();
}

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
                    uint32_t time)
{
    if (sem_handler->value > 0) {
        uint32_t level = interrupt_disable();
        sem_handler->value--;
        interrupt_enable(level);
    } else {
        if (time == WAIT_NONE) {   //no wait time, return
            return ERR_TIMEOUT;
        }

        p_tcb_t cur_task = task_get_self();
        pend_list_add(sem_handler, cur_task);
        cur_task->soft_timer.timeout_func = NULL;

        if (time != WAIT_FOREVER) {
            soft_timer_create(&cur_task->soft_timer,
                              cur_task->name,
                              ipc_timer,
                              cur_task,
                              time,
                              TYPE_ONESHOT);
            soft_timer_start(&cur_task->soft_timer);
        }

        //do schedule
        task_schedule();

        return cur_task->error;
    }
    return ERR_OK;
}

/*
 * This function is used to release the given semaphore.
 * Input:
 * sem_handler: semaphore handler
 * Output:
 * result:      0 - ok
 *              1 - fail
 */
err_t semaphore_release(p_sem_t sem_handler)
{
    if (!list_empty(&sem_handler->pend_list)) {
        //the first entry of pending list
        p_tcb_t pend_task = list_entry(sem_handler->pend_list.next, typeof(tcb_t), list);

        //stop software timer
        if (pend_task->soft_timer.timeout_func != NULL) {
            soft_timer_stop(&pend_task->soft_timer);
            pend_task->error = ERR_OK;
        }

        pend_list_del(pend_task);

        //do schedule
        task_schedule();
    } else {
        uint32_t level = interrupt_disable();
        sem_handler->value++;
        interrupt_enable(level);
    }
    return ERR_OK;
}

/*
 * This function is used to create a mutex.
 * Input:
 * mutex_handler: handler of mutex
 * Output:
 * create result: 0 - ok
 *                1 - fail
 */
err_t mutex_create(p_mutex_t mutex_handler)
{
    mutex_handler->recursive_time = 0;
    mutex_handler->origin_prio = 0xff;
    mutex_handler->owner = NULL;

    mutex_handler->sem.value = 1;
    //initialize semaphore pending list
    mutex_handler->sem.pend_list.next = &mutex_handler->sem.pend_list;
    mutex_handler->sem.pend_list.prev = &mutex_handler->sem.pend_list;

    return ERR_OK;
}

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
                 uint32_t time)
{
    p_tcb_t cur_task = task_get_self();

    if (mutex_handler->sem.value == 1) {   //mutex is available
        uint32_t level = interrupt_disable();
        mutex_handler->sem.value--;
        mutex_handler->owner = cur_task;
        mutex_handler->origin_prio = cur_task->prio;
        mutex_handler->recursive_time++;
        interrupt_enable(level);
    } else {   //mutex is not available
        if (cur_task == mutex_handler->owner) {   //recursive
            mutex_handler->recursive_time++;
        } else {   //other tasks
            if (time == WAIT_NONE) {   //no wait time, return error
                return ERR_TIMEOUT;
            }

            //add to pending list
            pend_list_add(&mutex_handler->sem, cur_task);
            cur_task->soft_timer.timeout_func = NULL;

            //prevent priority reverse
            p_tcb_t first_entry = list_entry(mutex_handler->sem.pend_list.next, typeof(tcb_t), list);
            if (mutex_handler->owner->prio > first_entry->prio) {
                list_del(&mutex_handler->owner->list);
                mutex_handler->owner->prio = first_entry->prio;
                insert_task_to_list(mutex_handler->owner);
            }

            if (time != WAIT_FOREVER) {
                soft_timer_create(&cur_task->soft_timer,
                                  cur_task->name,
                                  ipc_timer,
                                  cur_task,
                                  time,
                                  TYPE_ONESHOT);
                soft_timer_start(&cur_task->soft_timer);
            }

            //do schedule
            task_schedule();

            return cur_task->error;
        }
    }
    return ERR_OK;
}

/*
 * This function is used to release the given mutex.
 * Input:
 * mutex_handler: mutex handler
 * Output:
 * result:        0 - ok
 *                1 - fail
 */
err_t mutex_release(p_mutex_t mutex_handler)
{
    p_tcb_t cur_task = task_get_self();

    //only owner of mutex can release
    if (cur_task != mutex_handler->owner) {
        return ERR_FAIL;
    }

    mutex_handler->recursive_time--;

    if (mutex_handler->recursive_time == 0) {
        if (!list_empty(&mutex_handler->sem.pend_list)) {   //pend list is not empty
            p_tcb_t pend_task = list_entry(mutex_handler->sem.pend_list.next, typeof(tcb_t), list);

            //stop software timer
            if (pend_task->soft_timer.timeout_func != NULL) {
                soft_timer_stop(&pend_task->soft_timer);
                pend_task->error = ERR_OK;
            }

            pend_list_del(pend_task);

            //reset previous owner priority to original
            if (mutex_handler->owner->prio != mutex_handler->origin_prio) {
                list_del(&mutex_handler->owner->list);
                mutex_handler->owner->prio = mutex_handler->origin_prio;
                insert_task_to_list(mutex_handler->owner);
            }

            //change owner of mutex
            mutex_handler->owner = pend_task;
            mutex_handler->origin_prio = pend_task->prio;

            //when schedule to next task, pc reaches the end of @mutex_take(), so we have to add here
            mutex_handler->recursive_time++;

            //do schedule
            task_schedule();
        } else {   //pend list empty
            uint32_t level = interrupt_disable();
            mutex_handler->sem.value++;
            mutex_handler->origin_prio = 0xff;
            mutex_handler->owner = NULL;
            interrupt_enable(level);
        }
    }
    return ERR_OK;
}

/*
 * This function is used to create a event.
 * Input:
 * event_handler: handler of event
 * Output:
 * create result: 0 - ok
 *                1 - fail
 */
err_t event_create(p_event_t event_handler)
{
    event_handler->event.value = 0;
    event_handler->event.pend_list.next = &event_handler->event.pend_list;
    event_handler->event.pend_list.prev = &event_handler->event.pend_list;

    return ERR_OK;
}

/*
 * This function is used to add event to task.
 * Input:
 * event: event set to add
 * Output:
 * create result: 0 - ok
 *                1 - fail
 */
err_t event_add(uint32_t event)
{
    p_tcb_t cur_task = task_get_self();
    cur_task->event |= event;

    return ERR_OK;
}

/*
 * This function is used to delete event from task.
 * Input:
 * event: event set to delete
 * Output:
 * create result: 0 - ok
 *                1 - fail
 */
err_t event_del(uint32_t event)
{
    p_tcb_t cur_task = task_get_self();
    cur_task->event &= ~event;

    return ERR_OK;
}

/*
 * This function is used to wait event.
 * Input:
 * event_handler: event handler
 * flag:          event type
 * time:          time in tick to wait for the event
 * Output:
 * result:        0 - ok
 *                1 - fail
 *                2 - timeout
 */
err_t event_wait(p_event_t     event_handler,
                 event_flag_t  flag,
                 uint32_t      time)
{
    uint32_t ret = ERR_FAIL;
    p_tcb_t cur_task = task_get_self();
    cur_task->event_flag = flag;

    if (flag & EVENT_FLAG_AND) {
        if ((cur_task->event & event_handler->event.value) != cur_task->event) {
            //no wait time, return timeout
            if (time == WAIT_NONE) {
                return ERR_TIMEOUT;
            }

            //add current task to pending list
            pend_list_add(&event_handler->event, cur_task);
            cur_task->soft_timer.timeout_func = NULL;

            //add software timer
            if (time != WAIT_FOREVER) {
                soft_timer_create(&cur_task->soft_timer,
                                  cur_task->name,
                                  ipc_timer,
                                  cur_task,
                                  time,
                                  TYPE_ONESHOT);
                soft_timer_start(&cur_task->soft_timer);
            }

            //do schedule
            task_schedule();

            ret = cur_task->error;
        } else {   //cur_task->event & event_handler->event.value == cur_task->event
            ret = ERR_OK;
        }
    }

    if (flag & EVENT_FLAG_OR) {
        if (!(cur_task->event & event_handler->event.value)) {
            //no wait time, return timeout
            if (time == WAIT_NONE) {
                return ERR_TIMEOUT;
            }

            //add current task to pending list
            pend_list_add(&event_handler->event, cur_task);
            cur_task->soft_timer.timeout_func = NULL;

            //add software timer
            if (time != WAIT_FOREVER) {
                soft_timer_create(&cur_task->soft_timer,
                                  cur_task->name,
                                  ipc_timer,
                                  cur_task,
                                  time,
                                  TYPE_ONESHOT);
                soft_timer_start(&cur_task->soft_timer);
            }

            //do schedule
            task_schedule();

            ret = cur_task->error;
        } else {   //cur_task->event & event_handler->event.value == 1
            ret = ERR_OK;
        }
    }

    if (flag & EVENT_FLAG_CLEAR) {
        if (ret == ERR_OK) {
            event_handler->event.value &= ~cur_task->event;
        }
    }

    return ret;
}

/*
 * This function is used to send event.
 * Input:
 * event_handler: event handler
 * evet:          event to send
 * Output:
 * none
 */
void event_send(p_event_t event_handler,
                 uint32_t  event)
{
    event_handler->event.value |= event;

    p_tcb_t itr, itr_next;
    list_for_each_entry_safe(itr, itr_next, &event_handler->event.pend_list, list) {
        if (itr->event_flag & EVENT_FLAG_AND) {
            if ((itr->event & event_handler->event.value) == itr->event) {
                if (itr->soft_timer.timeout_func != NULL) {
                    soft_timer_stop(&itr->soft_timer);
                    itr->error = ERR_OK;
                }

                pend_list_del(itr);
                continue;
            }
        }

        if (itr->event_flag & EVENT_FLAG_OR) {
            if (itr->event & event_handler->event.value) {
                if (itr->soft_timer.timeout_func != NULL) {
                    soft_timer_stop(&itr->soft_timer);
                    itr->error = ERR_OK;
                }

                pend_list_del(itr);
                continue;
            }
        }
    }

    //do schedule
    task_schedule();
}
