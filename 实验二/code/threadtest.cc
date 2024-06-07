// threadtest.cc 
// 线程作业的简单测试用例。
//
// 创建两个线程，并通过调用 Thread::Yield 让它们之间进行上下文切换，
// 以说明线程系统的内部工作原理。

#include "copyright.h"
#include "system.h"
#include "dllist.h"
#include "hello.h"

// 在 dllist-driver.cc 中的函数声明
extern void Insert(DLList* L, int N, int whichThread);
extern void Remove(DLList* L, int N, int whichThread);

// 在 main.cc 中设置
int testnum = 1;       // 测试用例编号
int threadnum = 16;    // 线程数，默认值: 2
int itemnum = 6;       // 项数，默认值: 6
int errorType = 0;     // 错误类型

// 全局变量，指向双向链表
DLList* L;

//----------------------------------------------------------------------
// SimpleThread
// 循环 5 次，每次迭代都将 CPU 让给另一个准备好的线程。
//
// "which" 仅是一个用于识别线程的数字，用于调试目的。
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;

    for (num = 0; num < 5; num++) {
        printf("*** 线程 %d 循环了 %d 次\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// SimpleThread2
// 在这里操作双向链表。
//----------------------------------------------------------------------

void
SimpleThread2(int which)
{
    Insert(L, itemnum, which);
    Remove(L, itemnum, which);
}

//----------------------------------------------------------------------
// ThreadTest1
// 设置两个线程之间的来回 ping-pong，通过创建一个线程调用 SimpleThread，
// 然后调用 SimpleThread 自己。
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "进入 ThreadTest1");

    Thread* t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

//----------------------------------------------------------------------
// ThreadTest2
// 通过首先调用 Insert 创建一个双向链表，然后通过这些线程移除链表顶部的项。
//----------------------------------------------------------------------

char name[64] = { 0 };

void
ThreadTest2()
{
    DEBUG('t', "进入 ThreadTest2");

    // 分配链表
    L = new DLList();

    // fork 线程
    for (int i = 1; i < threadnum; ++i)
    {
        memset(name, 0, sizeof(name));
        snprintf(name, 64, "线程 %d", i);
        Thread* t = new Thread(name);
        t->Fork(SimpleThread2, i);
    }
    SimpleThread2(0);
}

#include "Table.h"

extern "C" int rand();

Table table(5);

void TestTableHandle(int which)
{
    int* object = new int, index;
    *object = rand();
    printf("将对象 %d 添加到表中，在线程 %d\n", *object, which);
    index = table.Alloc((void*)object);
    if (index != -1) {
        ASSERT(((int*)table.Get(index)) == object);
        printf("从表中获取对象 %d，在线程 %d\n", *(int*)(table.Get(index)), which);
        currentThread->Yield();
        table.Release(index);
    }
}

void TestTable()
{
    printf("测试 Table 类");

    // fork 线程
    for (int i = 1; i < threadnum; ++i)
    {
        memset(name, 0, sizeof(name));
        snprintf(name, 64, "线程 %d", i);
        Thread* t = new Thread(name);
        t->Fork(TestTableHandle, i);
    }
    TestTableHandle(0);
}

#include "BoundedBuffer.h"

BoundedBuffer *bb;

void TestBufferHandle(int which)
{
    int* buf = new int[16];
    // 一个线程写，其他线程消费数据
    if (which == 1) {
        printf("线程 %d 放入 15 个元素:", which);
        for (int i = 0;i < 16;i++)
        {
            buf[i] = rand();
            printf(" %d", buf[i]);
        }
        printf("\n");
        bb->Write(buf, 15);
    }
    else {
        printf("线程 %d 将获取 %d 个元素\n", which,which-1);
        bb->Read((void*)buf, which - 1);
        printf("线程 %d 中从缓冲区获取数据\n", which);
        for (int i = 0; i < which - 1; i++) {
            printf("%d ", buf[i]);
        }
        printf("\n");
    }
    delete[] buf;
}

void TestBuffer()
{
    bb = new BoundedBuffer(16);
    // fork 线程
    for (int i = 1; i < threadnum; ++i)
    {
        memset(name, 0, sizeof(name));
        snprintf(name, 64, "线程 %d", i);
        Thread* t = new Thread(name);
        t->Fork(TestBufferHandle, i);
    }
    TestBufferHandle(0);
}


//----------------------------------------------------------------------
// ThreadTest
// 调用一个测试例程。
//----------------------------------------------------------------------

void
ThreadTest()
{
    hello();
    switch (testnum) {
    case 1:
        ThreadTest1();
        break;
    case 2:
        ThreadTest2();	// 双向链表的测试在这里进行
        break;
    case 3:
        TestTable();
    case 4:
        TestBuffer();
    default:
        printf("未指定测试。\n");
        break;
    }
}
