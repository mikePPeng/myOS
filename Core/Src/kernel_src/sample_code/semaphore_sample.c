/*
 * Created by mikePPeng.
 * This is a sample code for semaphore.
 * Change Logs:
 * Date           Notes
 * Mar 11, 2021   the first version
 */

#include "kernel_inc/ipc.h"
#include "kernel_inc/task.h"

#define MAX_LEN 5
int array[MAX_LEN] = {0};
int ind = 0;

sem_t sem_empty;
sem_t sem_occupy;
sem_t sem_lock;

void producer_entry(void *parameter)
{
    while (1) {
        //producer take empty
        if (ERR_OK == semaphore_take(&sem_empty, WAIT_FOREVER)) {

            //lock and modify global array
            if (ERR_OK == semaphore_take(&sem_lock, WAIT_FOREVER)) {
                array[ind] = ind;
                printf("producer index is %d, producer put number %d into array.\r\n", ind, array[ind]);
                ind++;

                //unlock
                semaphore_release(&sem_lock);
            }
            //producer release occupy
            semaphore_release(&sem_occupy);
        }
        task_delay(2000);
    }
}

void consumer_entry(void *parameter)
{
    while (1) {
        //consumer take occupy
        if (ERR_OK == semaphore_take(&sem_occupy, 500)) {
            //lock and modify global array
            if (ERR_OK == semaphore_take(&sem_lock, WAIT_FOREVER)) {
                ind--;
                int num = array[ind];
                printf("consumer index is %d, consumer take number %d from array.\r\n", ind, num);


                //unlock
                semaphore_release(&sem_lock);
            }
            //producer release empty
            semaphore_release(&sem_empty);
        } else {
            printf("consumer wait for occupy timeout!\r\n");
        }
        task_delay(3000);
    }
}

void semaphore_sample_entry(void)
{
    if (heap_init() != ERR_OK) {
        printf("heap init failed!\r\n");
        return;
    }

    p_tcb_t producer_task = (p_tcb_t)os_malloc(sizeof(tcb_t));
    p_tcb_t consumer_task = (p_tcb_t)os_malloc(sizeof(tcb_t));

    task_create(producer_task, "producer", producer_entry, NULL, 3, 0x500, 0xffffffff);
    task_create(consumer_task, "consumer", consumer_entry, NULL, 1, 0x500, 0xffffffff);

    semaphore_create(&sem_empty, MAX_LEN);
    semaphore_create(&sem_occupy, 0);
    semaphore_create(&sem_lock, 1);

    os_start_schedule();
}
