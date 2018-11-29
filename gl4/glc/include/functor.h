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

// the erase ... family, don't call them on list, list do it's own job
template <typename Container, typename ValueType>
void erase_remove(Container& ctn, const ValueType& v);

template <typename Container, typename UnaryPredicate>
void erase_remove_if(Container& ctn, UnaryPredicate p);

template <typename Container>
void erase_unique(Container& ctn);

// https://www.fluentcpp.com/2017/10/10/partitioning-with-the-stl/
// turn
//    ....a..b...c....d....
//             ^
//             to
//    .......abcd..........
//             ^
template<typename BidirIterator, typename Predicate>
std::pair<BidirIterator, BidirIterator> gather(BidirIterator first, BidirIterator last,
    BidirIterator position, Predicate p);

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

//--------------------------------------------------------------------
template <typename Container, typename ValueType>
void erase_remove(Container& ctn, const ValueType& v)
{
  ctn.erase(std::remove(begin(ctn), end(ctn), v), end(ctn));
}

//--------------------------------------------------------------------
template <typename Container, typename UnaryPredicate>
void erase_remove_if(Container& ctn, UnaryPredicate p)
{
  ctn.erase(std::remove_if(begin(ctn), end(ctn), p), end(ctn));
}

//--------------------------------------------------------------------
template <typename Container>
void erase_unique(Container& ctn)
{
  ctn.erase(std::unique(begin(ctn), end(ctn)), end(ctn));
}

//--------------------------------------------------------------------
template<typename BidirIterator, typename Predicate>
std::pair<BidirIterator, BidirIterator> gather(BidirIterator first, BidirIterator last,
    BidirIterator position, Predicate p)
{
  return { std::stable_partition(first, position, std::not1(p)),
    std::stable_partition(position, last, p) };
}

}

#endif /* GL_GLC_FUNCTOR_H */
