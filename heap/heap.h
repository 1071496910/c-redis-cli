
#ifndef _C_REDIS_CLI_HEAP_HEAP_H
#define _C_REDIS_CLI_HEAP_HEAP_H

#include <strings.h>

template <typename T, int MAXSIZE> class MinHeap {
  T *array_[MAXSIZE];
  int size_;
  int current_index_;
  int capability_;

public:
  MinHeap();

  ~MinHeap();

public:
  int Add(const T &t);
  int Delete(const T &t);
  int GetLeftIndex();
  int GetRightIndex();
  T Top();
  T &Pop();
};

#endif //  _C_REDIS_CLI_HEAP_HEAP_H