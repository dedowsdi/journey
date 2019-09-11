#include <map>
#include <sstream>
#include <iostream>

#include "glad/glad.h"
#include <app.h>
#include <program.h>
#include <glm.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glc_utf8.h>
#include <stream_util.h>

namespace zxd {

using namespace glm;

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512

vec4 text_color(1.0f);
GLuint vao;
GLuint vbo;
GLuint linespace;
GLuint max_advance;

struct Glyph {
  GLfloat x_min;  // the same as bearying X
  GLfloat y_min;  // bearyingY - height
  GLfloat x_max;  // x_min + width
  GLfloat y_max;  // the same as bearying Y
  GLfloat bearingX;
  GLfloat bearingY;
  GLfloat width;   // width of bounding box
  GLfloat height;  // height of bounding box, bitmap.rows
  GLfloat origin;
  GLuint advance;  // 1/64 pixel
  GLuint texture;
};

std::map<FT_ULong, Glyph> glyphDict;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "freetype_text";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGB;
    m_info.wnd_width = WINDOWS_WIDTH;
    m_info.wnd_height = WINDOWS_HEIGHT;
  }

  struct freetype_text_program : public zxd::program {
    GLint ul_text_color;
    GLint ul_font_map;
    GLint ul_mvp_mat;

    mat4 mvp_mat;

    freetype_text_program() {
      mvp_mat = glm::ortho(0.0f, (GLfloat)WINDOWS_WIDTH, 0.0f,
        (GLfloat)WINDOWS_HEIGHT, -1.0f, 1.0f);
    }
    virtual void update_frame() {
      glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
      glUniform4fv(ul_text_color, 1, value_ptr(text_color));
      glUniform1i(ul_font_map, 0);
    }
    virtual void attach_shaders() {
      attach(GL_VERTEX_SHADER, "shader2/freetype_text.vs.glsl");
      attach(
        GL_FRAGMENT_SHADER, "shader2/freetype_text.fs.glsl");
    }
    virtual void bind_uniform_locations() {
      ul_mvp_mat = get_uniform_location("mvp_mat");
      ul_text_color = get_uniform_location("text_color");
      ul_font_map = get_uniform_location("font_map");
    }
    virtual void bind_attrib_locations() {
      bind_attrib_location(0, "vertex");
    }
  } prg;

  void printText2D(const std::string& text, GLuint x, GLuint y, GLfloat scale) {
    GLuint nextY = y;
    GLuint nextX = x;

    auto beg = make_utf8_beg(text.begin(), text.end());
    auto end = make_utf8_end(text.begin(), text.end());

    while(beg != end)
    {
      auto character = *beg++;
      if (character == '\n') {
        nextX = x;
        nextY -= linespace * scale;
        continue;
      }

      auto iter = glyphDict.find(character);
      if(iter == glyphDict.end())
        std::cerr << "failed to find glyph for " << character << std::endl;

      const Glyph& glyph = iter->second;

      if (character == ' ') {
        nextX += glyph.advance * scale;
        continue;
      }

      // clang-format off
      // freetype generate texture from left to right, top to bottom, which means
      // we must flip y
      GLfloat vertices[4][4] = {
        {nextX + glyph.x_min * scale, nextY + glyph.y_min * scale, 0, 1},
        {nextX + glyph.x_max * scale, nextY + glyph.y_min * scale, 1, 1},
        {nextX + glyph.x_max * scale, nextY + glyph.y_max * scale, 1, 0},
        {nextX + glyph.x_min * scale, nextY + glyph.y_max * scale, 0, 0}
      };
      // clang-format on

      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

      glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
      glEnableVertexAttribArray(0);

      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, glyph.texture);

      glDrawArrays(GL_QUADS, 0, 4);

      nextX += glyph.advance * scale;
    }
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    prg.update_frame();

    GLchar info[256];
    // clang-format off
    sprintf(info, 
      " q : text_color : %.2f %.2f %.2f %.2f \n"
      " %s \n",
      text_color[0], text_color[1], text_color[2], text_color[3], u8"冰与火之歌");
    // clang-format on

    printText2D(info, 20, 300, 1.0);

    glutSwapBuffers();
  }

  void load_character(const FT_Face& face, FT_Long c)
  {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER))
      std::cout << "faield to load char " << c << std::endl;

    FT_Bitmap& bitmap = face->glyph->bitmap;

    Glyph glyph;
    glyph.bearingX = face->glyph->bitmap_left;
    glyph.bearingY = face->glyph->bitmap_top;
    glyph.width = bitmap.width;
    glyph.height = bitmap.rows;  // bitmap.rows
    glyph.x_min = glyph.bearingX;
    glyph.y_min = glyph.bearingY - glyph.height;
    glyph.x_max = glyph.x_min + glyph.width;
    glyph.y_max = glyph.bearingY;
    // glyph.origin = face->glyph->origin;
    glyph.advance = face->glyph->advance.x;
    glyph.advance >>= 6;

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, glyph.width, glyph.height, 0,
      GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);

    glyph.texture = tex;
    glyphDict.insert(std::make_pair(c, glyph));

  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    prg.init();
    FT_Library ft;
    GLint error;

    error = FT_Init_FreeType(&ft);
    if (error)
      std::cout << "ERROR::FREETYPE: could not init FreeType library"
                << std::endl;

    FT_Face face;
    std::string font = stream_util::get_resource("font/SourceHanSerifSC-Regular.otf");
    error = FT_New_Face(ft, font.c_str(), 0, &face);
    if (error == FT_Err_Unknown_File_Format)
      std::cout << "ERROR:FREETYEP: unsupported face";
    else if (error == FT_Err_Cannot_Open_Resource)
      std::cout << "can not open resource " << font << std::endl;
    else if (error)
      std::cout << "ERROR::FREETYPE: failed to load font" << std::endl;

    error = FT_Set_Pixel_Sizes(face, /* handle to face object */
      0,                             /* pixel_width           */
      15);                           /* pixel_height          */
    if (error) std::cout << "faield to set pixel size" << std::endl;

    linespace = face->size->metrics.height;
    linespace = linespace >> 6;
    max_advance = face->size->metrics.max_advance;
    max_advance = max_advance >> 6;

    // create 8 bit gray scale bitmap image

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::stringstream ss;
    for (int i = 32; i < 127; ++i) {
      ss << i;
    }
    ss << u8"冰与火之歌";

    // load 128 ascii characters
    for (GLubyte c = 0; c < 128; c++) {
      // load glyph
      load_character(face, c);
    }
    std::string s = ss.str();
    auto beg = make_utf8_beg(s.begin(), s.end());
    auto end = make_utf8_end(s.begin(), s.end());
    while(beg != end)
    {
      load_character(face, *beg++);
    }

    // clear up
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // sub data will be changed for every character
    glBufferData(GL_ARRAY_BUFFER, 64, 0, GL_DYNAMIC_DRAW);
  }

  void reshape(int w, int h) { app::reshape(w, h); }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {}
  }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
