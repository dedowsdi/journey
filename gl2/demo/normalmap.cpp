/* normalmap.c
 *
 * used to give high detail to low quality geomety.
 *
 * there are two types of normal map: model space , tangent space.
 * benifit of tangent space:
 *   as z is always positive, you can only store xy components.
 *   reuse the same normal map on different surfaces?
 *   the ability to modify the texture mapping?
 *
 * you need a fine grained mesh and a corse grained mesh.
 * the fine grained mesh will be used to create normal map, each triangle in
 * fine grained mesh needs to find it's "parent" triangle and it's tangent space
 * in corse grained mesh, the normal is then converted into this tangent space.
 *
 * in order to render normal map, i use tex*2 - 1 as ndc, so the corresponding
 * normal will be rendered at resolution * tex.
 */
#include <sstream>

#include <app.h>
#include <program.h>
#include <light.h>
#include <stream_util.h>
#include <glmath.h>

namespace zxd {

glm::mat4 v_mat;
glm::mat4 p_mat;
using namespace glm;

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256

// clang-format off
struct pyramid0 {
  GLuint vao;
  vec3 vertices[12] = {
    vec3(0, 0, 0.5) , vec3(-1, -1, 0) , vec3(1,  -1, 0) ,
    vec3(0, 0, 0.5) , vec3(1,  -1, 0) , vec3(1,  1,  0) ,
    vec3(0, 0, 0.5) , vec3(1,  1,  0) , vec3(-1, 1,  0) ,
    vec3(0, 0, 0.5) , vec3(-1, 1,  0) , vec3(-1, -1, 0)
  };
  vec3 normals[12];
  vec2 texcoords[12] = {
    vec2(0.5,0.5), vec2(0,0), vec2(1,0),
    vec2(0.5,0.5), vec2(1,0), vec2(1,1),
    vec2(0.5,0.5), vec2(1,1), vec2(0,1),
    vec2(0.5,0.5), vec2(0,1), vec2(0,0)
  };

} pyramid0;

struct pyramid1 {
  GLuint vao;
vec3 vertices[4] = {
  vec3(-1, 1,  0),
  vec3(-1, -1, 0),
  vec3(1,  1,  0),
  vec3(1,  -1, 0)
};
vec3 normals[4] = {
  vec3(0, 0, 1),
  vec3(0, 0, 1),
  vec3(0, 0, 1),
  vec3(0, 0, 1)
};
vec3 tangents[4];
vec2 texcoords[4] = {
   vec2(0,1),
   vec2(0,0), 
   vec2(1,1),
   vec2(1,0) 
};

}pyramid1;
// clang-format on

struct render_normalmap_program : public zxd::program {

  virtual void attach_shaders() {
    attach(
      GL_VERTEX_SHADER, "shader2/render_normalmap.vs.glsl");
    attach(
      GL_FRAGMENT_SHADER, "shader2/render_normalmap.fs.glsl");
  }
  virtual void bind_uniform_locations() {}

  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "normal");
    bind_attrib_location(1, "texcoord");
  };
} render_prg;

struct use_normal_map_view_program : public zxd::program {

  GLint ul_normal_map;
  GLint ul_mv_mat_it;
  GLint ul_mv_mat;
  GLint ul_mvp_mat;

  virtual void update_uniforms(const mat4 &m_mat, const mat4 &v_mat, const mat4 &p_mat) {
    mat4 mv_mat = v_mat * m_mat;
    mat4 mv_mat_it = glm::inverse(glm::transpose(mv_mat));
    mat4 mvp_mat = p_mat * mv_mat;

    glUniformMatrix4fv(ul_mv_mat_it, 1, 0, value_ptr(mv_mat_it));
    glUniformMatrix4fv(ul_mv_mat, 1, 0, value_ptr(mv_mat));
    glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  }
  virtual void attach_shaders() {
    attach(
      GL_VERTEX_SHADER, "shader2/use_normalmap_view.vs.glsl");
    string_vector sv;
    sv.push_back("#version 120\n #define LIGHT_COUNT 1\n");
    sv.push_back(stream_util::read_resource("shader2/blinn.frag"));
    attach(
      GL_FRAGMENT_SHADER, sv, "shader2/use_normalmap_view.fs.glsl");

    name("use_normalmap_view");
  }
  virtual void bind_uniform_locations() {
    ul_mv_mat = get_uniform_location("mv_mat");
    ul_mv_mat_it = get_uniform_location("mv_mat_it");
    ul_mvp_mat = get_uniform_location("mvp_mat");
    ul_normal_map = get_uniform_location("normal_map");
  }

  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "normal");
    bind_attrib_location(2, "tangent");
    bind_attrib_location(3, "texcoord");
  };
} prg0;

