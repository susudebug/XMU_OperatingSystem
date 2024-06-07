#pragma once
#include "synch.h"

class BoundedBuffer {
public:
  // create a bounded buffer with a limit of 'maxsize' bytes
  BoundedBuffer(int maxsize);
  ~BoundedBuffer();

  // read 'size' bytes from the bounded buffer, storing into 'data'.
  // ('size' may be greater than 'maxsize')
  void Read(void* data, int size);

  // write 'size' bytes from 'data' into the bounded buffer.
  // ('size' may be greater than 'maxsize')
  void Write(void* data, int size);
private:

private:
  char* buffer_;
  int head_;
  int tail_;
  int size_;
  const int maxsize_;
  Lock* mutex_;
  Condition* not_full_;
  Condition* not_empty_;
};

