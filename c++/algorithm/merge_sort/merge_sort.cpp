#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include "common.h"
#include "timer.h"

namespace po = boost::program_options;
std::string inputFile;
std::string outputFile;
std::vector<int> data;

template <typename _It>
void merge(_It beg, _It mid, _It end) {

  std::vector<typename _It::value_type> lhs(beg, mid);
  std::vector<typename _It::value_type> rhs(mid, end);
  _It iterLeft = lhs.begin();  // index of left part
  _It iterLeftEnd = lhs.end();
  _It iterRight = rhs.begin();  // index of right part
  _It iterRightEnd = rhs.end();
  _It iter = beg;   // index of total part

  while (iter < end) {
    if (iterLeft == iterLeftEnd) {
      *iter++ = *iterRight++;
    } else if (iterRight == iterRightEnd) {
      *iter++ = *iterLeft++;
    } else if (*iterLeft <= *iterRight) {
      *iter++ = *iterLeft++;
    } else {
      *iter++ = *iterRight++;
    }
  }
}

template <typename _It>
void _sort(_It beg, _It end) {
  if (beg == end) return;

  unsigned int size = end - beg;

  if (size <= 1) return;
  if (size == 2) {
    _It next = beg + 1;
    if (*beg > *next) std::swap(*beg, *next);
    return;
  }

  // divide and conquer
  _It mid = beg + (size / 2);
  _sort(beg, mid);
  _sort(mid, end);
  merge(beg, mid, end);
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
  _sort(data.begin(), data.end());
  std::cout << "sort tooks " << timer.time() << " seconds " << std::endl;
  if (!std::is_sorted(data.begin(), data.end())) {
    throw std::runtime_error("sort failed");
  }
  zxd::writeFileFromVector(outputFile, data.begin(), data.end());

  return 0;
}
