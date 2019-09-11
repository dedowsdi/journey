/*
 *  texprox.c
 *  the brief program illustrates use of texture proxies.
 *  this program only prints out some messages about whether
 *  certain size textures are supported and then exits.
 *
 *  note that texture proxies dont't care if there are still enough texture
 *  resources to be used.
 */
#include <app.h>

namespace zxd {

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "texprox";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  void create_scene(void) {
    GLint proxy_components;

    glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_RGBA8, 64, 64, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, NULL);
    glGetTexLevelParameteriv(
      GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &proxy_components);
    printf("proxy_components are %d\n", proxy_components);
    if (proxy_components == GL_RGBA8)
      printf("proxy allocation succeeded\n");
    else
      printf("proxy allocation failed\n");

    glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_RGBA16, 2048 * 256, 2048 * 256, 0,
      GL_RGBA, GL_UNSIGNED_SHORT, NULL);
    glGetTexLevelParameteriv(
      GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &proxy_components);
    printf("proxy_components are %d\n", proxy_components);
    if (proxy_components == GL_RGBA16)
      printf("proxy allocation succeeded\n");
    else
      printf("proxy allocation failed\n");
  }

  void display(void) { exit(0); }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
  }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
