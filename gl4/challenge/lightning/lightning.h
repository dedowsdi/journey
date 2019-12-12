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
void create_billboards_along(std::vector<vertex>& _billboards,
  const vec3_vector& lines, const vec3& _camera_pos, GLfloat bb_width = 2,
  GLint gap_slices = 6);

void create_lightning(vec3_vector& lines, const vec3& start, const vec3& end,
  const std::string& _pattern, GLfloat _max_jitter = 0.5f,
  GLfloat _max_fork_angle = fpi4);

class lightning
{
public:
  lightning(const std::string& pattern_, const vec3_vector& vertices_);

  ~lightning();

  void update_vertex_count();

  void update_buffer(const vec3& camera_pos_);

  void draw(const mat4& mvp_mat);

  void set_use_cpu(bool b);

  void debug_print_billboards(const vec3& camrea_pos_);

  GLfloat get_billboard_width() const { return _billboard_width; }
  void set_billboard_width(GLfloat v){ _billboard_width = v; }

  GLfloat get_max_jitter() const { return _max_jitter; }
  void set_max_jitter(GLfloat v){ _max_jitter = v; }

  GLfloat get_max_fork_angle() const { return _max_fork_angle; }
  void set_max_fork_angle(GLfloat v){ _max_fork_angle = v; }

  const vec4& get_color0() const { return _color0; }
  void set_color0(const vec4& v){ _color0 = v; }

  const vec4& get_color1() const { return _color1; }
  void set_color1(const vec4& v){ _color1 = v; }

  const std::string& get_pattern() const { return _pattern; }
  void set_pattern(const std::string& v){ _pattern = v; }

  const vec3_vector& get_vertices() const { return _vertices; }
  void set_vertices(const vec3_vector& v){ _vertices = v; }

private:
  bool use_cpu = false; // for debug purpose
  GLuint _vao = 0;
  GLuint _cpu_buffer = 0;
  GLuint _vertex_count = 2;
  GLuint _buffer_size = 0; // pingpong _buffer size
  GLfloat _billboard_width = 1;
  GLfloat _max_jitter = 0.5f; // normalized, in size of subdivide
  GLfloat _max_fork_angle = fpi4; // along parent for gs shader, along global for cpu
  vec3 _camera_pos;
  vec4 _color0 = vec4(1);
  vec4 _color1 = vec4(0, 0, 1, 1);

  std::string _pattern;
  vec3_vector _vertices;
  std::vector<vertex> _billboards; // only used for cpu render
  pingpong _buffer;

};
}


#endif /* LIGHTNING_H */
