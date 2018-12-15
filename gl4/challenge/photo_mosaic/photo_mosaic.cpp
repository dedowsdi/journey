#include "app.h"
#include "bitmaptext.h"
#include <sstream>
#include "texutil.h"
#include <algorithm>

#define WIDTH 800
#define HEIGHT 800
#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

namespace zxd {

std::string file_path;
kcip kci_rows;
kcip kci_cols;
fipImage img; // photo

// a single tile in image pack
struct tile 
{
  vec3 color; //average color
  GLuint row;
  GLuint col;
};

typedef std::vector<tile> tile_vector;

vec3 get_averate_block_color_2d(vec3* pixels, const ivec2& tex_size, const vec2& start, const vec2& end)
{
  vec3 color(0);
  ivec2 start_pixel = vec2(start.x  * tex_size.x, start.y * tex_size.y);
  vec2 end_pixel = vec2(end.x * tex_size.x, end.y * tex_size.y);
  for (int y = start_pixel.y; y < end_pixel.y; ++y) 
  {
    for (int x = start_pixel.x; x < end_pixel.x; ++x) 
    {
      color += pixels[y*tex_size.x + x];
    }
  }

  color /= (end_pixel.x-start_pixel.x) * (end_pixel.y - start_pixel.y);
  return color;
};

class image_pack
{
protected:
  GLuint m_rows = -1;
  GLuint m_cols = -1;
  GLuint m_tex = -1;
  fipImage m_img;
  tile_vector m_tiles;
public:
  image_pack(){}

  void read_pack(const std::string& file)
  {
    if(m_tex == -1)
    {
      glGenTextures(1, &m_tex);
      glBindTexture(GL_TEXTURE_2D, m_tex);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
      glBindTexture(GL_TEXTURE_2D, m_tex);
    }

    m_img = fipLoadResource32(file);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_img.getWidth(), m_img.getHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, m_img.accessPixels());
    glGenerateMipmap(GL_TEXTURE_2D);

    m_tiles.reserve(m_cols * m_rows);
    vec3_vector pixels;
    pixels.resize(m_img.getWidth() * m_img.getHeight());
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, glm::value_ptr(pixels.front()));

    vec2 tz = tile_texcoord_size();
    for (int y = 0; y < m_rows; ++y) 
    {
      //std::cout << "build tile row " << y << std::endl;
      for (int x = 0; x < m_cols; ++x) 
      {
        tile t;
        t.row = y;
        t.col = x;
        vec2 start = vec2(static_cast<GLfloat>(x)/m_cols, static_cast<GLfloat>(x)/m_rows);
        t.color = get_averate_block_color_2d(&pixels.front(), ivec2(m_img.getWidth(), m_img.getHeight()),
            start, start + tz);
        std::cout << "tile " << y << ":" << x << " color : " << t.color << std::endl;;
        m_tiles.push_back(t);
      }
    }
  }

  GLuint rows() const { return m_rows; }
  void rows(GLuint v){ m_rows = v; }

  GLuint cols() const { return m_cols; }
  void cols(GLuint v){ m_cols = v; }

  // get tile with most similar color
  vec2 get_tile(const vec3& color)
  {
    GLfloat min_difference = 10000000;
    GLint tile_index = -1;
    for (int i = 0; i < m_tiles.size(); ++i) {
      GLfloat diff = color_difference2(m_tiles[i].color, color);
      //GLfloat diff = color_difference_256_2(m_tiles[i].color, color);
      if(diff < min_difference)
      {
        tile_index = i;
        min_difference = diff;
      }
    }
    tile& t = m_tiles[tile_index];
    return vec2(t.col, t.row) * tile_texcoord_size();
  }
  
  vec2 tile_texcoord_size()
  {
    return vec2(1.0/m_cols, 1.0/m_rows);
  }

  GLuint tex() const { return m_tex; }
  void tex(GLuint v){ m_tex = v; }
}pack;

class photo_mosaic_protroam : public program
{
public:
  GLint ul_diffuse_map;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader/photo_mosaic.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader/photo_mosaic.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_diffuse_map, "diffuse_map");
  }

  void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }

} prg;


struct photo
{
  GLuint vao;
  GLuint vbo;
  GLuint tex;
  vec3_vector pixels;

