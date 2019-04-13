// require variadic macros(c++ 11)
#define BOOST_TEST_MODULE test decorator
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/results_collector.hpp>

namespace ut = boost::unit_test;
namespace utd = boost::unit_test::data;
namespace utf = boost::unit_test::framework;
namespace tt = boost::test_tools;

// add lable decorator, then you can filter test case with
//    example --run_test=@label_name
BOOST_AUTO_TEST_CASE(case0,
    * ut::label("trivial") // add label decorator
    )
{
  BOOST_TEST(true);
}

BOOST_AUTO_TEST_CASE(case1,
    * ut::label("trivial") // add label decorator
    * ut::label("crap") // add label decorator
    * ut::description("some  description") // will be printed if run with --list_content
    )
{
  BOOST_TEST(true);
}

// if you define test suite in multiple parts, it's cocorator will be
// concatinated together
BOOST_AUTO_TEST_SUITE(suite0, * ut::label("trivial"))
  BOOST_AUTO_TEST_CASE(case0)
  {
    BOOST_TEST(true);
  }
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(suite0)
  BOOST_AUTO_TEST_CASE(case1)
  {
    BOOST_TEST(true);
  }
BOOST_AUTO_TEST_SUITE_END()

// you must add decorator explicitly for data-driven test case, test case
// template and test case template with fixture
BOOST_TEST_DECORATOR(*ut::label("trivial"))
BOOST_DATA_TEST_CASE(test_case2,
    utd::xrange(3))
{
    BOOST_TEST(true);
}

// depends_on decorator, only execute if case1 execute successfuly
BOOST_AUTO_TEST_CASE(case2, * ut::depends_on("suite0/case1"))
{
    BOOST_TEST(true);
}

// disabled decorator set default run status to false
BOOST_AUTO_TEST_CASE(case3, * ut::disabled())
{
  BOOST_TEST(false);
}

// enabled decorated can be used to overrite default run status
BOOST_AUTO_TEST_SUITE(suite2, * ut::disabled())
  BOOST_AUTO_TEST_CASE(case4, * ut::enabled())
  {
    BOOST_TEST(true);
  }
BOOST_AUTO_TEST_SUITE_END()

// enable if decorator implements compilation time run status
BOOST_AUTO_TEST_CASE(case5, * ut::enable_if<true>())
{
  BOOST_TEST(true);
}

struct if_either
{
  std::string tc1, tc2;
  if_either(std::string t1, std::string t2)
    : tc1(t1), tc2(t2) {}

  tt::assertion_result operator()(ut::test_unit_id)
  {
    auto& master = utf::master_test_suite();
    auto& collector = ut::results_collector_t::instance();
    auto& test1_result = collector.results(master.get(tc1));
    auto& test2_result = collector.results(master.get(tc2));

    if (test1_result.passed() || test2_result.passed())
      return true;

    tt::assertion_result ans(false);
    ans.message() << tc1 << " and " << tc2 << " failed";
    return ans;
  }
};

// runtime run status(depends_on works on single test case of test suite)
BOOST_AUTO_TEST_CASE(test3,
  * ut::precondition(if_either("test1", "test2")))
{
  BOOST_TEST(false);
}

BOOST_AUTO_TEST_CASE(test4,
  * ut::precondition(if_either("test2", "test3")))
{
  BOOST_TEST(false);
}
