#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include "common.h"
#include "timer.h"

namespace po = boost::program_options;
std::string inputFile;
std::string outputFile;
std::vector<int> data;

// works only for non negative int type
template <typename _It>
void countSort(_It beg, _It end) {
  if (beg == end) return;

  std::vector<typename _It::value_type> copy(beg, end);

  // init count vector
  typename _It::value_type k = *zxd::getMaximum(beg, end);
  std::vector<typename _It::value_type> count(k + 1, 0);  //+1 for maximum

  // set count[i] = number of value i in A
  std::for_each(
    copy.begin(), copy.end(), [&](decltype(*copy.begin()) v) { ++count[v]; });

  // set count[i] = number of value <= i in A
  for (int i = 1; i < count.size(); ++i) {
    count[i] += count[i - 1];
  }

  // place value one by one according to value of count[i]
  std::for_each(copy.begin(), copy.end(), [&](decltype(*copy.begin()) v) {
    // there are count[v] values <= v in current copied array
    *(beg + count[v] - 1) = v;
    --count[v];
  });
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
  countSort(data.begin(), data.end());
  std::cout << "sort tooks " << timer.time() << " seconds " << std::endl;
  zxd::writeFileFromVector(outputFile, data.begin(), data.end());
  if (!std::is_sorted(data.begin(), data.end())) {
    throw std::runtime_error("sort failed");
  }

  return 0;
}
