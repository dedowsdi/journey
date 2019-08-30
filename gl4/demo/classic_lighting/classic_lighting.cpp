#include "app.h"

#include <sstream>
#include <jsoncfg.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <sphere.h>
#include <torus.h>
#include <cone.h>
#include <stream_util.h>
#include <common.h>
#include <objman.h>

namespace rj = rapidjson;

namespace zxd {

const GLint width = 800;
const GLint height = 800;
GLuint texture;
json_cfg cfg;
app* app0;

mat4 p_mat;
mat4 v_mat;
mat4 m_mat;
mat4 mv_mat;
mat4 mv_mat_i;
mat4 m_mat_i;
mat4 mv_mat_it;
mat4 mvp_mat;

std::vector<std::shared_ptr<geometry_base>> geometries;

enum technique_enum
{
  PER_FLAGMENT_VIEW,
  PER_VERTEX_VIEW,
  PER_FLAGMENT_OBJECT,
  PER_VERTEX_OBJECT
};

std::array<std::string, 4> techniques =
{
  "PER_FLAGMENT_VIEW",
  "PER_VERTEX_VIEW",
  "PER_FLAGMENT_OBJECT",
  "PER_VERTEX_OBJECT"
};

std::array<std::string, 3> models =
{
  "sphere",
  "torus",
  "cone"
};

std::array<std::string, 2> specular_methods =
{
  "phong",
  "blinn"
};

std::array<std::string, 2> color_results =
{
  "single",
  "separate"
};

std::shared_ptr<kci> technique;
std::shared_ptr<kci> model;
std::shared_ptr<kci> specular_method;
std::shared_ptr<kci> color_result;
std::shared_ptr<kci> with_texture;
std::shared_ptr<geometry_base> target;

struct light_model
{
  GLint ul_local_viewer;
  GLint ul_ambient;
  GLint ul_v2e;

  enum class color_control_enum
  {
    SINGLE_COLOR = 0,
    SEPARATE_SPECULAR_COLOR = 1
  };
  bool local_viewer{false};

  // only used when local_viewer is false. It's (0,0,1) in view space, you must
  // convert it to current lighting space
  vec3 v2e;
  vec4 ambient{0.2, 0.2, 0.2, 1};
  color_control_enum color_control{color_control_enum::SINGLE_COLOR};

  void bind_uniform_locations(GLuint prg, const std::string& name = "lm")
  {
    ul_local_viewer  = glGetUniformLocation(prg, (name + ".local_viewer").c_str());
    ul_ambient       = glGetUniformLocation(prg, (name + ".ambient").c_str());
    ul_v2e           = glGetUniformLocation(prg, (name + ".v2e").c_str());
  }

  void update_uniforms()
  {
    glUniform1i(ul_local_viewer, local_viewer);
    glUniform4fv(ul_ambient, 1, value_ptr(ambient));
    glUniform3fv(ul_v2e, 1, value_ptr(v2e));
  }

  void read_setting(rj::Value& value)
  {
    assert(value.IsObject());
    for (const auto& item : value.GetObject())
    {
      auto name = item.name.GetString();
      if (strcmp(name, "ambient") == 0)
      {
        ambient = read_json_vec4(item.value);
      }
      else if (strcmp(name, "local_viewer") == 0)
      {
        local_viewer = item.value.GetBool();
      }
      else if (strcmp(name, "color_control") == 0)
      {
        color_control = static_cast<color_control_enum>(item.value.GetInt());
      }
      else
      {
        std::cout << "unknowm light model property : " << item.name.GetString()
                  << std::endl;
      }
    }
  }

};

struct material
{
  GLint ul_ambient;
  GLint ul_diffuse;
  GLint ul_specular;
  GLint ul_emission;
  GLint ul_shininess;

  vec4 ambient{0.2, 0.2, 0.2, 1};
  vec4 diffuse{0.8, 0.8, 0.8, 1};
  vec4 specular{0.0, 0.0, 0.0, 1};
  vec4 emission{0.0, 0.0, 0.0, 1};
  GLfloat shininess{0};

