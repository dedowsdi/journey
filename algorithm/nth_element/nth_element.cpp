#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include "common.h"
#include "timer.h"
#include <random>

namespace po = boost::program_options;
std::string inputFile;
std::string outputFile;
std::vector<int> data;
int nth;

// assume pivot at begin
template <typename _It>
inline _It partition(_It beg, _It end) {
  // create iterater at both ends
  typename _It::value_type key = *beg;
  _It left = beg + 1;
  _It right = end - 1;

  while (true) {
    // find greater one at left part
    while (*left < key) ++left;
    // find lesser one at right part
    while (*right > key) --right;

    if (left >= right)  // is > necessary ?
      break;

    // swap side
    std::swap(*left, *right);
    ++left;
    --right;
  }

  // place pivot
  std::swap(*beg, *right);
  return right;
}

template <typename _It>
inline _It nthElement(_It beg, _It end, unsigned index) {
  unsigned int size = end - beg;

  if (size < 1 || size < index + 1)
    return end;
  else if (size == 1)
    return beg;
  else if (size == 2) {
    _It next = beg + 1;
    if (*beg > *next) std::swap(*beg, *next);
    return beg + index;
  }

  // prepare pivot
  if (size >= 3) {
    _It third = beg + size * zxd::randomFloat();
    _It tail = end - 1;

    // find middle of 3
    _It pivot = beg;
    if (*third > *beg ^ *third > *tail) {
      pivot = third;
    } else if (*tail > *beg ^ *tail > *third) {
      pivot = tail;
    }

    // swap pivot to begin
    if (pivot != beg) {
      std::swap(*beg, *pivot);
    }
  }

  _It p = partition(beg, end);
  int i = p - beg;

  if (i == index)
    return p;
  else if (i > index)
    return nthElement(beg, p, index);
  else
    return nthElement(p + 1, end, index - i - 1);  // reset index for right part
}

int main(int argc, char *argv[]) {
  po::options_description desc("Allowed options");

  // clang-format off
  desc.add_options()
    ("help,h", "produce help essage")
    ("inputFile", po::value<std::string>(&inputFile), "data file")
    ("nth", po::value<int>(&nth), "nth");
  // clang-format on
  //
  po::positional_options_description pod;
  pod.add("inputFile", 1);
  pod.add("nth", 2);

  po::variables_map vm;
  po::store(
    po::command_line_parser(argc, argv).options(desc).positional(pod).run(),
    vm);
  po::notify(vm);

  if (vm.size() < 2) {
    std::cout << "wrong command, it should be " << argv[0] << " input_file nth"
              << std::endl;
    return 0;
  }

  data = zxd::readFileToVector<int>(inputFile);
  zxd::Timer timer;
  auto iter = nthElement(data.begin(), data.end(), nth);
  std::cout << "nth element tooks " << timer.time() << " seconds " << std::endl;
  std::cout << "nth element is " << *iter << std::endl;

  return 0;
}
