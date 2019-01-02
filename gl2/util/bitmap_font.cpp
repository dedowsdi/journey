#include <sstream>
#include <memory>

#include "glad/glad.h"
#include "app.h"
#include "freetype_text.h"
#include "quad.h"
#include "texutil.h"
#include "bitmap_text.h"
#include <fstream>
#include <iomanip>
#include "string_util.h"
#include "program.h"
#include "common_program.h"
#include "stream_util.h"
#include "glc_utf8.h"

namespace zxd {

using namespace glm;

bool is_export_bitmap = false;
GLuint fbo;
GLuint font_tex;

GLfloat wnd_aspect = 1;
GLuint wnd_width = 512;
GLuint wnd_height = 512 + 128;
GLuint tex_size = 256;
GLuint font_size = 12;  // height

std::string text;
std::string font;
std::string input_file;

std::unique_ptr<zxd::freetype_text> ft_draw;
std::unique_ptr<zxd::freetype_text> ft_info;
std::unique_ptr<zxd::bitmap_text> bt;

std::map<uint32_t, zxd::bitmap_text::glyph> glyphs;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "bitmap_font";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGB;
    m_info.wnd_width = wnd_width;
    m_info.wnd_height = wnd_height;
    m_info.samples = 16;
  }

  void display(void) {
    // render texture
    glViewport(0, 0, tex_size, tex_size);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glyphs.clear();

    GLuint x = 0;
    GLuint y = tex_size - ft_draw->get_linespace();

    auto beg = make_utf8_beg(text.begin(), text.end());
    auto end = make_utf8_end(text.begin(), text.end());

    while(beg != end)
    {
      auto character = *beg++;
      const zxd::freetype_text::Glyph& glyph = ft_draw->getGlyph(character);

      if ((x + glyph.advance) > tex_size) {
        x = 0;
        y -= ft_draw->get_linespace();
      }

      ft_draw->print(glyph, x, y);

      zxd::bitmap_text::glyph bmg;
      bmg.x_min = glyph.x_min;
      bmg.x_max = glyph.x_max;
      bmg.y_min = glyph.y_min;
      bmg.y_max = glyph.y_max;
      bmg.s_min = x + glyph.x_min;
      bmg.s_max = x + glyph.x_max;
      bmg.t_min = y + glyph.y_min;
      bmg.t_max = y + glyph.y_max;
      bmg.advance = glyph.advance;
      glyphs.insert(std::make_pair(character, bmg));

      x += glyph.advance;
    }

    // display texture
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_BLEND);

    if (is_export_bitmap) {
      export_bitmap();
      is_export_bitmap = false;
    }

    if (tex_size < wnd_width)
      glViewport(0, wnd_height - tex_size, tex_size, tex_size);
    else
      glViewport(0, wnd_height - wnd_width, wnd_width, wnd_width);

    draw_quad(font_tex, 0);

    glViewport(0, 0, wnd_width, wnd_height);

    char info[512];
    sprintf(info,
      "qQ : texture size : %d \n"
      "wW : font size : %d \n"
      "e : export to %s\n"
      "characters : %ld \n",
      tex_size, font_size, output_name().c_str(), utf8::distance(text.begin(), text.end()));

    glEnable(GL_BLEND);
    ft_info->print(info, 10, wnd_height - wnd_width - 10);

    bt->print(
      "bitmap text : when will TWOW be released? \n"
      "              2018? 2019? 2020?",
      10, 50);

    glutSwapBuffers();
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);

    // characters
    //text = stream_util::read_string(input_file);
    text = stream_util::read_file(input_file);
    text += u8" \x1"; // manually add space

    bt = std::make_unique<zxd::bitmap_text>();
    bt->init();

    // free type  printer
    ft_draw = std::make_unique<zxd::freetype_text>(font);
    ft_draw->set_height(font_size);
    ft_draw->init(text);

    ft_info = std::unique_ptr<zxd::freetype_text>(
      new zxd::freetype_text("font/DejaVuSansMono.ttf"));
    ft_info->init();
    ft_info->reset_content_as_ascii();

    // rtt
    glGenTextures(1, &font_tex);
    glBindTexture(GL_TEXTURE_2D, font_tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    resize_texture();

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, font_tex, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      printf("incomplete frame buffer");
  }

  void reshape(int w, int h) {
    app::reshape(w, h);

    ft_info->reshape(m_info.wnd_width, m_info.wnd_height);
    bt->reshape(m_info.wnd_width, m_info.wnd_height);
  }

  std::string output_name()
  {
    // use default name, it's face_max_advance_height
    std::stringstream ss;
    ss << string_util::basename(string_util::tail(font)) << "_"
    << ft_draw->get_height();
    //<< ft_draw->get_height() << "_" << ft_draw->get_max_advance();
    return ss.str();
  }

  void resize_texture() {
    glBindTexture(GL_TEXTURE_2D, font_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, tex_size, tex_size, 0, GL_RED,
      GL_UNSIGNED_BYTE, 0);
    ft_draw->reshape(tex_size, tex_size);
  }

  void resize_font() {
    ft_draw->set_height(font_size);
    ft_draw->reset_content(text);
  }

  void export_bitmap() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, font_tex);

    std::ofstream ofs(output_name() + ".fmt");
    std::string texture_name = output_name() + ".png";

    ofs << std::left;
    ofs << std::setw(50) << "#texture";
    ofs << std::setw(50) << "#face";
    ofs << std::endl;

    ofs << std::setw(50) << texture_name;
    ofs << std::setw(50) << string_util::tail(ft_draw->get_face());
    ofs << std::endl << std::endl;

    ofs << std::setw(10) << "#count";
    ofs << std::setw(10) << "size";
    ofs << std::setw(10) << "height";
    ofs << std::setw(12) << "linespace";
    ofs << std::setw(15) << "max_advance";
    ofs << std::endl;

    ofs << std::setw(10) << utf8::distance(text.begin(), text.end());
    ofs << std::setw(10) << tex_size;
    ofs << std::setw(10) << font_size;
    ofs << std::setw(12) << ft_draw->get_linespace();
    ofs << std::setw(15) << ft_draw->get_max_advance();
    ofs << std::endl << std::endl;

    ofs << std::setw(10) << "#code";
    ofs << std::setw(10) << "x_min";
    ofs << std::setw(10) << "x_max";
    ofs << std::setw(10) << "y_min";
    ofs << std::setw(10) << "y_max";
    ofs << std::setw(10) << "s_min";
    ofs << std::setw(10) << "s_max";
    ofs << std::setw(10) << "t_min";
    ofs << std::setw(10) << "t_max";
    ofs << std::setw(10) << "advance";
    ofs << std::endl;

    ofs
      << "---------------------------------------------------------------------"
         "-------------------------------"
      << std::endl;

    for (const auto& p : glyphs) {
      const zxd::bitmap_text::glyph& glyph = p.second;

      ofs << std::setw(10) << std::hex << p.first;
      ofs.unsetf(std::ios_base::hex);
      ofs << std::setw(10) << glyph.x_min;
      ofs << std::setw(10) << glyph.x_max;
      ofs << std::setw(10) << glyph.y_min;
      ofs << std::setw(10) << glyph.y_max;
      ofs << std::setw(10) << glyph.s_min;
      ofs << std::setw(10) << glyph.s_max;
      ofs << std::setw(10) << glyph.t_min;
      ofs << std::setw(10) << glyph.t_max;
      ofs << std::setw(10) << glyph.advance;
      ofs << std::endl;
    }

    ofs.close();

    zxd::save_texture(
      texture_name, GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, 8);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
        tex_size *= 2;
        resize_texture();
        break;

      case 'Q':
        if (tex_size == 1) {
          return;
        }
        tex_size /= 2;
        resize_texture();
        break;

      case 'w':
        font_size += 1;
        resize_font();
        break;

      case 'W':
        if (font_size == 1) {
          return;
        }
        font_size -= 1;
        resize_font();
        break;

      case 'e':
      case 'E':
        is_export_bitmap = true;
        break;
    }
  }
};
}
int main(int argc, char** argv) {
  if (argc < 3) {
    std::cout << "illegal command, it should be " << argv[0]
              << " font input_file " << std::endl;
    return 0;
  }

  zxd::font = argv[1];
  zxd::input_file = argv[2];

  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
