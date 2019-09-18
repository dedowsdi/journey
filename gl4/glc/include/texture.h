#ifndef GL4_GLC_TEXTURE_H
#define GL4_GLC_TEXTURE_H

#include <array>
#include <memory>

#include <gl.h>
#include <glm.h>

namespace zxd
{

class image
{
public:
  image(const std::string& filename = "");

  void load(const std::string& filename);

  const std::string& get_filename() const { return _filename; }

  GLuint get_width() const { return _width; }
  void set_width(GLuint v){ _width = v; }

  GLuint get_height() const { return _height; }
  void set_height(GLuint v){ _height = v; }

  const char* get_data() const;
  char* get_data();
  void set_data(std::vector<GLchar>&& data, GLuint width, GLuint height);
  void set_data(const std::vector<GLchar>& data, GLuint width, GLuint height);

private:
  GLuint _width;
  GLuint _height;
  std::string _filename;
  std::vector<GLchar> _data;
};

struct tex_image
{
  GLint internalFormat;
  GLsizei width;
  GLsizei height;
  GLint border;
  GLenum format;
};

class texture
{
  texture();
  ~texture();

  texture(const texture&) = delete;
  texture& operator=(const texture&) = delete;

  void bind(GLenum target);

  void generate_mipmap();

  GLuint get_object() const { return _object; }

  GLenum get_target() const { return _target; }

  const vec4& get_border_color() const { return _border_color; }
  void set_border_color(const vec4& v){ _border_color = v; }

  GLenum get_wrap_s() const { return _wrap_s; }
  void set_wrap_s(GLenum v){ _wrap_s = v; }

  GLenum get_wrap_t() const { return _wrap_t; }
  void set_wrap_t(GLenum v){ _wrap_t = v; }

  GLenum get_wrap_r() const { return _wrap_r; }
  void set_wrap_r(GLenum v){ _wrap_r = v; }

  GLenum get_min_filter() const { return _min_filter; }
  void set_min_filter(GLenum v){ _min_filter = v; }

  GLenum get_max_filter() const { return _max_filter; }
  void set_max_filter(GLenum v){ _max_filter = v; }


  const std::shared_ptr<image>& get_img() const { return _img; }
  void set_img(const std::shared_ptr<image>& v,
    GLenum internal_format = GL_RGBA, GLenum format = GL_RGBA,
    GLenum type = GL_UNSIGNED_BYTE);

private:
  GLuint _object{0};
  GLenum _target{GL_TEXTURE_2D};
  GLenum _wrap_s{GL_REPEAT};
  GLenum _wrap_t{GL_REPEAT};
  GLenum _wrap_r{GL_REPEAT};
  GLenum _min_filter{GL_NEAREST_MIPMAP_NEAREST};
  GLenum _max_filter{GL_LINEAR};
  vec4 _border_color{0,0,0,0};

  std::shared_ptr<image> _img;
};

}
#endif /* GL4_GLC_TEXTURE_H */
