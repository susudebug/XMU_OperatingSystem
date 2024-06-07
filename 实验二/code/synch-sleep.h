// synch.h 
// �����߳�ͬ�������ݽṹ��
//
// ���ﶨ��������ͬ����ʽ���ź��������������������������ź�����ʵ�֣����ں����ߣ��������˹��̽ӿ� ���� ���ǽ���Ϊ��һ�������һ����ʵ�֡�
//
// ��ע�⣬����ͬ�������ڳ�ʼ��ʱ����Ҫһ�������ơ���������ڵ���Ŀ�ġ�
//
// ��Ȩ���У�C��1992-1993 ���������Ǵ�ѧ���»ᡣ��������Ȩ�����йذ�Ȩ�������������Ƶ���Ϣ�������copyright.h��

#ifndef SYNCH_H
#define SYNCH_H

#include "copyright.h"
#include "thread.h"
#include "list.h"
#include "interrupt.h"

// ������ඨ����һ�����ź���������ֵΪ�Ǹ��������ź���ֻ������������P() �� V()��
//
// P() ���� �ȴ�ֱ�� value > 0��Ȼ��ݼ�
//
// V() ���� ��������Ҫʱ���ѵȴ��� P() �е��߳�
// 
// ��ע�⣬�ӿڲ������߳�ֱ�Ӷ�ȡ�ź�����ֵ����Ϊ��ʹ����ȡ�˸�ֵ������֪����ֻ�Ǹ�ֵ������ʲô������֪����ǰ��ֵ����Ϊ������ֵ����Ĵ���֮�󣬿��ܷ������������л���
// ��������һЩ�߳̿����Ѿ������� P �� V��������ʵ��ֵ���ڿ����Ѿ���ͬ�ˣ���

class Semaphore {
public:
  Semaphore(char* debugName, int initialValue);  // ���ó�ʼֵ
  ~Semaphore();                       // �ͷ��ź���
  char* getName() { return name; }    // ���Ը���

  void P();    // �����ź�����Ψһ����
  void V();    // ���Ƕ��� *ԭ�Ӳ���*

private:
  char* name;        // ���ڵ���
  int value;         // �ź���ֵ��ʼ�� >= 0
  List* queue;       // �� P() �еȴ�ֵ > 0 ���̶߳���
};

// ������ඨ����һ������������������ BUSY �� FREE��
// ������ֻ����������ֲ�����
//
// Acquire ���� �ȴ�����Ϊ FREE��Ȼ��������Ϊ BUSY
//
// Release ���� ��������Ϊ FREE����Ҫʱ���ѵȴ��� Acquire �е��߳�
//
// ���⣬����Լ����ֻ�л�������̲߳����ͷ�����
// ���ź���һ���������ܶ�ȡ��ֵ����Ϊ������ȡֵ��ֵ���������ı䣩��

class Lock {
public:
  Lock(char* debugName);       // ������ʼ��Ϊ FREE
  ~Lock();                      // �ͷ���
  char* getName() { return name; }  // ���Ը���

  void Acquire(); // ��Щ������Ψһ����
  void Release(); // ���Ƕ��� *ԭ�Ӳ���*

  bool isHeldByCurrentThread();  // �����ǰ�߳�
                                  // ���д������򷵻� true������
                                  // �� Release ���������������м�顣

private:
  char* name;         // ���ڵ���
  Thread* owner_;     // ������
  bool locked_;       // �Ƿ���ס
  List* waitlist_;    // �ȴ�����
};

// ������ඨ����һ������������������������û��ֵ�����߳̿����Ŷӵȴ�������
// �����������Ĳ��������������֣�
//
// Wait() ���� �ͷ������ó�CPUֱ���յ��źţ�
// Ȼ�����»�ȡ��
//
// Signal() ���� ����һ���ȴ��������ϵ��߳�
//
// Broadcast() ���� �������еȴ��������ϵ��߳�
//
// ���ж����������Ĳ��������ڵ�ǰ�̻߳�ȡ��������½��С�
// ʵ���ϣ��Ը����������������з��ʶ�������ͬһ����������
// ���仰˵���ڵ������������������߳�֮�����ǿ��ʵ�ֻ��⡣
//
// ��Nachos�У����������ٶ����� Mesa �������塣��һ�� Signal �� Broadcast ������һ���߳�ʱ��
// ��ֻ�ǽ��̷߳�������б������߳����»�ȡ������������ Wait() �ڲ���ɵġ�
// ���֮�£���Щ�˸��� Hoare �������嶨���������� ���� ���У������߳��ó�����CPU�����ѵ��̣߳�
// �����������в����뿪�ٽ���ʱ������Ȩ���������źŵ��̡߳�
//
// ʹ�� Mesa �������Ľ���ǣ�ĳ�������߳̿��Ի�ȡ�������ڻ��ѵ��߳��л�������֮ǰ�������ݽṹ��

class Condition {
public:
  Condition(char* debugName);    // ��������ʼ��Ϊ��û���̵߳ȴ���
  ~Condition();                  // �ͷ�����
  char* getName() { return (name); }

  void Wait(Lock* conditionLock);   // ������������������������
                                    // �� Wait() ���ͷ�����˯���� *ԭ�Ӳ���*
  void Signal(Lock* conditionLock); // conditionLock ���뱻
  void Broadcast(Lock* conditionLock); // ��ǰ�̳߳��У�������Щ����

private:
  char* name;
  List* waitlist_;
};

// �жϷ�Χ��
class InterruptScope final {
public:
  InterruptScope(IntStatus next);
  ~InterruptScope();
  InterruptScope(const InterruptScope&) = delete;
  InterruptScope& operator=(const InterruptScope