struct use_normal_map_tangent_program : public program {
  GLint ul_normal_map;
  GLint ul_m_camera;
  GLint ul_mvp_mat;

  virtual void update_uniforms(const mat4 &m_mat, const mat4& v_mat, const mat4& p_mat) {
    mat4 mv_mat = v_mat * m_mat;
    // mat4 mv_mat_i = glm::inverse(mv_mat);
    // mat4 m_mat_i = glm::inverse(m_mat);
    mat4 mvp_mat = p_mat * mv_mat;
    glUniformMatrix4fv(ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
  };
  virtual void attach_shaders() {
    string_vector sv;
    sv.push_back("#version 120\n #define LIGHT_COUNT 1\n");
    attach(
      GL_VERTEX_SHADER, sv, "shader2/use_normalmap_tangent.vs.glsl");
    sv.push_back(stream_util::read_resource("shader2/blinn.frag"));
    attach(
      GL_FRAGMENT_SHADER, sv, "shader2/use_normalmap_tangent.fs.glsl");

    name("use_normalmap_tangent");
  }
  virtual void bind_uniform_locations() {
    ul_mvp_mat = get_uniform_location("mvp_mat");
    ul_normal_map = get_uniform_location("normal_map");
    ul_m_camera = get_uniform_location("m_camera");
  }
  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "normal");
    bind_attrib_location(2, "tangent");
    bind_attrib_location(3, "texcoord");
  };
} prg1;

