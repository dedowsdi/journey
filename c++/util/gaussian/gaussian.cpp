#include <iostream>
#include <boost/program_options.hpp>
#include <iomanip>
#include "zmath.h"

namespace po = boost::program_options;

double mean = 0;
double standardDeviation = 1;
double step = 1;
int count = 4;
bool blur = false;

int main(int argc, char *argv[]) {
  po::options_description desc("Allowed options");

  // clang-format off
  desc.add_options()
    ("help,h", "produce help essage")
    ("mean,m", po::value<double>(&mean), "mean, expectation")
    ("standardDeviation,d", po::value<double>(&standardDeviation), "standard derivation")
    ("step,s", po::value<double>(&step), "step")
    ("count,c", po::value<int>(&count), "count")
    ("blur,b", po::value<bool>(&blur)->default_value(false)->implicit_value(true), "weight[0] + 2(weight[1-count])");
  // clang-format on
  //
  po::positional_options_description pod;
  pod.add("step", 1);
  pod.add("count", 1);

  po::variables_map vm;
  po::store(
    po::command_line_parser(argc, argv).options(desc).positional(pod).run(),
    vm);
  po::notify(vm);

  if (vm.size() < 2) {
    std::cout << "wrong command, it should be " << argv[0]
              << " step count [option]" << std::endl;
    return 0;
  }

  std::cout << std::setprecision(6);
  std::vector<double> results;
  for (int i = 0; i < count; ++i) {
    results.push_back(zxd::ZMath::gaussian(step * i, mean, standardDeviation));
  }

  if (blur) {
    double r = results[0];
    std::for_each(results.begin() + 1, results.end(),
      [&](decltype(*results.begin()) v) { r += 2 * v; });
    r = 1 / r;
    std::for_each(results.begin(), results.end(),
      [&](decltype(*results.begin()) v) { v *= r; });
  }

  std::for_each(results.begin(), results.end(),
    [&](decltype(*results.begin()) v) { std::cout << v << std::endl; });

  return 0;
}
