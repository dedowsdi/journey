#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common.h"
#include <iomanip>
using namespace glm;

namespace zxd {

class ColorPane {
public:
  ColorPane() : mColor(1, 1, 1){};
  ColorPane(GLfloat r, GLfloat g, GLfloat b) : mColor(r, g, b){};
  ColorPane(vec3 v) : mColor(v){};
  void addRed(GLfloat f) { mColor.r = glm::clamp(mColor.r + f, 0.0f, 1.0f); }
  void addGreen(GLfloat f) { mColor.g = glm::clamp(mColor.g + f, 0.0f, 1.0f); }
  void addBlue(GLfloat f) { mColor.b = glm::clamp(mColor.b + f, 0.0f, 1.0f); }
  GLfloat getRed() { return mColor.r; }
  GLfloat getGreen() { return mColor.g; };
  GLfloat getBlue() { return mColor.b; };

  GLfloat getX() const { return x; }
  void setX(GLfloat v) { x = v; }

  GLfloat getY() const { return y; }
  void setY(GLfloat v) { y = v; }

  const vec3 &getColor() const { return mColor; }
  void setColor(const vec3 &v) { mColor = v; }

protected:
  vec3 mColor;
  GLfloat x;
  GLfloat y;
};

typedef std::vector<ColorPane> ColorPaneVector;

struct RgbProgram : public zxd::Program {
  GLint attrib_vertex;
  GLint attrib_color;
  GLint loc_gamma;

  void updateUniforms(GLboolean gamma = 0) {
    glUniformMatrix4fv(
      loc_modelViewProjMatrix, 1, 0, value_ptr(modelViewProjMatrix));
    glUniform1i(loc_gamma, gamma);
  }

  virtual void attachShaders() {
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/rgb.vs.glsl");
    attachShaderFile(GL_FRAGMENT_SHADER, "data/shader/rgb.fs.glsl");
  }
  virtual void bindUniformLocations() {
    setUniformLocation(&loc_modelViewProjMatrix, "modelViewProjMatrix");
    setUniformLocation(&loc_gamma, "gamma");
  }
  virtual void bindAttribLocations() {
    attrib_vertex = getAttribLocation("vertex");
    attrib_color = getAttribLocation("color");
  };
} rgbProgram;

std::string inputModes[] = {"float", "byte", "normal"};

class RgbApp : public App {
protected:
  BitmapText mBitmapText;
  GLuint mMaxPanePerLine;
  GLuint mCurrentPaneIndex;
  GLfloat mPaneWidth;
  GLfloat mPaneHeight;

  GLboolean mGamma;
  GLboolean mHelp;
  GLuint mPaneVao;
  GLuint mBorderVao;
  GLuint mPaneVertexBuffer;
  GLuint mPaneColorBuffer;
  GLuint mBorderVertexBuffer;
  GLuint mBorderColorBuffer;
  GLfloat mBorderWidth;
  GLuint mInputMode;

  ColorPaneVector mPanes;

  Vec2Vector mPaneVertices;
  Vec3Vector mPaneColors;

  Vec2Vector mBorderVertices;
  Vec3Vector mBorderColors;

public:
  RgbApp() {}

protected:
  virtual void initInfo() {
    App::initInfo();
    mInfo.title = "hello world";
    mInfo.wndWidth = 1024;
    mInfo.wndHeight = 512;
    mInfo.samples = 4;
  }

  GLuint getNumRows() {
    return glm::ceil(mPanes.size() / static_cast<GLfloat>(mMaxPanePerLine));
  }
  GLuint getNumCols() {
    return mPanes.size() >= mMaxPanePerLine ? mMaxPanePerLine : mPanes.size();
  }
  GLuint getCurrentRow() { return mCurrentPaneIndex / mMaxPanePerLine; }
  GLuint getCurrentCol() {
    return mPanes.size() >= mMaxPanePerLine
             ? mCurrentPaneIndex % mMaxPanePerLine
             : mCurrentPaneIndex;
  }

