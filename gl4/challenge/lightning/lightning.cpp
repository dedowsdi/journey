#include "lightning.h"
#include "program.h"

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

namespace zxd
{

//--------------------------------------------------------------------
void create_billboards_along(std::vector<vertex>& billboards, const vec3_vector& lines, const vec3&
    camera_pos, GLfloat bb_width/* = 2*/, GLint gap_slices/* = 6*/)
{
  //billboards.reserve(lines.size() * (gap_slices*3 + 6) / 2);

  GLfloat radius = bb_width * 0.5f;

  GLfloat step_angle = f2pi / gap_slices;
  GLint half_slice = gap_slices * 0.5;

  for (int i = 0; i < lines.size(); ) {
    const vec3& lv0 = lines[i++];
    const vec3& lv1 = lines[i++];

    vec3 v01 = glm::normalize(lv1 - lv0);
    vec3 ev = camera_pos - 0.5f*(lv0+lv1);
    // constrain billoboards rotate to v01 only; after rototation, side should
    // be perp to both ev and v01
    vec3 side = radius * glm::normalize(glm::cross(v01, ev));

    vertex v0 = {lv0 - side, vec2(0)};
    vertex v1 = {lv0 + side, vec2(1, 0)};
    vertex v2 = {lv1 + side, vec2(1, 1)};
    vertex v3 = {lv1 - side, vec2(0, 1)};

    billboards.push_back(v0);
    billboards.push_back(v1);
    billboards.push_back(v2);

    billboards.push_back(v0);
    billboards.push_back(v2);
    billboards.push_back(v3);

    // close gap with semicircle
    vec3 up = radius * v01;
    // bottom
    for (int j = 1; j <= half_slice; ++j) {
      GLfloat angle = step_angle * j;
      GLfloat c = cos(angle);
      GLfloat s = sin(angle);
      billboards.push_back({lv0, vec2(0.5)});
      if(j == 1)
        billboards.push_back({v0.pos, vec2(0, 0.5)});
      else
        billboards.push_back(billboards[billboards.size() - 2]);
      billboards.push_back({lv0 - side * c - up * s, vec2(0.5 - 0.5*c, 0.5 - 0.5*s)});
    }

    // top
    for (int j = 1; j <= half_slice; ++j) {
      GLfloat angle = step_angle *  j;
      billboards.push_back({lv1, vec2(0.5)});
      GLfloat c = cos(angle);
      GLfloat s = sin(angle);
      if(j == 1)
        billboards.push_back({v2.pos, vec2(1, 0.5)});
      else
        billboards.push_back(billboards[billboards.size() - 2]);
      billboards.push_back({lv1 + side * c + up * s, vec2(0.5 + 0.5*c, 0.5 + 0.5*s)});
    }
  }
}

//--------------------------------------------------------------------
void create_lightning(vec3_vector& lines, const vec3& start, const vec3& end, const std::string& pattern,
    GLfloat max_jitter/* = 0.5f*/, GLfloat max_fork_angle/* = fpi4*/)
{
  // first segment
  lines.push_back(start);
  lines.push_back(end);
  GLfloat l = glm::distance(start, end);

  vec3 global_dir = glm::normalize(end - start);

  for(auto c : pattern)
  {
    if(c != 'j' && c != 'f')
      continue;

    l *= 0.5f;

    for(auto iter = lines.begin(); iter != lines.end(); )
    {
      // jitter
      const vec3& v0 = *iter;
      const vec3& v1 = *++iter;
      vec3 v2 = (v0 + v1) * 0.5f;
      vec3 v01 = v1 - v0;

      v2 += zxd::random_orthogonal(glm::normalize(v01)) * glm::linearRand(0.0f, max_jitter) * l;

      // insert 2 center lines
      iter = lines.insert(iter, v2);
      iter = lines.insert(iter, v2);
      std::advance(iter, 3);

      // fork
      if(c == 'f')
      {
        // fork len should be the same as normal segment
        GLfloat seg_len = glm::distance(v0, v2);
        GLfloat theta = glm::linearRand(-max_fork_angle, max_fork_angle);
        vec3 v3 = v2 + zxd::rotate_in_cone(global_dir, theta) * seg_len;
        iter = lines.insert(iter, v2);
        iter = lines.insert(iter, v3);
        std::advance(iter, 2);
      }
    }
  }
}

struct lightining_billboard_program : public program
{
  virtual void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/dummy.vs.glsl");
    attach(GL_GEOMETRY_SHADER, "shader4/lightning_billboard.gs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/lightning.fs.glsl");
  };
}billboard_prg;

struct lightining_lsystem_program : public program
{
  virtual void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/dummy.vs.glsl");
    attach(GL_GEOMETRY_SHADER, "shader4/lightning_lsystem.gs.glsl");
  };
}lsystem_prg;

struct lightining_cpu_program : public program
{
  virtual void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/lightning.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/lightning.fs.glsl");
  };
}render_billboard_prg;

//--------------------------------------------------------------------
lightning::lightning(const std::string& pattern_, const vec3_vector& vertices_, GLfloat billboard_width_,
    GLfloat max_jitter/* = 0.5f*/, GLfloat max_fork_angle/* = fpi4*/) :
  pattern(pattern_),
  vertices(vertices_),
  billboard_width(billboard_width_),
  max_jitter(max_jitter),
  max_fork_angle(max_fork_angle),
  use_cpu(false),
  cpu_buffer(-1)
{
  glGenVertexArrays(1, &vao);
  glGenBuffers(2, buffer.pointer());
  update_vertex_count();
}

