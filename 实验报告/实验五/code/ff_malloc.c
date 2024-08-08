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
	long x; /* ����ǿ���ڴ���� */
};
static union header list;
static union header *first = NULL;
void free(void* ptr) {

	/*���ָ���Ƿ�Ϊ�գ����򷵻�*/
	if (ptr == NULL) {
		return;
	}
	union header *iter, *block;
	iter = first;
	block = (union header*)ptr - 1;
	/* ���������ҵ����ʵ�λ�ò����ͷŵ��ڴ�飬ʹ�������ڴ��ַ˳�����С�*/
	while (block <= iter || block >= iter->meta.next) {
		if ((block > iter || block < iter->meta.next) &&
		        iter >= iter->meta.next) {
			break;
		}
		iter = iter->meta.next;
	}
	/* ������ͷŵ��ڴ�������������ڵ��ڴ�����ڣ�����кϲ�������*/
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
	/* true_size-ʵ����Ҫ������ڴ���С*/
	unsigned true_size =
	    (size + sizeof(union header) - 1) / sizeof(union header) + 1;
	/* �������Ϊ�գ���ʼ������ */
	if (first == NULL) {
		prev = &list;
		first = prev;
		list.meta.next = first;/*��������*/
		list.meta.len = 0;
	}
	p = prev->meta.next;
	/* ���������ҵ���һ����С���ʵ��ڴ�顣*/
	while (1) {
		/* ����ҵ����ʵ��ڴ�飬�����������ط�����ڴ�顣*/
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
		/* ���δ�ҵ����ʵ��ڴ�飬��ͨ��sbrkϵͳ���������µ��ڴ�*/
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
			/* �����䵽�����ڴ��ת��Ϊ union header ���ͣ����������С��
			���� free �������µ��ڴ����뵽���������С�*/
			block = (union header *)page;
			block->meta.len = alloc_size;
			free((void *)(block + 1));
		}
		prev = p;
		p = p->meta.next;
	}
	return NULL;
}
/* ����ff_malloc��������num������Ϊlen���ڴ�顣*/
void* calloc(size_t num, size_t len) {
	void* ptr = ff_malloc(num * len);
	/* Set the allocated array to 0's.*/
	if (ptr != NULL) {
		memset(ptr, 0, num * len);
	}
	return ptr;
}
