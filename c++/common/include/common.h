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
#include <memory>
#include <type_traits>
#include <typeinfo>
#ifndef _MSC_VER
#   include <cxxabi.h>
#endif

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

template <typename _It>
struct ContainerWrapper {
  _It beg, end;
  std::string delim;
  ContainerWrapper(_It b, _It e, const std::string& d = "\n")
      : beg(b), end(e), delim(d) {}
};

template <typename _It>
std::ostream& operator<<(std::ostream& os, const ContainerWrapper<_It>& w) {
  std::for_each(w.beg, w.end, [&](decltype(*w.beg) v) { os << v << w.delim; });
  return os;
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

// get random in [min, max)
inline float randomFloat(float min = 0, float max = 1) {
  static std::mt19937 engine;
  std::uniform_real_distribution<float> dist(min, max);
  return dist(engine);
}

// get min or max
template <typename _It, typename Compare>
inline _It getExtremeElement(_It beg, _It end, const Compare& cmp) {
  _It res = beg;
  for (_It iter = beg; iter != end; ++iter) {
    if (cmp(*iter, *res)) {
      res = iter;
    }
  }
  return res;
}

template <typename _It>
inline _It getMaximum(_It beg, _It end) {
  return getExtremeElement(beg, end, std::greater<typename _It::value_type>());
}

template <typename _It>
inline _It getMinimum(_It beg, _It end) {
  return getExtremeElement(beg, end, std::greater<typename _It::value_type>());
}

template <class T>
std::string type_name()
{
  typedef typename std::remove_reference<T>::type TR;
  std::unique_ptr<char, void(*)(void*)> own
  (
#ifndef _MSC_VER
   abi::__cxa_demangle(typeid(TR).name(), nullptr, nullptr, nullptr),
#else
   nullptr,
#endif
   std::free
  );
  std::string r = own != nullptr ? own.get() : typeid(TR).name();
  if (std::is_const<TR>::value)
    r += " const";
  if (std::is_volatile<TR>::value)
    r += " volatile";
  if (std::is_lvalue_reference<T>::value)
    r += "&";
  else if (std::is_rvalue_reference<T>::value)
    r += "&&";

  return r;
}

}

#endif /* COMMON_H */