  void init()
  {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    GLuint stride = sizeof(vec2) * 2;
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(8));
    glEnableVertexAttribArray(1);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getWidth(), img.getHeight(), 
        0, GL_BGRA, GL_UNSIGNED_BYTE, img.accessPixels());

    pixels.resize(img.getWidth() * img.getHeight());
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, glm::value_ptr(pixels.front()));
  }

  void update(GLuint rows, GLuint cols)
  {
    vec2 mosaic_size;
    mosaic_size.x = static_cast<GLfloat>(img.getWidth()) / cols;
    mosaic_size.y = static_cast<GLfloat>(img.getHeight()) / rows;

    vec2 mosaic_tc_size = vec2(1.0/cols, 1.0/rows);

    vec2_vector vertices; // vertex0, texcoord0, vertex1, texcoord1, ......
    // recrate triangles
    vec2 tile_texcoord_size = pack.tile_texcoord_size();
    

    for (int y = 0; y < rows; ++y) 
    {
      float row_start = y * mosaic_size.y;

      for (int x = 0; x < cols; ++x) 
      {
        vec2 v0 = vec2(x * mosaic_size.x, row_start);
        vec2 v1 = v0 + vec2(mosaic_size.x, 0);
        vec2 v2 = v0 + mosaic_size;
        vec2 v3 = v0 + vec2(0, mosaic_size.y);
        
        vec2 mosaic_tc_start = mosaic_tc_size * vec2(x, y);
        vec2 mosaic_tc_end = mosaic_tc_start + mosaic_tc_size;

        vec3 mosaic_color = get_averate_block_color_2d(&pixels.front(), 
            ivec2(img.getWidth(), img.getHeight()), mosaic_tc_start, mosaic_tc_end);

        vec2 tc0 = pack.get_tile(mosaic_color);
        vec2 tc1 = tc0 + vec2(tile_texcoord_size.x, 0);
        vec2 tc2 = tc0 + tile_texcoord_size;
        vec2 tc3 = tc0 + vec2(0, tile_texcoord_size.y);

        vertices.push_back(v0);
        vertices.push_back(tc0);
        vertices.push_back(v1);
        vertices.push_back(tc1);
        vertices.push_back(v2);
        vertices.push_back(tc2);

        vertices.push_back(v0);
        vertices.push_back(tc0);
        vertices.push_back(v2);
        vertices.push_back(tc2);
        vertices.push_back(v3);
        vertices.push_back(tc3);
        //std::cout << "build mosaic " << y << ":" << x << " to " << tc0 << " " << tc2 << std::endl;
      }
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec2), glm::value_ptr(vertices.front()), GL_STATIC_DRAW);
  }

  void draw()
  {
    glBindTexture(GL_TEXTURE_2D, pack.tex());
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, kci_cols->get_float() * kci_rows->get_float() * 6);
  }

}geometry;

class photo_mosaic_app : public app {
protected:
  bitmap_text m_text;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "photo_mosaic_app";
    m_info.wnd_width = img.getWidth();
    m_info.wnd_height = img.getHeight();
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    auto kci_callback = std::bind(std::mem_fn(&photo_mosaic_app::update_mosaic), this, std::placeholders::_1);
    kci_rows = m_control.add_control(GLFW_KEY_Q, img.getHeight()/20, 1, 10000, 1, kci_callback);
    kci_cols = m_control.add_control(GLFW_KEY_W, img.getWidth()/20, 1, 10000, 1, kci_callback);

    m_text.init();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);

    prg.init();
    prg.fix2d_camera(0, img.getWidth(), 0, img.getHeight());

    pack.rows(32);
    pack.cols(16);
    pack.read_pack("texture/wow_ui.jpg");

    geometry.init();
    update_mosaic(0);
  }

  virtual void update_mosaic(const kci* kci)
  {
    geometry.update(kci_rows->get_float(), kci_cols->get_float());
  }

  virtual void update() {}

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(prg.vp_mat));
    glUniform1i(prg.ul_diffuse_map, 0);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    geometry.draw();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : rows : " << kci_rows->get_float() << std::endl;;
    ss << "wW : cols : " << kci_cols->get_float() << std::endl;;
    m_text.print(ss.str(), 10, m_info.wnd_height - 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;
        default:
          break;
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
  }
};
}

int main(int argc, char *argv[]) {
  if(argc < 2)
  {
    std::cout << "it should be " << argv[0] << " picture";
    return 0;
  }

  zxd::file_path = argv[1];
  zxd::img = zxd::fipLoadResource32(zxd::file_path);
  zxd::photo_mosaic_app app;
  app.run();
}
