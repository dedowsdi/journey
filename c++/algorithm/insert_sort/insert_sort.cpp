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
void _sort(_It beg, _It end) {
  if (beg == end) return;

  // loop insert element
  for (_It i = beg + 1; i != end; ++i) {
    // shift elements forward, find insert spot. Just like shifting cards one by
    // one
    typename _It::value_type key = *i;
    _It j = i - 1;
    while (j != beg && *j > key) {
      *(j + 1) = *j;
      --j;
    }
    *(j + 1) = key;
  }
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
  zxd::writeFileFromVector(outputFile, data.begin(), data.end());

  return 0;
}
