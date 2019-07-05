#ifndef GLC_GLC_UTF8_H
#define GLC_GLC_UTF8_H

#include "utf8.h"

namespace zxd
{

template <typename It>
inline utf8::iterator<It> make_utf8_it(It it, It beg, It end )
{
  return utf8::iterator<It>(it, beg, end);
}

template <typename It>
inline utf8::iterator<It> make_utf8_beg(It beg, It end )
{
  return utf8::iterator<It>(beg, beg, end);
}

template <typename It>
inline utf8::iterator<It> make_utf8_end(It beg, It end )
{
  return utf8::iterator<It>(end, beg, end);
}

}

#endif /* GLC_GLC_UTF8_H */
