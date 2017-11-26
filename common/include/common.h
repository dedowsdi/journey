#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <random>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <sstream>

namespace zxd {

class RandomCompare {
public:
  RandomCompare() : mDist(0, 1) {}

  bool operator()(int lhs, int rhs) { return mDist(mRandEngine) == 1; }

protected:
  std::mt19937 mRandEngine;
  std::uniform_int_distribution<int> mDist;
};

template <typename T>
std::vector<T> readFileToVector(const std::string& file) {
  std::vector<T> data;
  std::ifstream ifs(file);
  if (!ifs) {
    std::stringstream ss;
    ss << "failed to open file " << file << std::endl;
    throw std::runtime_error(ss.str());
  }

  data.reserve(512);
  std::copy(std::istream_iterator<T>(ifs), std::istream_iterator<T>(),
    std::back_inserter(data));

  std::cout << "read " << data.size() << " items from  " << file << std::endl;
  return data;
}

template <typename _It>
void writeFileFromVector(
  const std::string& file, _It beg, _It end, const std::string& delim = "\n") {
  std::ofstream ofs(file, std::ios_base::out | std::ios_base::trunc);
  if (!ofs) {
    std::stringstream ss;
    ss << "failed to open file " << file << std::endl;
    throw std::runtime_error(ss.str());
    return;
  }

  std::copy(
    beg, end, std::ostream_iterator<typename _It::value_type>(ofs, "\n"));
}
}

#endif /* COMMON_H */
