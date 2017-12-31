#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include "common.h"
#include "program.h"
#include "glm.h"
#include "freetype_text.h"
#include <sstream>
#include "quad.h"
#include "texutil.h"
#include "bitmap_text.h"
#include <fstream>
#include <iomanip>
#include "stringutil.h"

using namespace glm;

bool isExportBitmap = false;
GLuint fbo;
GLuint fontTex;

GLfloat wndAspect = 1;
GLuint wndWidth = 512;
GLuint wndHeight = 512 + 128;
GLuint texSize = 256;
GLuint fontSize = 15;  // height
GLuint numCharacters = 256;

std::string text;
std::string font;
std::string outputName;

std::unique_ptr<zxd::FreetypeText> ft0;
std::unique_ptr<zxd::FreetypeText> ft1;
std::unique_ptr<zxd::BitmapText> bt;

std::vector<zxd::BitmapText::Glyph> glyphs;

zxd::QuadProgram quadProgram;

void resizeTexture();
void exportBitmap();

void display(void) {
  // render texture
  glViewport(0, 0, texSize, texSize);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glyphs.clear();

  GLuint x = 0;
  GLuint y = texSize - ft0->getLinespace();

  for (int i = 0; i < numCharacters; ++i) {
    const zxd::FreetypeText::Glyph& glyph = ft0->getGlyph(i);

    if ((x + glyph.advance) > texSize) {
      x = 0;
      y -= ft0->getLinespace();
    }

    ft0->print(glyph, x, y);

    zxd::BitmapText::Glyph bmGlyph;
    bmGlyph.xMin = glyph.xMin;
    bmGlyph.xMax = glyph.xMax;
    bmGlyph.yMin = glyph.yMin;
    bmGlyph.yMax = glyph.yMax;
    bmGlyph.sMin = x + glyph.xMin;
    bmGlyph.sMax = x + glyph.xMax;
    bmGlyph.tMin = y + glyph.yMin;
    bmGlyph.tMax = y + glyph.yMax;
    bmGlyph.advance = glyph.advance;
    glyphs.push_back(bmGlyph);

    x += glyph.advance;
  }

  // display texture
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_BLEND);

  if (isExportBitmap) {
    exportBitmap();
    isExportBitmap = false;
  }

  if (texSize < wndWidth)
    glViewport(0, wndHeight - texSize, texSize, texSize);
  else
    glViewport(0, wndHeight - wndWidth, wndWidth, wndWidth);

  zxd::drawQuad(fontTex);

  glViewport(0, 0, wndWidth, wndHeight);

  char info[512];
  sprintf(info,
    "qQ : texture size : %d \n"
    "wW : font size : %d \n"
    "e : export to bitmap_font.png\n",
    texSize, fontSize);

  glEnable(GL_BLEND);
  ft1->print(info, 10, wndHeight - wndWidth - 10);

  bt->print("bitmap text : When will TWOW be released", 10, 50);

  glutSwapBuffers();
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);

  bt = std::unique_ptr<zxd::BitmapText>(new zxd::BitmapText());
  bt->init();

  // free type  printer
  ft0 = std::unique_ptr<zxd::FreetypeText>(new zxd::FreetypeText(font));
  ft0->setNumCharacters(numCharacters);
  ft0->setHeight(fontSize);
  ft0->init();

  if (outputName.empty()) {
    // use default name, it's face_maxAdvance_height
    std::stringstream ss;
    ss << zxd::StringUtil::basename(zxd::StringUtil::tail(font)) << "_"
       << ft0->getHeight() << "_" << ft0->getMaxAdvance();
    outputName = ss.str();
  }

  ft1 = std::unique_ptr<zxd::FreetypeText>(
    new zxd::FreetypeText("data/font/DejaVuSansMono.ttf"));
  ft1->init();

  // characters
  text.reserve(numCharacters);
  for (int i = 0; i < numCharacters; ++i) {
    text.push_back(i);
  }

  // rtt
  glGenTextures(1, &fontTex);
  glBindTexture(GL_TEXTURE_2D, fontTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  resizeTexture();

  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fontTex, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("incomplete frame buffer");

  quadProgram.init();
}

