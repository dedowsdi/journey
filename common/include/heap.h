#ifndef HEAP_H
#define HEAP_H

#include <vector>
#include <stdexcept>
#include <functional>

namespace zxd {

template <typename _It>
void maxHeapify(_It beg, _It end, _It cur) {
  if (cur >= end) {
    throw new std::out_of_range("index out of range");
  }

  unsigned i = cur - beg;
  _It l = beg + ((i << 1) + 1);
  _It r = l + 1;

  _It it = l;

  if (r < end) { 
    it = *r > *l ? r : l;
  }

  if (it < end && *cur < *it) {
    std::swap(*cur, *it);
    maxHeapify(beg, end, it);
  }
}

template <typename _It>
void buildMaxHeap(_It beg, _It end) {
  unsigned size = end - beg;
  if (size == 0) return;

  // get last node that has child
  _It mid = beg + (size / 2 - 1);
  for (_It it = mid; it >= beg; --it) {
    maxHeapify(beg, end, it);
  }
}

template <typename T>
class MaxHeap {
protected:
  std::vector<T> mHeap;
  unsigned mSize;

public:
  unsigned size() { return mSize; }

  inline T& at(unsigned i) { return mHeap[i]; }

  inline T* parent(unsigned i) {
    i >>= 1;
    return i >= 1 ? at(i) : 0;
  }

  inline T* left(unsigned i) {
    i <<= 1;
    return i <= mSize ? at(i) : 0;
  }

  inline T* right(unsigned i) {
    i = (i << 1) + 1;
    return i <= mSize ? at(i) : 0;
  }

  inline bool empty() { return mSize == 0; }

  inline T& top() {
    throwEmpty();
    return at(1);
  }

protected:
  void heapify(unsigned i) {
    unsigned l = i << 1;
    unsigned r = (i << 1) + 1;
    T& iv = at(i);

    if (l <= mSize && at(l) > iv) {
      std::swap(at(l), iv);
      heapify(l);
    } else if (r <= mSize && at(r) > iv) {
      std::swap(at(r), iv);
      heapify(r);
    }
  }

  inline void throwEmpty() {
    if (mSize == 0) {
      throw std::out_of_range("empty heap");
    }
  }
};

template <typename _It>
void heapSort(_It beg, _It end) {
  buildMaxHeap(beg, end);
  for (_It iter = end - 1; iter > beg; --iter) {
    // move current max to the end
    std::swap(*beg, *end);
    maxHeapify(beg, iter, beg);
  }
}
}

#endif /* HEAP_H */
