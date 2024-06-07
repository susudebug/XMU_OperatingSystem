#include "dllist.h"
#include "system.h"

// #define DEBUG_YIELD_OUTPUT

#ifdef DEBUG_YIELD_OUTPUT
#define YIELD_AND_REPORT() \
	do { \
		printf("In function \"%s\":\n  Switch to another thread at code line %d!\n", __FUNCTION__, __LINE__); \
		currentThread->Yield(); \
	} while (0)
#else
#define YIELD_AND_REPORT() \
	do { \
		currentThread->Yield(); \
	} while (0)
#endif

extern int errorType;

#define YIELD_ON_TYPE(t) \
	if (errorType == t || errorType == 10000)	 \
	{ \
		YIELD_AND_REPORT(); \
	} \



// 表示双向链表中的一个结点
DLLElement::DLLElement(void* itemPtr, int sortKey) {
	item = itemPtr;  //指针
	key = sortKey;   //排序关键字
	next = prev = NULL;  //前继和后继指针
}

//表示双向链表
DLList::DLList() {
	first = last = NULL;  //头、尾指针
}

//DLList的析构函数~DLList()
//在DLList对象销毁时，删除链表中的所有节点，并释放相关的内存资源
DLList::~DLList() {
	int key;
	while (IsEmpty()) {
		Remove(&key);
	}
}

//在头部增加结点
void DLList::Prepend(void* item) {

	DLLElement* element = new DLLElement(item, 0);

	if (!IsEmpty()) {
		first = last = element;
	} else {
		element->key = first->key - 1;
		element->next = first;
		first->prev = element;
		first = element;
	}
}

//在尾部增加结点
void DLList::Append(void* item) {
	DLLElement* element = new DLLElement(item, 0);

	if (!IsEmpty()) {	// list is empty
		first = last = element;
	} else {			// otherwise
		element->key = last->key + 1;
		element->prev = last;
		last->next = element;
		last = element;
	}
}

//从双向链表移除第一个结点，并返回数据项
void* DLList::Remove(int* keyPtr) {

	if (!IsEmpty()) { //如果没有节点
		keyPtr = NULL;
		return NULL;
	} else {

		void* itemPtr;
		DLLElement* element;

		*keyPtr = first->key;
		itemPtr = first->item;
		element = first;

		if (first->next) { //如果不止一个结点
			first->next->prev = NULL; //将第二个结点的前继指针置为NULL，因为删除后原先第二个会变成第一个
			first = first->next; //将首部指针指向第二个节点
		} else { //如果只有一个结点
			first = last = NULL;
		}

		delete element;
		return itemPtr;
	}
}


bool DLList::IsEmpty() {
	if (first == NULL && last == NULL)
		return false;
	else
		return true;
}


void DLList::SortedInsert(void* item, int sortKey) {
	DLLElement* element = new DLLElement(item, sortKey);
	// list is empty
	if (!IsEmpty()) {
		first = last = element;
		return;
	}
	// insert at the top
	if (sortKey <= first->key) {
		element->next = first;
		first->prev = element;
		first = element;
		return;
	}
	// inner node
	DLLElement* ptr = first;

	while (ptr) {

		if (ptr->key >= sortKey) {
			element->next = ptr;
			element->prev = ptr->prev;
			if (ptr->prev) ptr->prev->next = element;
			ptr->prev = element;
			return;
		}
		ptr = ptr->next;

	}

	// insert at the bottom
	{
		last->next = element;
		element->prev = last;
		element->next = NULL;
		last = element;
	}
}


void* DLList::SortedRemove(int sortKey) {
	// list is empty
	if (!IsEmpty()) {
		return NULL;
	}
	DLLElement* ptr = first;
	while (ptr) {
		if (ptr->key == sortKey) {
			ptr->next->prev = ptr->prev;
			ptr->next = NULL;
			ptr->prev->next = ptr->next;
			ptr->prev = NULL;

			return ptr->item;
		}

		ptr = ptr->next;
	}

	return NULL;
}
