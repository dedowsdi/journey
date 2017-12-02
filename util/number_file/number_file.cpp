#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <random>
#include "common.h"
#include "string_util.h"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

std::string algorithm;
std::string file;
std::vector<int> data;
int start, end, count;
double abde = 5;

void randomData() {
  std::mt19937 mt;
  std::uniform_int_distribution<> dist(start, end);
  while (count--) {
    data.push_back(dist(mt));
  }
}

void randomSortData() {
  randomData();
  std::sort(data.begin(), data.end());
}

void sequenceData() {
  for (int i = start; i <= end; ++i) {
    data.push_back(i);
  }
}

int main(int argc, char *argv[]) {
  po::options_description desc("Allowed options");

  // clang-format off
  desc.add_options()
    ("help", "produce help essage")
    ("type", po::value<std::string>(&algorithm)->default_value("sequence"), "random,sequence,randomSorted")
    ("start", po::value<int>(&start)->default_value(0), "start number, included if count omited")
    ("end", po::value<int>(&end)->default_value(100), "end number, included if count omited")
    ("count", po::value<int>(&count)->default_value(100), "number counts")
    ("file", po::value<std::string>(&file)->default_value("number"),
    "target file name");
  // clang-format on

  po::positional_options_description pod;
  pod.add("start", 1);
  pod.add("end", 1);
  pod.add("file", 1);

  po::variables_map vm;
  // disable short to allow negative number
  po::store(po::command_line_parser(argc, argv)
              .options(desc)
              .positional(pod)
              .style(po::command_line_style::unix_style ^
                     po::command_line_style::allow_short)
              .run(),
    vm);
  po::notify(vm);

  if (!vm.count("count")) {
    count = end - start;
  }

  // overwrite existign one
  std::ofstream ofs(file, std::ios::out | std::ios::trunc);
  if (!ofs) {
    std::cerr << "faild to open " << file << std::endl;
    return 0;
  }

  data.reserve(count);
  if (algorithm == "randomSorted") {
    randomSortData();
  } else if (algorithm == "sequence") {
    sequenceData();
  } else if (algorithm == "random") {
    randomData();
  }
  std::copy(data.begin(), data.end(), std::ostream_iterator<int>(ofs, "\n"));
}
