// synch.cc
// 用于线程同步的例程。这里定义了三种同步例程：信号量、锁和条件变量（后两者的实现留给读者完成）。
//
// 任何同步例程的实现都需要一些原子操作。我们假设Nachos运行在单处理器上，因此原子性可以通过禁用中断来提供。
// 在中断被禁用时，不会发生上下文切换，因此当前线程可以确保在整个过程中持有CPU，直到重新启用中断。
//
// 由于某些情况下这些例程可能在中断已禁用的情况下被调用（例如Semaphore::V），所以我们始终简单地将中断状态重新设置回其原始值
// （无论其是否已禁用）而不是在原子操作结束时打开中断。
//
// 版权所有（C）1992-1993 加利福尼亚大学理事会。保留所有权利。有关版权声明和责任限制的信息，请参阅copyright.h。

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 初始化信号量，以便用于同步。
//
// “debugName”是一个任意名称，用于调试。
// “initialValue”是信号量的初始值。
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue) {
	name = debugName;
	value = initialValue;
	queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::~Semaphore
// 当不再需要时，释放信号量。假设没有人仍在等待信号量！
//----------------------------------------------------------------------

Semaphore::~Semaphore() {
	delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 等待直到信号量值 > 0，然后递减。必须原子地检查值并递减，因此我们需要在检查值之前禁用中断。
//
// 请注意，Thread::Sleep 假设在调用时中断被禁用。
//----------------------------------------------------------------------

void Semaphore::P() {
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
// 与P()一样，此操作必须是原子的，因此我们需要禁用中断。
//----------------------------------------------------------------------

void Semaphore::V() {
	Thread* thread;
	IntStatus oldLevel = interrupt->SetLevel(IntOff);

	thread = (Thread*)queue->Remove();
	if (thread != NULL)       // 使线程就绪，并立即消耗V
		scheduler->ReadyToRun(thread);
	value++;
	(void)interrupt->SetLevel(oldLevel);
}

// InterruptScope 类实现
InterruptScope::InterruptScope(IntStatus next) {
	old_ = interrupt->SetLevel(next);
}

InterruptScope::~InterruptScope() {
	(void)interrupt->SetLevel(old_);
}

// 锁的实现
Lock::Lock(char* debugName): name(debugName), owner_(NULL), sem_(debugName, 1), locked_(false) {
	waitlist_ = new List();
}

Lock::~Lock() {
	delete waitlist_;
}

void Lock::Acquire() {
	IntStatus oldLevel = interrupt->SetLevel(IntOff); // 禁用中断
	while (locked_) { // 当锁已被锁住时
		waitlist_->Append(currentThread); // 将当前线程添加到等待队列
		currentThread->Sleep(); // 将当前线程置于睡眠状态
	}
	locked_ = true; // 锁住锁
	owner_ = currentThread; // 将持有者设置为当前线程
	interrupt->SetLevel(oldLevel); // 重新启用中断
}

void Lock::Release() {
	IntStatus oldLevel = interrupt->SetLevel(IntOff); // 禁用中断
	ASSERT(isHeldByCurrentThread()); // 确保当前线程持有锁
	locked_ = false; // 解锁锁
	owner_ = NULL; // 将持有者设置为空
	if (!waitlist_->IsEmpty()) { // 如果等待队列不为空
		Thread* thread = waitlist_->Remove(); // 从等待队列中移除第一个线程
		scheduler->ReadyToRun(thread); // 将线程放入就绪队列
	}
	interrupt->SetLevel(oldLevel); // 重新启用中断
}

bool Lock::isHeldByCurrentThread() {
	return owner_ == currentThread;
}

// 条件变量的实现
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
	ASSERT(conditionLock->isHeldByCurrentThread()); // 确保当前线程持有锁
	IntStatus oldLevel = interrupt->SetLevel(IntOff); // 禁用中断
	if (!waitlist_->IsEmpty()) { // 如果等待队列不为空
		Thread* thread = (Thread*)waitlist_->Remove(); // 从等待队列中移除第一个线程
		scheduler->ReadyToRun(thread); // 将线程放入就绪队列
	}
	interrupt->SetLevel(oldLevel); // 重新启用中断
}

void Condition::Broadcast(Lock* conditionLock) {
    ASSERT(conditionLock->isHeldByCurrentThread()); // 确保当前线程持有锁

    IntStatus oldLevel = interrupt->SetLevel(IntOff); // 保存当前中断状态并禁用中断

    while (!waitlist_->IsEmpty()) { // 当等待队列不为空时
        Thread* thread =  (Thread*)waitlist_->Remove(); // 从等待队列中移除第一个线程
        scheduler->ReadyToRun(thread); // 将线程放入就绪队列
    }

    interrupt->SetLevel(oldLevel); // 恢复中断状态
}
