#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>

namespace bqi = boost::spirit::qi;
namespace bp = boost::phoenix;
namespace bpp = boost::phoenix::placeholders;

// stuff numbers into vector
template <typename It>
bool parse_numbers(It beg, It end, std::vector<double>& v)
{

  bool r = bqi::phrase_parse(beg, end, 
       //bqi::double_[bp::push_back(bp::ref(v), bqi::_1)]
       //>> *(',' >> bqi::double_[bp::push_back(bp::ref(v), bqi::_1)])
       
       bqi::double_[bp::push_back(bp::ref(v), bqi::_1)] %',' //list parser, the same as above, read as a list of doulbe split by ,
      ,
      bqi::space
      );

  return r && beg == end;
}

int main(int argc, char *argv[])
{
  std::string s;
  std::vector<double> v;
  std::cout << "input a bunch of numbers seperated by ," << std::endl;
  while(std::getline(std::cin, s))
  {
    v.clear();
    if(parse_numbers(s.begin(), s.end(), v))
    {
      std::copy(v.begin(), v.end(), std::ostream_iterator<double>(std::cout, " "));
      std::cout  << std::endl;
    }
    else
      std::cout << "parse failed" << std::endl;
  }

  return 0;
}
