#include "system.h"
#include "BoundedBuffer.h"

// 实现 BoundedBuffer 类的构造函数
BoundedBuffer::BoundedBuffer(int maxsize): maxsize_(maxsize), buffer_(new char[maxsize]), mutex_(new Lock("bblock")), not_empty_(new Condition("bbne")), not_full_(new Condition("bbnf")), head_(0), tail_(0), size_(0)
{
}

// 实现 BoundedBuffer 类的析构函数
BoundedBuffer::~BoundedBuffer()
{
    // 释放 buffer_ 数组占用的内存
    delete[] buffer_;
}

// 实现 BoundedBuffer 类的写方法 Write
void BoundedBuffer::Write(void* data, int size) {
    // 使用锁保护对共享资源的访问
    LockGuard g(mutex_);
    // 当缓冲区已满时，等待直到有空间可写
    while (!(size_ < maxsize_))
    {
        not_full_->Wait(mutex_);
    }
    // 将数据转换为字符指针
    char* data_ptr = (char*)(data);
    // 逐个将数据写入缓冲区
    for (int i = 0; i < size; i++) {
        buffer_[tail_] = data_ptr[i];
        tail_ = (tail_ + 1) % maxsize_;
        size_++;
    }
    // 唤醒等待数据读取的线程
    not_empty_->Broadcast(mutex_);
}

// 实现 BoundedBuffer 类的读方法 Read
void BoundedBuffer::Read(void* data, int size) {
    // 使用锁保护对共享资源的访问
    LockGuard g(mutex_);
    // 当缓冲区数据不足时，等待直到有足够数据可读
    while (!(size_ >= size))
    {
        not_empty_->Wait(mutex_);
    }
    // 将数据转换为字符指针
    char* data_ptr = (char*)(data);
    // 逐个从缓冲区读取数据
    for (int i = 0; i < size; i++) {
        data_ptr[i] = buffer_[head_];
        head_ = (head_ + 1) % maxsize_;
        size_--;
    }
    // 唤醒等待空闲空间的线程
    not_full_->Broadcast(mutex_);
}
