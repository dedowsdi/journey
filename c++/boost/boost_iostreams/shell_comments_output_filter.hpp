#ifndef SHELL_COMMENTS_OUTPUT_FILTER_HPP
#define SHELL_COMMENTS_OUTPUT_FILTER_HPP

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/operations.hpp>

namespace io = boost::iostreams;

class shell_comments_output_filter : public io::output_filter {
public:
    explicit shell_comments_output_filter(char comment_char = '#')
        : m_comment_char(comment_char), m_skip(false)
        { }

    template<typename Sink>
    bool put(Sink& dest, int c)
    {
      // Attempt to consume the given character of unfiltered data, writing
      // filtered data to dest as appropriate.  Return true if the character was
      // successfully consumed.
        
      // skip from # until \n
        m_skip = c == m_comment_char ?
            true :
            c == '\n' ?  false : m_skip;

        if (m_skip)
            return true;

        return io::put(dest, c);
    }

    template<typename Source>
    void close(Source&) { m_skip = false; }
private:
    char m_comment_char;
    bool m_skip;
};

#endif /* SHELL_COMMENTS_OUTPUT_FILTER_HPP */
