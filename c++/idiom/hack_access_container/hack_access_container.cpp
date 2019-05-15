#include <iostream>
#include <stack>
#include <deque>
#include <vector>
#include <algorithm>
#include <iterator>


using stack_c_ptr_t = std::deque<int> std::stack<int>::*;
constexpr stack_c_ptr_t stack_c_ptr();

template <stack_c_ptr_t p>
struct hack
{
  friend constexpr stack_c_ptr_t stack_c_ptr() { return p; }
};

// explicitly instaniate hack with &std::stack<int>::c
template struct hack<&std::stack<int>::c>;

int main(int argc, char *argv[])
{
  std::stack<int> is;
  is.push(1);
  is.push(2);
  is.push(3);

  auto& ctn = is.*stack_c_ptr();
  std::copy(ctn.begin(), ctn.end(), std::ostream_iterator<int>(std::cout, ","));

  return 0;
}
