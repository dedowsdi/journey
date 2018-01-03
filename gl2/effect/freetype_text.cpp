#include "glad/glad.h"
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include "common.h"
#include "program.h"
#include "glm.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>

using namespace glm;

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512
GLfloat wndAspect = 1;

vec4 textColor(1.0f);
GLuint vao;
GLuint vbo;
GLuint linespace;
GLuint maxAdvance;

struct Glyph {
  GLfloat xMin;  // the same as bearying X
  GLfloat yMin;  // bearyingY - height
  GLfloat xMax;  // xMin + width
  GLfloat yMax;  // the same as bearying Y
  GLfloat bearingX;
  GLfloat bearingY;
  GLfloat width;   // width of bounding box
  GLfloat height;  // height of bounding box, bitmap.rows
  GLfloat origin;
  GLuint advance;  // 1/64 pixel
  GLuint texture;
};

std::map<GLchar, Glyph> glyphDict;

struct MyProgram : public zxd::Program {
  GLint attrib_vertex;
  GLint loc_textColor;
  GLint loc_fontMap;
  MyProgram() {
    modelViewProjMatrix = glm::ortho(
      0.0f, (GLfloat)WINDOWS_WIDTH, 0.0f, (GLfloat)WINDOWS_HEIGHT, -1.0f, 1.0f);
  }
  virtual void doUpdateFrame() {
    glUniformMatrix4fv(
      loc_modelViewProjMatrix, 1, 0, value_ptr(modelViewProjMatrix));
    glUniform4fv(loc_textColor, 1, value_ptr(textColor));
    glUniform1i(loc_fontMap, 0);
  }
  virtual void doUpdateModel() {}
  virtual void attachShaders() {
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/freetype_text.vs.glsl");
    attachShaderFile(GL_FRAGMENT_SHADER, "data/shader/freetype_text.fs.glsl");
  }
  virtual void bindUniformLocations() {
    setUniformLocation(&loc_modelViewProjMatrix, "modelViewProjMatrix");
    setUniformLocation(&loc_textColor, "textColor");
    setUniformLocation(&loc_fontMap, "fontMap");
  }
  virtual void bindAttribLocations() {
    attrib_vertex = getAttribLocation("vertex");
  }
} myProgram;

void printText2D(const std::string& text, GLuint x, GLuint y, GLfloat scale) {
  GLuint nextY = y;
  GLuint nextX = x;

  for (int i = 0; i < text.size(); ++i) {
    char character = text[i];
    Glyph& glyph = glyphDict[character];

    if (character == '\n') {
      nextX = x;
      nextY -= linespace * scale;
      continue;
    } else if (character == ' ') {
      nextX += glyph.advance * scale;
      continue;
    }

    // clang-format off
    // freetype generate texture from left to right, top to bottom, which means
    // we must flip y
    GLfloat vertices[4][4] = {
      {nextX + glyph.xMin * scale, nextY + glyph.yMin * scale, 0, 1},
      {nextX + glyph.xMax * scale, nextY + glyph.yMin * scale, 1, 1},
      {nextX + glyph.xMax * scale, nextY + glyph.yMax * scale, 1, 0},
      {nextX + glyph.xMin * scale, nextY + glyph.yMax * scale, 0, 0}
    };
    // clang-format on

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glVertexAttribPointer(
      myProgram.attrib_vertex, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(myProgram.attrib_vertex);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, glyph.texture);

    glDrawArrays(GL_QUADS, 0, 4);

    nextX += (glyph.advance >> 6) * scale;
  }
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  myProgram.updateFrame();

  GLchar info[256];
  // clang-format off
  sprintf(info, 
      " q : textColor : %.2f %.2f %.2f %.2f \n"
      " w : \n"
      " w : \n"
      " w : \n"
      " w : \n",
      textColor[0], textColor[1], textColor[2], textColor[3]);
  // clang-format on

  printText2D(info, 20, 300, 1.0);

  glutSwapBuffers();
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  myProgram.init();
  FT_Library ft;
  GLint error;

  error = FT_Init_FreeType(&ft);
  if (error)
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library"
              << std::endl;

  FT_Face face;
  std::string font("data/font/DejaVuSansMono.ttf");
  error = FT_New_Face(ft, font.c_str(), 0, &face);
  if (error == FT_Err_Unknown_File_Format)
    std::cout << "ERROR:FREETYEP: unsupported face";
  else if (error == FT_Err_Cannot_Open_Resource)
    std::cout << "can not open resource " << font << std::endl;
  else if (error)
    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

  error = FT_Set_Pixel_Sizes(face, /* handle to face object */
    0,                             /* pixel_width           */
    15);                           /* pixel_height          */
  if (error) std::cout << "faield to set pixel size" << std::endl;

  linespace = face->size->metrics.height ;
  linespace = linespace >> 6;
  maxAdvance = face->size->metrics.max_advance;
  maxAdvance = maxAdvance >> 6;

  // create 8 bit gray scale bitmap image

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // load 128 ascii characters
  for (GLubyte c = 0; c < 128; c++) {
    // load glyph
    if (FT_Load_Char(face, c, FT_LOAD_RENDER))
      std::cout << "faield to load char " << c << std::endl;

    FT_Bitmap& bitmap = face->glyph->bitmap;

    Glyph glyph;
    glyph.bearingX = face->glyph->bitmap_left;
    glyph.bearingY = face->glyph->bitmap_top;
    glyph.width = bitmap.width;
    glyph.height = bitmap.rows;  // bitmap.rows
    glyph.xMin = glyph.bearingX;
    glyph.yMin = glyph.bearingY - glyph.height;
    glyph.xMax = glyph.xMin + glyph.width;
    glyph.yMax = glyph.bearingY;
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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, glyph.width, glyph.height, 0, GL_RED,
      GL_UNSIGNED_BYTE, bitmap.buffer);

    glyph.texture = tex;
    glyphDict.insert(std::make_pair(c, glyph));
  }

  // clear up
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  //sub data will be changed for every character
  glBufferData(GL_ARRAY_BUFFER, 64, 0, GL_DYNAMIC_DRAW);
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  wndAspect = GLfloat(w) / h;
}

void mouse(int button, int state, int x, int y) {
  switch (button) {
    default:
      break;
  }
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
  }
}
void idle() {}
void passiveMotion(int x, int y) {}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(WINDOWS_WIDTH, WINDOWS_HEIGHT);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  loadGL();
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutPassiveMotionFunc(passiveMotion);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(idle);
  glutMainLoop();

  return 0;
}
