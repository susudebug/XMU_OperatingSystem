#include "system.h"
#include "Table.h"

// ���캯������ʼ�� Table ʵ��
Table::Table(int size):size_(size) {
    // ����洢������飬����ʼ��Ϊ null ָ��
    items_ = new void* [size];
    // ��������������������������Ϊ "tablelock"
    lock_ = new Lock("tablelock");
}

// �����������ͷ� Table ʵ��ռ�õ���Դ
Table::~Table()
{
    // �ͷŴ洢��������ڴ�
    delete[] items_;
    // ɾ������������
    delete lock_;
}

// ����һ�����������󣬲�������洢�ڸ�������
int Table::Alloc(void *object)
{
    // ʹ�û����������Թ�����Դ�ķ���
    LockGuard _(lock_);
    // �����洢������
    for (int i = 0;i < size_;i++)
    {
        // �����ǰλ��Ϊ�գ���������λ��
        if (!items_[i])
        {
            items_[i] = object;
            return i; // ��������ֵ
        }
    }
    return -1; // ����޷��ҵ�����λ�ã��򷵻� -1
}

// ��ȡָ���������Ķ���
void* Table::Get(int index)
{
    // �������ֵ��Ч���򷵻ؿ�ָ��
    if (index < 0 || index >= size_)
    {
        return NULL;
    }

    // ʹ�û����������Թ�����Դ�ķ���
    LockGuard _(lock_);
    // ���ش洢��ָ���������Ķ���
    return items_[index];
}

// �ͷ�ָ���������Ķ���
void Table::Release(int index)
{
    // ʹ�û����������Թ�����Դ�ķ���
    LockGuard _(lock_);
    // ��ָ���������Ķ�����Ϊ��ָ��
    items_[index] = NULL;
}