  void bind_uniform_locations(GLuint prg, const std::string& name = "mtl")
  {
    ul_ambient   = glGetUniformLocation(prg, (name + ".ambient").c_str());
    ul_diffuse   = glGetUniformLocation(prg, (name + ".diffuse").c_str());
    ul_specular  = glGetUniformLocation(prg, (name + ".specular").c_str());
    ul_emission  = glGetUniformLocation(prg, (name + ".emission").c_str());
    ul_shininess = glGetUniformLocation(prg, (name + ".shininess").c_str());
  }

  void update_uniforms()
  {
    glUniform4fv(ul_ambient  , 1, value_ptr(ambient  ));
    glUniform4fv(ul_diffuse  , 1, value_ptr(diffuse  ));
    glUniform4fv(ul_specular , 1, value_ptr(specular ));
    glUniform4fv(ul_emission , 1, value_ptr(emission ));
    glUniform1f(ul_shininess, shininess);
  }

  void read_setting(rj::Value& value)
  {
    assert(value.IsObject());
    for (const auto& item : value.GetObject())
    {
      auto name = item.name.GetString();
      if (strcmp(name, "ambient") == 0)
      {
        ambient = read_json_vec4(item.value);
      }
      else if (strcmp(name, "diffuse") == 0)
      {
        diffuse = read_json_vec4(item.value);
      }
      else if (strcmp(name, "specular") == 0)
      {
        specular = read_json_vec4(item.value);
      }
      else if (strcmp(name, "emission") == 0)
      {
        emission = read_json_vec4(item.value);
      }
      else if (strcmp(name, "shininess") == 0)
      {
        shininess = item.value.GetFloat();
      }
      else
      {
        std::cout << "unknowm material property : " << item.name.GetString()
                  << std::endl;
      }
    }
  }

};

struct light_source
{
  GLint ul_position;
  GLint ul_ambient;
  GLint ul_diffuse;
  GLint ul_specular;
  GLint ul_emission;
  GLint ul_attenuation;
  GLint ul_spot_direction;
  GLint ul_spot_cutoff;
  GLint ul_cos_spot_cutoff;
  GLint ul_spot_exponent;

  vec4 position{0.0, 0.0, 1, 0};
  vec4 ambient{0.0, 0.0, 0.0, 1};
  vec4 diffuse{0.0, 0.0, 0.0, 1};
  vec4 specular{0.0, 0.0, 0.0, 1};
  vec4 emission{0.0, 0.0, 0.0, 1};
  vec3 attenuation{1, 0, 0};
  vec3 spot_direction{0, 0, -1};
  GLfloat spot_cutoff{180};
  GLfloat cos_spot_cutoff{0};
  GLfloat spot_exponent{0};

  void bind_uniform_locations(GLuint prg, const std::string& name)
  {
    ul_position        = glGetUniformLocation(prg, (name + ".position").c_str());
    ul_ambient         = glGetUniformLocation(prg, (name + ".ambient").c_str());
    ul_diffuse         = glGetUniformLocation(prg, (name + ".diffuse").c_str());
    ul_specular        = glGetUniformLocation(prg, (name + ".specular").c_str());
    ul_emission        = glGetUniformLocation(prg, (name + ".emission").c_str());
    ul_attenuation     = glGetUniformLocation(prg, (name + ".attenuation").c_str());
    ul_spot_direction  = glGetUniformLocation(prg, (name + ".spot_direction").c_str());
    ul_spot_cutoff     = glGetUniformLocation(prg, (name + ".spot_cutoff").c_str());
    ul_cos_spot_cutoff = glGetUniformLocation(prg, (name + ".cos_spot_cutoff").c_str());
    ul_spot_exponent   = glGetUniformLocation(prg, (name + ".spot_exponent").c_str());
  }

