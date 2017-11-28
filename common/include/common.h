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
#include <cmath>
#include <iomanip>

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

template <typename _It>
void print(_It beg, _It end, const std::string& delim = "\n") {
  std::for_each(beg, end, [&](decltype(*beg) v) { std::cout << v << delim; });
}

std::string centerText(
  const std::string& text, unsigned size, char fill = ' ') {
  if (text.size() > size) return text;

  float l = (size - text.size()) / 2;
  unsigned leftFill = std::floor(l);
  unsigned rightFill = std::ceil(l);
  std::string left(leftFill, fill);
  std::string right(rightFill, fill);

  std::stringstream ss;
  ss << left << text << right;
  return ss.str();
}

template <typename _It>
void printBinaryIntTree(_It beg, _It end) {
  int maxAbs = 0;
  std::for_each(beg, end, [&](decltype(*beg) v) {
    int a = std::abs(v);
    maxAbs = a > maxAbs ? a : maxAbs;
  });

  unsigned leaveWidth =
    std::ceil(std::log(maxAbs)) + 2;  // extra 2 for blank and -
  unsigned height = std::floor(std::log2(end - beg));
  unsigned totalWidth = std::pow(2, height) * leaveWidth;

  _It it = beg;
  for (int i = 0; i <= height; ++i) {
    unsigned nodeCount = std::pow(2, i);
    unsigned nodeWidth = totalWidth / (nodeCount);

    for (int j = 0; j < nodeCount && it != end; ++j, ++it) {
      std::stringstream ss;
      ss << *it;

      std::cout << std::setw(nodeWidth) << centerText(ss.str(), nodeWidth);
    }
    std::cout << std::endl;
  }
}
}

#endif /* COMMON_H */
