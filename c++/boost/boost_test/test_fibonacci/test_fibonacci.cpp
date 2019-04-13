#define BOOST_TEST_MODULE test fibonacci dataset
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <sstream>

namespace ut = boost::unit_test;
namespace utd = boost::unit_test::data;

class fibonacci_dataset
{
public:

  struct iterator
  {
    iterator() : a(1), b(1) {}
    int a;
    int b; // output

    int operator*() const {return b;}
    void operator++()
    {
      a += b;
      std::swap(a, b);
    }
  };

  fibonacci_dataset(){}

  // data set interface:
  using sample = int;
  enum {arity = 1};
  utd::size_t size() const {return utd::BOOST_TEST_DS_INFINITE_SIZE;}
  // iterator
  iterator begin() const {return iterator();}
};

namespace boost { namespace unit_test { namespace data { namespace monomorphic {
// register dataset
template<>
struct is_dataset<fibonacci_dataset> : boost::mpl::true_ {};
}}}}

// create a test-driven dataset
BOOST_DATA_TEST_CASE(
    test_fibonacci,
    fibonacci_dataset() ^ utd::make({1, 2, 3, 5, 8, 13, 21, 35, 56}),
    fib_sample, exp)
{
  BOOST_TEST(fib_sample == exp);
}
