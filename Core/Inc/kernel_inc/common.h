/*
 * Created by mikePPeng.
 * This is a header file for common APIs and definitions used in myOS.
 * Change Logs:
 * Date           Notes
 * Feb 24, 2021   the first version
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdint.h>

typedef enum error_code {
    ERR_OK = 0,
    ERR_FAIL,
    ERR_TIMEOUT,
} err_t;

struct list_head {
    struct list_head *next, *prev;
};

#define NAME_MAX_LEN 16
#define LOG_BUF_SIZE 256

#define ALIGN(size, align) (((size) + (align) - 1) & ~((align) - 1))
#define MAGIC 0xabcd

typedef struct heap_memory {
    uint16_t magic;
    uint16_t used;
    uint32_t size;
    uint32_t prev;
    uint32_t next;
} mem_t, *p_mem_t;

#define SIZEOF_MEM (sizeof(mem_t))

#define offset_of(type, member) ((size_t) &((type *)0)->member)

#define container_of(ptr, type, member) ({             \
    const typeof(((type *)0)->member) *__mptr = (ptr);     \
    (type *)((char *)__mptr - offset_of(type, member)); })

#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define list_head_init(name) struct list_head name = { &(name), &(name) }

#define list_empty(head) ((head)->next == (head))

#define list_for_each_entry(itr, head, member)                       \
    for (itr = container_of((head)->next, typeof(*itr), member);     \
         &itr->member != (head);                                     \
         itr = container_of(itr->member.next, typeof(*itr), member))

#define list_for_each_entry_safe(itr, itr_next, head, member)                                    \
    for (itr = container_of((head)->next, typeof(*itr), member),                                 \
         itr_next = container_of(itr->member.next, typeof(*itr), member);                        \
         &itr->member != (head);                                                                 \
         itr = itr_next, itr_next = container_of(itr->member.next, typeof(*itr_next), member))

/*
 * This function is used to add a new entry in list, between @prev and @next.
 * Input:
 * new:   entry to be added
 * prev:  entry before @new
 * next:  entry after @new
 * Output:
 * none
 */
static inline void __list_add(struct list_head *new,
                              struct list_head *prev,
                              struct list_head *next)
{
    new->next = next;
    next->prev = new;
    new->prev = prev;
    prev->next = new;
}

/*
 * This function is used to add a new entry in list, between @prev and @next.
 * Input:
 * new:   entry to be added
 * prev:  entry before @new
 * next:  entry after @new
 * Output:
 * none
 */
static inline void list_add_between(struct list_head *new,
                                    struct list_head *prev,
                                    struct list_head *next)
{
    __list_add(new, prev, next);
}

/*
 * This function is used to add a new entry after @head.
 * Input:
 * new:   entry to be added
 * head:  entry before @new
 * Output:
 * none
 */
static inline void list_add_after(struct list_head *new,
                                  struct list_head *head)
{
    __list_add(new, head, head->next);
}

/*
 * This function is used to add a new entry before @head.
 * Input:
 * new:  entry to be added
 * head: entry after @new
 * Output:
 * none
 */
static inline void list_add_before(struct list_head *new,
                                   struct list_head *head)
{
    __list_add(new, head->prev, head);
}

/*
 * This function is used to delete the entry between @prev and @next.
 * Input:
 * prev:  entry before the deleted entry
 * next:  entry after the deleted entry
 * Output:
 * none
 */
static inline void __list_del(struct list_head *prev,
                              struct list_head *next)
{
    prev->next = next;
    next->prev = prev;
}

/*
 * This function is used to delete the given entry.
 * Input:
 * entry: the entry to be deleted
 * Output:
 * none
 */
static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = NULL;
    entry->prev = NULL;
}

/*
 * This function is used to initialize heap memory.
 * Input:
 * none
 * Output:
 * result:     0 - ok
 *             1 - fail
 */
err_t heap_init(void);

/*
 * This function is used to malloc a given amount of memory from heap.
 * Input:
 * in_size: size to malloc
 * Output:
 * malloced address, or NULL if malloc is failed.
 */
void *os_malloc(uint32_t in_size);

/*
 * This function is used to free memory from heap.
 * Input:
 * addr: start memory to free
 * Output:
 * none
 */
void os_free(void *addr);

#endif
