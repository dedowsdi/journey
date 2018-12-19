#include "string_util.h"

#include <regex>

#include "stream_util.h"
#include <glm/glm.hpp>

namespace string_util
{

//--------------------------------------------------------------------
std::string tail(const std::string& path) {
  std::string::size_type pos = path.find_last_of("/\\");
  return pos == std::string::npos ? path : path.substr(pos + 1);
}

//--------------------------------------------------------------------
std::string basename(const std::string& filename) {
  std::string::size_type pos = filename.find_last_of(".");
  return pos == std::string::npos ? filename : filename.substr(0, pos);
}

//--------------------------------------------------------------------
std::string trim(const std::string& s, bool left/* = true*/, bool right/* = true*/, const std::string& token)
{
  if(!left && !right)
    return s;

  std::string result;

  if(left)
  {
    auto pos = s.find_first_not_of(token);
    if(pos != std::string::npos)
      result = s.substr(pos);
  }

  if(!right)
    return result;

  const std::string& candidate = left ? result : s;
  auto pos = candidate.find_last_not_of(token);

  return pos == std::string::npos ? candidate : candidate.substr(0, pos + 1);
}

//--------------------------------------------------------------------
std::string replace_string(const std::string& s, const std::string& a, const std::string& b)
{
  std::string res(s);
  auto pos = res.find(a);
  while(pos != std::string::npos)
  {
      res.replace(pos, a.length(), b);
      pos = res.find(a, pos);
  }
  return res;
}

//--------------------------------------------------------------------
std::vector<std::string> split(const std::string& s, const std::string& token/* = " \t\n"*/, unsigned splitCount/* = -1*/)
{
  std::vector<std::string> words;

  std::string::size_type index = s.find_first_not_of(token);
  while(index != std::string::npos)
  {
    auto next_index = s.find_first_of(token, index);
    auto count = next_index == std::string::npos ? next_index : next_index - index;
    words.push_back(s.substr(index, count));
    index = s.find_first_not_of(token, next_index);
  }
  return words;
}

//--------------------------------------------------------------------
std::vector<std::string> split_regex(const std::string& s, const std::string& pattern/* = R"(\s+)"*/, unsigned splitCount/* = -1*/)
{
  std::vector<std::string> words;
  std::regex re(pattern);

  std::sregex_token_iterator token_beg(s.begin(), s.end(), re, -1);
  std::sregex_token_iterator token_end;

  while(token_beg != token_end && splitCount--)
    words.push_back(*token_beg++);

  return words;
}

//--------------------------------------------------------------------
glm::vec2 stovec2(const std::string& s)
{
  std::stringstream ss(s);
  return stream_util::read_vec2(ss);
}

//--------------------------------------------------------------------
glm::vec3 stovec3(const std::string& s)
{
  std::stringstream ss(s);
  return stream_util::read_vec3(ss);
}

//--------------------------------------------------------------------
glm::vec4 stovec4(const std::string& s)
{
  std::stringstream ss(s);
  return stream_util::read_vec4(ss);
}

//--------------------------------------------------------------------
glm::mat4 stomat4(const std::string& s)
{
  std::stringstream ss(s);
  return stream_util::read_mat(ss);
}

}
