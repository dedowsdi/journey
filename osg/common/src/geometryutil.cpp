#include "geometryutil.h"

namespace geometryUtil
{
//--------------------------------------------------------------------
osg::Vec3Array* create_circle(GLfloat radius, GLuint slices,
const osg::Vec3& center/* = osg::Vec3()*/, const osg::Vec3& normal/* = glm::pza*/)
{
  osg::Vec3Array* vertices = new osg::Vec3Array;
  vertices->reserve(slices+1);
  GLfloat step_angle = glm::f2pi / slices;
  for (int i = 0; i < slices; ++i) {
    GLfloat angle = step_angle * i;
    vertices->push_back(osg::Vec3(radius* cos(angle), radius * sin(angle), 0));
  }
  vertices->push_back(vertices->front());

  return transform(vertices, osg::Matrix::rotate(glm::pza, normal) * osg::Matrix::translate(center));
}

//--------------------------------------------------------------------
osg::Vec3Array* extrude_along_line_strip(const osg::Vec3Array* vertices, GLfloat radius,
    GLuint num_faces/* = 6*/)
{
  if(vertices->size() < 2)
    throw std::runtime_error("you need at least 2 vertiices to extrude along");

  osg::Vec3Array* result = new osg::Vec3Array;
  result->reserve((num_faces + 1) * vertices->size());

  osg::ref_ptr<osg::Vec3Array> circle = create_circle(radius, num_faces);

  osg::ref_ptr<osg::Vec3Array> circle0 = transform(circle,
      osg::Matrix::rotate(glm::pza, vertices->at(1) - vertices->at(0)) *
      osg::Matrix::translate(vertices->front()));

  for (int i = 1; i < vertices->size(); ++i) 
  {
    osg::ref_ptr<osg::Vec3Array> circle1 = transform(circle, 
        osg::Matrix::rotate(glm::pza, vertices->at(i) - vertices->at(i-1)) * 
        osg::Matrix::translate(vertices->at(i)));

    for (int j = 0; j < circle0->size() - 1; ++j) {
      result->push_back(circle0->at(j));
      result->push_back(circle0->at(j+1));
      result->push_back(circle1->at(j+1));

      result->push_back(circle0->at(j));
      result->push_back(circle1->at(j+1));
      result->push_back(circle1->at(j));
    } 
    circle0 = circle1;
  }

  OSG_NOTICE << "finished extruding, generate " << result->size() << " vertices" << std::endl;
  return result;
}

//--------------------------------------------------------------------
osg::Vec3Array* transform(const osg::Vec3Array* vertices, const osg::Matrix& m)
{
  osg::Vec3Array* result = new osg::Vec3Array;
  result->reserve(vertices->size());

  for(auto& item : *vertices)
  {
    result->push_back(item * m);
  }
  return result;
}
}
