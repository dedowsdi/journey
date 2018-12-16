#include "app.h"
#include "bitmaptext.h"
#include "texutil.h"
#include "quad.h"
#include "common_program.h"
#include "functor.h"

#include <algorithm>
#include <sstream>

#define MAX_WIDTH 1280
#define MAX_HEIGHT 640

namespace zxd {

std::string file_path;
fipImage img;
quad q;
GLuint tex0, tex1; // original, sorted
std::vector<u8vec3> rgb_pixels; // rgb_pixels of tex1
std::vector<vec3> hsb_pixels; // hsb cache
std::vector<u8vec3> pixels;

quad_program prg;

GLint sort_type = 0;
std::string sort_type_str[] = {"r","g","b","hue","saturation","brightness"};

class app_name : public app {
protected:
  bitmap_text m_text;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "app_name";
    GLfloat img_aspect = static_cast<GLfloat>(img.getWidth()) / img.getHeight();
    if(img_aspect >= 1)
    {
      m_info.wnd_width = MAX_WIDTH;
      m_info.wnd_height = MAX_WIDTH * 0.5 / img_aspect;
    }
    else
    {
      m_info.wnd_height = MAX_HEIGHT;
      m_info.wnd_width = MAX_HEIGHT * 2 * img_aspect;
    }
  }

  void sort_by_hsb_at(int component_index)
  {
    std::vector<int> indexes(pixels.size());
    std::iota(indexes.begin(), indexes.end(), 0);
    std::sort(indexes.begin(), indexes.end(), 
        [&](int id0, int id1)->bool
        {
          return hsb_pixels[id0][component_index] < hsb_pixels[id1][component_index];
        });

    for (int i = 0; i < indexes.size(); ++i) 
      pixels[i] = rgb_pixels[indexes[i]];
  }

  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.init();

    glGenTextures(1, &tex0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.getWidth(), img.getHeight(), 0, 
        GL_BGRA, GL_UNSIGNED_BYTE, img.accessPixels());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    rgb_pixels.resize(img.getWidth() * img.getHeight());
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, glm::value_ptr(rgb_pixels.front()));

    // build cache
    hsb_pixels.reserve(rgb_pixels.size());
    for(auto& pixel : rgb_pixels)
    {
      vec3 v(pixel.x/255.0f, pixel.y/255.0f, pixel.z/255.0f);
      hsb_pixels.push_back(rgb2hsb(v));
    }
    pixels = rgb_pixels;

    glGenTextures(1, &tex1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    sort_pixels();

    q.include_texcoord(true);
    q.build_mesh();
  }

  void updatePixels()
  {
    glBindTexture(GL_TEXTURE_2D, tex1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.getWidth(), img.getHeight(), 0, 
        GL_RGB, GL_UNSIGNED_BYTE, glm::value_ptr(pixels.front()));
  }

  void sort_pixels()
  {
    switch(sort_type)
    {
      case 0:
          std::sort(pixels.begin(), pixels.end(), zxd::sort_by_elem_at<u8vec3, 0>());
        break;

      case 1:
          std::sort(pixels.begin(), pixels.end(), zxd::sort_by_elem_at<u8vec3, 1>());
        break;

      case 2:
          std::sort(pixels.begin(), pixels.end(), zxd::sort_by_elem_at<u8vec3, 2>());
        break;

      case 3:
        sort_by_hsb_at(0);
        break;

      case 4:
        sort_by_hsb_at(1);
        break;

      case 5:
        sort_by_hsb_at(2);
        break;
      default:break;
    }

    updatePixels();
  }


  virtual void update() {}

  virtual void display() {

    prg.use();
    glEnable(GL_SCISSOR_TEST);

    glViewport(0, 0, wnd_width()*0.5, wnd_height());
    glScissor(0, 0, wnd_width()*0.5, wnd_height());
    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, tex0);
    prg.update_uniforms(0);
    q.draw();

    glViewport(wnd_width()*0.5, 0, wnd_width()*0.5, wnd_height());
    glScissor(wnd_width()*0.5, 0, wnd_width()*0.5, wnd_height());
    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, tex1);
    prg.update_uniforms(0);
    q.draw();
    glDisable(GL_SCISSOR_TEST);

    glViewport(0, 0, wnd_width(), wnd_height());
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q : sort rgb_pixels : " << sort_type_str[sort_type] << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 20, vec4(0,0,0,1));
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(wnd_width(), wnd_height());
  }

  virtual void glfw_key(
      GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;

        case GLFW_KEY_Q:
          sort_type = ++sort_type % 6;
          sort_pixels();
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

  zxd::app_name app;
  app.run();
}
