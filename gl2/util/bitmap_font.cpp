#include <sstream>
#include <memory>
#include <algorithm>
#include <fstream>
#include <iomanip>

#include <app.h>
#include <freetype_text.h>
#include <quad.h>
#include <texutil.h>
#include <bitmap_text.h>
#include <string_util.h>
#include <program.h>
#include <common_program.h>
#include <stream_util.h>
#include <glc_utf8.h>

namespace zxd {

using namespace glm;

bool is_export_bitmap = false;

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
std::unique_ptr<zxd::bitmap_text> bt_draw;
std::unique_ptr<zxd::bitmap_text> bt_info;

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
    // display texture
    glClear(GL_COLOR_BUFFER_BIT);

    if (is_export_bitmap) {
      export_bitmap();
      is_export_bitmap = false;
    }

    if (tex_size < wnd_width)
      glViewport(0, wnd_height - tex_size, tex_size, tex_size);
    else
      glViewport(0, wnd_height - wnd_width, wnd_width, wnd_width);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    draw_quad(bt_draw->texture(), 0);

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

    bt_info->print(
      "bitmap text : when will TWOW be released? \n"
      "              2018? 2019? 2020?",
      10, 50);

    glutSwapBuffers();
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);

    // read utf8 character source file
    //text = stream_util::read_string(input_file);
    text = stream_util::read_file(input_file);
    text.erase(std::remove_if(text.begin(), text.end(),
          [](auto c)->bool
          {
            return c == ' ' || c == '\n';
          }), text.end());
    text += u8" \x1"; // manually add space

    bt_info = std::make_unique<zxd::bitmap_text>();
    bt_info->init(true);

    // free type  printer
    ft_draw = std::make_unique<zxd::freetype_text>(font);
    ft_draw->set_height(font_size);
    ft_draw->init(text, true);

    ft_info = std::unique_ptr<zxd::freetype_text>(
      new zxd::freetype_text("font/DejaVuSansMono.ttf"));
    ft_info->init("", true);
    ft_info->reset_content_as_ascii();

    resize_texture();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);

    ft_info->reshape(m_info.wnd_width, m_info.wnd_height);
    bt_info->reshape(m_info.wnd_width, m_info.wnd_height);
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
    ft_draw->reshape(tex_size, tex_size);
    bt_draw = ft_draw->create_bitmap_text(tex_size, tex_size);
  }

  void resize_font() {
    ft_draw->set_height(font_size);
    ft_draw->reset_content(text);
    resize_texture();
  }

  void export_bitmap() {
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
    ofs << std::setw(15) << "tex_width";
    ofs << std::setw(15) << "tex_height";
    ofs << std::setw(10) << "height";
    ofs << std::setw(12) << "linespace";
    ofs << std::setw(15) << "max_advance";
    ofs << std::endl;

    ofs << std::setw(10) << utf8::distance(text.begin(), text.end());
    ofs << std::setw(15) << tex_size;
    ofs << std::setw(15) << tex_size;
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

    ofs << "---------------------------------------------------------------------"
           "-------------------------------"
        << std::endl;

    for (const auto& p : bt_draw->glyph_dict()) {
      const zxd::bitmap_text::glyph& glyph = p.second;

      ofs << std::setw(10) << std::hex << p.first;
      ofs.unsetf(std::ios_base::hex);
      ofs << std::setw(10) << glyph.x_min;
      ofs << std::setw(10) << glyph.x_max;
      ofs << std::setw(10) << glyph.y_min;
      ofs << std::setw(10) << glyph.y_max;
      ofs << std::setw(10) << glm::round(glyph.s_min * tex_size);
      ofs << std::setw(10) << glm::round(glyph.s_max * tex_size);
      ofs << std::setw(10) << glm::round(glyph.t_min * tex_size);
      ofs << std::setw(10) << glm::round(glyph.t_max * tex_size);
      ofs << std::setw(10) << glyph.advance;
      ofs << std::endl;
    }

    ofs.close();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bt_draw->texture());
    zxd::save_texture(texture_name, GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, 8);
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
