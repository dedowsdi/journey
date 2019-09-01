#ifndef GL_GLC_STREAM_H
#define GL_GLC_STREAM_H

#include "gl.h"
#include <glm/fwd.hpp>
#include <istream>
#include <ostream>
#include <vector>
#include <map>

namespace zxd
{

template <glm::length_t L, typename T, glm::qualifier Q>
std::istream& operator>>(std::istream& is, glm::vec<L, T, Q>& v);

template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
std::istream& operator>>(std::istream& is, glm::mat<C, R, T, Q>& v);

template <glm::length_t L, typename T, glm::qualifier Q>
std::ostream& operator<<(std::ostream& os, const glm::vec<L, T, Q>& v);

template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
std::ostream& operator<<(std::ostream& os,  const glm::mat<C, R, T, Q>& v);

}

namespace stream_util
{
  std::string read_resource(const std::string& filepath);

  // read shader, comment leading #version
  std::string read_shader_block(const std::string& filepath);
  std::string read_file(const std::string& filepath);
  // no space no linebreak
  std::string read_string(const std::string& filepath);
  std::string get_resource(const std::string& name);
}

#endif /* GL_GLC_STREAM_H */
