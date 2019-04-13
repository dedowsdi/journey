#include <iostream>
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>

namespace bqi = boost::spirit::qi;
namespace bp = boost::phoenix;
namespace bpp = boost::phoenix::placeholders;

template <typename It>
bool adder(It beg, It end, double& n)
{

  bool r = bqi::phrase_parse(beg, end, 
       bqi::double_[bp::ref(n) = bqi::_1]
       >> *(',' >> bqi::double_[bp::ref(n) += bqi::_1]) // * means 0 or more
      ,
      bqi::space
      );

  return r && beg == end;
}

int main(int argc, char *argv[])
{

  std::string s;
  double d;
  std::cout << "input a bunch of numbers seperated by ," << std::endl;
  while(std::getline(std::cin, s))
  {
    if(adder(s.begin(), s.end(), d))
      std::cout << d << std::endl;
    else
      std::cout << "parse failed" << std::endl;
  }

  return 0;
}
