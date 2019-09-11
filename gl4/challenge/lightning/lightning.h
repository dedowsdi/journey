#ifndef LIGHTNING_H
#define LIGHTNING_H

#include <string>

#include <gl.h>
#include <pingpong.h>

namespace zxd
{

struct vertex
{
  vec3 pos;
  vec2 texcoord;
};

// all in model space
void create_billboards_along(std::vector<vertex>& billboards,
  const vec3_vector& lines, const vec3& camera_pos, GLfloat bb_width = 2,
  GLint gap_slices = 6);

void create_lightning(vec3_vector& lines, const vec3& start, const vec3& end,
  const std::string& pattern, GLfloat max_jitter = 0.5f,
  GLfloat max_fork_angle = fpi4);

struct lightning
{
  bool use_cpu; // for debug purpose
  GLuint vao;
  GLuint cpu_buffer;
  GLuint vertex_count = 2;
  GLuint buffer_size = 0; // pingpong buffer size
  GLfloat billboard_width;
  GLfloat max_jitter; // normalized, in size of subdivide
  GLfloat max_fork_angle; // along parent for gs shader, along global for cpu
  pingpong buffer;
  vec3 camera_pos;
  vec4 color0 = vec4(1);
  vec4 color1 = vec4(0, 0, 1, 1);
  std::string pattern;
  vec3_vector vertices;
  std::vector<vertex> billboards; // only used for cpu render

  lightning(const std::string& pattern_, const vec3_vector& vertices_, GLfloat billboard_width_,
      GLfloat max_jitter = 0.5f, GLfloat max_fork_angle = fpi4);

  void update_vertex_count();

  void update_buffer(const vec3& camera_pos_);

  void draw(const mat4& mvp_mat);

  void set_use_cpu(bool b);

  void debug_print_billboards(const vec3& camrea_pos_);
};
}


#endif /* LIGHTNING_H */
