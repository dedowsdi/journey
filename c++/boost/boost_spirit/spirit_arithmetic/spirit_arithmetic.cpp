#define BOOST_TEST_MODULE test arithmetic expression
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/spirit/include/qi.hpp>
#include <numeric>
#include <sstream>
#include <string>

namespace ut = boost::unit_test;
namespace utd = boost::unit_test::data;
namespace utf = boost::unit_test::framework;

namespace bs = boost::spirit;
namespace qi = boost::spirit::qi;

template <typename It, typename FloatType>
struct arithmetic_grammar : public qi::grammar<It, FloatType(), qi::ascii::space_type>
{
  arithmetic_grammar() : 
  arithmetic_grammar::base_type(expression)
  {
  }

  qi::rule<It, FloatType(), qi::ascii::space_type> expression;

};


const arithmetic_grammar<std::string::iterator, float> grammar;

template <typename It, typename FloatType>
bool parse(It beg, It end, FloatType& result)
{
  auto r = qi::phrase_parse(beg, end, grammar, qi::ascii::space, result);
  return r  && beg == end;
}


BOOST_AUTO_TEST_SUITE(arithmetic)

  BOOST_DATA_TEST_CASE(add_minus,
      utd::random(-1.0f, 1.0f) ^ utd::random(-1.0f, 1.0f) ^ utd::xrange(10),
      v0, v1, v2
      )
  {
    std::stringstream ss;
    ss << v0 << " + " <<  v1;
    std::string s = ss.str();
    float result;
    BOOST_TEST_REQUIRE(parse(s.begin(), s.end(), result));
  }

BOOST_AUTO_TEST_SUITE_END()
