// threadtest.cc 
// �߳���ҵ�ļ򵥲���������
//
// ���������̣߳���ͨ������ Thread::Yield ������֮������������л���
// ��˵���߳�ϵͳ���ڲ�����ԭ��

#include "copyright.h"
#include "system.h"
#include "dllist.h"
#include "hello.h"

// �� dllist-driver.cc �еĺ�������
extern void Insert(DLList* L, int N, int whichThread);
extern void Remove(DLList* L, int N, int whichThread);

// �� main.cc ������
int testnum = 1;       // �����������
int threadnum = 16;    // �߳�����Ĭ��ֵ: 2
int itemnum = 6;       // ������Ĭ��ֵ: 6
int errorType = 0;     // ��������

// ȫ�ֱ�����ָ��˫������
DLList* L;

//----------------------------------------------------------------------
// SimpleThread
// ѭ�� 5 �Σ�ÿ�ε������� CPU �ø���һ��׼���õ��̡߳�
//
// "which" ����һ������ʶ���̵߳����֣����ڵ���Ŀ�ġ�
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;

    for (num = 0; num < 5; num++) {
        printf("*** �߳� %d ѭ���� %d ��\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// SimpleThread2
// ���������˫������
//----------------------------------------------------------------------

void
SimpleThread2(int which)
{
    Insert(L, itemnum, which);
    Remove(L, itemnum, which);
}

//----------------------------------------------------------------------
// ThreadTest1
// ���������߳�֮������� ping-pong��ͨ������һ���̵߳��� SimpleThread��
// Ȼ����� SimpleThread �Լ���
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "���� ThreadTest1");

    Thread* t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

//----------------------------------------------------------------------
// ThreadTest2
// ͨ�����ȵ��� Insert ����һ��˫������Ȼ��ͨ����Щ�߳��Ƴ����������
//----------------------------------------------------------------------

char name[64] = { 0 };

void
ThreadTest2()
{
    DEBUG('t', "���� ThreadTest2");

    // ��������
    L = new DLList();

    // fork �߳�
    for (int i = 1; i < threadnum; ++i)
    {
        memset(name, 0, sizeof(name));
        snprintf(name, 64, "�߳� %d", i);
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
    printf("������ %d ��ӵ����У����߳� %d\n", *object, which);
    index = table.Alloc((void*)object);
    if (index != -1) {
        ASSERT(((int*)table.Get(index)) == object);
        printf("�ӱ��л�ȡ���� %d�����߳� %d\n", *(int*)(table.Get(index)), which);
        currentThread->Yield();
        table.Release(index);
    }
}

void TestTable()
{
    printf("���� Table ��");

    // fork �߳�
    for (int i = 1; i < threadnum; ++i)
    {
        memset(name, 0, sizeof(name));
        snprintf(name, 64, "�߳� %d", i);
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
    // һ���߳�д�������߳���������
    if (which == 1) {
        printf("�߳� %d ���� 15 ��Ԫ��:", which);
        for (int i = 0;i < 16;i++)
        {
            buf[i] = rand();
            printf(" %d", buf[i]);
        }
        printf("\n");
        bb->Write(buf, 15);
    }
    else {
        printf("�߳� %d ����ȡ %d ��Ԫ��\n", which,which-1);
        bb->Read((void*)buf, which - 1);
        printf("�߳� %d �дӻ�������ȡ����\n", which);
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
    // fork �߳�
    for (int i = 1; i < threadnum; ++i)
    {
        memset(name, 0, sizeof(name));
        snprintf(name, 64, "�߳� %d", i);
        Thread* t = new Thread(name);
        t->Fork(TestBufferHandle, i);
    }
    TestBufferHandle(0);
}


//----------------------------------------------------------------------
// ThreadTest
// ����һ���������̡�
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
        ThreadTest2();	// ˫������Ĳ������������
        break;
    case 3:
        TestTable();
    case 4:
        TestBuffer();
    default:
        printf("δָ�����ԡ�\n");
        break;
    }
}
