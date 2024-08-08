// synch.h 
// ����ͬ���̵߳����ݽṹ��
//
// ���ﶨ��������ͬ�����ƣ��ź������������������������ź�����ʵ���Ѿ����������ں����ߣ�ֻ�ṩ�˹��̽ӿ� - ���ǽ���Ϊ��һ����ҵ��һ����ʵ�֡�
//
// ��ע�⣬���е�ͬ�������ڳ�ʼ��ʱ������һ����name��������������ڵ���Ŀ�ġ�

#ifndef SYNCH_H
#define SYNCH_H

#include "copyright.h"
#include "thread.h"
#include "list.h"
#include "interrupt.h"

// �����ඨ����һ�����ź���������ֵ�ǷǸ��������ź���ֻ���������� P() �� V()��
//
// 	P() -- �ȴ�ֱ��ֵ > 0��Ȼ��ݼ�
//
// 	V() -- �����������Ҫ�����ѵȴ��� P() �е��߳�
// 
// ��ע�⣬�ӿ�*������*һ���߳�ֱ�Ӷ�ȡ�ź�����ֵ - ��ʹ����ȡ�˸�ֵ����֪����ֻ�Ǹ�ֵ������ʲô������֪�����ڵ�ֵ�Ƕ��٣�
// ��Ϊ�ڽ�ֵ���ݵ��Ĵ���֮�󣬿��ܷ����������л����������߳̿��ܵ����� P �� V�������ʵ��ֵ���ڿ����Ѿ���ͬ�ˡ�

class Semaphore {
public:
  Semaphore(char* debugName, int initialValue);	// ���ó�ʼֵ
  ~Semaphore();   					// �ͷ��ź���
  char* getName() { return name; }			// ���Ը���

  void P();	 // �ź�����Ψһ����
  void V();	 // ���Ƕ���*ԭ�Ӳ���*

private:
  char* name;        // ���ڵ���
  int value;         // �ź���ֵ��ʼ�� >= 0
  List* queue;       // �� P() �еȴ�ֵ > 0 ���߳�
};

// �����ඨ����һ������������������ BUSY �� FREE��
// ��ֻ�������ֲ����� 
//
//	Acquire -- �ȴ�ֱ����Ϊ FREE��Ȼ��������Ϊ BUSY
//
//	Release -- ��������Ϊ FREE�����ѵȴ��� Acquire �ϵ��߳�
//
// ���⣬���չ�����ֻ�л�������̲߳����ͷ��������ź������ƣ������ܶ�ȡ����ֵ
// ����Ϊ�ڶ�ȡ���ֵ���ܻ��������ģ���

class Lock {
public:
  Lock(char* debugName);  		// ��ʼ����Ϊ FREE
  ~Lock();				// �ͷ���
  char* getName() { return name; }	// ���Ը���

  void Acquire(); // ����Ψһ����
  void Release(); // ���Ƕ���*ԭ�Ӳ���*

  bool isHeldByCurrentThread();	// �����ǰ�̳߳��д������򷵻� true���� Release �к� Condition ���������м��ʱ�����á�

private:
  char* name;				// ���ڵ���
  // ����һЩ�����Ķ�������Ҫ����
  Thread* owner_;
  Semaphore sem_;
};

// �����ඨ����һ������������������������û��ֵ�����߳̿����Ŷӵȴ��ڱ����ϡ�
// �����������Ĳ����������¼��֣� 
//
//	Wait() -- �ͷ��������� CPU ֱ�����źŻ��ѣ�Ȼ�����»�ȡ��
//
//	Signal() -- ����һ���̣߳�������κ��̵߳ȴ���������
//
//	Broadcast() -- �������еȴ��������ϵ��߳�
//
// ���ж����������Ĳ��������ڵ�ǰ�̻߳����������½��С���ʵ�ϣ��Ը����������������з��ʶ���������ͬ����������
// ���仰˵�������ڵ������������������߳�֮��ʵ�ֻ��⡣

// �� Nachos �У��ٶ�����������ѭ *Mesa* �������塣�� Signal �� Broadcast ������һ���߳�ʱ��
// ��ֻ�ǽ��̷߳��ھ����б��ϣ������߳����»�ȡ������ Wait() ���Ѵ��������֮�£�
// ��Щ���� *Hoare* ���������������������� -- ����������£������߳��������в�������Ȩ�� CPU �������ѵ��̣߳�
// ���ѵ��߳����뿪�ٽ����󽫿���Ȩ���ظ��źŷ����ߡ�
//
// ʹ�� Mesa �������ĺ���ǣ��ڻ��ѵ��߳��л�������֮ǰ��ĳЩ�����߳̿����Ѿ�������������������ݽṹ��

class Condition {
public:
  Condition(char* debugName);		// ��ʼ������Ϊ "���˵ȴ�"
  ~Condition();			// �ͷ�����
  char* getName() { return (name); }

  void Wait(Lock* conditionLock); 	// ��Щ������������ 3 ���������ͷ����ͽ���˯���� Wait() �ж��� *ԭ��* ��
  void Signal(Lock* conditionLock);   // ��Щ�����������ɵ�ǰ�̳߳��е� conditionLock ����
  void Broadcast(Lock* conditionLock);

private:
  char* name;
  // ����һЩ�����Ķ�������Ҫ����
  Semaphore waitlist_; // �ȴ��б�
  Lock lock_; // ������������
  int wait_; // �ȴ����߳���
};

// InterruptScope ���ʵ��
class InterruptScope final {
public:
  InterruptScope(IntStatus next);
  ~InterruptScope();
  InterruptScope(const InterruptScope&) = delete;
  InterruptScope& operator=(const InterruptScope&) = delete;
private:
  IntStatus old_;
};

// LockGuard ���ʵ��
class LockGuard final {
public:
  LockGuard(Lock& lock):lock_(&lock) {
    lock_->Acquire();
  }
  
  LockGuard(Lock* lock):lock_(lock) {
    lock_->Acquire();
  }

  ~LockGuard()
  {
    lock_->Release();
  }
private:
  Lock* lock_;
};

#endif // SYNCH_H
