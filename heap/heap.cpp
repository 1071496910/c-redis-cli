#include "heap.h"

template <typename T, int MAXSIZE>
MinHeap<T, MAXSIZE>::MinHeap()
    : size_(0), current_index_(0), capability_(MAXSIZE) {
  bzero(array_, MAXSIZE * (sizeof(T)));
}

template <typename T, int MAXSIZE> MinHeap<T, MAXSIZE>::~MinHeap() {
  for (int i = 0; i < size_; i++) {
    delete array_[i];
  }
}

template <typename T, int MAXSIZE> int MinHeap<T, MAXSIZE>::Add(const T &t) {
  if (size_ == 0) {
    array_[0] = new T(t);
  }
}

template <typename T, int MAXSIZE> int MinHeap<T, MAXSIZE>::GetLeftIndex() {
  int left_index = current_index_ * 2 + 1;
  return left_index > MAXSIZE ? -1 : left_index;
}

template <typename T, int MAXSIZE> int MinHeap<T, MAXSIZE>::GetRightIndex() {
  int right_index = current_index_ * 2 + 2;
  return right_index > MAXSIZE ? -1 : right_index;
}

template <typename T, int MAXSIZE> T MinHeap<T, MAXSIZE>::Top() {
  if (size_ > 0) {
    return T(array_[0]);
  }
  return T();
}

template <typename T, int MAXSIZE> T& MinHeap<T, MAXSIZE>::Pop() {
  if (size_ > 0) {
    return T(array_[0]);
  }
  return T();
}