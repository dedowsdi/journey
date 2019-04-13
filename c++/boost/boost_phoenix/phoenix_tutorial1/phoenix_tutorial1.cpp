#include <iostream>
#include <boost/phoenix.hpp>
//#include <boost/phoenix/function/lazy_prelude.hpp>
#include <functional>
#include <vector>
#include <algorithm>

namespace bp = boost::phoenix;
namespace bpp = boost::phoenix::placeholders;

// lazy function implementor
struct is_odd_impl
{
  //using result_type = bool; // for compilers that doesn't support decltype

  template <typename Arg>
  bool operator()(Arg arg1) const
  {
    return arg1 % 2 == 1;
  }
};

// lazy function
bp::function<is_odd_impl> is_odd;

int main(int argc, char *argv[])
{
  // boost placeholder is a callable function object(std is not)
  std::cout << bpp::arg1(1, 2) << std::endl;
  std::cout << bpp::arg2(1, 2) << std::endl;

  std::vector<int> v = {1, 2, 3, 4, 5};
  auto iter = std::find_if(v.begin(), v.end(), bpp::arg1 % 2 == 0);
  if(iter != v.end())
    std::cout << "first even : "<< *iter << std::endl;
  iter = std::find_if(v.begin(), v.end(), is_odd(bpp::arg1));
  if(iter != v.end())
    std::cout << "first ordd : "<< *iter << std::endl;

  // lazy statements, it [] not {}
  std::for_each(v.begin(), v.end(), bp::if_(bpp::arg1 > 2) [std::cout << bpp::arg1 << std::endl]);

  // lazy construct, new, delete, cast
  auto ls = bp::construct<std::string>("abc");
  std::cout << ls() << std::endl;

  auto li = bp::new_<int>(bpp::arg1);
  auto pi = li(5);
  std::cout << *pi << std::endl;
  auto cf = bp::static_cast_<float>(bpp::arg1);
  float f = cf(*pi);
  std::cout << f << std::endl;
  auto dli = bp::delete_(bpp::arg1);
  dli(pi);
  // need phoenix/function/lazy_preclude.hpp, but it violates ODR?
  //auto p = bp::plus(1, 2);
  //std::cout << p() << std::endl;
  //auto pw = bp::pow(2, 3);
  //std::cout << pw() << std::endl;
  
  return 0;
}
