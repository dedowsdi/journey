#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include "string_util.h"
#include <boost/program_options.hpp>
#include "common.h"

namespace po = boost::program_options;

std::string file;
std::string searchType;
std::vector<int> data;
int value;

template <typename _It, class T>
_It upperBound(_It beg, _It end, T value) {
  if (beg == end) {
    return end;
  }

  _It a = beg;
  _It c = end - 1;
  _It b = a + (c - a) / 2;

  // special case
  if (*c < value) return end;
  if (*a > value) return a;

  while (c - a > 1) {
    if (*b > value) {
      c = b;
      b = a + (c - a) / 2;
    } else if (*b <= value) {
      a = b;
      b = a + (c - a) / 2;
    }
  }

  return c;
}

template <typename _It, class T>
_It lowerBound(_It beg, _It end, T value) {
  if (beg == end) {
    return end;
  }

  _It a = beg;
  _It c = end - 1;
  _It b = a + (c - a) / 2;

  // special case
  if (*c < value) return end;
  if (*a > value) return a;

  while (c - a > 1) {
    if (*b >= value) {
      c = b;
      b = a + (c - a) / 2;
    } else if (*b < value) {
      a = b;
      b = a + (c - a) / 2;
    } 
  }

  return c;
}

template <typename _It, class T>
_It binarySearch(_It beg, _It end, T value) {
  if (beg == end) {
    return end;
  }

  _It a = beg;
  _It c = end - 1;
  _It b = a + (c - a) / 2;

  // special case
  if (*c < value || *a > value) return end;
  if (*a == value) return a;
  if (*c == value) return c;

  /*
   * when a+1 = b = c-1
   *   if b < value
   *      a = b
   *      b = (a+b)/2, b won't change, the loop won't end if conditionis c > a
   */
  while (c - a > 1) {
    if (*b > value) {
      c = b;
      b = a + (c - a) / 2;
    } else if (*b < value) {
      a = b;
      b = a + (c - a) / 2;
    } else {
      return b;
    }
  }

  return end;
}

void doBinarySearch() {
  std::vector<int>::iterator iter =
    binarySearch(data.begin(), data.end(), value);

  if (iter == data.end()) {
    std::cout << value << " not found" << std::endl;
  } else {
    std::cout << "found " << value << " at line " << iter - data.begin() + 1
              << std::endl;
  }
}

void doBound() {
  std::vector<int>::iterator iter0 =
    lowerBound(data.begin(), data.end(), value);

  std::vector<int>::iterator iter1 =
    upperBound(data.begin(), data.end(), value);

  if (iter0 == data.end()) {
    std::cout << value << " not found" << std::endl;
  } else {
    std::cout << "found " << value << " between [" << iter0 - data.begin() + 1
              << "," << iter1 - data.begin() + 1 << ")" << std::endl;
  }
}

int main(int argc, char *argv[]) {

  po::options_description desc("Allowed options");

  // clang-format off
  desc.add_options()
    ("help,h", "produce help essage")
    ("file", po::value<std::string>(&file), "data file")
    ("value", po::value<int>(&value), "target")
    ("type,t", po::value<std::string>(&searchType)->default_value("binary"), "binary or bound");
  // clang-format on

  po::positional_options_description pod;
  pod.add("file", 1);
  pod.add("value", 1);

  po::variables_map vm;
  po::store(
    po::command_line_parser(argc, argv).options(desc).positional(pod).run(),
    vm);
  po::notify(vm);

  data = zxd::readFileToVector<int>(file);

  if (searchType == "binary") {
    doBinarySearch();
  } else if (searchType == "bound") {
    doBound();
  }

  return 0;
}
