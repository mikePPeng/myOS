/*
 * Created by mikePPeng.
 * This is sample code for memory malloc and free.
 * Change Logs:
 * Date           Notes
 * Mar 17, 2021   the first version
 */

#include "kernel_inc/common.h"
#include "kernel_inc/task.h"

static int t1 = 1;
static int t2 = 2;

void *mem = NULL;

void mem_entry(void *parameter)
{
    int num = *(int *)parameter;

    while (1) {
        if (num == 1) {
            mem = os_malloc(0x1000);
            if (mem != NULL) {
                printf("0x%x bytes of memory is successfully malloced at %p!\r\n", num * 0x1000, mem);
            }
            memset(mem, 0x66, num * 0x1000);
        } else {
            os_free(mem);
            printf("memory at %p is successfully freed!\r\n", mem);
            mem = NULL;
        }

        task_delay(1000);
    }
}

void memory_sample_entry(void)
{
    if (heap_init() != ERR_OK) {
        printf("heap init failed!\r\n");
        return;
    }

    p_tcb_t task1_handler = (p_tcb_t)os_malloc(sizeof(tcb_t));
    p_tcb_t task2_handler = (p_tcb_t)os_malloc(sizeof(tcb_t));

    task_create(task1_handler, "task1", mem_entry, &t1, 1, 0x1000, 0xffffffff);
    task_create(task2_handler, "task2", mem_entry, &t2, 2, 0x1000, 0xffffffff);

    os_start_schedule();
}