//--------------------------------------------------------------------
void lightning::update_vertex_count()
{
  vertex_count = vertices.size();
  for(auto c : pattern)
  {
    if(c == 'j')
      vertex_count *= 2;
    else if(c == 'f')
      vertex_count *= 3;
  }

  if(vertex_count * sizeof(vec3) > buffer_size)
  {
    glBindBuffer(GL_ARRAY_BUFFER, buffer.ping());
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(vec3), 0, GL_STREAM_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.pong());
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(vec3), 0, GL_STREAM_COPY);
    buffer_size = vertex_count * sizeof(vec3);
  }
}

//--------------------------------------------------------------------
void lightning::update_buffer(const vec3& camera_pos_)
{
  camera_pos = camera_pos_;
  if(use_cpu)
  {
    billboards.clear();
    for (int i = 0; i < vertices.size(); i+=2) {
      vec3_vector lsystem_lines;
      lsystem_lines.reserve(pow(3, pattern.size()) * vertices.size());
      create_lightning(lsystem_lines, vertices[i], vertices[i+1], pattern, max_jitter, max_fork_angle);
      create_billboards_along(billboards, lsystem_lines, camera_pos, billboard_width);
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, cpu_buffer);
    glBufferData(GL_ARRAY_BUFFER, billboards.size() * sizeof(vertex),
      &billboards.front(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET(sizeof(vec3)));
    glEnableVertexAttribArray(1);
    //std::cout << billboards.size() << std::endl;

    return;
  }

  if(!lsystem_prg.is_inited())
  {
    const char * varying[] = {"pos"};
    lsystem_prg.init();
    glTransformFeedbackVaryings(lsystem_prg, 1, varying, GL_SEPARATE_ATTRIBS);
    lsystem_prg.link();
  }

  update_vertex_count();
  glEnable(GL_RASTERIZER_DISCARD);
  GLuint num_vertices = vertices.size();

  glBindVertexArray(vao);
  lsystem_prg.use();
  for (int i = 0; i < pattern.size(); ++i) 
  {
    buffer.swap();

    char c = pattern[i];
    if(c != 'j' && c != 'f')
      continue;

    glBindBuffer(GL_ARRAY_BUFFER, buffer.ping());
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer.pong());

    if(i == 0)
    {
      glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(vec3), 
          glm::value_ptr(vertices.front()));
    }
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    glUniform1i(0, c == 'j' ? 0 : 1);
    glUniform1f(1, max_jitter);
    glUniform1f(2, max_fork_angle);
    glUniform1f(3, glm::linearRand(0.0f, 10.0f));
    glBeginTransformFeedback(GL_LINES);
    glDrawArrays(GL_LINES, 0, num_vertices);
    glEndTransformFeedback();

    num_vertices *= c== 'j' ? 2 : 3;

    //vec3_vector data;
    //data.resize(num_vertices);
    //glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, num_vertices * sizeof(vec3), &data.front());
    //for(auto item : data)
    //{
    //std::cout << item << std::endl;
    //}
    //std::cout << "-----------------------" << std::endl;
  }

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, buffer.pong());
  glVertexAttribPointer(0, 3, GL_FLOAT, 1, 0, BUFFER_OFFSET(0));

  glDisable(GL_RASTERIZER_DISCARD);
}

//--------------------------------------------------------------------
void lightning::draw(const mat4& mvp_mat)
{
  glDisable(GL_CULL_FACE); // you can enable it, it doesn't matter
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBlendEquation(GL_MAX);

  if(use_cpu)
  {
    if(!render_billboard_prg.is_inited())
      render_billboard_prg.init();

    render_billboard_prg.use();
    glUniformMatrix4fv(0, 1, 0, glm::value_ptr(mvp_mat));
    glUniform4fv(4, 1, glm::value_ptr(color0));
    glUniform4fv(5, 1, glm::value_ptr(color1));

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, billboards.size());
    return;
  }

  if(!billboard_prg.is_inited())
    billboard_prg.init();

  billboard_prg.use();
  glUniform1f(0, billboard_width);
  glUniform1i(1, 6);
  glUniform3fv(2, 1, glm::value_ptr(camera_pos));
  glUniformMatrix4fv(3, 1, 0, glm::value_ptr(mvp_mat));
  glUniform4fv(4, 1, glm::value_ptr(color0));
  glUniform4fv(5, 1, glm::value_ptr(color1));

  glBindVertexArray(vao);
  glDrawArrays(GL_LINES, 0, vertex_count);
}

//--------------------------------------------------------------------
void lightning::set_use_cpu(bool b)
{
  use_cpu = b;
  if(use_cpu)
  {
    if(cpu_buffer == -1)
      glGenBuffers(1, &cpu_buffer);

  }
  else
  {
    glBindVertexArray(vao);
    glDisableVertexAttribArray(1);
  }
}

//--------------------------------------------------------------------
void lightning::debug_print_billboards(const vec3& camrea_pos_)
{
  std::cout << "camera pos" << camera_pos << std::endl;
  int i = 0;
  for(auto& item : billboards)
  {
    if(i++ % 3 == 0)
      std::cout  << "triangle " << i/3 << std::endl;
    std::cout <<  "   " << item.pos << " , " << std::endl;
  }
}

}
