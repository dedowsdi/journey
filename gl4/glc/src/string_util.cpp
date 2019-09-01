#include "string_util.h"

#include <regex>
#include <iostream>
#include <sstream>

#include "stream_util.h"
#include <glm/glm.hpp>

namespace string_util
{

//--------------------------------------------------------------------
std::string tail(const std::string& path)
{
  std::string::size_type pos = path.find_last_of("/\\");
  return pos == std::string::npos ? path : path.substr(pos + 1);
}

//--------------------------------------------------------------------
std::string basename(const std::string& filename)
{
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
  if (a == b)
    return s;

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

template <typename T>
T to_vec(const std::string& s)
{
  using namespace zxd;
  std::stringstream ss(s);
  T v;
  ss >> v;
  return v;
}

template <typename T>
T to_mat(const std::string& s)
{
  using namespace zxd;
  std::stringstream ss(s);
  T v;
  ss >> v;
  return v;
}

//--------------------------------------------------------------------
glm::vec2 to_vec2(const std::string& s)
{
  return to_vec<glm::vec2>(s);
}

//--------------------------------------------------------------------
glm::vec3 to_vec3(const std::string& s)
{
  return to_vec<glm::vec3>(s);
}

//--------------------------------------------------------------------
glm::vec4 to_vec4(const std::string& s)
{
  return to_vec<glm::vec4>(s);
}

//--------------------------------------------------------------------
glm::mat4 to_mat4(const std::string& s)
{
  return to_mat<glm::mat4>(s);
}

//--------------------------------------------------------------------
std::string uint32_to_utf8(uint32_t value)
{
  std::string s;
  for (int i = 0; i < 4; ++i)
  {
    char c = (value >> (i*8)) & 0xff;
    //if(c != 0)
      s.push_back(c);
  }
  return s;
}

}
