/*
 * Created by mikePPeng.
 * This file contains common APIs used in myOS.
 * Change Logs:
 * Date           Notes
 * Mar 17, 2021   the first version
 */

#include <stdarg.h>
#include "kernel_inc/common.h"
#include "kernel_inc/ipc.h"

uint32_t heap_start;
uint32_t heap_end;
mutex_t heap_mutex;

/*
 * This function is used to initialize heap memory.
 * Input:
 * none
 * Output:
 * result:     0 - ok
 *             1 - fail
 */
err_t heap_init(void)
{
    extern uint32_t _end; /* Symbol defined in the linker script */
    extern uint32_t _estack; /* Symbol defined in the linker script */
    extern uint32_t _Min_Stack_Size; /* Symbol defined in the linker script */

    heap_start = (uint32_t)&_end;
    heap_end = (uint32_t)&_estack - (uint32_t)&_Min_Stack_Size;

    if (heap_end - heap_start <= SIZEOF_MEM) {
        printf("heap too small!\r\n");
        return ERR_FAIL;
    }

    p_mem_t mem = (p_mem_t)(heap_end - SIZEOF_MEM);
    mem->magic = MAGIC;
    mem->used = 1;
    mem->size = 0;
    mem->next = 0;
    mem->prev = heap_start;

    mem = (p_mem_t)heap_start;
    mem->magic = MAGIC;
    mem->used = 0;
    mem->size = heap_end - heap_start - 2 * SIZEOF_MEM;
    mem->next = heap_end - SIZEOF_MEM;
    mem->prev = 0;

    if (mutex_create(&heap_mutex) != ERR_OK) {
        printf("heap mutex create fail!\r\n");
        return ERR_FAIL;
    }

    return ERR_OK;
}

/*
 * This function is used to malloc a given amount of memory from heap.
 * Input:
 * in_size: size to malloc
 * Output:
 * malloced address, or NULL if malloc is failed.
 */
void *os_malloc(uint32_t in_size)
{
    p_mem_t mem = (p_mem_t)heap_start;
    uint32_t size = ALIGN(in_size, 4);

    mutex_take(&heap_mutex, WAIT_FOREVER);

    while (mem != NULL && (mem->used || mem->size < size)) {
        mem = (p_mem_t)(mem->next);
    }

    if (mem == NULL) {
        printf("Not enough heap memory left to malloc %lu bytes.\r\n", size);
        return NULL;
    } else {
        //check magic
        if (mem->magic != MAGIC) {
            printf("memory is corrupted during malloc!\r\n");
            return NULL;
        }

        if (size == mem->size) {
            //whole block is assigned to this malloc
            mem->used = 1;
        } else {
            //split into two parts
            uint32_t cur_next = mem->next;
            uint32_t cur_size = mem->size;

            mem->size = size;
            mem->used = 1;
            mem->next = (uint32_t)mem + SIZEOF_MEM + size;

            p_mem_t mem_next = (p_mem_t)mem->next;
            mem_next->magic = MAGIC;
            mem_next->used = 0;
            mem_next->next = cur_next;
            mem_next->prev = (uint32_t)mem;
            mem_next->size = cur_size - size - SIZEOF_MEM;
        }
    }

    mutex_release(&heap_mutex);

    return (void *)((uint32_t)mem + SIZEOF_MEM);
}

/*
 * This function is used to free memory from heap.
 * Input:
 * addr: start memory to free
 * Output:
 * none
 */
void os_free(void *addr)
{
    if (addr == NULL) {
        printf("invalid address to free!\r\n");
        return;
    }

    mutex_take(&heap_mutex, WAIT_FOREVER);

    p_mem_t mem = (p_mem_t)((uint32_t)addr - SIZEOF_MEM);

    if (mem->magic != MAGIC) {
        printf("memory is corrupted during free!\r\n");
        return;
    }

    mem->used = 0;

    //merge with prev and next, if they are also unused
    p_mem_t mem_prev = (p_mem_t)(mem->prev);
    p_mem_t mem_next = (p_mem_t)(mem->next);

    if (mem_prev->used == 0) {
        mem_prev->next = (uint32_t)mem_next;
        mem_prev->size = mem_prev->size + mem->size + SIZEOF_MEM;
        mem_next->prev = (uint32_t)mem_prev;

        mem = mem_prev;
    }

    if (mem_next->used == 0) {
        mem->next = mem_next->next;
        mem->size = mem->size + mem_next->size + SIZEOF_MEM;
        p_mem_t cur_next = (p_mem_t)mem_next->next;
        cur_next->prev = (uint32_t)mem;
    }

    mutex_release(&heap_mutex);

    return;
}

//pure string ends with '\n' will call puts(), optimized by compiler
int puts(const char *str)
{
    extern int _write(int file, char *ptr, int len);
    return _write(0, (char *)str, strlen(str));
}

int printf(const char *format, ...)
{
    int ret;
    char str_buf[LOG_BUF_SIZE] = {0};

    va_list list;
    va_start(list, format);
    ret = vsnprintf(str_buf, LOG_BUF_SIZE, format, list);
    va_end(list);

    ret = puts(str_buf);
    return ret;
}
