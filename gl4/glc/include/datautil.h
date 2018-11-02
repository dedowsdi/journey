#ifndef DATAUTIL_H
#define DATAUTIL_H

namespace data_util
{

template <typename _It, typename Compare >
void insertion_sort(_It beg, _It end, Compare comp) {
  if (beg == end) return;

  // loop insert element
  for (_It i = beg + 1; i != end; ++i) {
    // shift elements forward, find insert spot. Just like shifting cards one by
    // one
    const typename _It::value_type key = *i;
    _It j = i - 1;
    while (j >= beg && comp(*j, key)) {
      *(j + 1) = *j;
      --j;
    }
    *(j + 1) = key;
  }
}

}


#endif /* DATAUTIL_H */