  virtual void createScene() {
    mMaxPanePerLine = 8;
    mBorderWidth = 20.0f;
    mGamma = GL_FALSE;
    mHelp = GL_TRUE;
    mReading = GL_FALSE;
    mInputMode = 0;

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_LINE_SMOOTH);
    mBitmapText.init();
    mBitmapText.reshape(mInfo.wndWidth, mInfo.wndHeight);

    rgbProgram.init();
    rgbProgram.projMatrix = ortho(0.0f, 1.0f, 0.0f, 1.0f);
    glEnableVertexAttribArray(rgbProgram.attrib_vertex);
    glEnableVertexAttribArray(rgbProgram.attrib_color);

    // clang-format off
    Vec3Vector colors = Vec3Vector( {
      {0.0, 0.0, 0.0},
      {1.0, 1.0, 1.0}, 
      {1.0, 0.0, 0.0}, 
      {0.0, 1.0, 0.0},
      {0.0, 0.0, 1.0},
      {1.0, 1.0, 0.0}, 
      {0.0, 1.0, 1.0}, 
      {1.0, 0.0, 1.0},
      {0.753, 0.753, 0.753},
      {0.502, 0.502, 0.502}, 
      {0.502, 0.0, 0.0},
      {0.502, 0.502, 0.0},
      {0.0, 0.502, 0.0}, 
      {0.502, 0.0, 0.502},
      {0.0, 0.502, 0.502},
      {0.0, 0.0, 0.502},
      {1.0, 1.0, 1.0}
    });
    // clang-format on

    glGenVertexArrays(1, &mPaneVao);
    glGenVertexArrays(1, &mBorderVao);

    for (GLuint i = 0; i < colors.size(); ++i) {
      ColorPane pane(colors[i]);
      mPanes.push_back(pane);

      for (int j = 0; j < 6; ++j) {
        mPaneColors.push_back(colors[i]);
      }
    }

    mCurrentPaneIndex = mPanes.size() - 1;
    for (int i = 0; i < 6; ++i) mBorderColors.push_back(vec3(0.0));

    glBindVertexArray(mBorderVao);

