#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <memory>

namespace ut = boost::unit_test;
namespace utf = boost::unit_test::framework;

// a test case function
void foo()
{
  BOOST_TEST(true);
  BOOST_TEST(1 == 2);
}

class A
{
public:
  void test0()
  {
    BOOST_TEST(true);
  }
  void test1()
  {
    BOOST_TEST(false);
  }
};

bool init_unit_test_suite()
{
  utf::master_test_suite().p_name.value = "manual test case";
  // The class instance can't be defined in the initialization function scope,
  // since it becomes invalid as soon as the test execution exits it. 
  auto a = std::make_shared<A>();

  utf::master_test_suite().add(BOOST_TEST_CASE(&foo));
  utf::master_test_suite().add(BOOST_TEST_CASE([a](){a->test0();}));
  utf::master_test_suite().add(BOOST_TEST_CASE([a](){a->test1();}));

  return true;
}

int main(int argc, char *argv[])
{
  // custom entry point
  ut::unit_test_main(init_unit_test_suite, argc, argv);
  return 0;
}

