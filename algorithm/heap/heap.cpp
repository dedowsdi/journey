#include <iostream>
#include <vector>
#include "heap.h"
#include "common.h"

int main(int argc, char *argv[]) {
  std::vector<int> v({5, 13, 2, 25, 7, 17, 20, 8, 4});
  zxd::buildMaxHeap(v.begin(), v.end());
  zxd::printBinaryIntTree(v.begin(), v.end());

  return 0;
}
