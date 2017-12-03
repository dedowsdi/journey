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
inline void quickSort(_It beg, _It end) {

  unsigned int size = end - beg;

  if (size <= 1)
    return;
  else if (size == 2) {
    _It next = beg + 1;
    if (*beg > *next) std::swap(*beg, *next);
    return;
  }

  // prepare pivot
  if (size >= 3) {
    _It third = beg + size * zxd::randomFloat();
    _It tail = end - 1;

    //find middle of 3
    _It pivot = beg;
    if (*third > *beg ^ *third > *tail) {
      pivot = third;
    } else if (*tail > *beg ^ *tail > *third) {
      pivot = tail;
    }

    //swap pivot to begin
    if (pivot != beg) {
      std::swap(*beg, *pivot);
    }
  }

  _It p = partition(beg, end);

  // std::cout << "left ";
  // zxd::print(beg, mid, " ");
  // std::cout << " pivot " << *mid << " right ";
  // zxd::print(mid + 1, end, " ");
  // std::cout << std::endl;

  quickSort(beg, p);
  quickSort(p + 1, end);
}

int main(int argc, char *argv[]) {
  po::options_description desc("Allowed options");

  // clang-format off
  desc.add_options()
    ("help,h", "produce help essage")
    ("inputFile", po::value<std::string>(&inputFile), "data file")
    ("outputFile", po::value<std::string>(&outputFile), "output file");
  // clang-format on
  //
  po::positional_options_description pod;
  pod.add("inputFile", 1);
  pod.add("outputFile", 1);

  po::variables_map vm;
  po::store(
    po::command_line_parser(argc, argv).options(desc).positional(pod).run(),
    vm);
  po::notify(vm);

  if (vm.size() < 2) {
    std::cout << "wrong command, it should be " << argv[0]
              << " input_file output_file" << std::endl;
    return 0;
  }

  data = zxd::readFileToVector<int>(inputFile);
  zxd::Timer timer;
  quickSort(data.begin(), data.end());
  std::cout << "sort tooks " << timer.time() << " seconds " << std::endl;

  if (!std::is_sorted(data.begin(), data.end())) {
    throw std::runtime_error("sort failed");
  }

  zxd::writeFileFromVector(outputFile, data.begin(), data.end());

  return 0;
}
