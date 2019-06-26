/*
 * implementing of std::next_permutation and std::pre_permutation
 *
 * permutation in lexicograph order:
 *
 *  the biggest one is in desending order from start to end
 *  the smallest one is in asending order from start to end
 *
 *  let P2 be next permutation of permutaton P1, P2 share the same x prefix as
 *  P2, P2[x+1] must be the smallest number that's greater than p1[x] in the
 *  suffix.  All character in P2 after x+1 must be in asending order.
 *
 *      persude code:
 *        find the largest x that P[x] < P[x+1], P[x] is the fist character that break the desending in suffix
 *        find the largest y that P[x] < P[y], P[y] is the smallest character in the desending suffix that's greater then P[x]
 *        swap P[x] and P[y]
 *        revert P[x+1] until n, this operation will convert desending suffix to asending 
 *
 *  let P0 be pre permutation of permutaton P1, P0 share the same x prefix as
 *  P1, P0[x+1] must be the largest number that's smaller than p1[x] in the
 *  suffix. All character in P2 after x+1 must be in desending order.
 *      persude code:
 *        find the largest x that P[x] > P[x+1], P[x] is the fist character that break the asending in suffix
 *        find the largest y that P[x] > P[y], P[y] is the largest character in the assending suffix that's smaller then P[x]
 *        swap P[x] and P[y]
 *        revert P[x+1] until n, this operation will convert assending suffix to desending 
 *
 */

#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include <iterator>
#include <algorithm>

#define WIDTH 800
#define HEIGHT 200

namespace zxd
{

bool next_order = true;

class app_name : public app
{
protected:
  std::vector<char> m_code;

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "app_name";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);


    m_code.push_back('0');
    m_code.push_back('1');
    m_code.push_back('2');
    m_code.push_back('3');
    m_code.push_back('4');
    m_code.push_back('5');
    m_code.push_back('6');
    m_code.push_back('7');
    m_code.push_back('8');
    m_code.push_back('9');

  }

  void next_permutatoin()
  {
    // next premutation
    // find the largest x that P[x] < P[x+1]
    GLint x = m_code.size() - 1;
    while(--x >= 0)
    {
      if(m_code[x] < m_code[x+1])
          break;
    }

    if(x < 0)
      return;

    // find the largest y that P[x] < P[y]
    GLint y = m_code.size() - 1;
    while(y > x) // we know that y is at least x+1
    {
      if(m_code[y] > m_code[x])
        break;
      --y;
    }

    // swap P[x] and P[y]
    std::swap(m_code[x], m_code[y]);

    // revert P[x+1] until n
    std::reverse(m_code.begin() + x + 1, m_code.end());
  }

  void prev_permutation()
  {
    // pre premutation
    // find the largest x that P[x] > P[x+1]
    GLint x = m_code.size() - 1;
    while(--x >= 0)
    {
      if(m_code[x] > m_code[x+1])
          break;
    }

    if(x < 0)
      return;

    // find the largest y that P[x] > P[y]
    GLint y = m_code.size() - 1;
    while(y > x) // we know that y is at least x+1
    {
      if(m_code[y] < m_code[x])
        break;
      --y;
    }

    // swap P[x] and P[y]
    std::swap(m_code[x], m_code[y]);

    // revert P[x+1] until n
    std::reverse(m_code.begin() + x + 1, m_code.end());
  }

  virtual void update()
  {
    //static GLint count = 0;
    //if(++count % 60 != 0)
      //return;

    if(next_order)
      next_permutatoin();
    else
      prev_permutation();

    
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    std::copy(m_code.begin(), m_code.end(), std::ostream_iterator<char>(ss));
    m_text.print(ss.str(), wnd_width() / 2 - 350, wnd_height()/2 - 50, vec4(0,0,0,1), 8);
    ss.str("");
    ss << "q : " << (next_order ? "next lexicographcal order" : "previous lexicographical order");
    m_text.print(ss.str(), 20, wnd_height()- 20, vec4(0,0,0,1));
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h)
  {
    app::glfw_resize(wnd, w, h);
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods)
  {
    if (action == GLFW_PRESS)
    {
      switch (key)
      {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;
        case GLFW_KEY_Q:
          next_order = !next_order;
          break;
        default:
          break;
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
  }
};
}

int main(int argc, char *argv[])
{
  zxd::app_name app;
  app.run();
}