    glGenBuffers(1, &mBorderVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mBorderVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vec2), 0, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(
      rgbProgram.attrib_vertex, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(rgbProgram.attrib_vertex);

    glGenBuffers(1, &mBorderColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mBorderColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, mBorderColors.size() * sizeof(vec3),
      value_ptr(mBorderColors.front()), GL_DYNAMIC_DRAW);  // will be updated
    glVertexAttribPointer(
      rgbProgram.attrib_color, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(rgbProgram.attrib_color);

    resizePane();
    resetPaneVertexBuffer();
    resetPaneColorBuffer();
    updateBorderVertexBuffer();
  }

  ColorPane &getCurrentPane() { return mPanes[mCurrentPaneIndex]; }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(rgbProgram);
    glViewport(0, mInfo.wndHeight * 0.1, mInfo.wndWidth, mInfo.wndHeight * 0.9);

    // render pane
    glBindVertexArray(mPaneVao);
    rgbProgram.modelViewProjMatrix = rgbProgram.projMatrix;
    rgbProgram.updateUniforms(mGamma);
    glDrawArrays(GL_TRIANGLES, 0, mPaneVertices.size());

    // render border on current selected color pane
    glLineWidth(mBorderWidth);
    glBindVertexArray(mBorderVao);
    const ColorPane &pane = getCurrentPane();
    rgbProgram.modelViewProjMatrix =
      rgbProgram.projMatrix * translate(vec3(pane.getX(), pane.getY(), 0));
    rgbProgram.updateUniforms(mGamma);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glLineWidth(1);

    glViewport(0, 0, mInfo.wndWidth, mInfo.wndHeight);
    // render text
    // for (int i = 0; i < mPanes.size(); ++i) {
    // ColorPane &pane = mPanes[i];
    // const vec3 &color = pane.getColor();
    // std::stringstream ss;
    // ss.precision(3);
    // ss << color.r << " " << color.g << " " << color.b;
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // mBitmapText.print(ss.str(), pane.getX() * mInfo.wndWidth + 5,
    // pane.getY() * mInfo.wndHeight + 8, vec4(0, 0, 0, 1));
    // glDisable(GL_BLEND);
    //}
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::stringstream ss;
    if (mReading) {
      ss << "input " << inputModes[mInputMode] << " color : " << mInput;
    } else {
      const vec3 &color = pane.getColor();
      ss.precision(3);
      ss.setf(std::ios_base::fixed);
      ss << color.r << " " << color.g << " " << color.b;
      ss << "          ";
      ss << std::setw(3) << static_cast<GLint>(color.r * 255) << " "
         << std::setw(3) << static_cast<GLint>(color.g * 255) << " "
         << std::setw(3) << static_cast<GLint>(color.b * 255) << " ";
      ss << "          ";
      ss << color.r * 2 - 1 << " " << color.g * 2 - 1 << " " << color.b * 2 - 1;
    }
    mBitmapText.print(ss.str(), 5, 10, vec4(1));

    if (mHelp) {
      std::stringstream ss;
      ss << " h : toggle help" << std::endl;
      ss << " esc : exit" << std::endl;
      ss << " a : add new color pane" << std::endl;
      ss << " d : delete color pane" << std::endl;
      ss << " arrow : select color pane" << std::endl;
      ss << " g : gamma correction : " << (mGamma == GL_FALSE ? 0 : 1)
         << std::endl;
      ss << " i : input mode : " << inputModes[mInputMode] << std::endl;
      ss << " enter : start or finish input color" << std::endl;
      ss << " [ctrl|alt]qQ : red" << std::endl;
      ss << " [ctrl|alt]wW : green" << std::endl;
      ss << " [ctrl|alt]eE : blue" << std::endl;
      mBitmapText.print(ss.str(), 1, mInfo.wndHeight - 25, vec4(0, 0, 1, 1));
    }
    glDisable(GL_BLEND);
  }

  virtual void update() {}

  virtual void glfwResize(GLFWwindow *wnd, int w, int h) {
    App::glfwResize(wnd, w, h);
    mBitmapText.reshape(w, h);
  }

  GLfloat getQuantity(int mods) {
    GLfloat quantity = 0.01;
    if (mods & GLFW_MOD_CONTROL) quantity = 0.001;
    //if (mods & GLFW_MOD_ALT) quantity = 0.001;
    if (mods & GLFW_MOD_SHIFT) quantity *= -1;
    return quantity;
  }

  void finisheReading() {
    App::finisheReading();
    std::stringstream ss(mInput);
    vec3 color;
    if (ss >> color[0] && ss >> color[1] && ss >> color[2]) {
      ColorPane &pane = getCurrentPane();
      if (mInputMode == 0) {
      } else if (mInputMode == 1) {
        color /= 255.0f;
      } else if (mInputMode == 2) {
        color = glm::normalize(color);
        color = color * 0.5f + 0.5f;
      }
      pane.setColor(glm::clamp(color, vec3(0), vec3(1)));

      updatePaneColorBuffer();
    }
  }

  virtual void glfwKey(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ENTER:
        case GLFW_KEY_KP_ENTER:
          if (mReading) {
            finisheReading();
          } else {
            startReading();
          }
          break;
        default:
          break;
      }
    }

