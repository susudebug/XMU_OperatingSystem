// synch.cc 
// ����ͬ���̵߳����̡����ﶨ��������ͬ�����̣��ź������������������������ߵ�ʵ������������ɣ���
//
// �κ�ͬ�����̵�ʵ�ֶ���ҪһЩ������ԭ�Ӳ��������Ǽ��� Nachos �ڵ������������У����ԭ���Կ���ͨ�������ж���ʵ�֡�
// ���жϱ�����ʱ�����ᷢ���������л�����˵�ǰ�߳̿��Ա�֤�����������ڼ���� CPU��ֱ���жϱ��������á�
//
// ��ΪĳЩ���̿������ж��ѱ����õ�����±����ã����� Semaphore::V�����������ǲ�����ԭ�Ӳ�������ʱ���жϣ�
// ���Ǽ򵥵ؽ��ж�״̬�ָ�����ԭʼֵ�����������ѽ��û��������ã���

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// ��ʼ���ź������Ա�����ͬ����
//
// "debugName" ��һ����������ƣ����ڵ��ԡ�
// "initialValue" ���ź����ĳ�ʼֵ��
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::~Semaphore
// ��������Ҫʱ���ͷ��ź���������û�������ڵȴ��ź�����
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// �ȴ�ֱ���ź���ֵ > 0��Ȼ��ݼ������ֵ�͵ݼ�������ԭ�ӵģ�
// ���������ڼ��ֵ֮ǰ��Ҫ�����жϡ�
//
// ע�⣬������ʱ��Thread::Sleep �����ж��ѽ��á�
//----------------------------------------------------------------------

void Semaphore::P()
{
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
// �� P() һ�����˲���������ԭ�ӵģ�����������Ҫ�����жϡ�
//----------------------------------------------------------------------

void Semaphore::V()
{
    Thread* thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread*)queue->Remove();
    if (thread != NULL)       // ʹ�߳̾��������������� V
        scheduler->ReadyToRun(thread);
    value++;
    (void)interrupt->SetLevel(oldLevel);
}

// InterruptScope ���ʵ��
InterruptScope::InterruptScope(IntStatus next)
{
    old_ = interrupt->SetLevel(next);
}

InterruptScope::~InterruptScope()
{
    (void)interrupt->SetLevel(old_);
}

// Lock ���ʵ��
Lock::Lock(char* debugName):name(debugName), owner_(NULL), sem_(debugName, 1) {
}

Lock::~Lock() {
}

void Lock::Acquire() {
    sem_.P();
    owner_ = currentThread;
}

void Lock::Release() {
    sem_.V();
}

bool Lock::isHeldByCurrentThread()
{
    return owner_ == currentThread;
}

// Condition ���ʵ��
Condition::Condition(char* debugName):name(debugName), waitlist_("waitlist", 0), lock_("lock"), wait_(0) {
}

Condition::~Condition() {
}

void Condition::Wait(Lock* conditionLock) {
    ASSERT(conditionLock->isHeldByCurrentThread());
    {
        LockGuard _(lock_); // ʹ�� LockGuard �������ٽ���
        conditionLock->Release(); // �ͷ�������
        wait_++; // �ȴ����߳�����һ
        waitlist_.P(); // �ȴ�������������
        wait_--; // �ȴ����߳�����һ
    }
    conditionLock->Acquire(); // ���»�ȡ������
}

void Condition::Signal(Lock* conditionLock) {
    ASSERT(conditionLock->isHeldByCurrentThread());

    if (wait_ > 0) // ����еȴ����߳�
    {
        waitlist_.V(); // ����һ���ȴ����߳�
    }
}

void Condition::Broadcast(Lock* conditionLock) {
    ASSERT(conditionLock->isHeldByCurrentThread());

    for (int i = 0; i < wait_; i++) // �������еȴ����߳�
    {
        waitlist_.V();
    }
}
