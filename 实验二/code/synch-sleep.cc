// synch.cc
// �����߳�ͬ�������̡����ﶨ��������ͬ�����̣��ź������������������������ߵ�ʵ������������ɣ���
//
// �κ�ͬ�����̵�ʵ�ֶ���ҪһЩԭ�Ӳ��������Ǽ���Nachos�����ڵ��������ϣ����ԭ���Կ���ͨ�������ж����ṩ��
// ���жϱ�����ʱ�����ᷢ���������л�����˵�ǰ�߳̿���ȷ�������������г���CPU��ֱ�����������жϡ�
//
// ����ĳЩ�������Щ���̿������ж��ѽ��õ�����±����ã�����Semaphore::V������������ʼ�ռ򵥵ؽ��ж�״̬�������û���ԭʼֵ
// ���������Ƿ��ѽ��ã���������ԭ�Ӳ�������ʱ���жϡ�
//
// ��Ȩ���У�C��1992-1993 ���������Ǵ�ѧ���»ᡣ��������Ȩ�����йذ�Ȩ�������������Ƶ���Ϣ�������copyright.h��

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// ��ʼ���ź������Ա�����ͬ����
//
// ��debugName����һ���������ƣ����ڵ��ԡ�
// ��initialValue�����ź����ĳ�ʼֵ��
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue) {
	name = debugName;
	value = initialValue;
	queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::~Semaphore
// ��������Ҫʱ���ͷ��ź���������û�������ڵȴ��ź�����
//----------------------------------------------------------------------

Semaphore::~Semaphore() {
	delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// �ȴ�ֱ���ź���ֵ > 0��Ȼ��ݼ�������ԭ�ӵؼ��ֵ���ݼ������������Ҫ�ڼ��ֵ֮ǰ�����жϡ�
//
// ��ע�⣬Thread::Sleep �����ڵ���ʱ�жϱ����á�
//----------------------------------------------------------------------

void Semaphore::P() {
	IntStatus oldLevel = interrupt->SetLevel(IntOff);    // �����ж�

	while (value == 0) {            // �ź���������
		queue->Append((void*)currentThread);    // ���Խ���˯��
		currentThread->Sleep();
	}
	value--;                    // �ź������ã�������ֵ

	(void)interrupt->SetLevel(oldLevel);    // ���������ж�
}

//----------------------------------------------------------------------
// Semaphore::V
// �����ź���ֵ����Ҫʱ���ѵȴ��ߡ�
// ��P()һ�����˲���������ԭ�ӵģ����������Ҫ�����жϡ�
//----------------------------------------------------------------------

void Semaphore::V() {
	Thread* thread;
	IntStatus oldLevel = interrupt->SetLevel(IntOff);

	thread = (Thread*)queue->Remove();
	if (thread != NULL)       // ʹ�߳̾���������������V
		scheduler->ReadyToRun(thread);
	value++;
	(void)interrupt->SetLevel(oldLevel);
}

// InterruptScope ��ʵ��
InterruptScope::InterruptScope(IntStatus next) {
	old_ = interrupt->SetLevel(next);
}

InterruptScope::~InterruptScope() {
	(void)interrupt->SetLevel(old_);
}

// ����ʵ��
Lock::Lock(char* debugName): name(debugName), owner_(NULL), sem_(debugName, 1), locked_(false) {
	waitlist_ = new List();
}

Lock::~Lock() {
	delete waitlist_;
}

void Lock::Acquire() {
	IntStatus oldLevel = interrupt->SetLevel(IntOff); // �����ж�
	while (locked_) { // �����ѱ���סʱ
		waitlist_->Append(currentThread); // ����ǰ�߳���ӵ��ȴ�����
		currentThread->Sleep(); // ����ǰ�߳�����˯��״̬
	}
	locked_ = true; // ��ס��
	owner_ = currentThread; // ������������Ϊ��ǰ�߳�
	interrupt->SetLevel(oldLevel); // ���������ж�
}

void Lock::Release() {
	IntStatus oldLevel = interrupt->SetLevel(IntOff); // �����ж�
	ASSERT(isHeldByCurrentThread()); // ȷ����ǰ�̳߳�����
	locked_ = false; // ������
	owner_ = NULL; // ������������Ϊ��
	if (!waitlist_->IsEmpty()) { // ����ȴ����в�Ϊ��
		Thread* thread = waitlist_->Remove(); // �ӵȴ��������Ƴ���һ���߳�
		scheduler->ReadyToRun(thread); // ���̷߳����������
	}
	interrupt->SetLevel(oldLevel); // ���������ж�
}

bool Lock::isHeldByCurrentThread() {
	return owner_ == currentThread;
}

// ����������ʵ��
Condition::Condition(char* debugName): name(debugName) {
	waitlist_ = new List();
}
Condition::~Condition() {
	delete waitlist_;
}

void Condition::Wait(Lock* conditionLock) {
	ASSERT(conditionLock->isHeldByCurrentThread());
	InterruptScope cli(IntOff);
	conditionLock->Release();
	waitlist_->Append((void*)currentThread);
	currentThread->Sleep();
	conditionLock->Acquire();
}
void Condition::Signal(Lock* conditionLock) {
	ASSERT(conditionLock->isHeldByCurrentThread()); // ȷ����ǰ�̳߳�����
	IntStatus oldLevel = interrupt->SetLevel(IntOff); // �����ж�
	if (!waitlist_->IsEmpty()) { // ����ȴ����в�Ϊ��
		Thread* thread = (Thread*)waitlist_->Remove(); // �ӵȴ��������Ƴ���һ���߳�
		scheduler->ReadyToRun(thread); // ���̷߳����������
	}
	interrupt->SetLevel(oldLevel); // ���������ж�
}

void Condition::Broadcast(Lock* conditionLock) {
    ASSERT(conditionLock->isHeldByCurrentThread()); // ȷ����ǰ�̳߳�����

    IntStatus oldLevel = interrupt->SetLevel(IntOff); // ���浱ǰ�ж�״̬�������ж�

    while (!waitlist_->IsEmpty()) { // ���ȴ����в�Ϊ��ʱ
        Thread* thread =  (Thread*)waitlist_->Remove(); // �ӵȴ��������Ƴ���һ���߳�
        scheduler->ReadyToRun(thread); // ���̷߳����������
    }

    interrupt->SetLevel(oldLevel); // �ָ��ж�״̬
}
