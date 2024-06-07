#include "dllist.h"
#include "system.h"

#define DEBUG_YIELD_OUTPUT

#ifdef DEBUG_YIELD_OUTPUT
#define YIELD_AND_REPORT() \
do { \
printf("�ں��� \"%s\" ��:\n  �ڴ����� %d ���л�����һ���̣߳�\n", __FUNCTION__, __LINE__); \
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

// DLL Ԫ�ع��캯��
DLLElement::DLLElement(void* itemPtr, int sortKey)
{
	item = itemPtr;
	key = sortKey;
	next = prev = NULL;
}

// ˫�������캯��
DLList::DLList()
{
	first = last = NULL;
	lock_ = new Lock("listlock");
	condvar_ = new Condition("listcondvar");
}

// ˫��������������
DLList::~DLList()
{
	int key;
	// ɾ�������е�����Ԫ��
	while (IsEmpty())
	{
		Remove(&key);
	}

	delete lock_;
	delete condvar_;
}

// ������ͷ������Ԫ��
void DLList::Prepend(void* item)
{
	LockGuard g(lock_);
	RAIIListValidator _(*this, __FUNCTION__);

	DLLElement* element = new DLLElement(item, 0);

	// �������Ϊ�գ������ͷ��Ԫ��
	if (!IsEmpty())
	{
		first = last = element;
	}
	else
	{
		element->key = first->key - 1;
		element->next = first;
		first->prev = element;
		first = element;
	}

	// ���ѵȴ��������������߳�
	condvar_->Signal(lock_);
}

// ������β������Ԫ��
void DLList::Append(void* item)
{
	LockGuard g(lock_);
	RAIIListValidator _(*this, __FUNCTION__);

	DLLElement* element = new DLLElement(item, 0);

	// �������Ϊ�գ������β��Ԫ��
	if (!IsEmpty())		
	{
		first = last = element;
	}
	else				
	{
		element->key = last->key + 1;
		element->prev = last;
		last->next = element;
		last = element;
	}

	// ���ѵȴ��������������߳�
	condvar_->Signal(lock_);
}

// �Ƴ������е�Ԫ��
void* DLList::Remove(int* keyPtr)
{
	LockGuard g(lock_);
	YIELD_ON_TYPE(101);

	// �������Ϊ�գ���ȴ�ֱ���ǿ�
	while (IsEmpty())
		condvar_->Wait(lock_);		

	if (!IsEmpty())
	{
		keyPtr = NULL;
		return NULL;
	}
	else
	{
		RAIIListValidator _(*this, __FUNCTION__);
		void* itemPtr;
		DLLElement* element;

		YIELD_ON_TYPE(102);
		*keyPtr = first->key;
		itemPtr = first->item;
		element = first;
		YIELD_ON_TYPE(103);

		if (first->next)
		{
			RAIIListValidator _(*this, __FUNCTION__);
			YIELD_ON_TYPE(104);
			first->next->prev = NULL;
			YIELD_ON_TYPE(105);
			first = first->next;
			YIELD_ON_TYPE(106);
		}
		else
		{
			YIELD_ON_TYPE(107);
			first = last = NULL;
		}
		YIELD_ON_TYPE(108);
		delete element;
		YIELD_ON_TYPE(109);

		return itemPtr;
	}
}

// ��������Ƿ�Ϊ��
bool DLList::IsEmpty()
{
	LockGuard g(lock_);
	if (first == NULL && last == NULL)
		return false;
	else
		return true;
}

// ������˳�����Ԫ��
void DLList::SortedInsert(void* item, int sortKey)
{
	DLLElement* element = new DLLElement(item, sortKey);

	LockGuard g(lock_);	
	
	YIELD_ON_TYPE(1);

	// �������Ϊ�գ���ֱ�Ӳ���Ԫ����ΪΨһԪ��
	if (!IsEmpty())
	{
		RAIIListValidator _(*this, __FUNCTION__);
		first = last = element;
		YIELD_ON_TYPE(102);
		return;
	}

	YIELD_ON_TYPE(3);

	// ������ͷ������Ԫ��
	if (sortKey <= first->key)
	{
		RAIIListValidator _(*this, __FUNCTION__);
		YIELD_ON_TYPE(4);

		RAIINodeGuard _1(*element, "element");
		RAIINodeGuard _2(*first, "first");
		element->next = first;
		YIELD_ON_TYPE(5);
		first->prev = element; 
		YIELD_ON_TYPE(6);
		first = element;
		YIELD_ON_TYPE(7);
		return;
	}

	YIELD_ON_TYPE(8);

	// �������в�����Ԫ��
	DLLElement* ptr = first;
	YIELD_ON_TYPE(9);

	while (ptr)
	{
		YIELD_ON_TYPE(10);
		if (ptr->key >= sortKey)
		{
			RAIIListValidator _(*this, __FUNCTION__);
			YIELD_ON_TYPE(11);

			RAIINodeGuard _1(*element, "element");
			RAIINodeGuard _2(*ptr, "ptr");

			element->next = ptr;
			YIELD_ON_TYPE(12);
			element->prev = ptr->prev;

			YIELD_ON_TYPE(13);

			if (ptr->prev) ptr->prev->next = element;
			YIELD_ON_TYPE(14);
			ptr->prev = element;
			YIELD_ON_TYPE(15);
			return;
		}

		YIELD_ON_TYPE(16);
		ptr = ptr->next;
		YIELD_ON_TYPE(17);
	}
	YIELD_ON_TYPE(18);

	// ������β������Ԫ��
	{
		RAIINodeGuard _1(*last, "last");
		RAIINodeGuard _2(*element, "element");

		last->next = element;

		YIELD_ON_TYPE(19);

		element->prev = last;
		element->next = NULL;

		YIELD_ON_TYPE(20);
		last = element;
	}
	YIELD_ON_TYPE(21);

	condvar_->Signal(lock_);
}

// ��������Ƴ�Ԫ��
void* DLList::SortedRemove(int sortKey)
{
	RAIIListValidator _(*this, __FUNCTION__);
	LockGuard g(lock_);

	while (IsEmpty())
		 condvar_->Wait(lock_);	

	// �������Ϊ�գ��򷵻� NULL
	if (!IsEmpty())
	{
		return NULL;
	}

	DLLElement* ptr = first;
	while (ptr)
	{
		if (ptr->key == sortKey)
		{
			YIELD_ON_TYPE(201);
			ptr->next->prev = ptr->prev;
			YIELD_ON_TYPE(202);
			ptr->next = NULL;
			YIELD_ON_TYPE(203);
			ptr->prev->next = ptr->next;
			YIELD_ON_TYPE(204);
			ptr->prev = NULL;
			YIELD_ON_TYPE(205);
			return ptr->item;
		}

		ptr = ptr->next;
	}

	return NULL;
}

// RAIIListValidator ���캯��
RAIIListValidator::RAIIListValidator(DLList& list, const char* tag)
	: list_(&list), tag_(tag)
{
	if (!test())
	{
		printf("�� \"%s\" ��:\n  ����������", tag);
		ASSERT(false);
	}
}

// RAIIListValidator ��������
RAIIListValidator::~RAIIListValidator()
{
	if (!test())
	{
		printf("�� \"%s\" ��:\n  ����������", tag_);
		ASSERT(false);
	}
}

// ��������Ƿ�����ĸ�������
bool RAIIListValidator::test()
{
	if (list_->IsEmpty())
	{
		return true;
	}

	if (list_->first)
		ASSERT(list_->first->prev == NULL);
	if (list_->last)
		ASSERT(list_->last->next == NULL);

	for (DLLElement* p = list_->first;p;p = p->next)
	{
		if (p && p->next)
			ASSERT(p->next->prev == p);
		if (p && p->prev)
			ASSERT(p->prev->next == p);
	}

	return is_sorted_asc(list_->first) || is_sorted_dsc(list_->first);
}

// ��������Ƿ���������ĸ�������
bool RAIIListValidator::is_sorted_asc(DLLElement* head)
{
	if (head == NULL)
		return true;

	for (DLLElement* t = head; t->next != NULL; t = t->next)
		if (t->key <= t->next->key)
			return false;
	return true;
}

// ��������Ƿ񰴽�������ĸ�������
bool RAIIListValidator::is_sorted_dsc(DLLElement* head)
{
	if (head == NULL)
		return true;

	for (DLLElement* t = head; t->next != NULL; t = t->next)
		if (t->key >= t->next->key)
			return false;
	return true;
}

// RAIINodeGuard ���캯��
RAIINodeGuard::RAIINodeGuard(DLLElement& ele, char* name)
	: ele_(&ele), name_(name)
{
	printf("�߳� %s ���� %s ���ܱ�������", currentThread->getName(), name_);
	if (ele_->prev || ele_->next)
	{
		if (!ele_->prev)
		{
			printf("���ǵ�һ���ڵ㡣");
		}
		if (!ele_->next)
		{
			printf("�������һ���ڵ㡣");
		}
	}

	printf("\n");
	test();
}

// RAIINodeGuard ��������
RAIINodeGuard::~RAIINodeGuard()
{
	test();
	printf("�߳� %s �˳� %s ���ܱ�������\n", currentThread->getName(), name_);
}

// ���ڵ��ϵ�ĸ�������
void RAIINodeGuard::test()
{
	if (ele_->prev && ele_->prev->next != ele_)
	{
		printf("%s ��ǰһ���ڵ��ϵ����\n", name_);
		ASSERT(false);
	}

	if (ele_->next && ele_->next->prev != ele_)
	{
		printf("%s �ĺ�һ���ڵ��ϵ����\n", name_);
		ASSERT(false);
	}

	int diff1 = ele_->prev ? ele_->key - ele_->prev->key : 0;
	int diff2 = ele_->next ? ele_->next->key - ele_->key : 0;
	if (diff1 && diff2 && diff1 * diff2 < 0)
	{
		printf("%s ��˳�����ֵΪ��", name_);
		if (ele_->prev)
		{
			printf("%d ", ele_->prev->key);
		}

		printf("%d ", ele_->key);

		if (ele_->next)
		{
			printf("%d ", ele_->next->key);
		}

		printf("\n");

		ASSERT(false);
	}
}
