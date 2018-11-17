#include <iostream>

template<typename T>
void P(const T& x)
{
  std::cout << x << std::endl;
}

struct O
{
  int v = 5; // default member initializer prevent the class from being an aggregate.
  O(int i):v(i){}
  O& operator--(){std::cout << "--" << std::endl; v--; P(v); return *this;}
  O& operator--(int i){std::cout << "--i" << std::endl;v--; P(v);  return *this;}
  O operator-(const O& rhs){std::cout << "-" << std::endl;P(v);  return O(2);}
  O operator|(const O& rhs){std::cout << "|" << std::endl;P(v);  return O(3);}
};

int main(int argc, char *argv[])
{
  O o(5);
  o-----o
  |  o  |
  o-----o;
  return 0;
}
