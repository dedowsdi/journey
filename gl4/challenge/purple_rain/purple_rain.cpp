/*
 * each rain_drop drop is a unique geometry.
 * is it a bad way to crate rain_drop with DrawArraysInstance?
 */
#include "app.h"
#include "program.h"
#include "common.h"
#include "bitmaptext.h"
#include <sstream>

#define RAIN_COLOR (vec3(138, 43, 226)/255.0f)
#define BG_COLOR (vec3(230, 230, 250)/255.0f)
#define NUM_DROPS 300000
#define RADIUS 2000.0f

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

namespace zxd
{

mat4_vector mvp_mats; // not used, matrix calculation moved to vertex shader
GLuint mvp_vbo;

vec4_vector posz;
GLuint posz_vbo;

// unit rain_drop drop geometry in x z
struct rain_drop_geometry
{
  GLuint vao;
  GLuint vbo;
  vec3_vector vertices;

  void build_mesh()
  {
    vertices.push_back(vec3(0));
    vertices.push_back(vec3(0, 0, -1));

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(decltype(vertices)::value_type),
      value_ptr(vertices[0]), GL_STATIC_DRAW);
    
    //glVertexAttribPointer(
      //location, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    //glEnableVertexAttribArray(location);
  }
  void bind_attrib_locations(GLint al_vertex)
  {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(
      al_vertex, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(al_vertex);
  }

  void draw(GLuint prim_count)
  {
    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_LINES, 0, 2, prim_count);
  }
};

struct rain_drop
{
  GLfloat zspeed;
  GLfloat accel;
  GLfloat size;
  vec3 pos;
  mat4 m_scale_mat;

  rain_drop()
  {
    reset();
  }

  void update(GLfloat dt)
  {
    pos.z += zspeed * dt;
    zspeed += accel * dt;
    if(pos.z < 0)
      reset();
  }

  void reset()
  {
    zspeed = glm::linearRand(0.25f, 1.0f) * -50;
    accel = glm::linearRand(0.0f, 1.0f) * -200;
    size = glm::linearRand(2.0f, 10.0f);
    pos = glm::linearRand(vec3(-RADIUS, -RADIUS, RADIUS*1.5), vec3(RADIUS, RADIUS, RADIUS*1.8));
    m_scale_mat = glm::scale(vec3(1, 1.0f, size));
  }

  mat4 m_mat()
  {
    //return glm::translate(pos) * m_scale_mat ;
    //return glm::translate(m_scale_mat, pos);
    glm::mat4 m(m_scale_mat);
    m[3] += vec4(pos, 0);
    return m;
  }

  vec4 posz()
  {
    return vec4(pos, size);
  }

};

typedef std::vector<rain_drop> rain_vector;

class purple_rain_program : public program
{

public:
  GLint ul_color;
  GLint al_vertex;
  GLint al_mvp_mats;
  GLint al_posz;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "data/shader/purple_rain.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/purple_rain.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_color, "color");
    uniform_location(&ul_vp_mat, "vp_mat");
  }

  void bind_attrib_locations()
  {
    al_vertex = attrib_location("vertex");
    al_mvp_mats = attrib_location("mvp_mats");
    al_posz = attrib_location("posz");
  }

} prg;


class purple_rain_app : public app
{
protected:

  rain_vector m_rains;
  rain_drop_geometry m_geometry;
  bitmap_text m_text;

  void init_info()
  {
    app::init_info();
    this->m_info.title = "purple rain_drop";
  }

  void create_scene()
  {
    glClearColor(BG_COLOR.x, BG_COLOR.y, BG_COLOR.z, 1);
    m_text.init();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);

    m_geometry.build_mesh();

    prg.init();
    prg.p_mat = glm::perspectiveRH(fpi * 0.25f, wnd_aspect(), 0.1f, 50000.0f);
    prg.v_mat = glm::lookAt(vec3(RADIUS*1.8, -RADIUS*1.8, RADIUS*1.8), vec3(0, 0, RADIUS * 0.5), vec3(0, 0, 1));
    prg.vp_mat = prg.p_mat * prg.v_mat;
    app::set_v_mat(&prg.v_mat);

    m_rains.resize(NUM_DROPS);
    mvp_mats.resize(NUM_DROPS);
    posz.resize(NUM_DROPS);

    // generate rain_drop positions and size
    //for (int i = 0; i < NUM_DROPS; ++i) {
      //m_mats.push_back(glm::translate(vec3(glm::linearRand(-RADIUS, RADIUS), 0, glm::linearRand(RADIUS, RADIUS + 20))));
    //}
    //glBindVertexArray(m_geometry.vao);
    //glGenBuffers(1, &mvp_vbo);
    //glBindBuffer(GL_ARRAY_BUFFER, mvp_vbo);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * m_rains.size(),  0, GL_STREAM_DRAW);

    glGenBuffers(1, &posz_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, posz_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * m_rains.size(),  0, GL_STREAM_DRAW);

  }

  void update()
  {

    for(auto iter = m_rains.begin(); iter != m_rains.end(); ++iter){
      iter->update(m_delta_time);
    }

    // update instance attribute buffer
    
    if(m_dirty_view)
      prg.vp_mat = prg.p_mat * prg.v_mat;

    //for (size_t i = 0; i < mvp_mats.size(); ++i) {
      //mvp_mats[i] = prg.vp_mat * m_rains[i].m_mat();
    //}
    //glBindBuffer(GL_ARRAY_BUFFER, mvp_vbo);
    //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mat4) * mvp_mats.size(), glm::value_ptr(mvp_mats[0]));
    
    for (size_t i = 0; i < posz.size(); ++i) {
      posz[i] = m_rains[i].posz();
    }
    glBindBuffer(GL_ARRAY_BUFFER, posz_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * posz.size(), glm::value_ptr(posz[0]));

  }

  void display()
  {
    glClear(GL_COLOR_BUFFER_BIT);
    
    prg.use();
    glUniform3fv(prg.ul_color, 1, glm::value_ptr(RAIN_COLOR));
    glUniformMatrix4fv(prg.ul_vp_mat, 1, 0, glm::value_ptr(prg.vp_mat));

    m_geometry.bind_attrib_locations(prg.al_vertex);

    //glBindBuffer(GL_ARRAY_BUFFER, mvp_vbo);
    //matrix_attrib_pointer(prg.al_mvp_mats);

    glBindBuffer(GL_ARRAY_BUFFER, posz_vbo);
    glVertexAttribPointer(prg.al_posz, 4, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
    glVertexAttribDivisor(prg.al_posz, 1);
    glEnableVertexAttribArray(prg.al_posz);

    m_geometry.draw(m_rains.size());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << m_fps << std::endl;
    m_text.print(ss.str(), 10, m_info.wnd_height - 20, vec4(0, 0, 0, 1));
    glDisable(GL_BLEND);
  }
};

}

int main(int argc, char *argv[])
{
  zxd::purple_rain_app app;
  app.run();
  return 0;
}