  void update_uniforms(const mat4& m)
  {
    auto pos = m * position;
    auto dir = vec3(m * vec4(spot_direction, 0));
    auto cos_spot_cutoff = cos(spot_cutoff * fpi / 180.0f);
    glUniform4fv(ul_position, 1, value_ptr(pos));
    glUniform4fv(ul_ambient, 1, value_ptr(ambient));
    glUniform4fv(ul_diffuse, 1, value_ptr(diffuse));
    glUniform4fv(ul_specular, 1, value_ptr(specular));
    glUniform4fv(ul_emission, 1, value_ptr(emission));
    glUniform3fv(ul_attenuation, 1, value_ptr(attenuation));
    glUniform3fv(ul_spot_direction, 1, value_ptr(dir));
    glUniform1f(ul_spot_cutoff, spot_cutoff);
    glUniform1f(ul_cos_spot_cutoff, cos_spot_cutoff);
    glUniform1f(ul_spot_exponent, spot_exponent);
  }

  void read_setting(rj::Value& value)
  {
    assert(value.IsObject());
    for (const auto& item : value.GetObject())
    {
      auto name = item.name.GetString();
      if (strcmp(name, "position") == 0)
      {
        position = read_json_vec4(item.value);
      }
      else if (strcmp(name, "ambient") == 0)
      {
        ambient = read_json_vec4(item.value);
      }
      else if (strcmp(name, "diffuse") == 0)
      {
        diffuse = read_json_vec4(item.value);
      }
      else if (strcmp(name, "specular") == 0)
      {
        specular = read_json_vec4(item.value);
      }
      else if (strcmp(name, "attenuation") == 0)
      {
        attenuation = read_json_vec3(item.value);
      }
      else if (strcmp(name, "spot_direction") == 0)
      {
        spot_direction = read_json_vec3(item.value);
      }
      else if (strcmp(name, "spot_cutoff") == 0)
      {
        spot_cutoff = item.value.GetFloat();
      }
      else if (strcmp(name, "spot_exponent") == 0)
      {
        spot_exponent = item.value.GetFloat();
      }
      else
      {
        std::cout << "unknowm light property : " << item.name.GetString()
                  << std::endl;
      }
    }
  }
};

light_model lm;
material mtl;
std::vector<light_source> lights;

void reload_cfg();

void reset_technique(const kci* k);

void render_per_flagment_view();
void render_per_flagment_object();
void render_per_vertex_view();
void render_per_vertex_object();

void update_lighting_uniforms(light_model& lm, material& mtl,
  std::vector<light_source>& lights, const mat4& m);


class lighting_program : public program
{
public:

  void bind_uniform_locations(
    light_model& lm, material& mtl, std::vector<light_source>& lights)
  {
    lm.bind_uniform_locations(object);
    mtl.bind_uniform_locations(object);
    for (auto i = 0u; i < lights.size(); ++i)
    {
      std::stringstream ss;
      ss << "lights[" << i << "]";
      lights[i].bind_uniform_locations(object, ss.str());
    }
  }

protected:

  std::map<std::string, std::string> get_lighting_replace_map()
  {
    std::map<std::string, std::string> m;
    m["// _LIGHTING_PLACEHOLDER_"] =
      stream_util::read_shader_block("classic_lighting_data/lighting.frag");
    // m["// _RESULT_COLOR_PLACEHOLDER_"] =
    //   stream_util::read_shader_block("classic_lighting_data/color.frag");

    std::stringstream ss;
    ss << "#define LIGHT_COUNT " << light_count << "\n";
    if(phong)
      ss << "#define PHONG\n";
    if(with_texture)
      ss << "#define WITH_TEXTURE\n";
    if(single)
      ss << "#define SINGLE_COLOR\n";
    ss << "#define _VERTEX_ fi.vertex";

    m["// _PP_PLACEHOLDER_"] = ss.str();

    return m;
  }

  std::map<std::string, std::string> get_texture_replace_map()
  {
    std::map<std::string, std::string> m;
    std::stringstream ss;
    if(with_texture)
      ss << "#define WITH_TEXTURE\n";
    if(single)
      ss << "#define SINGLE_COLOR\n";

    m["// _PP_PLACEHOLDER_"] = ss.str();
    return m;
  }

