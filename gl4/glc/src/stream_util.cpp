#include "stream_util.h"

#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>

#include <boost/filesystem.hpp>
#include "string_util.h"
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace zxd
{

template <glm::length_t L, typename T, glm::qualifier Q>
std::istream& operator>>(std::istream& is, glm::vec<L, T, Q>& v)
{
  for (auto i = 0; i < L; ++i)
  {
    is >> v[i];
  }
  if (is.fail())
  {
    throw std::runtime_error("failed to read vec" + std::to_string(L));
  }
  return is;
}

template std::istream& operator>>(std::istream&, glm::vec2&);
template std::istream& operator>>(std::istream&, glm::vec3&);
template std::istream& operator>>(std::istream&, glm::vec4&);

template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
std::istream& operator>>(std::istream& is, glm::mat<C, R, T, Q>& v)
{
  for (auto i = 0; i < C; ++i)
  {
    for (auto j = 0u; j < R; ++j)
    {
      is >> v[i][j];
    }
  }
  if (is.fail())
  {
    throw std::runtime_error("failed to read vec" + std::to_string(R));
  }
  return is;
}

template std::istream& operator>>(std::istream&, glm::mat4&);

}

namespace stream_util
{


//--------------------------------------------------------------------
std::string read_resource(const std::string& filepath)
{
  return read_file(get_resource(filepath));
}

//--------------------------------------------------------------------
std::string read_shader_block(const std::string& filepath)
{
  auto content = read_resource(filepath);
  std::string version = "#version";
  if (content.size() >= version.size() &&
      content.substr(0, version.size()) == version)
  {
    content.insert(0, "//");
  }
  return content;
}

//--------------------------------------------------------------------
std::string read_file(const std::string &filepath)
{
  std::ifstream ifs(filepath);
  if (!ifs)
  {
    std::stringstream ss;
    ss << "failed to open file " << filepath << std::endl;
    throw std::runtime_error(ss.str());
  }

  // approximate size
  ifs.seekg(0, std::ios::end);
  GLuint size = ifs.tellg();
  ifs.seekg(0, std::ios::beg);

  std::string s;
  s.reserve(size);

  std::copy(std::istreambuf_iterator<char>(ifs),
    std::istreambuf_iterator<char>(), std::back_inserter(s));

  return s;
}

//--------------------------------------------------------------------
std::string read_string(const std::string &filepath)
{
  std::ifstream ifs(filepath);
  if (!ifs)
  {
    std::stringstream ss;
    ss << "failed to open file " << filepath << std::endl;
    throw std::runtime_error(ss.str());
  }

  // approximate size
  ifs.seekg(0, std::ios::end);
  GLuint size = ifs.tellg();
  ifs.seekg(0, std::ios::beg);

  std::string s;
  s.reserve(size);

  std::copy(std::istream_iterator<char>(ifs),
    std::istream_iterator<char>(), std::back_inserter(s));

  return s;
}

//--------------------------------------------------------------------
std::string get_resource(const std::string& name)
{
  namespace bfs = boost::filesystem;

  // simply return self if it's absolute path
  auto p = bfs::path(name);
  if (p.is_absolute())
    return name;

  std::vector<std::string> resources = {".", "./data"};
  char* resource_list = std::getenv("GL_FILE_PATH");
  if(resource_list != NULL)
  {
    std::vector<std::string> extra = string_util::split(resource_list, ";");
    resources.insert(resources.end(),
        std::make_move_iterator(extra.begin()), std::make_move_iterator(extra.end()));
  }

  for(auto& dir : resources)
  {
    bfs::path p(dir);
    //p.normalize();
    p.lexically_normal();
    p /= name;
    if(bfs::exists(p))
      return bfs::canonical(p).string();
  }

  std::stringstream ss;
  ss << name << " not found in all resources : \n\t";
  std::copy(resources.begin(), resources.end(),
    std::ostream_iterator<std::string>(ss, "\n\t"));
  throw std::runtime_error(std::string(ss.str()));
}
}
