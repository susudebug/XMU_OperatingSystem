/* High-level overview: This works by maintaining a singly-linked list of previously-used memory segments that have been freed. */
#define NALLOC 1024
#define myalloc bf_malloc

#include <errno.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
/* This header is stored at the beginning of memory segments in the list. */
union header
{
    struct
    {
        union header *next;
        unsigned len;
    } meta;
    long x; /* Presence forces alignment of headers in memory. */
};
static union header list;
static union header *first = NULL;
void free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }
    union header *iter, *block;
    iter = first;
    block = (union header *)ptr - 1;
    /* Traverse to the spot in the list to insert the freed fragment,
     * such that the list is ordered by memory address (for coalescing). */
    while (block <= iter || block >= iter->meta.next)
    {
        if ((block > iter || block < iter->meta.next) &&
            iter >= iter->meta.next)
        {
            break;
        }
        iter = iter->meta.next;
    }
    /* If the new fragment is adjacent in memory to any others, merge
     * them (we only have to check the adjacent elements because the
     * order semantics are enforced). */
    if (block + block->meta.len == iter->meta.next)
    {
        block->meta.len += iter->meta.next->meta.len;
        block->meta.next = iter->meta.next->meta.next;
    }
    else
    {
        block->meta.next = iter->meta.next;
    }
    if (iter + iter->meta.len == block)
    {
        iter->meta.len += block->meta.len;
        iter->meta.next = block->meta.next;
    }
    else
    {
        iter->meta.next = block;
    }
    first = iter;
}
void *bf_malloc(size_t size)
{
    union header *p, *prev, *best_fit = NULL, *best_fit_prev = NULL;
    prev = first;
    unsigned true_size =
        (size + sizeof(union header) - 1) /
            sizeof(union header) +
        1;
    /* If the list of previously allocated fragments is empty,
     * initialize it. */
    if (first == NULL)
    {
        prev = &list;
        first = prev;
        list.meta.next = first;
        list.meta.len = 0;
    }
    p = prev->meta.next;
/* 遍历空闲链表，寻找最佳适应块*/
    while (1)
    {
        if (p->meta.len >= true_size)
        {
            if (best_fit == NULL || p->meta.len < best_fit->meta.len)
            {
                best_fit = p;
                best_fit_prev = prev;
                if (p->meta.len == true_size)
                {
                    break;
                }
            }
        }
        /* If we reach the beginning of the list, no satisfactory fragment
         * was found, so we have to request a new one. */
        if (p == first)
        {
            break;
        }
        prev = p;
        p = p->meta.next;
    }
    /*如果有找到最佳匹配块，分配给用户*/
    if (best_fit != NULL)
    {
        union header *q = first;
        printf("freelist:{");
        do
        {
            printf("%d ", q->meta.len);
            q = q->meta.next;
        } while (q != first);
        printf("};\n");
        
        if (best_fit->meta.len == true_size)
        {
            /* If the fragment is exactly the right size, we do not have
             * to split it. */
            best_fit_prev->meta.next = best_fit->meta.next;
        }
        else
        {
            /* Otherwise, split the fragment, returning the first half and
             * storing the back half as another element in the list. */
            best_fit->meta.len -= true_size;
            best_fit += best_fit->meta.len;
            best_fit->meta.len = true_size;
        }
        first = best_fit_prev;

        return (void *)(best_fit + 1);
    }
    else/*没找到最佳空闲块则使用sbrk系统调用分配内存*/
    {
        char *page;
        union header *block;
        unsigned alloc_size = true_size;
        /* We have to request memory of at least a certain size. */
        if (alloc_size < NALLOC)
        {
            alloc_size = NALLOC;
        }
        page = sbrk((intptr_t)(alloc_size * sizeof(union header)));
        if (page == (char *)-1)
        {
            /* There was no memory left to allocate. */
            errno = ENOMEM;
            return NULL;
        }
        /* Create a fragment from this new memory and add it to the list
         * so the above logic can handle breaking it if necessary. */
        block = (union header *)page;
        block->meta.len = alloc_size;
        free((void *)(block + 1));
        return bf_malloc(size);
    }
}
void *calloc(size_t num, size_t len)
{
    void *ptr = myalloc(num * len);
    /* Set the allocated array to 0's.*/
    if (ptr != NULL)
    {
        memset(ptr, 0, num * len);
    }
    return ptr;
}