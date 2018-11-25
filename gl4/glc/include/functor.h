#ifndef FUNCTOR_H
#define FUNCTOR_H
#include <map>

namespace zxd
{
template <typename T, typename U>
struct get_first
{
  const T& operator()(const std::pair<T,U>& item)
  {
    return item.first;
  }
};

template <typename T, typename U>
struct get_second
{
  const T& operator()(const std::pair<T,U>& item)
  {
    return item.second;
  }
};

// Vec must provide operator []
template<typename Vec, unsigned char Index = 0>
struct sort_by_elem_at
{
  bool operator()(const Vec& lhs, const Vec& rhs)
  {
    return lhs[Index] < rhs[Index];
  }
};

template<typename AssociativeContainer, typename PredicateT>
void erase_if(AssociativeContainer& ctn, const PredicateT& pt)
{
  for(auto iter = ctn.begin(); iter != ctn.end(); )
  {
    if(pt(*iter))
      iter = ctn.erase(iter);
    else
      ++iter;
  }
}

}


#endif /* FUNCTOR_H */
