#include "app.h"
#include "bitmap_text.h"
#include "quad.h"
#include "string_util.h"
#include "timer.h"
#include "pingpong.h"

#include <sstream>
#include <algorithm>
#include <iomanip>

namespace zxd
{

bool dirty = true;
GLuint mtex;
GLuint tex;
GLuint btex;
GLuint tbo;
GLuint mfbo;
GLuint fbo;
GLuint iterations = 500;
GLuint num_colors = 512;
// not so useful, it doesn't matter too much if an implicit synchronization is
// forced, the scene is too simple.
GLuint num_pbo = 5;
npingpong pbo;
vec4_vector colors;
quad q;
timer t;

std::vector<GLuint> histogram;
std::vector<GLfloat> pixels;
std::vector<GLfloat> hues;
dvec2 center = dvec2(-0.714483886562187, 0.350309627591928);
dvec2 radius = dvec2(1.3);
dvec4 rect;
bool verbose = true;
bool direct_draw = false;
bool use_cpu_shader = false;
bool use_pbo = false;
bool zoom_in = false;
bool zoom_out = false;
double zoom_speed = 0.93;

GLuint num_ctrl_colors = 6;
vec3_vector ctrl_colors =
{
  vec3(0  , 7  , 100) / 255.0f,
  vec3(32 , 107, 203) / 255.0f,
  vec3(237, 255, 255) / 255.0f,
  vec3(255, 170, 0)   / 255.0f,
  vec3(0  , 2  , 0)   / 255.0f,
  vec3(0  , 7  , 100) / 255.0f
};

kcip kci_iterations;

class mandelbrot_compute_program : public program
{
protected:

  void attach_shaders()
  {
    string_vector sv;
    sv.push_back("#version 430 core\n");
    sv.push_back("#define MAX_ITERATIONS " + string_util::to(iterations) + "\n");
    attach(GL_COMPUTE_SHADER, sv, "shader4/mandelbrot.cs.glsl");
  }

} cpu_prg;

class mandelbrot_iteration_program : public program
{
public:
  GLint ul_rect;
  GLint ul_colors;
  GLint ul_resolution;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/quad.vs.glsl");
    string_vector sv;
    sv.push_back("#version 430 core\n");
    sv.push_back("#define MAX_ITERATIONS " + string_util::to(iterations) + "\n");
    if(direct_draw)
    {
      sv.push_back("#define DIRECT_DRAW\n");
      sv.push_back("#define NUM_COLORS " + string_util::to(num_colors) + "\n");
    }
    attach(GL_FRAGMENT_SHADER, sv, "shader4/mandelbrot_iteration.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_rect, "rect");
    uniform_location(&ul_resolution, "resolution");
    if(direct_draw)
      uniform_location(&ul_colors, "colors");
  }

  void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "color");
  }

} iter_prg;

class mandelbrot_color_program : public program
{
public:
  GLint ul_iteration_map;
  GLint ul_hue_buffer;
  GLint ul_colors;
  GLint ul_itotal;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/quad.vs.glsl");
    string_vector sv;
    sv.push_back("#version 430 core\n");
    sv.push_back("#define MAX_ITERATIONS " + string_util::to(iterations) + "\n");
    sv.push_back("#define NUM_COLORS " + string_util::to(num_colors) + "\n");
    attach(GL_FRAGMENT_SHADER, sv, "shader4/mandelbrot_color.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_iteration_map, "iteration_map");
    uniform_location(&ul_hue_buffer, "hue_buffer");
    uniform_location(&ul_colors, "colors");
    uniform_location(&ul_itotal, "itotal");
  }

  void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "color");
  }

} color_prg;

