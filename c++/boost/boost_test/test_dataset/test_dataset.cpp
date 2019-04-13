#define BOOST_TEST_MODULE test dataset
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <iostream>
#include <vector>

namespace ut = boost::unit_test;
namespace utd = boost::unit_test::data;

// xrange and random support dict like params

// if you add infinite dataset a and finite dataset b, b will never be called
BOOST_DATA_TEST_CASE(
    test_add,
    utd::xrange(2) + utd::xrange(2)
    )
{
  std::cout << "test_add : " << sample << std::endl;
}

BOOST_DATA_TEST_CASE(
    test_zip,
    utd::xrange(3) ^ utd::xrange(3, 6),
    val0,
    val1
    )
{
  std::cout << "test_zip : " << val0 << "," <<  val1 << std::endl;
}

BOOST_DATA_TEST_CASE(
    test_cartesian_product,
    utd::xrange(3) * utd::xrange(3, 6), // cartesian product
    val0,
    val1
    )
{
  std::cout << "test_cartesian_product : " << val0 << "," <<  val1 << std::endl;
}

BOOST_DATA_TEST_CASE(
    test_singleton,
     // if you zip singleton with an finite dataset b, size = b.size()
     // if you zip singleton with an infinite dataset, size = 1
    utd::make(5) ^ utd::xrange(3),
    val0,
    val1
    )
{
  std::cout << "test_singleton : " << val0 << "," <<  val1 << std::endl;
}

BOOST_DATA_TEST_CASE(
    test_random,
    // utd::random() generates [0.0f, 1.0f)
    // utd::random(1, 5) generates integer in [1,5]
    
    utd::xrange(3) ^
    // you can even provide utd::seed or utd::engine if you want
    utd::random(utd::distribution = std::uniform_real_distribution<float>(1.0f, 2.0f)) ,
    val0,
    val1
    )
{
  std::cout << "test_random : " << val0 << "," <<  val1 << std::endl;
}

BOOST_DATA_TEST_CASE(
    test_carray,
    utd::make({"abc", "def"})
    )
{
  std::cout << "test_random : " << sample << std::endl;
}

std::vector<int> int_vec = {1,2,3,4,5};

BOOST_DATA_TEST_CASE(
    test_stl,
    utd::make(int_vec)
    )
{
  std::cout << "test_stl : " << sample << std::endl;
}
