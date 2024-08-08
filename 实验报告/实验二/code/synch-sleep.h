// synch.h 
// 用于线程同步的数据结构。
//
// 这里定义了三种同步方式：信号量、锁和条件变量。给出了信号量的实现；对于后两者，仅给出了过程接口 —— 它们将作为第一个任务的一部分实现。
//
// 请注意，所有同步对象在初始化时都需要一个“名称”。这仅用于调试目的。
//
// 版权所有（C）1992-1993 加利福尼亚大学理事会。保留所有权利。有关版权声明和责任限制的信息，请参阅copyright.h。

#ifndef SYNCH_H
#define SYNCH_H

#include "copyright.h"
#include "thread.h"
#include "list.h"
#include "interrupt.h"

// 下面的类定义了一个“信号量”，其值为非负整数。信号量只有两个操作：P() 和 V()：
//
// P() —— 等待直到 value > 0，然后递减
//
// V() —— 递增，必要时唤醒等待在 P() 中的线程
// 
// 请注意，接口不允许线程直接读取信号量的值（因为即使您读取了该值，您所知道的只是该值曾经是什么。您不知道当前的值，因为在您将值读入寄存器之后，可能发生了上下文切换，
// 并且其他一些线程可能已经调用了 P 或 V，所以真实的值现在可能已经不同了）。

class Semaphore {
public:
  Semaphore(char* debugName, int initialValue);  // 设置初始值
  ~Semaphore();                       // 释放信号量
  char* getName() { return name; }    // 调试辅助

  void P();    // 这是信号量的唯一操作
  void V();    // 它们都是 *原子操作*

private:
  char* name;        // 用于调试
  int value;         // 信号量值，始终 >= 0
  List* queue;       // 在 P() 中等待值 > 0 的线程队列
};

// 下面的类定义了一个“锁”。锁可以是 BUSY 或 FREE。
// 在锁上只允许进行两种操作：
//
// Acquire —— 等待锁变为 FREE，然后将其设置为 BUSY
//
// Release —— 将锁设置为 FREE，必要时唤醒等待在 Acquire 中的线程
//
// 此外，按照约定，只有获得锁的线程才能释放它。
// 与信号量一样，您不能读取锁值（因为在您读取值后，值可能立即改变）。

class Lock {
public:
  Lock(char* debugName);       // 将锁初始化为 FREE
  ~Lock();                      // 释放锁
  char* getName() { return name; }  // 调试辅助

  void Acquire(); // 这些是锁的唯一操作
  void Release(); // 它们都是 *原子操作*

  bool isHeldByCurrentThread();  // 如果当前线程
                                  // 持有此锁，则返回 true。用于
                                  // 在 Release 和条件变量操作中检查。

private:
  char* name;         // 用于调试
  Thread* owner_;     // 持有者
  bool locked_;       // 是否被锁住
  List* waitlist_;    // 等待队列
};

// 下面的类定义了一个“条件变量”。条件变量没有值，但线程可以排队等待变量。
// 对条件变量的操作仅有以下三种：
//
// Wait() —— 释放锁，让出CPU直到收到信号，
// 然后重新获取锁
//
// Signal() —— 唤醒一个等待在条件上的线程
//
// Broadcast() —— 唤醒所有等待在条件上的线程
//
// 所有对条件变量的操作必须在当前线程获取锁的情况下进行。
// 实际上，对给定条件变量的所有访问都必须由同一把锁保护。
// 换句话说，在调用条件变量操作的线程之间必须强制实现互斥。
//
// 在Nachos中，条件变量假定遵守 Mesa 风格的语义。当一个 Signal 或 Broadcast 唤醒另一个线程时，
// 它只是将线程放入就绪列表，唤醒线程重新获取锁的责任是在 Wait() 内部完成的。
// 相比之下，有些人根据 Hoare 风格的语义定义条件变量 —— 其中，唤醒线程让出锁和CPU给唤醒的线程，
// 后者立即运行并在离开临界区时将控制权还给发出信号的线程。
//
// 使用 Mesa 风格语义的结果是，某个其他线程可以获取锁，并在唤醒的线程有机会运行之前更改数据结构。

class Condition {
public:
  Condition(char* debugName);    // 将条件初始化为“没有线程等待”
  ~Condition();                  // 释放条件
  char* getName() { return (name); }

  void Wait(Lock* conditionLock);   // 这是条件变量的三个操作；
                                    // 在 Wait() 中释放锁和睡眠是 *原子操作*
  void Signal(Lock* conditionLock); // conditionLock 必须被
  void Broadcast(Lock* conditionLock); // 当前线程持有，所有这些操作

private:
  char* name;
  List* waitlist_;
};

// 中断范围类
class InterruptScope final {
public:
  InterruptScope(IntStatus next);
  ~InterruptScope();
  InterruptScope(const InterruptScope&) = delete;
  InterruptScope& operator=(const InterruptScope
