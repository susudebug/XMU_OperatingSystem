/* High-level overview: This works by maintaining a singly-linked list of previously-used memory segments that have
been freed. */
#define NALLOC 1024
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
/* This header is stored at the beginning of memory segments in the list. */
union header {
	struct {
		union header *next;
		unsigned len;
	} meta;
	long x; /* 用于强制内存对齐 */
};
static union header list;
static union header *first = NULL;
void free(void* ptr) {

	/*检查指针是否为空，空则返回*/
	if (ptr == NULL) {
		return;
	}
	union header *iter, *block;
	iter = first;
	block = (union header*)ptr - 1;
	/* 遍历链表，找到合适的位置插入释放的内存块，使链表按照内存地址顺序排列。*/
	while (block <= iter || block >= iter->meta.next) {
		if ((block > iter || block < iter->meta.next) &&
		        iter >= iter->meta.next) {
			break;
		}
		iter = iter->meta.next;
	}
	/* 如果新释放的内存块与链表中相邻的内存块相邻，则进行合并操作。*/
	if (block + block->meta.len == iter->meta.next) {
		block->meta.len += iter->meta.next->meta.len;
		block->meta.next = iter->meta.next->meta.next;
	} else {
		block->meta.next = iter->meta.next;
	}
	if (iter + iter->meta.len == block) {
		iter->meta.len += block->meta.len;
		iter->meta.next = block->meta.next;
	} else {
		iter->meta.next = block;
	}
	first = iter;
}
void *ff_malloc(size_t size) {
	union header *p, *prev;
	prev = first;
	/* true_size-实际需要分配的内存块大小*/
	unsigned true_size =
	    (size + sizeof(union header) - 1) / sizeof(union header) + 1;
	/* 如果链表为空，初始化链表。 */
	if (first == NULL) {
		prev = &list;
		first = prev;
		list.meta.next = first;/*环形链表*/
		list.meta.len = 0;
	}
	p = prev->meta.next;
	/* 遍历链表，找到第一个大小合适的内存块。*/
	while (1) {
		/* 如果找到合适的内存块，更新链表并返回分配的内存块。*/
		if (p->meta.len >= true_size) {
        union header *q = first;
        printf("freelist:{");
        do
        {
            printf("%d ", q->meta.len);
            q = q->meta.next;
        } while (q != first);
        printf("};\n");
        
			if (p->meta.len == true_size) {
				/* If the fragment is exactly the right size, we do not have
				* to split it. */
				prev->meta.next = p->meta.next;
			} else {
				/* Otherwise, split the fragment, returning the first half and
				* storing the back half as another element in the list. */
				p->meta.len -= true_size;
				p += p->meta.len;
				p->meta.len = true_size;
			}
			first = prev;
        
			return (void *)(p + 1);
		}
		/* 如果未找到合适的内存块，则通过sbrk系统调用请求新的内存*/
		if (p == first) {
			char *page;
			union header *block;
			unsigned alloc_size = true_size;
			/* We have to request memory of at least a certain size. */
			if (alloc_size < NALLOC) {
				alloc_size = NALLOC;
			}
			page = sbrk((intptr_t) (alloc_size * sizeof(union header)));
			if (page == (char *) -1) {
				/* There was no memory left to allocate. */
				errno = ENOMEM;
				return NULL;
			}
			/* 将分配到的新内存块转换为 union header 类型，并设置其大小。
			调用 free 函数将新的内存块插入到空闲链表中。*/
			block = (union header *)page;
			block->meta.len = alloc_size;
			free((void *)(block + 1));
		}
		prev = p;
		p = p->meta.next;
	}
	return NULL;
}
/* 调用ff_malloc函数分配num个长度为len的内存块。*/
void* calloc(size_t num, size_t len) {
	void* ptr = ff_malloc(num * len);
	/* Set the allocated array to 0's.*/
	if (ptr != NULL) {
		memset(ptr, 0, num * len);
	}
	return ptr;
}
