#include "glad/glad.h"
#include "app.h"
#include "freetype_text.h"
#include <sstream>
#include "quad.h"
#include "texutil.h"
#include "bitmaptext.h"
#include <fstream>
#include <iomanip>
#include "stringutil.h"
#include "program.h"
#include "common_program.h"

namespace zxd {

using namespace glm;

bool is_export_bitmap = false;
GLuint fbo;
GLuint font_tex;

GLfloat wnd_aspect = 1;
GLuint wnd_width = 512;
GLuint wnd_height = 512 + 128;
GLuint tex_size = 256;
GLuint font_size = 15;  // height
GLuint num_characters = 256;

std::string text;
std::string font;
std::string output_name;

std::unique_ptr<zxd::freetype_text> ft0;
std::unique_ptr<zxd::freetype_text> ft1;
std::unique_ptr<zxd::bitmap_text> bt;

std::vector<zxd::bitmap_text::glyph> glyphs;

quad quad0;

quad_program quad_prg;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "bitmap_font";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGB;
    m_info.wnd_width = wnd_width;
    m_info.wnd_height = wnd_height;
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
    GLuint y = tex_size - ft0->get_linespace();

    for (int i = 0; i < num_characters; ++i) {
      const zxd::freetype_text::Glyph& glyph = ft0->getGlyph(i);

      if ((x + glyph.advance) > tex_size) {
        x = 0;
        y -= ft0->get_linespace();
      }

      ft0->print(glyph, x, y);

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
      glyphs.push_back(bmg);

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

    quad_prg.use();
    quad_prg.update_uniforms(0);
    glBindTexture(GL_TEXTURE_2D, font_tex);
    quad0.draw();

    glViewport(0, 0, wnd_width, wnd_height);

    char info[512];
    sprintf(info,
      "qQ : texture size : %d \n"
      "wW : font size : %d \n"
      "e : export to %s\n",
      tex_size, font_size, output_name.c_str());

    glEnable(GL_BLEND);
    ft1->print(info, 10, wnd_height - wnd_width - 10);

    bt->print(
      "bitmap text : when will TWOW be released? \n"
      "              2018? 2019? 2020?",
      10, 50);

    glutSwapBuffers();
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);

    bt = std::unique_ptr<zxd::bitmap_text>(new zxd::bitmap_text());
    bt->init();

    // free type  printer
    ft0 = std::unique_ptr<zxd::freetype_text>(new zxd::freetype_text(font));
    ft0->set_num_characters(num_characters);
    ft0->set_height(font_size);
    ft0->init();

    if (output_name.empty()) {
      // use default name, it's face_max_advance_height
      std::stringstream ss;
      ss << zxd::string_util::basename(zxd::string_util::tail(font)) << "_"
         << ft0->get_height() << "_" << ft0->get_max_advance();
      output_name = ss.str();
    }

    ft1 = std::unique_ptr<zxd::freetype_text>(
      new zxd::freetype_text("data/font/DejaVuSansMono.ttf"));
    ft1->init();

    // characters
    text.reserve(num_characters);
    for (int i = 0; i < num_characters; ++i) {
      text.push_back(i);
    }

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

    quad_prg.init();
    quad0.build_mesh();
    quad0.bind(quad_prg.al_vertex, -1, quad_prg.al_texcoord);
  }

  void reshape(int w, int h) {
    app::reshape(w, h);

    ft1->reshape(m_info.wnd_width, m_info.wnd_height);
    bt->reshape(m_info.wnd_width, m_info.wnd_height);
  }

  void resize_texture() {
    glBindTexture(GL_TEXTURE_2D, font_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, tex_size, tex_size, 0, GL_RED,
      GL_UNSIGNED_BYTE, 0);
    ft0->reshape(tex_size, tex_size);
  }

  void resize_font() {
    ft0->set_height(font_size);
    ft0->update_glyph_dict();
  }

  void export_bitmap() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, font_tex);

    std::ofstream ofs(output_name + ".fmt");
    std::string texture_name = output_name + ".png";

    ofs << std::left;
    ofs << std::setw(50) << "#texture";
    ofs << std::setw(50) << "#face";
    ofs << std::endl;

    ofs << std::setw(50) << texture_name;
    ofs << std::setw(50) << ft0->get_face();
    ofs << std::endl << std::endl;

    ofs << std::setw(10) << "#count";
    ofs << std::setw(10) << "size";
    ofs << std::setw(10) << "height";
    ofs << std::setw(12) << "linespace";
    ofs << std::setw(15) << "max_advance";
    ofs << std::endl;

    ofs << std::setw(10) << num_characters;
    ofs << std::setw(10) << tex_size;
    ofs << std::setw(10) << font_size;
    ofs << std::setw(12) << ft0->get_linespace();
    ofs << std::setw(15) << ft0->get_max_advance();
    ofs << std::endl << std::endl;

    ofs << std::setw(10) << "#char";
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

    for (int i = 0; i < glyphs.size(); ++i) {
      const zxd::bitmap_text::glyph& glyph = glyphs[i];

      ofs << std::setw(10) << i;
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
              << " font size [numCharacters] [output name] " << std::endl;
    return 0;
  }

  zxd::font = argv[1];
  zxd::font_size = std::stoi(argv[2]);

  if (argc >= 4) {
    zxd::num_characters = std::stoi(argv[3]);
  }
  if (argc >= 5) {
    zxd::output_name = std::stoi(argv[4]);
  }
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