  string_vector get_define_texture()
  {
    std::stringstream ss;
    ss << "#version 430 core\n";
    if (with_texture)
    {
      ss << "#define WITH_TEXTURE\n";
    }
    string_vector sv;
    sv.push_back(ss.str());
    return sv;
  }

public:
  GLuint light_count = 1;
  bool phong = false;
  bool with_texture = 0;
  bool single = true;


};

class per_fragment_view_program : public lighting_program
{
public:
  GLint ul_mvp_mat = 0;
  GLint ul_mv_mat = 1;
  GLint ul_mv_mat_it = 2;
  GLint ul_diffuse_map = 3;
  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "classic_lighting_data/per_fragment_view.vs.glsl",
      get_texture_replace_map());

    attach(GL_FRAGMENT_SHADER,
      "classic_lighting_data/per_fragment_view.fs.glsl",
      get_lighting_replace_map());
  }
};

class per_vertex_view_program : public lighting_program
{
public:
  GLint ul_mvp_mat = 0;
  GLint ul_mv_mat = 1;
  GLint ul_mv_mat_it = 2;
  GLint ul_diffuse_map = 3;
  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER,
      "classic_lighting_data/per_vertex_view.vs.glsl", get_lighting_replace_map());
    attach(GL_FRAGMENT_SHADER, "classic_lighting_data/per_vertex.fs.glsl",
      get_texture_replace_map());
  }
};

class per_fragment_object_program : public lighting_program
{
public:
  GLint ul_mvp_mat = 0;
  GLint ul_eye = 1;
  GLint ul_diffuse_map = 2;
  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER,
      "classic_lighting_data/per_fragment_object.vs.glsl",
      get_texture_replace_map());
    attach(GL_FRAGMENT_SHADER,
      "classic_lighting_data/per_fragment_object.fs.glsl",
      get_lighting_replace_map());
  }
};

class per_vertex_object_program : public lighting_program
{
public:
  GLint ul_mvp_mat = 0;
  GLint ul_eye = 1;
  GLint ul_diffuse_map = 3;
  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "classic_lighting_data/per_vertex_object.vs.glsl",
      get_lighting_replace_map());
    attach(GL_FRAGMENT_SHADER, "classic_lighting_data/per_vertex.fs.glsl", get_texture_replace_map());
  }
};

std::array<std::shared_ptr<lighting_program>, 4> prgs;
std::shared_ptr<lighting_program> prg;

class classic_lighting_app : public app {
private:

public:
  void init_info() override;
  void create_scene() override;

  void update() override;

  void display() override;

  void glfw_resize(GLFWwindow *wnd, int w, int h) override;

  void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) override;
  void glfw_mouse_button(GLFWwindow *wnd, int button, int action,
    int mods) override;

  void glfw_mouse_move(GLFWwindow *wnd, double x, double y) override;

  void reload_shaders() override;
};

void classic_lighting_app::init_info() {
  app::init_info();
  m_info.title = "classic_lighting_app";
  m_info.wnd_width = width;
  m_info.wnd_height = height;
}

