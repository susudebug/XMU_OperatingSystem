#include "system.h"
#include "Table.h"

// 构造函数，初始化 Table 实例
Table::Table(int size):size_(size) {
    // 分配存储项的数组，并初始化为 null 指针
    items_ = new void* [size];
    // 创建互斥锁，并设置锁的名称为 "tablelock"
    lock_ = new Lock("tablelock");
}

// 析构函数，释放 Table 实例占用的资源
Table::~Table()
{
    // 释放存储项的数组内存
    delete[] items_;
    // 删除互斥锁对象
    delete lock_;
}

// 分配一个索引给对象，并将对象存储在该索引处
int Table::Alloc(void *object)
{
    // 使用互斥锁保护对共享资源的访问
    LockGuard _(lock_);
    // 遍历存储项数组
    for (int i = 0;i < size_;i++)
    {
        // 如果当前位置为空，则分配给该位置
        if (!items_[i])
        {
            items_[i] = object;
            return i; // 返回索引值
        }
    }
    return -1; // 如果无法找到可用位置，则返回 -1
}

// 获取指定索引处的对象
void* Table::Get(int index)
{
    // 如果索引值无效，则返回空指针
    if (index < 0 || index >= size_)
    {
        return NULL;
    }

    // 使用互斥锁保护对共享资源的访问
    LockGuard _(lock_);
    // 返回存储在指定索引处的对象
    return items_[index];
}

// 释放指定索引处的对象
void Table::Release(int index)
{
    // 使用互斥锁保护对共享资源的访问
    LockGuard _(lock_);
    // 将指定索引处的对象置为空指针
    items_[index] = NULL;
}
