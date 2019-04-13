#define BOOST_TEST_MODULE test template
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

using test_types = boost::mpl::list<int, long, char>;

BOOST_AUTO_TEST_CASE_TEMPLATE(
    test0,
    T,
    test_types
    )
{
  BOOST_TEST(sizeof(T) == 4);
}

