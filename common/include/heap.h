#ifndef HEAP_H
#define HEAP_H

#include <vector>

namespace zxd {

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
};
}

#endif /* HEAP_H */
