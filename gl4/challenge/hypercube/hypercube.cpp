/*
 * 0 dimention : point
 * 1 dimention : line, defined by 2 points
 * 2 dimention : plane, defined by 2 lines, rotate along point
 * 3 dimention : cube, defined by 2 planes, rotate along line
 * 4 dimention : hypercube, defined by 2 cubes, rotate alone plane
 * ...
 * nuts
 */
#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common_program.h"

#define WIDTH 800
#define HEIGHT 800

namespace zxd
{

lightless_program prg;

glm::mat4 v_mat;
glm::mat4 p_mat;

struct hypercube
{
  GLuint vao;
  GLuint vbo;
  vec4_vector hvertices;
  vec3_vector vertices;
  vec3_vector lines;

  void init()
  {
    hvertices.reserve(16);
    vertices.reserve(16 + 12*2*2 + 4*2);

    hvertices.push_back(vec4(-1, -1, -1, -1));
    hvertices.push_back(vec4(1 , -1, -1, -1));
    hvertices.push_back(vec4(1 , -1, 1 , -1));
    hvertices.push_back(vec4(-1, -1, 1 , -1));
    hvertices.push_back(vec4(-1,  1, -1, -1));
    hvertices.push_back(vec4(1 ,  1, -1, -1));
    hvertices.push_back(vec4(1 ,  1, 1 , -1));
    hvertices.push_back(vec4(-1,  1, 1 , -1));

    hvertices.push_back(vec4(-1, -1, -1,  1));
    hvertices.push_back(vec4(1 , -1, -1,  1));
    hvertices.push_back(vec4(1 , -1, 1 ,  1));
    hvertices.push_back(vec4(-1, -1, 1 ,  1));
    hvertices.push_back(vec4(-1,  1, -1,  1));
    hvertices.push_back(vec4(1 ,  1, -1,  1));
    hvertices.push_back(vec4(1 ,  1, 1 ,  1));
    hvertices.push_back(vec4(-1,  1, 1 ,  1));

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
  }

  void project_to_3d()
  {
    vertices.clear();

    // https://www.youtube.com/watch?v=iGO12Z5Lw8s
    // project along W to 3d (xyz), scale along W
    GLfloat lw = 2; // light or camera w position
    for (int i = 0; i < 16; ++i)
    {
      const vec4& hv = hvertices[i];
      glm::mat4x3 m(mat3(1/(lw - hv.w)));
      vec3 v;
      v.x = glm::dot(glm::row(m, 0), hv);
      v.y = glm::dot(glm::row(m, 1), hv);
      v.z = glm::dot(glm::row(m, 2), hv);
      vertices.push_back(v);
    }
  }

  void update()
  {
    rotate();
    project_to_3d();
    build_lines();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3),
        glm::value_ptr(vertices.front()), GL_DYNAMIC_DRAW);
  }

  void rotate()
  {
    GLfloat xy = 0.01; // rotate along xy plane
    GLfloat zw = 0.01; // rotate along zw plane

    mat4 rotate_zw = glm::rotate(zw, pza);
    GLfloat c = glm::cos(xy);
    GLfloat s = glm::sin(xy);
    mat4 rotate_xy = mat4(1.0f, 0.0f, 0.0f, 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f,
                          0.0f, 0.0f, c   , s   ,
                          0.0f, 0.0f, -s  , c    );
    mat4 m = rotate_xy * rotate_zw;
    for(auto& vertex : hvertices)
    {
      vertex = m*vertex;
    }

    
  }
  
  void build_lines()
  {
    for (int i = 0; i < 4; ++i) 
    {
      const vec3& fv0 = vertices[i];
      const vec3& fv1 = vertices[(i + 1) % 4];
      const vec3& bv0 = vertices[4 + i];
      const vec3& bv1 = vertices[4 + (i + 1) % 4];

      // front face lines
      vertices.push_back(fv0);
      vertices.push_back(fv1);
      // back face lines
      vertices.push_back(bv0);
      vertices.push_back(bv1);
      // lines between front and back
      vertices.push_back(fv0);
      vertices.push_back(bv0);

      const vec3& hfv0 = vertices[8 + i];
      const vec3& hfv1 = vertices[8 + (i + 1) % 4];
      const vec3& hbv0 = vertices[8 + 4 + i];
      const vec3& hbv1 = vertices[8 + 4 + (i + 1) % 4];

      vertices.push_back(hfv0);
      vertices.push_back(hfv1);
      vertices.push_back(hbv0);
      vertices.push_back(hbv1);
      vertices.push_back(hfv0);
      vertices.push_back(hbv0);

      // connect 2 cubes;
      vertices.push_back(fv0);
      vertices.push_back(hfv0);
      vertices.push_back(bv0);
      vertices.push_back(hbv0);
    }
  }

  void draw()
  {
    glBindVertexArray(vao);
    prg.use();
    mat4 mvp_mat = p_mat * v_mat;
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
    glUniform4f(prg.ul_color, 1, 1, 1, 1);

    glPointSize(10);
    glDrawArrays(GL_POINTS, 0, 16);
    glDrawArrays(GL_LINES, 16, vertices.size() - 16);
  }
};

class hypercube_app : public app
{
protected:
  hypercube m_hcube;

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "hypercube_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);


    prg.init();
    p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 100.0f);
    v_mat = zxd::isometric_projection(5);
    set_v_mat(&v_mat);

    m_hcube.init();
  }

  virtual void update()
  {
    m_hcube.update();
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    m_hcube.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "";
    m_text.print(ss.str(), 10, wnd_height()- 20);
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
  zxd::hypercube_app app;
  app.run();
}
