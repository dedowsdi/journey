#ifndef HEAP_H
#define HEAP_H

#include <vector>
#include <stdexcept>
#include <functional>
#include "common.h"

namespace zxd {

// compare current and it's left, right children, swap and recursive into
// swapped children if necessary.
template <typename _It, typename CompareClass>
inline void heapify(_It beg, _It end, _It cur, const CompareClass& cmp) {
  if (cur >= end) throw new std::out_of_range("index out of range");

  // get left, right children
  unsigned i = cur - beg;
  _It l = beg + ((i << 1) + 1);
  _It r = l + 1;

  _It it = r < end && cmp(*r, *l) ? r : l;

  if (it < end && cmp(*it, *cur)) {
    std::swap(*cur, *it);

    // heapify down newly swapped value
    heapify(beg, end, it, cmp);
  }
}

template <typename _It, typename CompareClass>
inline void buildHeap(_It beg, _It end, const CompareClass& cmp) {
  unsigned size = end - beg;
  if (size == 0) return;

  // get last node that has child
  _It mid = beg + (size / 2 - 1);
  for (_It it = mid; it >= beg; --it) {
    heapify(beg, end, it, cmp);
  }
}

template <typename _It>
void heapSort(_It beg, _It end) {
  std::greater<typename _It::value_type> cmp;
  buildHeap(beg, end, cmp);
  //zxd::printBinaryIntTree(beg, end);
  for (_It iter = end - 1; iter > beg; --iter) {
    // move current max to the end
    std::swap(*beg, *iter);
    heapify(beg, iter, beg, cmp);
  }
}


template <typename T, typename Compare = std::less<T>>
class Heap {
protected:
  unsigned mSize;
  std::vector<T> mHeap;
  Compare mCmp;

public:
  typedef typename std::vector<T>::iterator iterator;

public:
  unsigned size() { return mSize; }

  inline T& at(unsigned i) {
    if (i >= mSize) throw new std::out_of_range("index out of range");
    return mHeap[i];
  }
  inline bool empty() { return mSize == 0; }

  inline T& top() {
    if (mSize == 0) throw new std::out_of_range("index out of range");
    return at(0);
  }

  iterator begin() { return mHeap.begin(); }
  iterator end() { return mHeap.begin() + mSize(); }

  template <typename _It>
  void assign(_It beg, _It end) {
    mHeap.assign(beg, end);
    mSize = end - beg;
    buildHeap(beg, end, mCmp);
  }

  T pop() {
    --mSize;
    // get top value
    iterator beg = begin();
    T value = *beg;
    // swap top and tail, then reheapify
    iterator tail = end() - 1;
    std::swap(*beg, *tail);
    heapify(beg);

    return value;
  }

protected:
  inline void heapify(unsigned i) {
    zxd::heapify(begin(), end(), begin() + i, mCmp);
  }
  inline void heapify(iterator iter) {
    zxd::heapify(begin(), end(), iter, mCmp);
  }
};
}

#endif /* HEAP_H */