void classic_lighting_app::create_scene() {
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  // programs
  prgs[0] = std::make_shared<per_fragment_view_program>();
  prgs[1] = std::make_shared<per_vertex_view_program>();
  prgs[2] = std::make_shared<per_fragment_object_program>();
  prgs[3] = std::make_shared<per_vertex_object_program>();

  // matrixes
  p_mat = perspective(fpi4, wnd_aspect(), 0.1f, 100.0f);
  lookat(vec3(0, -4, 0), vec3(0), vec3(0, 0, 1), &v_mat);
  m_mat = mat4(1);

  // drawables
  auto sphere0 = std::make_shared<sphere>();
  sphere0->include_normal(true);
  sphere0->include_texcoord(true);
  sphere0->slice(64);
  sphere0->stack(64);
  sphere0->build_mesh();

  auto torus0 = std::make_shared<torus>();
  torus0->include_normal(true);
  torus0->include_texcoord(true);
  torus0->rings(64);
  torus0->sides(64);
  torus0->build_mesh();

  auto cone0 = std::make_shared<cone>();
  cone0->include_normal(true);
  cone0->include_texcoord(true);
  cone0->slice(64);
  cone0->stack(64);
  cone0->build_mesh();

  geometries.push_back(std::move(sphere0));
  geometries.push_back(std::move(torus0));
  geometries.push_back(std::move(cone0));

  // chess texture
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLuint tex_width = 1024;
  GLuint tex_height = 1024;
  auto tex_bytes = create_chess_image(tex_width, tex_height, 64, 64);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA,
    GL_UNSIGNED_BYTE, &tex_bytes.front());
  glGenerateMipmap(GL_TEXTURE_2D);

  // kui
  model = m_control.add_enum('W', {0, 1, 2}, 0);

  auto reload_shader_callback = [this](auto) { reload_shaders(); };
  specular_method = m_control.add_enum('E', {0, 1}, 1, reload_shader_callback);
  color_result = m_control.add_enum('R', {0, 1}, 0, reload_shader_callback);

  technique = m_control.add_enum('Q', {0, 1, 2, 3}, 0, reset_technique);

  with_texture = m_control.add_bool('U', false, [this](auto) {
    for (auto& g : geometries)
    {
      g->set_attrib_activity(2, with_texture->get_bool());
    }
    reload_shaders();
  });

  reload_cfg();
  reset_technique(technique.get());

  auto p = compute_window_mat(0, 0, width, height) * p_mat * v_mat * m_mat *
              vec4(0, 0, 0, 1);
  auto wp = vec2(p) / p.w;
  add_input_handler(std::make_shared<trackball_objman>(&m_mat, &v_mat, wp));
}


void classic_lighting_app::update() {
  if (cfg.modified_outside())
  {
    reload_cfg();
  }
}

void classic_lighting_app::display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mv_mat = v_mat * m_mat;
  mv_mat_i = inverse(mv_mat);
  mv_mat_it = transpose(mv_mat_i);
  m_mat_i = inverse(m_mat);
  mvp_mat = p_mat * mv_mat;

  auto itech = technique->get_int_enum();
  target = geometries[model->get_int_enum()];

  if (with_texture->get_bool())
  {
    glBindTexture(GL_TEXTURE_2D, texture);
  }

  switch (itech)
  {
    case PER_FLAGMENT_VIEW:
      render_per_flagment_view();
      break;
    case PER_FLAGMENT_OBJECT:
      render_per_flagment_object();
      break;
    case PER_VERTEX_VIEW:
      render_per_vertex_view();
      break;
    case PER_VERTEX_OBJECT:
      render_per_vertex_object();
      break;
    default:
      break;
  }

  if(!m_display_help)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  ss << "Q : technique : " << techniques[technique->get_int_enum()] << std::endl;
  ss << "W : model :" << models[model->get_int_enum()] << std::endl;
  ss << "E : specular method : "
     << specular_methods[specular_method->get_int_enum()] << std::endl;
  ss << "R : colors : " << color_results[static_cast<GLint>(lm.color_control)]
     << std::endl;
  ss << "U : with texture : " << with_texture->get_bool() << std::endl;

  m_text.print(ss.str(), 10, m_info.wnd_height - 20);
  glDisable(GL_BLEND);
}

void classic_lighting_app::glfw_resize(GLFWwindow *wnd, int w, int h) {
  app::glfw_resize(wnd, w, h);
}

void classic_lighting_app::glfw_key(
  GLFWwindow *wnd, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      default:
        break;
    }
  }
  app::glfw_key(wnd, key, scancode, action, mods);
}

void classic_lighting_app::glfw_mouse_button(
  GLFWwindow* wnd, int button, int action, int mods)
{
  app::glfw_mouse_button(wnd, button, action, mods);
}

void classic_lighting_app::glfw_mouse_move(GLFWwindow* wnd, double x, double y)
{
  app::glfw_mouse_move(wnd, x, y);
}


