#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <tuple>
#include "common.h"
#include "timer.h"
#include <limits>

namespace po = boost::program_options;
std::string inputFile;
std::string outputFile;
std::vector<int> data;

// divide and conquer, it O(nlgn)
template <typename _It>
std::tuple<_It, _It, typename _It::value_type> maxCrossArray(
  _It beg, _It end, _It mid) {
  // static typename _It::value_type minimum =
  // std::numeric_limits<typename _It::value_type>::lowest();

  typename _It::value_type leftMax = *(mid - 1);
  typename _It::value_type leftSum = leftMax;
  _It leftIter = mid - 1;
  for (_It iter = leftIter - 1; iter >= beg; --iter) {
    leftSum += *iter;
    if (leftSum > leftMax) {
      leftMax = leftSum;
      leftIter = iter;
    }
  }

  typename _It::value_type rightMax = *mid;
  typename _It::value_type rightSum = rightMax;
  _It rightIter = mid;
  for (_It iter = rightIter + 1; iter < end; ++iter) {
    rightSum += *iter;
    if (rightSum > rightMax) {
      rightMax = rightSum;
      rightIter = iter;
    }
  }
  ++rightIter;  // past end
  return std::make_tuple(leftIter, rightIter, leftMax + rightMax);
}

template <typename _It>
std::tuple<_It, _It, typename _It::value_type> _maxSubArray(_It beg, _It end) {
  int size = end - beg;

  if (size == 1) return std::make_tuple(beg, end, *beg);

  // divide and conquer
  _It mid = beg + (size / 2);
  auto left = _maxSubArray(beg, mid);         // max sub array in left part
  auto right = _maxSubArray(mid, end);        // max sub array in right part
  auto cross = maxCrossArray(beg, end, mid);  // max sub array cross mid

  typename _It::value_type l = std::get<2>(left);
  typename _It::value_type r = std::get<2>(right);
  typename _It::value_type c = std::get<2>(cross);

  // std::cout << "l:" << l << " r:" << r << " c:" << c << std::endl;

  if (l >= r && l >= c) {
    return left;
  } else if (r >= l && r >= c) {
    return right;
  } else {
    return cross;
  }
}

/*
 * kadan's algorithm relies on the fact that:
 *   B(i+1) = max(A[i+1], B(i) + A[i+1])
 *   proof:
 *     assume [i,j] is boundary of B(i),
 *     boundary of B(i+1) will be [x, j + 1]
 *     if i < x < j, then sum of sub array [i, x] < 0, which means [x+1, j] 
 *     is boundary of B(i), that's out contradiction.
 *
 * assume B(i) is sum of max sub array that ends at i
 */
template <typename _It>
std::tuple<_It, _It, typename _It::value_type> maxSubArray(_It beg, _It end) {
  // special case : all negative numbers or positive numbers
  _It maxIter = end;
  for (_It iter = beg; iter != end; ++iter) {
    if (*maxIter < *iter) {
      maxIter = iter;
    }
    // stop looking if it's not special case
    if (*maxIter > 0) {
      break;
    }
  }

  // all non positive, simply return the biggest one as max sub array
  if (*maxIter <= 0) {
    return make_tuple(maxIter, maxIter + 1, *maxIter);
  }

  typename _It::value_type maxSoFar = 0;  // total max sum
  // max subarray that ends at current position
  typename _It::value_type maxEndingHere = 0;
  _It maxSoFarBeg = beg, maxSoFarEnd = beg, maxEndingHereBeg = beg,
      maxEndingHereEnd = beg;

  for (_It iter = beg; iter != end; ++iter) {
    maxEndingHere += *iter;

    if (maxEndingHere < 0) {
      // reset
      maxEndingHere = 0;
      maxEndingHereBeg = end;
      maxEndingHereEnd = end;
    } else {
      // update boundary of current sub array
      if (maxEndingHereBeg == end) {
        maxEndingHereBeg = iter;
      }
      maxEndingHereEnd = iter + 1;

      if (maxEndingHere > maxSoFar) {
        maxSoFar = maxEndingHere;
        maxSoFarBeg = maxEndingHereBeg;
        maxSoFarEnd = maxEndingHereEnd;
      }
    }
  }

  return std::make_tuple(maxSoFarBeg, maxSoFarEnd, maxSoFar);
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
  auto sa = maxSubArray(data.begin(), data.end());
  std::cout << "max_subarray tooks " << timer.time() << " seconds "
            << std::endl;

  std::cout << "[" << std::get<0>(sa) - data.begin() << ","
            << std::get<1>(sa) - data.begin() << "):" << std::get<2>(sa)
            << std::endl;

  zxd::writeFileFromVector(outputFile, std::get<0>(sa), std::get<1>(sa));

  return 0;
}
