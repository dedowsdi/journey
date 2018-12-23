#ifndef GL_GLC_FUNCTOR_H
#define GL_GLC_FUNCTOR_H
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <set>

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

struct dereference
{
  template <typename T>
  const T& operator*(const T* p) const
  {
    return *p;
  }
};

struct dereference_less
{
  template <typename P>
  bool operator()(P* lhs, P* rhs)
  {
    return *lhs < *rhs;
  }
};

template<typename AssociativeContainer, typename PredicateT>
void erase_if(AssociativeContainer& ctn, const PredicateT& pt);

template <typename M>
auto get_keys(const M& m);

template <typename M>
auto get_values(const M& m);

// the erase ... family, don't call them on list, list do it's own job
template <typename SequenceContainer, typename ValueType>
void erase_remove(SequenceContainer& ctn, const ValueType& v);

template <typename SequenceContainer, typename UnaryPredicate>
void erase_remove_if(SequenceContainer& ctn, UnaryPredicate p);

template <typename Container>
void erase_unique(Container& ctn);

// only used to delete raw pointer in sequential container
// you can not use remove style, as it leavs unspecified data in the end
template <typename Container, typename UnaryPredicate>
void erase_partition_delete(Container& ctn, UnaryPredicate p);

// uniq key, not key value pair
template<typename AssociativeContainer, typename DuplicatePolicy>
void unique_associative(AssociativeContainer& container, DuplicatePolicy areDuplicates);

template<typename Key, typename Value, typename Comparator>
void unique(std::multimap<Key, Value, Comparator>& container);

template<typename Key, typename Comparator>
void unique(std::multiset<Key, Comparator>& container);

template<typename Key, typename Value, typename Comparator>
void unique(std::unordered_multimap<Key, Value, Comparator>& container);

template<typename Key, typename Comparator>
void unique(std::unordered_multiset<Key, Comparator>& container);

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
  template <typename Container, typename UnaryPredicate>
void erase_partition_delete(Container& ctn, UnaryPredicate p)
{
  auto iter = std::partition(begin(ctn), end(ctn), std::not1(p));
  std::for_each(iter, end(ctn), std::default_delete<typename Container::value_type>());
  ctn.erase(iter, end(ctn));
}

//--------------------------------------------------------------------
  template<typename AssociativeContainer, typename DuplicatePolicy>
void unique_associative(AssociativeContainer& container, DuplicatePolicy areDuplicates)
{
  if (container.empty())
    return;

  auto it = begin(container);
  auto previousIt = it;
  ++it;
  while (it != end(container))
  {
    if (areDuplicates(*previousIt, *it))
    {
      it = container.erase(it);
    }
    else
    {
      previousIt = it;
      ++it;
    }
  }
}

//--------------------------------------------------------------------
  template<typename Key, typename Value, typename Comparator>
void unique(std::multimap<Key, Value, Comparator>& container)
{
  auto value_comp = container.value_comp();
  return unique_associative(container,
      [&value_comp](std::pair<const Key, Value> const& element1, std::pair<const Key, Value> const& element2)
      {
        return !value_comp(element1, element2) && value_comp()(element2, element1);
      });
}

//--------------------------------------------------------------------
  template<typename Key, typename Comparator>
void unique(std::multiset<Key, Comparator>& container)
{
  auto value_comp = container.value_comp();
  return unique_associative(container,
      [&value_comp](Key const& element1, Key const& element2)
      {
        return !value_comp()(element1, element2) && !value_comp()(element2, element1);
      });
}

//--------------------------------------------------------------------
  template<typename Key, typename Value, typename Comparator>
void unique(std::unordered_multimap<Key, Value, Comparator>& container)
{
  auto key_eq = container.key_eq();
  return unique_associative(container, 
      [&key_eq](std::pair<const Key, Value> const& element1, std::pair<const Key, Value> const& element2)
      {
        return key_eq(element1.first, element2.first);
      });
}

//--------------------------------------------------------------------
  template<typename Key, typename Comparator>
void unique(std::unordered_multiset<Key, Comparator>& container)
{
  auto key_eq = container.key_eq();
  return unique_associative(container,
      [&key_eq](Key const& element1, Key const& element2)
      {
        return key_eq(element1, element2);
      });
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
