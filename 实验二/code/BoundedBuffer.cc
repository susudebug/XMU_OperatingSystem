#include "system.h"
#include "BoundedBuffer.h"

// ʵ�� BoundedBuffer ��Ĺ��캯��
BoundedBuffer::BoundedBuffer(int maxsize): maxsize_(maxsize), buffer_(new char[maxsize]), mutex_(new Lock("bblock")), not_empty_(new Condition("bbne")), not_full_(new Condition("bbnf")), head_(0), tail_(0), size_(0)
{
}

// ʵ�� BoundedBuffer �����������
BoundedBuffer::~BoundedBuffer()
{
    // �ͷ� buffer_ ����ռ�õ��ڴ�
    delete[] buffer_;
}

// ʵ�� BoundedBuffer ���д���� Write
void BoundedBuffer::Write(void* data, int size) {
    // ʹ���������Թ�����Դ�ķ���
    LockGuard g(mutex_);
    // ������������ʱ���ȴ�ֱ���пռ��д
    while (!(size_ < maxsize_))
    {
        not_full_->Wait(mutex_);
    }
    // ������ת��Ϊ�ַ�ָ��
    char* data_ptr = (char*)(data);
    // ���������д�뻺����
    for (int i = 0; i < size; i++) {
        buffer_[tail_] = data_ptr[i];
        tail_ = (tail_ + 1) % maxsize_;
        size_++;
    }
    // ���ѵȴ����ݶ�ȡ���߳�
    not_empty_->Broadcast(mutex_);
}

// ʵ�� BoundedBuffer ��Ķ����� Read
void BoundedBuffer::Read(void* data, int size) {
    // ʹ���������Թ�����Դ�ķ���
    LockGuard g(mutex_);
    // �����������ݲ���ʱ���ȴ�ֱ�����㹻���ݿɶ�
    while (!(size_ >= size))
    {
        not_empty_->Wait(mutex_);
    }
    // ������ת��Ϊ�ַ�ָ��
    char* data_ptr = (char*)(data);
    // ����ӻ�������ȡ����
    for (int i = 0; i < size; i++) {
        data_ptr[i] = buffer_[head_];
        head_ = (head_ + 1) % maxsize_;
        size_--;
    }
    // ���ѵȴ����пռ���߳�
    not_full_->Broadcast(mutex_);
}
