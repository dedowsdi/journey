#include <iostream>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace bqi = boost::spirit::qi;

int main(int argc, char *argv[])
{

  // don't use auto
  // '"' is a char, that's why -'"' is needed, otherwise the trailing '"' will
  // never be matched
  bqi::rule<std::string::iterator, std::string()> rule = '"' >> *(bqi::char_-'"')  >> '"';
  std::string s;

  while(std::getline(std::cin, s))
  {
    auto beg = s.begin();
    auto end = s.end();

    std::string result;

    bool r = bqi::parse(beg, end, rule, result);
    if(r && beg == end)
      std::cout << result << std::endl;
    else
      std::cout << "parse failed" << std::endl;
  }
  
  return 0;
}
