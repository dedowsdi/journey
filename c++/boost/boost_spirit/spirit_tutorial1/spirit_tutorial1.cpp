#include <iostream>
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>

template <typename It>
bool parse_complex(It beg, It end, std::complex<double>& c)
{

  using boost::spirit::qi::double_;
  using boost::spirit::qi::_1;
  using boost::spirit::qi::phrase_parse;
  using boost::spirit::qi::ascii::space;
  using boost::phoenix::ref;

  double rN = 0.0;
  double iN = 0.0;
  std::string s = "(2,3)";
  // use phoenix ref as action, qi::_1 as a placehould for attribute value
  bool r = phrase_parse(beg, end,
      '(' >> double_[ref(rN) = _1]
          >> -(',' >> double_[ref(iN) = _1]) >> ')' // - means match 0 or 1
          | double_[ref(rN) = _1],
      space);

  if(!r || beg != end)
    return false;
  c.real(rN);
  c.imag(iN);
  return true;
}

void print_complex(const std::complex<double>& c)
{
  std::cout << c.real() << ":" << c.imag() << std::endl;
}

int main(int argc, char *argv[])
{
  std::complex<double> c;

  std::string s;
  std::cout << "input (a,b) or a" << std::endl;
  while(std::getline(std::cin, s))
  {
    if(parse_complex(s.begin(), s.end(), c))
      print_complex(c);
    else
      std::cout << "parse failed" << std::endl;
  }
  
  return 0;
}