    if (!mReading) {
      if (action == GLFW_PRESS) {
        switch (key) {
          case GLFW_KEY_UP:
            up();
            break;
          case GLFW_KEY_DOWN:
            down();
            break;
          case GLFW_KEY_LEFT:
            left();
            break;
          case GLFW_KEY_RIGHT:
            right();
            break;
          case GLFW_KEY_A:
            addPane();
            break;
          case GLFW_KEY_D:
            deleteCurrentPane();
            break;
          case GLFW_KEY_G:
            mGamma = !mGamma;
            break;
          case GLFW_KEY_H:
            mHelp = !mHelp;
            break;
          case GLFW_KEY_I:
            mInputMode =
              (mInputMode + 1) % (sizeof(inputModes) / sizeof(inputModes[0]));
            break;

          default:
            break;
        }
      }

      if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
          case GLFW_KEY_Q:
            getCurrentPane().addRed(getQuantity(mods));
            updatePaneColorBuffer();
            break;
          case GLFW_KEY_W:
            getCurrentPane().addGreen(getQuantity(mods));
            updatePaneColorBuffer();
            break;
          case GLFW_KEY_E:
            getCurrentPane().addBlue(getQuantity(mods));
            updatePaneColorBuffer();
            break;
          default:
            break;
        }
      }
    }
    App::glfwKey(wnd, key, scancode, action, mods);
  }

  virtual void glfwMouseButton(
    GLFWwindow *wnd, int button, int action, int mods) {
    App::glfwMouseButton(wnd, button, action, mods);
  }

  virtual void glfwMouseMove(GLFWwindow *wnd, double x, double y) {
    App::glfwMouseMove(wnd, x, y);
  }

  virtual void glfwMouseWheel(GLFWwindow *wnd, double xoffset, double yoffset) {
    App::glfwMouseWheel(wnd, xoffset, yoffset);
  }

  // select panes
  void left() {
    if (mCurrentPaneIndex > 0) --mCurrentPaneIndex;
  }
  void right() {
    if (mCurrentPaneIndex + 1 < mPanes.size()) ++mCurrentPaneIndex;
  }
  void down() {
    if (mCurrentPaneIndex >= getNumCols()) mCurrentPaneIndex -= getNumCols();
  }
  void up() {
    if (mCurrentPaneIndex + getNumCols() < mPanes.size())
      mCurrentPaneIndex += getNumCols();
  }
  // void selectPane(GLuint index);
  void addPane() {
    GLuint numRows = getNumRows();
    GLuint numCols = getNumCols();
    GLuint row = numRows - 1;
    GLuint col = mPanes.size() % numCols;
    GLboolean needResizePane = 0;

    if (mMaxPanePerLine > numCols) {
      needResizePane = 1;
    } else if (col == 0) {
      ++row;
      needResizePane = 1;
    }

    vec3 color(glm::linearRand(vec3(0.0), vec3(1.0)));
    for (int i = 0; i < 6; ++i) mPaneColors.push_back(color);

    // add new color pane
    ColorPane pane;
    GLfloat x = col * mPaneWidth;
    GLfloat y = row * mPaneHeight;
    pane.setColor(color);
    pane.setX(x);
    pane.setY(y);
    mPanes.push_back(pane);

    if (needResizePane) {
      resizePane();
      updateBorderVertexBuffer();
    } else {
      mPaneVertices.push_back(vec2(x, y));
      mPaneVertices.push_back(vec2(x + mPaneWidth, y));
      mPaneVertices.push_back(vec2(x + mPaneWidth, y + mPaneHeight));
      mPaneVertices.push_back(vec2(x, y));
      mPaneVertices.push_back(vec2(x + mPaneWidth, y + mPaneHeight));
      mPaneVertices.push_back(vec2(x, y + mPaneHeight));
    }
    resetPaneVertexBuffer();
    resetPaneColorBuffer();

    mCurrentPaneIndex = mPanes.size() - 1;
  }
  void deleteCurrentPane() {
    if (mPanes.size() == 1) return;

    mPanes.erase(mPanes.begin() + mCurrentPaneIndex);
    auto iter0 = mPaneVertices.begin() + mCurrentPaneIndex * 6;
    mPaneVertices.erase(iter0, iter0 + 6);

    auto iter1 = mPaneColors.begin() + mCurrentPaneIndex * 6;
    mPaneColors.erase(iter1, iter1 + 6);

    resizePane();
    updateBorderVertexBuffer();
    resetPaneVertexBuffer();
    resetPaneColorBuffer();
    if (mCurrentPaneIndex == mPanes.size()) {
      mCurrentPaneIndex = mPanes.size() - 1;
    }
  }

  void resizePane() {
    GLuint col = 0;
    GLuint row = 0;

    GLuint numRows = getNumRows();
    GLuint numCols = getNumCols();

    mPaneWidth = 1.0f / numCols;
    mPaneHeight = 1.0f / numRows;

    for (GLuint i = 0; i < mPanes.size(); ++i) {
      if (col == mMaxPanePerLine) {
        col = 0;
        ++row;
      }

      ColorPane &pane = mPanes[i];
      pane.setX(mPaneWidth * col);
      pane.setY(mPaneHeight * row);
      ++col;
    }

    mPaneVertices.clear();
    // build pane mesh
    for (int i = 0; i < mPanes.size(); ++i) {
      ColorPane &pane = mPanes[i];
      GLfloat x = pane.getX();
      GLfloat y = pane.getY();
      // std::cout << x << " : " << y << std::endl;
      mPaneVertices.push_back(vec2(x, y));
      mPaneVertices.push_back(vec2(x + mPaneWidth, y));
      mPaneVertices.push_back(vec2(x + mPaneWidth, y + mPaneHeight));
      mPaneVertices.push_back(vec2(x, y));
      mPaneVertices.push_back(vec2(x + mPaneWidth, y + mPaneHeight));
      mPaneVertices.push_back(vec2(x, y + mPaneHeight));
    }

    // build border mesh
    mBorderVertices.clear();
    mBorderVertices.push_back(vec2(0, 0));
    mBorderVertices.push_back(vec2(0 + mPaneWidth, 0));
    mBorderVertices.push_back(vec2(0 + mPaneWidth, 0 + mPaneHeight));
    mBorderVertices.push_back(vec2(0, 0 + mPaneHeight));
  }

  void resetPaneVertexBuffer() {
    glBindVertexArray(mPaneVao);

    if (glIsBuffer(mPaneVertexBuffer)) glDeleteBuffers(1, &mPaneVertexBuffer);

    glGenBuffers(1, &mPaneVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mPaneVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mPaneVertices.size() * sizeof(vec2),
      value_ptr(mPaneVertices.front()), GL_STATIC_DRAW);
    glVertexAttribPointer(
      rgbProgram.attrib_vertex, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(rgbProgram.attrib_vertex);
  }

  void resetPaneColorBuffer() {
    glBindVertexArray(mPaneVao);

    if (glIsBuffer(mPaneColorBuffer)) glDeleteBuffers(1, &mPaneColorBuffer);
    glGenBuffers(1, &mPaneColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mPaneColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, mPaneColors.size() * sizeof(vec3),
      value_ptr(mPaneColors.front()), GL_DYNAMIC_DRAW);  // will be updated
    glVertexAttribPointer(
      rgbProgram.attrib_color, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(rgbProgram.attrib_color);
  }

  void updateBorderVertexBuffer() {
    glBindBuffer(GL_ARRAY_BUFFER, mBorderVertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * mBorderVertices.size(),
      value_ptr(mBorderVertices.front()));
  }

  void updatePaneColorBuffer() {
    ColorPane &pane = getCurrentPane();
    for (int i = 0; i < 6; ++i) {
      mPaneColors[mCurrentPaneIndex * 6 + i] = pane.getColor();
    }
    glBindBuffer(GL_ARRAY_BUFFER, mPaneColorBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec3) * 6 * mCurrentPaneIndex,
      sizeof(vec3) * 6, value_ptr(mPaneColors[mCurrentPaneIndex * 6]));
  }
};
}

int main(int argc, char *argv[]) {
  zxd::RgbApp app;
  app.run();
}
