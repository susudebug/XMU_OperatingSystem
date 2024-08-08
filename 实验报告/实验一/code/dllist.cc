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



// ��ʾ˫�������е�һ�����
DLLElement::DLLElement(void* itemPtr, int sortKey) {
	item = itemPtr;  //ָ��
	key = sortKey;   //����ؼ���
	next = prev = NULL;  //ǰ�̺ͺ��ָ��
}

//��ʾ˫������
DLList::DLList() {
	first = last = NULL;  //ͷ��βָ��
}

//DLList����������~DLList()
//��DLList��������ʱ��ɾ�������е����нڵ㣬���ͷ���ص��ڴ���Դ
DLList::~DLList() {
	int key;
	while (IsEmpty()) {
		Remove(&key);
	}
}

//��ͷ�����ӽ��
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

//��β�����ӽ��
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

//��˫�������Ƴ���һ����㣬������������
void* DLList::Remove(int* keyPtr) {

	if (!IsEmpty()) { //���û�нڵ�
		keyPtr = NULL;
		return NULL;
	} else {

		void* itemPtr;
		DLLElement* element;

		*keyPtr = first->key;
		itemPtr = first->item;
		element = first;

		if (first->next) { //�����ֹһ�����
			first->next->prev = NULL; //���ڶ�������ǰ��ָ����ΪNULL����Ϊɾ����ԭ�ȵڶ������ɵ�һ��
			first = first->next; //���ײ�ָ��ָ��ڶ����ڵ�
		} else { //���ֻ��һ�����
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
