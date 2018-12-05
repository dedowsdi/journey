/*
 * associate type only let you search with key by by default, c++14 introduce
 * new feature that let you search by comparable type.
 */
#include <set>

struct A
{
  int id;
  float f;
};

struct CompareA
{
  // all you need to do is define is_transparent and extra two operator
  using is_transparent = void;

  bool operator()(const A& lhs, const A& rhs)
  {
    return lhs.id < rhs.id;
  }

  bool operator()(const A& lhs, int id)
  {
    return lhs.id < id;
  }

  bool operator()(int id, const A& rhs)
  {
    return id < rhs.id;
  }
};

int main(int argc, char *argv[])
{

  std::set<A> as;
  //as.find(5); // don't work, my gcc is too old
  
  return 0;
}
