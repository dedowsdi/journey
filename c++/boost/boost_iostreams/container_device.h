#ifndef CONTAINER_DEVICE_H
#define CONTAINER_DEVICE_H

/*
 * to define device in boost iostreams, you need to provide:
 *   char_type
 *   category
 *   size_type
 *   size
 *   std::streamsize read(char_type* s, std::streamsize n){}
 *   stream_offset seek(stream_offset off, std::ios_base::seekdir way)
 *   std::streamsize write(const char_type* s, std::streamsize n){}
 */
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/positioning.hpp>
#include <iosfwd>
#include <algorithm>

namespace io = boost::iostreams;

template<typename Container>
class container_device{
public:
  // define char_type and category
  typedef typename Container::value_type char_type;
  typedef io::seekable_device_tag category;

private:
  typedef typename Container::size_type size_type;
  Container& mContainer;
  size_type mPos; // next read position

public:

  container_device(Container& container):
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

  // write in device is not the same as write in sink
  std::streamsize write(const char_type* s, std::streamsize n){

    std::streamsize write_size = 0;

    // over write existing data
    if (mPos != mContainer.size()) {
      write_size = std::min(n, static_cast<std::streamsize>(mContainer.size() - mPos));
      std::copy(s, s + write_size, mContainer.begin() + mPos);
      mPos += write_size;
    }

    // append whatever left to back
    if (write_size < n) {
      mContainer.insert(mContainer.end(), s + write_size, s+n);
      mPos = mContainer.size();
    }
    
    return n;
  }

  io::stream_offset seek(io::stream_offset off, std::ios_base::seekdir way){
     // Determine new value of mPos
    io::stream_offset next;
    if (way == std::ios_base::beg) {
      next = off;
    }else if (way == std::ios_base::cur) {
      next = mPos + off;
    }else if (way == std::ios_base::end) {
      next = mContainer.size() + off - 1; // off must be non positive in this case
    }else{
      throw std::ios_base::failure("back seek direction");
    }

    if (next < 0 || next >= static_cast<io::stream_offset>(mContainer.size())) {
      throw std::ios_base::failure("bad seek offset");
    }

    mPos = next;
    return mPos;
  }


};

#endif /* CONTAINER_DEVICE_H */
