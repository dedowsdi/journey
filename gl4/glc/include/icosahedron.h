#ifndef GL4_GLC_ISOSAHEDRON_H
#define GL4_GLC_ISOSAHEDRON_H

#include "geometry.h"

namespace zxd
{

// http://www.songho.ca/opengl/gl_sphere.html
class icosahedron : public geometry_base
{
public:

  enum class mesh_type
  {
    MININUM,
    PAPER_UNWRAPPER
  };

  icosahedron(GLfloat radius = 1.0f, GLuint subdivisions = 0,
    mesh_type mt = mesh_type::MININUM);

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v; }

  GLuint subdivisions() const { return m_subdivisions; }
  void subdivisions(GLuint v){ m_subdivisions = v; }

  mesh_type get_mesh_type() const { return m_mesh_type; }
  void set_mesh_type(mesh_type v){ m_mesh_type = v; }

private:

  void build_vertex() override;
  void build_normal() override;
  void build_texcoord() override;

  void build_minimun_mesh();
  void build_paper_unwrapper_mesh();

  mesh_type m_mesh_type;
  GLfloat m_radius = 1;
  GLuint m_subdivisions = 1;
};

}
#endif /* GL4_GLC_ISOSAHEDRON_H */
