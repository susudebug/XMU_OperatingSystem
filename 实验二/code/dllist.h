#pragma once

#include "synch.h"

class DLLElement {
public:
	friend class RAIIListValidator;
	friend class RAIINodeGuard;
	DLLElement(void* itemPtr, int sortKey);	// initialize a list element

	DLLElement* next;			// next element on list
	// NULL if this is the last
	DLLElement* prev;			// previous element on list
	// NULL if this is the first

	int key;					// priority, for a sorted list
	void* item;					// pointer to item on the list
};

class DLList {
public:
	friend class RAIIListValidator;

	DLList();					// initialize the list
	~DLList();					// de-allocate the list

	void Prepend(void* item);	// add to head of list (set key = min_key-1)
	void Append(void* item);	// add to tail of list (set key = max_key+1)
	void* Remove(int* keyPtr);	// remove from head of list
	// set *keyPtr to key of the removed item
	// return item (or NULL if list is empty)

	bool IsEmpty(); // return true if list has elements

	// routines to put/get items on/off list in order (sorted by key)
	void SortedInsert(void* item, int sortKey);
	void* SortedRemove(int sortKey);	// remove first item with key==sortKey
	// return NULL if no such item exists
private:
	DLLElement* first;			// head of the list, NULL if empty
	DLLElement* last;			// last element of the list, NULL if empty

	Lock* lock_;
	Condition* condvar_;
};

class RAIIListValidator {
public:
	RAIIListValidator(DLList& list, const char* tag);
	~RAIIListValidator();
private:
	bool test();
	bool is_sorted_asc(DLLElement* head);
	bool is_sorted_dsc(DLLElement* head);
	DLList* list_;
	const char* tag_;
};

class RAIINodeGuard {
public:
	RAIINodeGuard(DLLElement& ele, char* name);
	~RAIINodeGuard();
private:
	void test();
	DLLElement* ele_;
	char* name_;
};
