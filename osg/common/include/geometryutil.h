#include <osg/Vec3>
#include <osg/Geometry>
#include "glm.h"

namespace geometryUtil
{
  osg::Vec3Array* create_circle(GLfloat radius, GLuint slices,
      const osg::Vec3& center = osg::Vec3(), const osg::Vec3& normal = glm::pza);

  // no branch
  osg::Vec3Array* extrude_along_line_strip(const osg::Vec3Array* vertices, GLfloat radius,
      GLuint num_faces = 6);

  osg::Vec3Array* transform(const osg::Vec3Array* vertices, const osg::Matrix& m);
}
