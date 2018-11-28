/*
 * radix sort is not compare sort, it's faster than quick sort for int (or
 * float, string).
 *
 * radix sort:
 *   find the digits of the largest number
 *   loop each digits:
 *     place value into bucket according to current digit value
 *     take data from bucket, bottom to top, 0 to 15
 *     it's garenteed that after x times of bucket sort, the first x hex digets
 *     is sorted
 * 
 */
#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>

#include <boost/program_options.hpp>

#include "common.h"
#include "timer.h"

namespace po = boost::program_options;
std::string inputFile;
std::string outputFile;
typedef std::vector<int> IntVector;
typedef std::vector<IntVector> IntBuckets;
IntVector data;

void radixSort16(IntVector::iterator beg, IntVector::iterator end)
{
  // find the largest one
  int maxInt = *std::max_element(beg, end);

  int times = 8; // max bucket sort times
  while(!(maxInt >> (times - 1) * 4 & 0xf ))
    --times;

  std::cout << "max int : 0x" << std::hex << maxInt << " need " << times << " bucket sort" << std::endl;

  IntBuckets buckets(16);

  for (int i = 0; i < times; ++i) 
  {
    // find bucket by current hex digit value
    std::for_each(beg, end,
        [&](int value)
        {
          buckets[value >> i*4 & 0xf].push_back(value);
        });
    
    // take value from bucket, from bottom to top, 0 to 15
    auto iter = beg;
    for (auto& bucket : buckets) {
      iter = std::copy(bucket.begin(), bucket.end(), iter);
      bucket.clear(); // don't forget to clean it
    }

    if(iter != end)
      throw std::runtime_error("something is wrong");
  }
}

int main(int argc, char *argv[])
{
  po::options_description desc("allowed options");

  desc.add_options()
    ("help, h", "produce help message")
    ("inputFile, i", po::value<std::string>(&inputFile), "data file")
    ("outputFile, o", po::value<std::string>(&outputFile), "output file")
  ;

  po::positional_options_description pod;
  pod.add("inputFile", 1);
  pod.add("outputFile", 1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).positional(pod).run(), vm);
  po::notify(vm);

  if(vm.size() < 1)
    std::cout << "wrong command, it should be " << argv[0] << " input_file [output_file]" << std::endl;

  data = zxd::readFileToVector<int>(inputFile);
  auto dataCopy = data;

  zxd::Timer timer;
  radixSort16(data.begin(), data.end());
  std::cout << "radix sort tooks " << timer.miliseconds() << "ms" << std::endl;

  if(!std::is_sorted(data.begin(), data.end()))
    throw std::runtime_error("sort failed");

  timer.reset();
  std::sort(dataCopy.begin(), dataCopy.end());
  std::cout << "quick sort tooks " << timer.miliseconds() << "ms" << std::endl;

  if(!outputFile.empty())
    zxd::writeFileFromVector(outputFile, data.begin(), data.end());
  
  return 0;
}
