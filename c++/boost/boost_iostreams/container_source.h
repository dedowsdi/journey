#ifndef CONTAINER_SOURCE_H
#define CONTAINER_SOURCE_H

/*
 * to define source in boost iostreams, you need to provide:
 *   char_type
 *   category
 *   size_type
 *   std::streamsize read(char_type* s, std::streamsize n){}
 */
#include <boost/iostreams/categories.hpp>
#include <iosfwd>
#include <algorithm>

namespace io = boost::iostreams;

template<typename Container>
class container_source{
public:
  // define char_type and category
  typedef typename Container::value_type char_type;
  typedef io::source_tag category; // source only

private:
  typedef typename Container::size_type size_type;
  Container& mContainer;
  size_type mPos; // next read position

public:

  container_source(Container& container):
    mContainer(container), mPos(0)
  {
  }

  // write may return fewer characters than requested, in which case the Sink is call non-blocking
  std::streamsize read(char_type* s, std::streamsize n){

    std::streamsize read_size = std::min(n, static_cast<std::streamsize>(mContainer.size() - mPos));

    if (read_size == 0)
      return -1;

    std::copy(mContainer.begin() + mPos, mContainer.begin() + mPos + read_size, s);
    mPos += read_size;

    return read_size;
  }

};

#endif /* CONTAINER_SOURCE_H */