GLuint normal_map;
std::vector<zxd::light_source> lights;
zxd::light_model lm;
zxd::material mtl;
GLint light_space = 0;  // 0 : view, 1 : tangent

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "normalmap";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = IMAGE_WIDTH * 2;
    m_info.wnd_height = IMAGE_HEIGHT;
  }

  void generate_normal_map() {
    glViewport(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    glScissor(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(pyramid0.vao);
    glUseProgram(render_prg);

    glDrawArrays(GL_TRIANGLES, 0, 12);

    glBindTexture(GL_TEXTURE_2D, normal_map);
    glCopyTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGB, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, 0);
  }

  void bind_uniform_locations(zxd::program &prg) {
    lm.bind_uniform_locations(prg.get_object(), "lm");
    for (int i = 0; i < lights.size(); ++i) {
      std::stringstream ss;
      ss << "lights[" << i << "]";
      lights[i].bind_uniform_locations(prg.get_object(), ss.str());
    }
    mtl.bind_uniform_locations(prg.get_object(), "mtl");
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    render_prg.init();
    prg0.init();
    p_mat = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 20.0f);
    v_mat = glm::lookAt(vec3(5, -5, 5), vec3(0.0f), vec3(0, 1, 0));
    prg1.init();

    set_v_mat(&v_mat);
    // create mesh data
    zxd::generate_face_normals(
      pyramid0.vertices, pyramid0.vertices + 12, pyramid0.normals);

    mat4 tbn = zxd::get_tangetn_basis(pyramid1.vertices[0],
      pyramid1.vertices[1], pyramid1.vertices[2], pyramid1.texcoords[0],
      pyramid1.texcoords[1], pyramid1.texcoords[2], &pyramid1.normals[0]);
    mat4 tbn_i = inverse(tbn);
    vec3 tangent = vec3(glm::column(tbn, 0));
    // vec3 tangent = vec3(glm::row(tbn, 0));

    for (int i = 0; i < 4; ++i) pyramid1.tangents[i] = tangent;

    // transform modle normals to tbn space
    for (int i = 0; i < 12; ++i)
      pyramid0.normals[i] = zxd::transform_vector(tbn_i, pyramid0.normals[i]);

    // create vertex arries
    glGenVertexArrays(1, &pyramid0.vao);
    glBindVertexArray(pyramid0.vao);

    {
      GLuint vbo;
      glGenBuffers(1, &vbo);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid0.normals) + sizeof(pyramid0.texcoords), 0, GL_STATIC_DRAW );

      auto normal_size = sizeof(pyramid0.normals);
      glBufferSubData(GL_ARRAY_BUFFER, 0, normal_size, glm::value_ptr(pyramid0.normals[0]));
      glBufferSubData(GL_ARRAY_BUFFER, normal_size, sizeof(pyramid0.texcoords), glm::value_ptr(pyramid0.texcoords[0]));

      glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
      glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, BUFFER_OFFSET(normal_size));
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
    }

    glGenVertexArrays(1, &pyramid1.vao);
    glBindVertexArray(pyramid1.vao);
    {
      GLuint vbo;
      glGenBuffers(1, &vbo);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      auto total_size = sizeof(pyramid1.vertices) + sizeof(pyramid1.normals)
          + sizeof(pyramid1.tangents) + sizeof(pyramid1.texcoords);
      glBufferData(GL_ARRAY_BUFFER, total_size, 0, GL_STATIC_DRAW );

      auto start = 0;
      auto size = sizeof(pyramid1.vertices);
      glBufferSubData(GL_ARRAY_BUFFER, start, size, glm::value_ptr(pyramid1.vertices[0]));
      glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(start));
      start += size;
      size = sizeof(pyramid1.normals);
      glBufferSubData(GL_ARRAY_BUFFER, start, size, glm::value_ptr(pyramid1.normals[0]));
      glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(start));
      start += size;
      size = sizeof(pyramid1.tangents);
      glBufferSubData(GL_ARRAY_BUFFER, start, size, glm::value_ptr(pyramid1.tangents[0]));
      glVertexAttribPointer(2, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(start));
      start += size;
      size = sizeof(pyramid1.texcoords);
      glBufferSubData(GL_ARRAY_BUFFER, start, size, glm::value_ptr(pyramid1.texcoords[0]));
      glVertexAttribPointer(3, 2, GL_FLOAT, 0, 0, BUFFER_OFFSET(start));
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);
      glEnableVertexAttribArray(3);
    }

    glGenTextures(1, &normal_map);
    glBindTexture(GL_TEXTURE_2D, normal_map);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGB,
      GL_UNSIGNED_BYTE, 0);

    // light
    zxd::light_source dir_light;
    dir_light.position = vec4(1, -1, 1, 0);
    dir_light.diffuse = vec4(1, 1, 1, 1);
    dir_light.specular = vec4(1, 1, 1, 1);
    dir_light.linear_attenuation = 1.0f;

    lights.push_back(dir_light);

    lm.local_viewer = 1;

    // mtl
    mtl.ambient = vec4(0.2);
    mtl.diffuse = vec4(0.8);
    mtl.specular = vec4(1.0);
    mtl.shininess = 50;

    bind_uniform_locations(prg0);
  }

  void display(void) {
    generate_normal_map();

    glViewport(IMAGE_WIDTH, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    glEnable(GL_SCISSOR_TEST);
    glScissor(IMAGE_WIDTH, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw low quality mesh with normal map
    if (light_space == 0) {
      glUseProgram(prg0);
      prg0.update_uniforms(mat4(1.0), v_mat, p_mat);
      glUniform1i(prg0.ul_normal_map, 0);

      for (int i = 0; i < lights.size(); ++i) {
        lights[i].update_uniforms(v_mat);
      }
    } else {
      glUseProgram(prg1);
      mat4 m_mat = mat4(1.0);
      prg1.update_uniforms(m_mat, v_mat, p_mat);
      glUniform1i(prg1.ul_normal_map, 0);

      mat4 mv_mat_i = glm::inverse(v_mat * m_mat);
      mat4 m_mat_i = glm::inverse(m_mat);
      // get camera model position
      vec3 camera = vec3(glm::column(mv_mat_i, 3));
      glUniform3fv(prg1.ul_m_camera, 1, glm::value_ptr(camera));

      for (int i = 0; i < lights.size(); ++i) {
        lights[i].update_uniforms(m_mat_i);
      }
    }
    lm.update_uniforms();
    mtl.update_uniforms();

    glBindVertexArray(pyramid1.vao);

    glDisable(GL_SCISSOR_TEST);
    glBindTexture(GL_TEXTURE_2D, normal_map);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glUseProgram(0);
    glDisable(GL_TEXTURE_2D);

    glViewport(0, 0, IMAGE_WIDTH * 2, IMAGE_HEIGHT);

    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, m_info.wnd_height - 20);
    GLchar info[256];

    sprintf(
      info, "q : lighting space : %s \n", light_space == 0 ? "view" : "tbn");
    glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte *)info);
    glEnable(GL_TEXTURE_2D);

    glFlush();
    ZCGEA
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    // create normal map
    generate_normal_map();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
        light_space ^= 1;
        if (light_space == 0) {
          bind_uniform_locations(prg0);
        } else {
          bind_uniform_locations(prg1);
        }
        break;

      default:
        break;
    }
  }
};
}
int main(int argc, char **argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
