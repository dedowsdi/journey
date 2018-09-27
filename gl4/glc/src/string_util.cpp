#include "string_util.h"
#include "stream_util.h"

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
std::string trim(const std::string& s, bool left/* = true*/, bool right/* = true*/)
{
  if(!left && !right)
    return s;

  std::string result;

  if(left)
  {
    auto pos = s.find_first_not_of(" \t");
    if(pos != std::string::npos)
      result = s.substr(pos);
  }

  if(!right)
    return result;

  const std::string& candidate = left ? result : s;
  auto pos = candidate.find_last_not_of(" \t");

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
