#define BOOST_TEST_MODULE test fixture
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <iostream>

namespace ut = boost::unit_test;
namespace utd = boost::unit_test::data;
namespace utf = boost::unit_test::framework;

struct F
{
  F() {
    BOOST_TEST_MESSAGE(__PRETTY_FUNCTION__);
  }
  ~F() {
    BOOST_TEST_MESSAGE(__PRETTY_FUNCTION__);
  }
  //optional setup and teardown
  void setup() {
    i = 0;
    BOOST_TEST_MESSAGE(__PRETTY_FUNCTION__);
  }
  void teardown() {
    BOOST_TEST_MESSAGE(__PRETTY_FUNCTION__);
  }
  int i;
};

// F will be created before test case, destroyed after test case,
// change it's status after test is meanningless
BOOST_FIXTURE_TEST_CASE(test_case1, F)
{
  // you can access fixture member directly(except private one)
  BOOST_TEST(i == 1);
  ++i;
}

BOOST_FIXTURE_TEST_CASE(test_case2, F)
{
  // you can access fixture member directly(except private one)
  BOOST_CHECK_EQUAL(i, 1);
}

// the lifetime of F is exactly the same as test case level fixture
BOOST_FIXTURE_TEST_SUITE(suite0, F)
  BOOST_AUTO_TEST_CASE(test_case0)
  {
    BOOST_CHECK_EQUAL(i, 0);
    BOOST_TEST_MESSAGE("running suite0/test_case0");
    ++i; // meanningless
  }
  BOOST_AUTO_TEST_CASE(test_case1)
  {
    BOOST_CHECK_EQUAL(i, 1);
    BOOST_TEST_MESSAGE("running suite0/test_case1");
    ++i; // meaningless
  }
BOOST_AUTO_TEST_SUITE_END() // it's still auto

// suite level fixture, you can't access fixture member, only one instance of F
// exists for the entire test suite.(you can try static member)
BOOST_AUTO_TEST_SUITE(suite1, *ut::fixture<F>())
  BOOST_AUTO_TEST_CASE(test_case0)
  {
    BOOST_TEST_MESSAGE("running suite1/test_case0");
  }
  BOOST_AUTO_TEST_CASE(test_case1)
  {
    BOOST_TEST_MESSAGE("running suite1/test_case1");
  }
BOOST_AUTO_TEST_SUITE_END() // it's still auto

// global level fixture. global level fixture doesn't call setup or teardown????
BOOST_GLOBAL_FIXTURE(F);
