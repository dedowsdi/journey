#include "app.h"
#include "bitmap_text.h"
#include "quad.h"
#include "program.h"
#include "common_program.h"
#include "texutil.h"
#include <sstream>
#include <queue>
#include <algorithm>

namespace zxd {

quad_program prg;

// https://en.wikipedia.org/wiki/Flood_fill
void rand_rgb_flood_fill(glm::u8vec3* pixels, const ivec2& size, const ivec2& cursor)
{
  GLint width = size.x;
  GLint height = size.y;

  glm::u8vec3 color0 = *(pixels + width * cursor.y + cursor.x);
  glm::u8vec3 color1 = glm::linearRand(glm::vec3(0), glm::vec3(255));
  std::queue<ivec2> q;
  q.push(cursor);

  while(!q.empty())
  {
    const ivec2& item = q.front();
    q.pop();

    if(*(pixels + width * item.y + item.x) == color1)
      continue;

    // get west and east bounday
    GLint west = item.x;
    GLint east = item.x + 1;
    while(west > 0 && *(pixels + width * item.y + west -1) == color0)
      --west;

    while(east < width  && *(pixels + width * item.y + east) == color0)
      ++east;

    //std::cout << "item " << item.x << ":" << item.y << " west : " << west << ", east : " << east << std::endl;

    while(west < east)
    {
      // change color
      *(pixels + width * item.y + west) = color1;

      // add north and south item
      if(item.y < (height - 1) && *(pixels + width * (item.y + 1) + west) == color0 )
        q.push(vec2(west, item.y + 1));

      if(item.y > 0 && *(pixels + width * (item.y - 1) + west) == color0 )
        q.push(vec2(west, item.y - 1));

      ++west;
    }
  }

}

class flood_fill : public app {
protected:
  bitmap_text m_text;
  quad m_quad;
  GLuint m_texture;
  fipImage m_image;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "sine wave";
    m_image = fipLoadResource("texture/z.png");
    m_info.wnd_width = m_image.getWidth();
    m_info.wnd_height = m_image.getHeight();
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.init();

    m_quad.include_texcoord(true);
    m_quad.build_mesh();
    m_quad.bind_vao();

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_image.getWidth(), m_image.getHeight(),
        0, GL_RGB, GL_UNSIGNED_BYTE, m_image.accessPixels());
  }

  virtual void update() {
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    prg.use();

    glUniform1i(prg.ul_quad_map, 0);
    m_quad.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    m_text.print(ss.str(), 10, 492);
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
  }
  
  virtual void glfw_mouse_button(
    GLFWwindow *wnd, int button, int action, int mods){

    app::glfw_mouse_button(wnd, button, action, mods);
    if(button != GLFW_MOUSE_BUTTON_LEFT)
      return;

    dvec2 cursor;
    glfwGetCursorPos(m_wnd, &cursor[0], &cursor[1]);
    cursor[1] = glfw_to_gl(cursor[1]);

    GLchar* pixels = new GLchar[m_image.getWidth() * m_image.getHeight() * 3];
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    rand_rgb_flood_fill(reinterpret_cast<glm::u8vec3 *>(pixels), ivec2(wnd_width(), wnd_height()), cursor); 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_image.getWidth(), m_image.getHeight(),
        0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    delete[] pixels;

  }

};
}

int main(int argc, char *argv[]) {
  zxd::flood_fill app;
  app.run();
}
