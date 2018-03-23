#ifndef CONTAINER_SINK_H
#define CONTAINER_SINK_H

/*
 * to define sink in boost iostreams, you need to provide:
 *   char_type
 *   category
 *   size_type
 *   std::streamsize write(const char_type* s, std::streamsize n){}
 */
#include <boost/iostreams/categories.hpp>
#include <iosfwd>
#include <algorithm>

namespace io = boost::iostreams;

template<typename Container>
class container_sink{
public:
  // define char_type and category
  typedef typename Container::value_type char_type;
  typedef io::sink_tag category; // source only

private:
  typedef typename Container::size_type size_type;
  Container& mContainer;

public:

  container_sink(Container& container):
    mContainer(container)
  {
  }

  // write may return fewer characters than requested, in which case the Sink is call non-blocking
  std::streamsize write(const char_type* s, std::streamsize n){
    
    mContainer.insert(mContainer.end(), s, s+n);
    return n;
  }

};

#endif /* CONTAINER_SINK_H */
