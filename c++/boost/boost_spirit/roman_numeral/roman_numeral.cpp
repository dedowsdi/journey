#include <iostream>
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix/operator.hpp>

namespace bqi = boost::spirit::qi;
namespace bp = boost::phoenix;
namespace bpp = boost::phoenix::placeholders;

// map sequence of char to unsigned
struct hundreds_ : public bqi::symbols<char, unsigned>
{
  hundreds_()
  {
    add
    ("C"    , 100)
    ("CC"   , 200)
    ("CCC"  , 300)
    ("CD"   , 400)
    ("D"    , 500)
    ("DC"   , 600)
    ("DCC"  , 700)
    ("DCCC" , 800)
    ("CM"   , 900)
    ;
  }
} hundreds;

struct tens_ : bqi::symbols<char, unsigned>
{
  tens_()
  {
    add
    ("X"    , 10)
    ("XX"   , 20)
    ("XXX"  , 30)
    ("XL"   , 40)
    ("L"    , 50)
    ("LX"   , 60)
    ("LXX"  , 70)
    ("LXXX" , 80)
    ("XC"   , 90)
    ;
  }

} tens;

struct ones_ : bqi::symbols<char, unsigned>
{
  ones_()
  {
    add
    ("I"    , 1)
    ("II"   , 2)
    ("III"  , 3)
    ("IV"   , 4)
    ("V"    , 5)
    ("VI"   , 6)
    ("VII"  , 7)
    ("VIII" , 8)
    ("IX"   , 9)
    ;
  }

} ones;

// A grammar encapsulates one or more rules
template <typename It>
struct roman : bqi::grammar<It, unsigned()> // use unsigned as signature
{
  roman() : roman::base_type(start)
  {
    // eps always success and it consumes nothing, it's used to pre or post init
    start = bqi::eps[bqi::_val = 0] >>
    (
      // val is another Phoenix placeholder representing the rule's synthesized
      // attribute
      // a || b is the same as a >> -b | b, but more efficient
      +bqi::lit('M')[bqi::_val += 1000] // i guess lit is literal
      || hundreds [bqi::_val += bqi::_1]
      || tens [bqi::_val += bqi::_1]
      || ones [bqi::_val += bqi::_1]
    );
  }
  // use unsigned as it's attribute
  bqi::rule<It, unsigned()> start;
};

int main(int argc, char *argv[])
{

  std::string s;
  roman<std::string::iterator> roman_parser;
  unsigned result;
  std::cout << "pls input roman numerals(smaller than 1999)" << std::endl;
  while(std::getline(std::cin, s))
  {
    auto beg = s.begin();
    auto end = s.end();
    bool r = bqi::parse(beg, end, roman_parser, result);
    if(r && beg == end)
    {
      std::cout << result << std::endl;
    }
    else
    {
      std::cout << "parse failed" << std::endl;
    }
  }

  return 0;
}