void reshape(int w, int h) {
  wndWidth = w;
  wndHeight = h;
  wndAspect = GLfloat(wndWidth) / wndHeight;

  ft1->reshape(wndWidth, wndHeight);
  bt->reshape(wndWidth, wndHeight);
}

void mouse(int button, int state, int x, int y) {
  switch (button) {
    default:
      break;
  }
}

void resizeTexture() {
  glBindTexture(GL_TEXTURE_2D, fontTex);
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RED, texSize, texSize, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
  ft0->reshape(texSize, texSize);
}

void resizeFont() {
  ft0->setHeight(fontSize);
  ft0->updateGlyphDict();
}

void exportBitmap() {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, fontTex);

  std::ofstream ofs(outputName + ".fmt");
  std::string textureName = outputName + ".png";

  ofs << std::left;
  ofs << std::setw(50) << "#texture";
  ofs << std::setw(50) << "#face";
  ofs << std::endl;

  ofs << std::setw(50) << textureName;
  ofs << std::setw(50) << ft0->getFace();
  ofs << std::endl << std::endl;

  ofs << std::setw(10) << "#count";
  ofs << std::setw(10) << "size";
  ofs << std::setw(10) << "height";
  ofs << std::setw(12) << "linespace";
  ofs << std::setw(15) << "max_advance";
  ofs << std::endl;

  ofs << std::setw(10) << numCharacters;
  ofs << std::setw(10) << texSize;
  ofs << std::setw(10) << fontSize;
  ofs << std::setw(12) << ft0->getLinespace();
  ofs << std::setw(15) << ft0->getMaxAdvance();
  ofs << std::endl << std::endl;

  ofs << std::setw(10) << "#char";
  ofs << std::setw(10) << "xMin";
  ofs << std::setw(10) << "xMax";
  ofs << std::setw(10) << "yMin";
  ofs << std::setw(10) << "yMax";
  ofs << std::setw(10) << "sMin";
  ofs << std::setw(10) << "sMax";
  ofs << std::setw(10) << "tMin";
  ofs << std::setw(10) << "tMax";
  ofs << std::setw(10) << "advance";
  ofs << std::endl;

  ofs << "---------------------------------------------------------------------"
         "-------------------------------"
      << std::endl;

  for (int i = 0; i < glyphs.size(); ++i) {
    const zxd::BitmapText::Glyph& glyph = glyphs[i];

    ofs << std::setw(10) << i;
    ofs << std::setw(10) << glyph.xMin;
    ofs << std::setw(10) << glyph.xMax;
    ofs << std::setw(10) << glyph.yMin;
    ofs << std::setw(10) << glyph.yMax;
    ofs << std::setw(10) << glyph.sMin;
    ofs << std::setw(10) << glyph.sMax;
    ofs << std::setw(10) << glyph.tMin;
    ofs << std::setw(10) << glyph.tMax;
    ofs << std::setw(10) << glyph.advance;
    ofs << std::endl;
  }

  ofs.close();

  zxd::saveTexture(textureName, GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, 8);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    case 'q':
      texSize *= 2;
      resizeTexture();
      glutPostRedisplay();
      break;

    case 'Q':
      if (texSize == 1) {
        return;
      }
      texSize /= 2;
      resizeTexture();
      glutPostRedisplay();
      break;

    case 'w':
      fontSize += 1;
      resizeFont();
      glutPostRedisplay();
      break;

    case 'W':
      if (fontSize == 1) {
        return;
      }
      fontSize -= 1;
      resizeFont();
      glutPostRedisplay();
      break;

    case 'e':
    case 'E':
      isExportBitmap = true;
      glutPostRedisplay();
      break;
  }
}
void idle() {}
void passiveMotion(int x, int y) {}

int main(int argc, char** argv) {
  if (argc <= 3) {
    std::cout << "illegal command, it should be " << argv[0]
              << " font size [numCharacters] [output name] " << std::endl;
    return 0;
  }

  font = argv[1];
  fontSize = std::stoi(argv[2]);

  if (argc >= 4) {
    numCharacters = std::stoi(argv[3]);
  }
  if (argc >= 5) {
    outputName = std::stoi(argv[4]);
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(wndWidth, wndHeight);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  initExtension();
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
