#ifndef TYPE_LIST_H
#define TYPE_LIST_H

#include "type_util.h"

#define TYPE_LIST1(T1                                 ) mp::type_list< T1, mp::null_type>
#define TYPE_LIST2(T1, T2                             ) mp::type_list< T1, TYPE_LIST1(T2) >
#define TYPE_LIST3(T1, T2, T3                         ) mp::type_list< T1, TYPE_LIST2(T2,T3) >
#define TYPE_LIST4(T1, T2, T3, T4                     ) mp::type_list< T1, TYPE_LIST3(T2,T3,T4) >
#define TYPE_LIST5(T1, T2, T3, T4, T5                 ) mp::type_list< T1, TYPE_LIST4(T2,T3,T4,T5) >
#define TYPE_LIST6(T1, T2, T3, T4, T5, T6             ) mp::type_list< T1, TYPE_LIST5(T2,T3,T4,T5,T6) >
#define TYPE_LIST7(T1, T2, T3, T4, T5, T6, T7         ) mp::type_list< T1, TYPE_LIST6(T2,T3,T4,T5,T6,T7) >
#define TYPE_LIST8(T1, T2, T3, T4, T5, T6, T7, T8     ) mp::type_list< T1, TYPE_LIST7(T2,T3,T4,T5,T6,T7,T8) >
#define TYPE_LIST9(T1, T2, T3, T4, T5, T6, T7, T8, T9 ) mp::type_list< T1, TYPE_LIST8(T2,T3,T4,T5,T6,T7,T8,T9) >

namespace mp{

template<typename H, typename T>
class type_list{};


// tl_length
template<typename TList> struct tl_length;

template<> struct tl_length<null_type>{
  enum {value = 0};
};

template<typename H, typename T> struct tl_length<type_list<H,T>>{
  enum {value = 1 + tl_length<T>::value};
};

// index
template<typename TList, unsigned index> struct tl_at;

template<typename H, typename T> struct tl_at< type_list<H,T>, 0>{
  typedef H result;
};

template<typename H, typename T, unsigned index> struct tl_at< type_list<H,T>, index>{
  typedef typename tl_at<T, index-1>::result result;
};

// indexof
template<typename TList, typename N> struct tl_indexof;

template<typename N> struct tl_indexof<null_type, N>{
  enum{value = -1}; // not found
};

template<typename H, typename T> struct tl_indexof<type_list<H,T>, H>{
  enum{value = 0}; // found at head
};

template<typename H, typename T, typename N> struct tl_indexof< type_list<H,T>, N>{
private:
  enum{temp = tl_indexof<T, N>::value}; // recurse, store value at temp.
public:
  enum{value = temp == -1 ? -1 : temp + 1};
};

// tl_append
template <typename TList, typename N> struct tl_append;

template <> struct tl_append<null_type, null_type>{
  typedef null_type result;
};

template <typename N> struct tl_append<null_type, N>{
  typedef type_list<N, null_type> result;
};

template <typename H, typename T> struct tl_append< type_list<H, T>, null_type>{
  typedef type_list<H, T> result;
};

template <typename H, typename T, typename N> struct tl_append< type_list<H, T>, N>{
  typedef type_list<H,  typename tl_append<T, N>::result > result; // recurse
};

// tl_erase
template <typename TList, typename U> struct tl_erase;

template <typename U> struct tl_erase< null_type, U>{
  typedef null_type result;
};

template <typename H, typename T> struct tl_erase< type_list<H,T>, H >{
  typedef T result;
};

template <typename H, typename T, typename U> struct tl_erase< type_list<H,T>, U >{
  typedef type_list<H, typename tl_erase<T,U>::result> result;
};

// erase_all
template <typename TList, typename U> struct erase_all;

template <typename U> struct erase_all< null_type, U>{
  typedef null_type result;
};

template <typename H, typename T> struct erase_all< type_list<H,T>, H >{
  // Go all the way down the list removing the type
  typedef typename erase_all<T, H>::result result;
};

template <typename H, typename T, typename U> struct erase_all< type_list<H,T>, U >{
  typedef type_list<H, typename erase_all<T,U>::result> result;
};

// tl_unique
template <typename TList> struct tl_unique;

template <> struct tl_unique<null_type>{
  typedef null_type result;
};

template <typename H, typename T> struct tl_unique< type_list<H,T> >{
  typedef typename tl_unique<T>::result temp; // recurse
  // remove duplicated head from tail
  typedef type_list<H, typename erase_all<temp, H>::result > result;
};

// tl_replace
template <typename TList, typename U, typename V> struct tl_replace;

template <typename U, typename V> struct tl_replace< null_type, U, V>{
  typedef null_type result;
};

template <typename H, typename T, typename V> struct tl_replace< type_list<H,T>, H, V >{
  typedef type_list<V,T> result;
};

template <typename H, typename T, typename U, typename V> struct tl_replace< type_list<H,T>, U, V >{
  typedef type_list<H, typename tl_replace<T,U, V>::result> result;
};

// tl_replace_all
template <typename TList, typename U, typename V> struct tl_replace_all;

template <typename U, typename V> struct tl_replace_all< null_type, U, V>{
  typedef null_type result;
};

template <typename H, typename T, typename V> struct tl_replace_all< type_list<H,T>, H, V >{
  typedef type_list<V, typename tl_replace_all<T,H, V>::result > result;
};

template <typename H, typename T, typename U, typename V> struct tl_replace_all< type_list<H,T>, U, V >{
  typedef type_list<H, typename tl_replace_all<T,U, V>::result> result;
};

// get most derived of U in TList, or U if not found
template<typename TList, typename U> struct tl_most_derived;

template<typename U> struct tl_most_derived<null_type, U>{
  typedef U result;
};

template<typename H, typename T, typename U> struct tl_most_derived< type_list<H,T>, U>{
private:
  typedef typename tl_most_derived< T,U >::result candidate;
public:
  typedef typename type_select< SUPERSUBCLASS(H, candidate), candidate, H>::result result;
};

// sort
template<typename TList> struct tl_shift_derived_to_front;

template<> struct tl_shift_derived_to_front<null_type>{
  typedef null_type result;
};

template<typename H, typename T> struct tl_shift_derived_to_front<type_list<H,T> >{
private:
  typedef typename tl_most_derived<T, H>::result most_derived_of_head_in_tail;
  // swap most derived and head
  typedef typename tl_replace<T, most_derived_of_head_in_tail, H>::result tail;
public:
  typedef type_list<most_derived_of_head_in_tail, tail> result;
};

}

#endif /* TYPE_LIST_H */
