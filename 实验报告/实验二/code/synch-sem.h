// synch.h 
// 用于同步线程的数据结构。
//
// 这里定义了三种同步机制：信号量、锁和条件变量。对于信号量的实现已经给出；对于后两者，只提供了过程接口 - 它们将作为第一个作业的一部分实现。
//
// 请注意，所有的同步对象在初始化时都带有一个“name”参数。这仅用于调试目的。

#ifndef SYNCH_H
#define SYNCH_H

#include "copyright.h"
#include "thread.h"
#include "list.h"
#include "interrupt.h"

// 以下类定义了一个“信号量”，其值是非负整数。信号量只有两个操作 P() 和 V()：
//
// 	P() -- 等待直到值 > 0，然后递减
//
// 	V() -- 递增，如果需要，唤醒等待在 P() 中的线程
// 
// 请注意，接口*不允许*一个线程直接读取信号量的值 - 即使您读取了该值，您知道的只是该值曾经是什么。您不知道现在的值是多少，
// 因为在将值传递到寄存器之后，可能发生上下文切换，而其他线程可能调用了 P 或 V，因此真实的值现在可能已经不同了。

class Semaphore {
public:
  Semaphore(char* debugName, int initialValue);	// 设置初始值
  ~Semaphore();   					// 释放信号量
  char* getName() { return name; }			// 调试辅助

  void P();	 // 信号量的唯一操作
  void V();	 // 它们都是*原子操作*

private:
  char* name;        // 用于调试
  int value;         // 信号量值，始终 >= 0
  List* queue;       // 在 P() 中等待值 > 0 的线程
};

// 以下类定义了一个“锁”。锁可以是 BUSY 或 FREE。
// 锁只允许两种操作： 
//
//	Acquire -- 等待直到锁为 FREE，然后将其设置为 BUSY
//
//	Release -- 将锁设置为 FREE，唤醒等待在 Acquire 上的线程
//
// 此外，按照惯例，只有获得锁的线程才能释放它。与信号量类似，您不能读取锁的值
// （因为在读取后该值可能会立即更改）。

class Lock {
public:
  Lock(char* debugName);  		// 初始化锁为 FREE
  ~Lock();				// 释放锁
  char* getName() { return name; }	// 调试辅助

  void Acquire(); // 锁的唯一操作
  void Release(); // 它们都是*原子操作*

  bool isHeldByCurrentThread();	// 如果当前线程持有此锁，则返回 true。在 Release 中和 Condition 变量操作中检查时很有用。

private:
  char* name;				// 用于调试
  // 还有一些其他的东西你需要定义
  Thread* owner_;
  Semaphore sem_;
};

// 以下类定义了一个“条件变量”。条件变量没有值，但线程可以排队等待在变量上。
// 对条件变量的操作仅有以下几种： 
//
//	Wait() -- 释放锁，放弃 CPU 直到被信号唤醒，然后重新获取锁
//
//	Signal() -- 唤醒一个线程，如果有任何线程等待在条件上
//
//	Broadcast() -- 唤醒所有等待在条件上的线程
//
// 所有对条件变量的操作必须在当前线程获得锁的情况下进行。事实上，对给定条件变量的所有访问都必须由相同的锁保护。
// 换句话说，必须在调用条件变量操作的线程之间实现互斥。

// 在 Nachos 中，假定条件变量遵循 *Mesa* 风格的语义。当 Signal 或 Broadcast 唤醒另一个线程时，
// 它只是将线程放在就绪列表上，唤醒线程重新获取锁（在 Wait() 中已处理）。相比之下，
// 有些根据 *Hoare* 风格的语义来定义条件变量 -- 在这种情况下，唤醒线程立即运行并将控制权和 CPU 交给唤醒的线程，
// 唤醒的线程在离开临界区后将控制权返回给信号发送者。
//
// 使用 Mesa 风格语义的后果是，在唤醒的线程有机会运行之前，某些其他线程可能已经获得了锁并更改了数据结构。

class Condition {
public:
  Condition(char* debugName);		// 初始化条件为 "无人等待"
  ~Condition();			// 释放条件
  char* getName() { return (name); }

  void Wait(Lock* conditionLock); 	// 这些是条件变量的 3 个操作；释放锁和进入睡眠在 Wait() 中都是 *原子* 的
  void Signal(Lock* conditionLock);   // 这些操作都必须由当前线程持有的 conditionLock 进行
  void Broadcast(Lock* conditionLock);

private:
  char* name;
  // 还有一些其他的东西你需要定义
  Semaphore waitlist_; // 等待列表
  Lock lock_; // 条件变量的锁
  int wait_; // 等待的线程数
};

// InterruptScope 类的实现
class InterruptScope final {
public:
  InterruptScope(IntStatus next);
  ~InterruptScope();
  InterruptScope(const InterruptScope&) = delete;
  InterruptScope& operator=(const InterruptScope&) = delete;
private:
  IntStatus old_;
};

// LockGuard 类的实现
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
