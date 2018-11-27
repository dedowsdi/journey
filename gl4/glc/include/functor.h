#ifndef GL_GLC_FUNCTOR_H
#define GL_GLC_FUNCTOR_H
#include <map>
#include <vector>
#include <algorithm>

namespace zxd
{

template <typename T>
struct get_first
{
  const auto& operator()(const T& item) { return item.first; }
};

template <typename T>
struct get_sencond
{
  const auto& operator()(const T& item) { return item.second; }
};

// Vec must provide operator []
template<typename Vec, unsigned char Index = 0>
struct sort_by_elem_at
{
  bool operator()(const Vec& lhs, const Vec& rhs) { return lhs[Index] < rhs[Index]; }
};

template<typename AssociativeContainer, typename PredicateT>
void erase_if(AssociativeContainer& ctn, const PredicateT& pt);

template <typename M>
auto get_keys(const M& m);

template <typename M>
auto get_values(const M& m);

//--------------------------------------------------------------------
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

//--------------------------------------------------------------------
template <typename M>
auto get_keys(const M& m)
{
  std::vector<typename M::key_type> keys;
  keys.reserve(m.size());
  std::transform(m.begin(), m.end(), std::back_inserter(keys),
      get_first<typename M::value_type>());

  return keys;
}

//--------------------------------------------------------------------
template <typename M>
auto get_values(const M& m)
{
  std::vector<typename M::mapped_type> values;
  values.reserve(m.size());
  std::transform(m.begin(), m.end(), std::back_inserter(values),
      get_first<typename M::value_type>());

  return values;
}

}

#endif /* GL_GLC_FUNCTOR_H */
