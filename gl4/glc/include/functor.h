#ifndef FUNCTOR_H
#define FUNCTOR_H
#include <map>

namespace zxd
{
template <typename T, typename U>
class get_first
{
  const T& operator()(const std::pair<T,U>& item)
  {
    return item.first;
  }
};

template <typename T, typename U>
class get_second
{
  const T& operator()(const std::pair<T,U>& item)
  {
    return item.second;
  }
};
}


#endif /* FUNCTOR_H */
