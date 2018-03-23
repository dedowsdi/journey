#ifndef SHELL_COMMENTS_INPUT_FILTER_HPP
#define SHELL_COMMENTS_INPUT_FILTER_HPP

#include <boost/iostreams/char_traits.hpp> // EOF, WOULD_BLOCK
#include <boost/iostreams/concepts.hpp>    // input_filter
#include <boost/iostreams/operations.hpp>  // get
#include <iostream>

namespace io = boost::iostreams;

// the helper class input_filter provides a member type char_type equal to char
// and a category tag convertible to input_filter_tag and to closable_tag. 
class shell_comments_input_filter : public io::input_filter {
public:
    explicit shell_comments_input_filter(char comment_char = '#')
        : m_comment_char(comment_char), m_skip(false)
        { }

    template<typename Source>
    int get(Source& src)
    {
        // Attempt to produce one character of filtered data, reading from src
        // as necessary. If successful, return the character; otherwise return
        // EOF to indicate end-of-stream, or WOULD_BLOCK
        
        int c;
        while (true) {
            // WOULD_BLOCK indicates that input is temporarily unavailable
            if ((c = boost::iostreams::get(src)) == EOF || c == io::WOULD_BLOCK)
                break;

            // skip everything from # until \n
            m_skip = c == m_comment_char ?
                true :
                c == '\n' ?  false : m_skip;

            if (!m_skip)
                break;
        }
        return c;
    }

    // make your filer Closable.
    template<typename Source>
    void close(Source&) { m_skip = false; }

private:
    char m_comment_char;
    bool m_skip;
};

#endif /* SHELL_COMMENTS_INPUT_FILTER_HPP */