class mandelbrot_set_app : public app
{
protected:

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "mandelbrot_set_app";
    m_info.wnd_width = 512;
    m_info.wnd_height = 512;
    m_info.samples = 16;
    m_info.decorated = false;
    m_info.fullscreen = false;
    m_info.double_buffer = false;
  }

  void resize_textrure()
  {
    if(!glIsTexture(tex))
      glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, wnd_width(), wnd_height(),
        0, GL_RED, GL_UNSIGNED_BYTE, 0);

    if(!glIsFramebuffer(fbo))
      glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      printf("incomplete frame buffer\n");

    if(!glIsTexture(mtex))
      glGenTextures(1, &mtex);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mtex);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_info.samples, GL_R32F,
        wnd_width(), wnd_height(), false);

    //glDeleteFramebuffers(1, &mfbo);
    if(!glIsFramebuffer(mfbo))
      glGenFramebuffers(1, &mfbo);
    glBindFramebuffer(GL_FRAMEBUFFER, mfbo);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mtex, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      printf("incomplete frame buffer\n");

    pixels.resize(wnd_width()*wnd_height());

    pbo.clear();
    GLuint* buffers = new GLuint[num_pbo];
    glGenBuffers(num_pbo, buffers);
    for (int i = 0; i < num_pbo; ++i)
    {
      glBindBuffer(GL_PIXEL_PACK_BUFFER, buffers[i]);
      glBufferData(GL_PIXEL_PACK_BUFFER, wnd_width()*wnd_height()*sizeof(GLuint), 0, GL_DYNAMIC_DRAW);
      pbo.add_resource(buffers[i]);
    }
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    delete[] buffers;
  }

  virtual void create_scene()
  {
    //m_pause = true;
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);


    q.include_texcoord(true);
    q.build_mesh();

    resize_textrure();

    glGenBuffers(1, &tbo);
    glBindBuffer(GL_TEXTURE_BUFFER, tbo);
    glBufferData(GL_TEXTURE_BUFFER, (iterations + 1) * sizeof(GLfloat), 0, GL_STREAM_DRAW);
    
    glGenTextures(1, &btex);
    glBindTexture(GL_TEXTURE_BUFFER, btex);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tbo);

    auto callback = std::bind(std::mem_fn(&mandelbrot_set_app::reset_iteration),
        this, std::placeholders::_1);
    kci_iterations = m_control.add_control<GLint>(GLFW_KEY_Q, iterations, 10, 100000000, 100, callback);
    reset_iteration(0);

    reset_colors();

    GLdouble aspect = radius.x / radius.y;
    radius.x *= wnd_aspect() / aspect;
  }

  void reset_iteration(const kci* kci = 0)
  {
    dirty = true;
    iterations = kci_iterations->get_int();
    histogram.resize(iterations+1);
    hues.resize(histogram.size());

    if(direct_draw)
    {
      iter_prg.reload();
      iter_prg.use();
      glUniform4fv(iter_prg.ul_colors, colors.size(), glm::value_ptr(colors.front()));
    }
    else
    {
      if(use_cpu_shader)
        cpu_prg.reload();
      else
        iter_prg.reload();
      color_prg.reload();
      color_prg.use();
      glUniform4fv(color_prg.ul_colors, colors.size(), glm::value_ptr(colors.front()));
    }

    glBindBuffer(GL_TEXTURE_BUFFER, tbo);
    glBufferData(GL_TEXTURE_BUFFER, (iterations + 1) * sizeof(GLfloat), 0, GL_STREAM_DRAW);
  }

  void reset_ctrl_colors()
  {
    dirty = true;
    ctrl_colors.clear();
    ctrl_colors.push_back(glm::linearRand(vec3(0), vec3(1)));
    for (int i = 0; i < num_ctrl_colors; ++i)
    {
      //vec3 color = ctrl_colors.back() + glm::linearRand(vec3(0.05), vec3(0.15));
      //color -= glm::step(vec3(1), color);
      //ctrl_colors.push_back(color);
      ctrl_colors.push_back(glm::linearRand(vec3(0), vec3(1)));
    }
  }

  void reset_colors()
  {
    colors.clear();
    colors.reserve(num_colors);

    // hues are not evenly distributed, the 1st 4 hues might occupies 80% , the
    // last one might occupies 10%
    //float_vector ctrl_positions = {0, 0.16, 0.42, 0.6425, 0.8575, 1};
    float_vector ctrl_positions = {0, 0.16, 0.42, 0.6425, 0.8575};
    GLfloat weight_step = (1 - ctrl_positions[4]) / (num_ctrl_colors - 5);
    for (int i = 5; i < num_ctrl_colors; ++i)
    {
      ctrl_positions.push_back(ctrl_positions.back() + weight_step );
    }
    //float_vector ctrl_positions = {0, 0.16, 0.42, 0.83, 0.95, 1};
    //float_vector ctrl_positions;
    //ctrl_positions.push_back(0);
    //ctrl_positions.push_back(0.4);
    //ctrl_positions.push_back(0.8);
    //for (int i = 0; i < num_colors; ++i)
    //{
      //ctrl_positions.push_back( static_cast<GLfloat>(i) / num_ctrl_colors );
    //}
    //ctrl_positions.push_back(1.0);

    colors.push_back(vec4(ctrl_colors.front(), 1));
    for (int i = 1; i < num_colors; ++i)
    {
      GLfloat idx = static_cast<GLfloat>(i) / (num_colors - 1);
      auto iter = std::lower_bound(ctrl_positions.begin(), ctrl_positions.end(), idx);
      GLint ci1 = std::distance(ctrl_positions.begin(), iter);
      GLint ci0 = (ci1 - 1);
      GLfloat p1 = ctrl_positions[ci1];
      GLfloat p0 = ctrl_positions[ci0];
      vec3 color = glm::mix(ctrl_colors[ci0], ctrl_colors[ci1], 
          //glm::smoothstep(0.0f, 1.0f, (idx - p0)/(p1 - p0)));
          zxd::cubic_in_out((idx - p0)/(p1 - p0)));
      colors.push_back(vec4(color, 1));
    }

    update_colors();
  }

  void update_colors()
  {
    if(direct_draw)
    {
      iter_prg.use();
      glUniform4fv(iter_prg.ul_colors, colors.size(), glm::value_ptr(colors.front()));
    }
    else
    {
      color_prg.use();
      glUniform4fv(color_prg.ul_colors, colors.size(), glm::value_ptr(colors.front()));
    }
  }
  
  void print_colors()
  {
    for(auto& item : ctrl_colors)
    {
      std::cout << item << std::endl;
    }
  }

  virtual void update()
  {
    if(use_pbo)
      pbo.shift();
    if(zoom_in)
    {
      if(radius.x <= 0.00000000001 || radius.y <= 0.00000000001)
        return;
      radius *= zoom_speed;
      //glDisable(GL_MULTISAMPLE);
      dirty = true;
    }
    else if(zoom_out)
    {
      radius /= zoom_speed;
      //glDisable(GL_MULTISAMPLE);
      dirty = true;
    }
    //glEnable(GL_MULTISAMPLE);
  }

  void rtt_histogram()
  {
    // render iterations
    glBindFramebuffer(GL_FRAMEBUFFER, mfbo);

    iter_prg.use();
    glUniform2d(iter_prg.ul_resolution, wnd_width(), wnd_height());
    glUniform4dv(iter_prg.ul_rect, 1, glm::value_ptr(rect));
    q.draw();

    // blit back to normal texture
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glBlitFramebuffer(0, 0, wnd_width(), wnd_height(),
        0, 0, wnd_width(), wnd_height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // update historgram
    std::fill(histogram.begin(), histogram.end(), 0);
    if(use_pbo)
    {
      glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo.last_ping());
      GLfloat* data = static_cast<GLfloat*>(glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY));
      //std::cout << "map buffer cost " << it.time_miliseconds() << "ms" << std::endl;
      for (int i = 0; i < pixels.size(); ++i)
      {
        ++histogram[*data++];
      }
      //std::cout << "read buffer cost " << it.time_miliseconds() << "ms" << std::endl;
      glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

      //glReadBuffer(GL_COLOR_ATTACHMENT0);
      glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo.pong());
      glReadPixels(0, 0, wnd_width(), wnd_height(), GL_RED, GL_FLOAT, BUFFER_OFFSET(0));

      glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    }
    else
    {
      glReadBuffer(GL_COLOR_ATTACHMENT0);
      glReadPixels(0, 0, wnd_width(), wnd_height(), GL_RED, GL_FLOAT, &pixels.front());
      t.reset();
      for(auto item : pixels)
      {
        ++histogram[item];
      }
    }
  }

  void cs_histogram()
  {
    cpu_prg.use();
    glBindImageTexture(0, tex, 0, GL_FALSE, 0,  GL_WRITE_ONLY, GL_R32F);
    glUniform4dv(0, 1, glm::value_ptr(rect));
    glDispatchCompute(32, 32, 1);

    glBindTexture(GL_TEXTURE_2D, tex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, &pixels.front());
    for(auto item : pixels)
    {
      ++histogram[item];
    }
  }

  void render_smooth_histogram()
  {
    if(use_cpu_shader)
      cs_histogram();
    else
      rtt_histogram();

    std::partial_sum(histogram.begin(), histogram.end(), hues.begin());
    //std::cout << "update hues cost " << t.time_miliseconds() << "ms" << std::endl;
    GLfloat total_iteration = hues.back();

    glBindBuffer(GL_TEXTURE_BUFFER, tbo);
    glBufferSubData(GL_TEXTURE_BUFFER, 0, hues.size() * sizeof(hues.front()), &hues.front());

    // final render
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    color_prg.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, btex);
    glUniform1i(color_prg.ul_iteration_map, 0);
    glUniform1i(color_prg.ul_hue_buffer, 1);
    glUniform1f(color_prg.ul_itotal, 1.0/total_iteration);
    q.draw();
  }

  void render_smooth()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    iter_prg.use();
    glUniform2d(iter_prg.ul_resolution, wnd_width(), wnd_height());
    glUniform4dv(iter_prg.ul_rect, 1, glm::value_ptr(rect));
    q.draw();
  }

  virtual void display()
  {

    if(!dirty)
      return;

    rect = dvec4(center - radius, center + radius);

    if(direct_draw)
      render_smooth();
    else
      render_smooth_histogram();

    dirty = false;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    dvec2 center = (rect.xy() + rect.zw()) * 0.5;
    dvec2 radius = (rect.zw() - rect.xy()) * 0.5;
    std::stringstream ss;
    ss.precision(15);
    ss.setf(std::ios_base::fixed, std::ios_base::floatfield);
    ss << "qQ : iterations : " << iterations << std::endl;
    ss << "samples : " << m_info.samples << std::endl;
    if(verbose)
    {
      ss << "u : random smooth colors" << std::endl;
      ss << "p : print colors" << std::endl;
      ss << "j : use compute shade : " << use_cpu_shader << std::endl;
      ss << "k : direct draw : " << direct_draw << std::endl;
      ss << "l : pbo : " << use_pbo << std::endl;
    }
    ss << "center : " << center.x << " " << center.y << std::endl;
    ss << "radius : " << radius.x << " " << radius.y << std::endl;
    ss.precision(1);
    //ss << "fps : " << m_fps << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 20, vec4(0,0,1,1));
    glDisable(GL_BLEND);

  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h)
  {
    app::glfw_resize(wnd, w, h);
    resize_textrure();
    GLdouble aspect = radius.x / radius.y;
    radius.x *= wnd_aspect() / aspect;
    dirty = true;
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
        case GLFW_KEY_U:
          reset_ctrl_colors();
          reset_colors();
          break;
        case GLFW_KEY_P:
          print_colors();
          break;
        case GLFW_KEY_UP:
          zoom_in ^= 1;
          break;
        case GLFW_KEY_DOWN:
          zoom_out ^= 1;
          break;
        case GLFW_KEY_L:
          use_pbo ^= 1;
          break;
        case GLFW_KEY_K:
          direct_draw ^= 1;
          reset_iteration();
          update_colors();
          break;
        case GLFW_KEY_J:
          use_cpu_shader ^= 1;
          reset_iteration();
          update_colors();
          break;
        default:
          break;
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
  }

  virtual void glfw_mouse_button(
    GLFWwindow *wnd, int button, int action, int mods)
  {
    if(action == GLFW_PRESS)
    {
      if(GLFW_MOUSE_BUTTON_LEFT == button)
        zoom_in = true;
      else if(GLFW_MOUSE_BUTTON_RIGHT == button)
        zoom_out = true;
    }
    else if(action == GLFW_RELEASE)
    {
      if(GLFW_MOUSE_BUTTON_LEFT == button)
        zoom_in = false;
      else if(GLFW_MOUSE_BUTTON_RIGHT == button)
        zoom_out = false;
    }

    app::glfw_mouse_button(wnd, button, action, mods);
  }

  virtual void glfw_mouse_move(GLFWwindow *wnd, double x, double y)
  {
    if(glfwGetMouseButton(m_wnd, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS ||
        glfwGetMouseButton(m_wnd, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS || 
        glfwGetMouseButton(m_wnd, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
      {
        dvec2 pos;
        glfwGetCursorPos(m_wnd, &pos.x, &pos.y);
        pos = glfw_to_gl(pos);
        dvec2 dt_pos = pos - m_last_cursor_position;
        dvec2 offset = dt_pos / dvec2(wnd_width(), wnd_height()) * (rect.zw() - rect.xy());
        center -= offset;
        dirty = true;
      }
    app::glfw_mouse_move(wnd, x, y);
  }

};
}

int main(int argc, char *argv[])
{
  zxd::mandelbrot_set_app app;
  app.run();
}
