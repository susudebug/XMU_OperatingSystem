// synch.cc 
// 用于同步线程的例程。这里定义了三种同步例程：信号量、锁和条件变量（后两者的实现留给读者完成）。
//
// 任何同步例程的实现都需要一些基本的原子操作。我们假设 Nachos 在单处理器上运行，因此原子性可以通过禁用中断来实现。
// 当中断被禁用时，不会发生上下文切换，因此当前线程可以保证在整个操作期间持有 CPU，直到中断被重新启用。
//
// 因为某些例程可能在中断已被禁用的情况下被调用（例如 Semaphore::V），所以我们不会在原子操作结束时打开中断，
// 而是简单地将中断状态恢复到其原始值（无论它是已禁用还是已启用）。

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 初始化信号量，以便用于同步。
//
// "debugName" 是一个任意的名称，用于调试。
// "initialValue" 是信号量的初始值。
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::~Semaphore
// 当不再需要时，释放信号量。假设没有人仍在等待信号量！
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 等待直到信号量值 > 0，然后递减。检查值和递减必须是原子的，
// 所以我们在检查值之前需要禁用中断。
//
// 注意，当调用时，Thread::Sleep 假设中断已禁用。
//----------------------------------------------------------------------

void Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);    // 禁用中断

    while (value == 0) {            // 信号量不可用
        queue->Append((void*)currentThread);    // 所以进入睡眠
        currentThread->Sleep();
    }
    value--;                    // 信号量可用，消耗其值

    (void)interrupt->SetLevel(oldLevel);    // 重新启用中断
}

//----------------------------------------------------------------------
// Semaphore::V
// 递增信号量值，必要时唤醒等待者。
// 与 P() 一样，此操作必须是原子的，所以我们需要禁用中断。
//----------------------------------------------------------------------

void Semaphore::V()
{
    Thread* thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread*)queue->Remove();
    if (thread != NULL)       // 使线程就绪，并立即消耗 V
        scheduler->ReadyToRun(thread);
    value++;
    (void)interrupt->SetLevel(oldLevel);
}

// InterruptScope 类的实现
InterruptScope::InterruptScope(IntStatus next)
{
    old_ = interrupt->SetLevel(next);
}

InterruptScope::~InterruptScope()
{
    (void)interrupt->SetLevel(old_);
}

// Lock 类的实现
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

// Condition 类的实现
Condition::Condition(char* debugName):name(debugName), waitlist_("waitlist", 0), lock_("lock"), wait_(0) {
}

Condition::~Condition() {
}

void Condition::Wait(Lock* conditionLock) {
    ASSERT(conditionLock->isHeldByCurrentThread());
    {
        LockGuard _(lock_); // 使用 LockGuard 来保护临界区
        conditionLock->Release(); // 释放条件锁
        wait_++; // 等待的线程数加一
        waitlist_.P(); // 等待在条件变量上
        wait_--; // 等待的线程数减一
    }
    conditionLock->Acquire(); // 重新获取条件锁
}

void Condition::Signal(Lock* conditionLock) {
    ASSERT(conditionLock->isHeldByCurrentThread());

    if (wait_ > 0) // 如果有等待的线程
    {
        waitlist_.V(); // 唤醒一个等待的线程
    }
}

void Condition::Broadcast(Lock* conditionLock) {
    ASSERT(conditionLock->isHeldByCurrentThread());

    for (int i = 0; i < wait_; i++) // 唤醒所有等待的线程
    {
        waitlist_.V();
    }
}
