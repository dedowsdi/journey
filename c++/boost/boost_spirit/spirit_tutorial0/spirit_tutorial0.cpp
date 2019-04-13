#include <iostream>
#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;

int main(int argc, char *argv[])
{
  using namespace qi;
  std::string s = "{ 123 }";

  auto callback =  [](int i){ std::cout << i << std::endl; } ;
  // int_ is an parser, callback is an action
  auto r = qi::phrase_parse(s.begin(), s.end(), '{' >> int_[callback] >>'}', qi::ascii::space);
  std::cout << r << std::endl;
  return 0;
}
