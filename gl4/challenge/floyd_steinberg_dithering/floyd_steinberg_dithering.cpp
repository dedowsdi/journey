#include "app.h"
#include "bitmaptext.h"
#include <sstream>
#include "common_program.h"
#include "texutil.h"
#include "quad.h"

namespace zxd {

lightless_program prg;
fipImage img;
quad q;
vec3_vector pixels;

class dither_app : public app {
protected:
  bitmap_text m_text;
  GLuint m_tex0;
  GLuint m_tex1;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "dither_app";
    m_info.wnd_width = img.getWidth() * 2;
    m_info.wnd_height = img.getHeight();
  }

  GLuint index(GLuint row, GLuint col)
  {
    return img.getWidth() * row + col;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.with_texcoord = true;
    prg.init();
    prg.mvp_mat = mat4(1);

    q.include_texcoord(true);
    q.build_mesh();

    glGenTextures(1, &m_tex0);
    glBindTexture(GL_TEXTURE_2D, m_tex0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.getWidth(), img.getHeight(), 0,
        GL_BGRA, GL_UNSIGNED_BYTE, img.accessPixels());

    pixels.resize(img.getWidth() * img.getHeight());
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, glm::value_ptr(pixels.front()));

    glGenTextures(1, &m_tex1);
    glBindTexture(GL_TEXTURE_2D, m_tex1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.getWidth(), img.getHeight(), 0, GL_RGB, GL_FLOAT, 0);

    vec3_vector pallete;
    pallete.push_back(vec3(0));
    //pallete.push_back(vec3(0.25));
    //pallete.push_back(vec3(0.5));
    //pallete.push_back(vec3(0.75));
    pallete.push_back(vec3(1));

    for(auto& item : pixels)
    {
      item = vec3(zxd::rgb2luminance(item));
    }

    for (int y = img.getHeight() - 1; y >= 0; --y) 
    {
      for (int x = 0; x < img.getWidth(); ++x) 
      {
        vec3 old_pixel = pixels[index(y,  x)];
        const vec3& new_pixel = step(vec3(0.5), old_pixel);
        pixels[index(y, x)] = new_pixel;
        vec3 quantize_error = old_pixel - new_pixel;
        //std::cout << quantize_error << std::endl;
        if(x < img.getWidth() - 1)
          pixels[index(y, x + 1    )] += quantize_error * (7.0f / 16);
        if(y == 0)
          continue;
        if(x >= 1)
          pixels[index(y - 1, x - 1)] += quantize_error * (3.0f / 16);
        pixels[index(y - 1    , x)] += quantize_error * (5.0f / 16);
        if(x < img.getWidth() - 1)
          pixels[index(y - 1, x + 1)] += quantize_error * (1.0f / 16);
      }
    }

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.getWidth(), img.getHeight(),
        GL_RGB, GL_FLOAT, glm::value_ptr(pixels.front()));

  }

  virtual void update() {}

  virtual void display() {

    glViewport(0, 0, img.getWidth(), img.getHeight());
    glScissor(0, 0, img.getWidth(), img.getHeight());
    glEnable(GL_SCISSOR_TEST);

    prg.use();
    glBindTexture(GL_TEXTURE_2D, m_tex0);
    glUniform1i(prg.ul_diffuse_map, 0);
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(prg.mvp_mat));
    q.draw();

    glViewport(img.getWidth(), 0, img.getWidth(), img.getHeight());
    glScissor(img.getWidth(), 0, img.getWidth(), img.getHeight());
    glEnable(GL_SCISSOR_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    glBindTexture(GL_TEXTURE_2D, m_tex1);
    glUniform1i(prg.ul_diffuse_map, 0);
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(prg.mvp_mat));
    q.draw();


    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "";
    //m_text.print(ss.str(), 10, wnd_height()- 20);
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
        default:
          break;
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
  }
};
}

int main(int argc, char *argv[]) {
  zxd::dither_app app;
  zxd::img = zxd::fipLoadResource32("texture/sunflower.png");
  app.run();
}
