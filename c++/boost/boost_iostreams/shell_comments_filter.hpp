#ifndef SHELL_COMMENTS_FILTER_HPP
#define SHELL_COMMENTS_FILTER_HPP

#include <cstdio> // EOF
#include <iostream>
#include <boost/iostreams/filter/stdio.hpp>

namespace io = boost::iostreams;

// Filters which derive from stdio_filter are DualUseFilters, which mean they
// can be used either for output or for input, but not both simultaneously. 
class shell_comments_stdio_filter : public io::stdio_filter{
private:
  char m_comment_char;

public:
  explicit shell_comments_stdio_filter(char comment_char = '#'):
    m_comment_char(comment_char)
  {
  }

private:

  virtual void do_filter()
  {
    bool skip = false;
    int c;
    
    // stdio_filter allows a programmer to define a Filter by reading unfiltered
    // data from standard input and writing filtered data to standard output.
    // there must be some kind of redirect, following code works for perfectly
    // for std::stringstream.
    while((c = std::cin.get()) != EOF){
      // skip everything from # until \n
      skip = c == m_comment_char ? 
        true :
        c == '\n' ? false : skip;

      if (!skip) {
        std::cout.put(c);
      }
    }
  }

};

#endif /* SHELL_COMMENTS_FILTER_HPP */
