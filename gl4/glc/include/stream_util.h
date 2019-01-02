#ifndef GL_GLC_STREAM_H
#define GL_GLC_STREAM_H

#include "gl.h"
#include <glm/fwd.hpp>
#include <istream>
#include <vector>

namespace stream_util
{
  std::vector<float> read_floats(std::istream& is, GLuint count = -1);
  glm::vec4 read_vec4(std::istream& is);
  glm::vec3 read_vec3(std::istream& is);
  glm::vec2 read_vec2(std::istream& is);
  glm::mat4 read_mat(std::istream& is);

  std::string read_resource(const std::string &filepath);
  std::string read_file(const std::string &filepath);
  // no space no linebreak
  std::string read_string(const std::string &filepath);
  std::string get_resource(const std::string& name);
}

#endif /* GL_GLC_STREAM_H */
