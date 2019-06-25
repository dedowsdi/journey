#ifndef GL_GLC_STRING_UTIL_H
#define GL_GLC_STRING_UTIL_H
#include <string>
#include <sstream>
#include <iterator>
#include <exception>
#include <iostream>
#include <vector>
#include <glm/fwd.hpp>

//#include "glm.h"

namespace string_util 
{
  std::string tail(const std::string& path);
  std::string basename(const std::string& filename);
  
  template<typename T>
  std::string to(T t)
  {
    std::stringstream ss;
    bool fail = false;
    try
    {
      ss << t;
      fail = ss.fail();
    }catch(const std::exception& e)
    {
      fail = true;
      std::cout << e.what() << std::endl;
    }

    if(fail)
      std::cout << "failed to conver string from " << t; 

    return ss.str();
  }

  std::string trim(const std::string& s, bool left = true, bool right = true, const std::string& token = " \t");
  std::string replace_string(const std::string& s, const std::string& a, const std::string& b);
  std::vector<std::string> split(const std::string& s, const std::string& token = " \t\n", unsigned splitCount = -1);
  // assume pattern is greedy
  std::vector<std::string> split_regex(const std::string& s, const std::string& pattern = R"(\s+)", unsigned splitCount = -1);
  template<typename It>
  std::string join(It beg, It end, const std::string& seperator = ",");

  glm::vec2 stovec2(const std::string& s);
  glm::vec3 stovec3(const std::string& s);
  glm::vec4 stovec4(const std::string& s);
  glm::mat4 stomat4(const std::string& s);
  std::string uint32_to_utf8(uint32_t i);

// \033 escape
// [    fixed
// code0;code1;code2....
// m    fixed
const std::string bash_fg_black        = "\033[0;30m";         
const std::string bash_fg_red          = "\033[0;31m";         
const std::string bash_fg_green        = "\033[0;32m";         
const std::string bash_fg_yellow       = "\033[0;33m";         
const std::string bash_fg_blue         = "\033[0;34m";         
const std::string bash_fg_magenta      = "\033[0;35m";         
const std::string bash_fg_cyan         = "\033[0;36m";         
const std::string bash_fg_white        = "\033[0;37m";         

const std::string bash_bg_black        = "\033[0;40m";         
const std::string bash_bg_red          = "\033[0;41m";         
const std::string bash_bg_green        = "\033[0;42m";         
const std::string bash_bg_yellow       = "\033[0;43m";         
const std::string bash_bg_blue         = "\033[0;44m";         
const std::string bash_bg_magenta      = "\033[0;45m";         
const std::string bash_bg_cyan         = "\033[0;46m";         
const std::string bash_bg_white        = "\033[0;47m";         

const std::string bash_reset           = "\033[0m";
const std::string bash_bold_bright     = "\033[1m";
const std::string bash_underline       = "\033[4m";
const std::string bash_inverse         = "\033[7m";
const std::string bash_bold_bright_off = "\033[21m";
const std::string bash_underline_off   = "\033[24m";
const std::string bash_inverse_off     = "\033[27m";

const std::string bash_warning         = "\033[0;1;31m";


//--------------------------------------------------------------------
template<typename It>
std::string join(It beg, It end, const std::string& seperator/* = ","*/)
{
  if(beg == end)
    return "";

  typedef typename std::iterator_traits<It>::value_type value_type;
  std::ostringstream oss;
  auto lbo = beg;
  std::advance(lbo, std::distance(beg, end) - 1);

  std::copy(beg, lbo, std::ostream_iterator<value_type>(oss, seperator.c_str()));
  oss << *lbo;

  return oss.str();
}
};

#endif /* GL_GLC_STRING_UTIL_H */
