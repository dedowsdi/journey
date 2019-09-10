#ifndef GL_GLC_CUBOID_H
#define GL_GLC_CUBOID_H

#include "geometry.h"
#include "glm.h"

namespace zxd
{

class cuboid : public common_geometry
{
public:

  enum class type
  {
    CT_8,
    CT_24
  };

  cuboid(const glm::vec3 &half_diag, type _type = type::CT_24);

  cuboid(GLfloat size = 1, type _type = type::CT_24);

  const glm::vec3& half_diag() const { return m_half_diag; }
  void half_diag(const glm::vec3& v){ m_half_diag = v; }

  type get_type() const { return m_type; }
  void set_type(type v){ m_type = v; }

private:

  vertex_build build_vertices() override;
  array_uptr build_normals(const array& vertices) override;
  array_uptr build_texcoords(const array& vertices) override;

  type m_type;
  glm::vec3 m_half_diag;

};

}

#endif /* GL_GLC_CUBOID_H */
