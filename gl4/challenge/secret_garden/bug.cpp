#include "bug.h"

#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include "color.h"

#define BUFFER_OFFSET(x) ((GLubyte*)NULL + (x))

namespace zxd
{

//--------------------------------------------------------------------
bug::bug(GLuint wing_count/* = 50*/, GLuint vertex_count/* = 15*/)
:
  m_vao(-1),
  m_vbo(-1),
  m_wing_count(wing_count),
  m_vertex_count(vertex_count)
  {
  }

//--------------------------------------------------------------------
void bug::update()
{
  // update vertex data
  vec3_vector vertices;
  vec3_vector colors;

  vertices.reserve(m_wing_count * m_vertex_count);
  colors.reserve(vertices.size());

  for (int wing_index = 0; wing_index < m_wing_count; ++wing_index)
  {
    float wing_ratio = static_cast<GLfloat>(wing_index)/m_wing_count;
    vertices.push_back(vec3(0));
    colors.push_back(vec3(1, 0, 0));

    float sat = mix(0.0, 0.6, wing_ratio);

    for (int vertex_index = 1; vertex_index < m_vertex_count; ++vertex_index)
    {
      float x = 10 * glm::perlin(vec2(vertex_index * 0.06 + wing_index * 1.0 / m_wing_count));
      float y = 10 * glm::perlin(vec2(vertex_index * 0.05 + wing_index * 1.0 / m_wing_count));
      //int wing_shape = wing_index + vertex_index;
      //float x = m_size * glm::perlin(vec2(wing_shape * 0.06));
      //float y = m_size * glm::perlin(vec2(wing_shape * 0.05));
      float bri = m_size * m_size * 2 / (x*x + y*y);

      vertices.push_back(vec3(x, y, 0));
      //std::cout << vertices.back() << std::endl;
      colors.push_back(zxd::hsb2rgb(vec3(0, sat, bri)));
   }
  }

  update_buffer(vertices, colors);
}

//--------------------------------------------------------------------
void bug::draw()
{
  glEnable(GL_BLEND);
  glBlendColor(0, 0, 0, 0.03);
  glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
  glEnable(GL_PRIMITIVE_RESTART);
  glDisable(GL_CULL_FACE);
  glPrimitiveRestartIndex(-1);

  glBindVertexArray(m_vao);
  //glDrawArrays(GL_TRIANGLE_FAN, 0, m_wing_count * m_vertex_count);
  glDrawElements(GL_TRIANGLE_FAN, m_wing_count * (m_vertex_count + 1),
      GL_UNSIGNED_INT, BUFFER_OFFSET(0));
  //glDrawElementsBaseVertex(GL_TRIANGLE_FAN, 15,
      //GL_UNSIGNED_INT, BUFFER_OFFSET(0), 0);
}

//--------------------------------------------------------------------
void bug::update_buffer(const vec3_vector& vertices, const vec3_vector& color)
{
  GLuint pos_bytes = m_wing_count * m_vertex_count * sizeof(vec3);
  if(m_vao == -1)
  {
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, pos_bytes * 2, 0, GL_STREAM_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(pos_bytes));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    std::vector<GLuint> elements;
    elements.reserve(m_wing_count * (m_vertex_count + 1));

    for (int i = 0; i < m_wing_count; ++i )
    {
      for (int j = 0; j < m_vertex_count; ++j)
      {
        elements.push_back(j + i * m_vertex_count);
      }
      elements.push_back(-1);
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLuint),
        &elements.front(), GL_STATIC_DRAW);

  }

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, pos_bytes, glm::value_ptr(vertices.front()));
  glBufferSubData(GL_ARRAY_BUFFER, pos_bytes, pos_bytes, glm::value_ptr(color.front()));
}
}
