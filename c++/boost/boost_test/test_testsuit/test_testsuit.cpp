#define BOOST_TEST_MODULE test testsuite
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

// master test suite is root, test suite is branch, test case is leaf
BOOST_AUTO_TEST_SUITE(test_suite0)

  BOOST_AUTO_TEST_CASE(test_case0)
  {
    BOOST_TEST(true);
  }

// you can nest test suite
  BOOST_AUTO_TEST_SUITE(test_suite1)
    BOOST_AUTO_TEST_CASE(test_case0)
    {
      BOOST_ERROR("some error 0");
    }
  BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(test_case0)
{
  BOOST_TEST(true);
}

// you can seperate test suite contents, even in different files
BOOST_AUTO_TEST_SUITE(test_suite0)
  BOOST_AUTO_TEST_CASE(test_case1)
  {
    BOOST_ERROR("some error 1");
  }
BOOST_AUTO_TEST_SUITE_END()