//--------------------------------------------------------------------
void classic_lighting_app::reload_shaders() 
{
  lm.color_control =
    static_cast<light_model::color_control_enum>(color_result->get_int_enum());
  for (auto& p : prgs)
  {
    p->light_count = lights.size();
    p->phong = specular_method->get_int_enum() == 0;
    p->single =
      lm.color_control == light_model::color_control_enum::SINGLE_COLOR;
    p->with_texture = with_texture->get_bool();
    p->reload();
  }
  if(prg)
    prg->bind_uniform_locations(lm, mtl, lights);
}

//--------------------------------------------------------------------
void reload_cfg()
{
  cfg.load("classic_lighting_data/lighting.json");
  lm.read_setting(cfg.get_value("model"));
  mtl.read_setting(cfg.get_value("material"));

  auto& json_lights = cfg.get_value("lights");
  assert(json_lights.IsArray());

  lights.resize(json_lights.Size());
  for (unsigned i = 0; i < lights.size() ; ++i)
  {
    lights[i].read_setting(json_lights[i]);
  }
  std::cout << "read " << lights.size() << " lights"  << std::endl;
  app0->reload_shaders();
}

//--------------------------------------------------------------------
void reset_technique(const kci* k)
{
  prg = prgs[k->get_int_enum()];
  prg->bind_uniform_locations(lm, mtl, lights);
}

//--------------------------------------------------------------------
void render_per_flagment_view()
{
  lm.v2e = vec3(0, 0, 1);
  auto p = std::static_pointer_cast<per_fragment_view_program>(prg);
  p->use();
  glUniformMatrix4fv(p->ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  glUniformMatrix4fv(p->ul_mv_mat, 1, 0, value_ptr(mv_mat));
  glUniformMatrix4fv(p->ul_mv_mat_it, 1, 0, value_ptr(mv_mat_it));
  if (p->with_texture)
    glUniform1i(p->ul_diffuse_map, 0);
  update_lighting_uniforms(lm, mtl, lights, v_mat);

  target->draw();
}

//--------------------------------------------------------------------
void render_per_flagment_object()
{
  lm.v2e = vec3(mat3(mv_mat_i)[2]);
  auto p = std::static_pointer_cast<per_fragment_object_program>(prg);
  p->use();
  vec3 eye = vec3(mv_mat_i[3]);
  glUniformMatrix4fv(p->ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  glUniform3fv(p->ul_eye, 1, value_ptr(eye));
  if (p->with_texture)
    glUniform1i(p->ul_diffuse_map, 0);
  update_lighting_uniforms(lm, mtl, lights, m_mat_i);

  target->draw();
}

//--------------------------------------------------------------------
void render_per_vertex_view()
{
  lm.v2e = vec3(0, 0, 1);
  auto p = std::static_pointer_cast<per_vertex_view_program>(prg);
  p->use();
  glUniformMatrix4fv(p->ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  glUniformMatrix4fv(p->ul_mv_mat, 1, 0, value_ptr(mv_mat));
  glUniformMatrix4fv(p->ul_mv_mat_it, 1, 0, value_ptr(mv_mat_it));
  if (p->with_texture)
    glUniform1i(p->ul_diffuse_map, 0);
  update_lighting_uniforms(lm, mtl, lights, v_mat);

  target->draw();
}

//--------------------------------------------------------------------
void render_per_vertex_object()
{
  lm.v2e = vec3(mat3(mv_mat_i)[2]);
  auto p = std::static_pointer_cast<per_vertex_object_program>(prg);
  p->use();
  vec3 eye = vec3(mv_mat_i[3]);
  glUniformMatrix4fv(p->ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  glUniform3fv(p->ul_eye, 1, value_ptr(eye));
  if (p->with_texture)
    glUniform1i(p->ul_diffuse_map, 0);
  update_lighting_uniforms(lm, mtl, lights, m_mat_i);

  target->draw();
}

void update_lighting_uniforms(light_model& lm, material& mtl,
  std::vector<light_source>& lights, const mat4& m)
{
  lm.update_uniforms();
  mtl.update_uniforms();
  for (auto& light : lights)
  {
    light.update_uniforms(m);
  }
}

}

int main(int argc, char *argv[]) {
  zxd::classic_lighting_app app;
  zxd::app0 = &app;
  app.run();
}
