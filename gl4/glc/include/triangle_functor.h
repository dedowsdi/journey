#ifndef GL_GLC_TRIANGLE_FUNCTOR_H
#define GL_GLC_TRIANGLE_FUNCTOR_H

// changed from osg
#include <primitive_set.h>
#include <glm.h>

namespace zxd
{

template<class T>
class triangle_functor : public primitive_functor, public T
{
public:

  triangle_functor()
  {
    m_vertex_array_size=0;
    m_vertex_array_ptr=0;
    m_mode_cache=0;
    m_treat_vertex_data_as_temporary=false;
  }

  virtual ~triangle_functor() {}

  virtual void set_vertex_array(unsigned int count, const vec3* vertices)
  {
    m_vertex_array_size = count;
    m_vertex_array_ptr = vertices;
  }

  void setTreatVertexDataAsTemporary(bool treatVertexDataAsTemporary) { m_treat_vertex_data_as_temporary=treatVertexDataAsTemporary; }
  bool getTreatVertexDataAsTemporary() const { return m_treat_vertex_data_as_temporary; }

  virtual void drawArrays(GLenum mode,GLint first,GLsizei count)
  {
    //if (m_vertex_array_ptr==0 || count==0) return;
    if (count==0) return;

    switch(mode)
    {
      case(GL_TRIANGLES):
        {
          const vec3* vlast = &m_vertex_array_ptr[first+count];
          for(const vec3* vptr=&m_vertex_array_ptr[first];vptr<vlast;vptr+=3)
            this->operator()(*(vptr),*(vptr+1),*(vptr+2),m_treat_vertex_data_as_temporary);
          break;
        }
      case(GL_TRIANGLE_STRIP):
        {
          const vec3* vptr = &m_vertex_array_ptr[first];
          for(GLsizei i=2;i<count;++i,++vptr)
          {
            if ((i%2)) this->operator()(*(vptr),*(vptr+2),*(vptr+1),m_treat_vertex_data_as_temporary);
            else       this->operator()(*(vptr),*(vptr+1),*(vptr+2),m_treat_vertex_data_as_temporary);
          }
          break;
        }
        //case(GL_QUADS):
        //{
        //const vec3* vptr = &m_vertex_array_ptr[first];
        //for(GLsizei i=3;i<count;i+=4,vptr+=4)
        //{
        //this->operator()(*(vptr),*(vptr+1),*(vptr+2),m_treat_vertex_data_as_temporary);
        //this->operator()(*(vptr),*(vptr+2),*(vptr+3),m_treat_vertex_data_as_temporary);
        //}
        //break;
        //}
        //case(GL_QUAD_STRIP):
        //{
        //const vec3* vptr = &m_vertex_array_ptr[first];
        //for(GLsizei i=3;i<count;i+=2,vptr+=2)
        //{
        //this->operator()(*(vptr),*(vptr+1),*(vptr+2),m_treat_vertex_data_as_temporary);
        //this->operator()(*(vptr+1),*(vptr+3),*(vptr+2),m_treat_vertex_data_as_temporary);
        //}
        //break;
        //}
        //case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
      case(GL_TRIANGLE_FAN):
        {
          const vec3* vfirst = &m_vertex_array_ptr[first];
          const vec3* vptr = vfirst+1;
          for(GLsizei i=2;i<count;++i,++vptr)
          {
            this->operator()(*(vfirst),*(vptr),*(vptr+1),m_treat_vertex_data_as_temporary);
          }
          break;
        }
      case(GL_POINTS):
      case(GL_LINES):
      case(GL_LINE_STRIP):
      case(GL_LINE_LOOP):
      default:
        // can't be converted into to triangles.
        break;
    }
  }

  virtual void drawElements(GLenum mode,GLsizei count,const GLuint* indices)
  {
    if (indices==0 || count==0) return;

    typedef const GLuint* IndexPointer;

    switch(mode)
    {
      case(GL_TRIANGLES):
        {
          IndexPointer ilast = &indices[count];
          for(IndexPointer  iptr=indices;iptr<ilast;iptr+=3)
            this->operator()(m_vertex_array_ptr[*iptr],m_vertex_array_ptr[*(iptr+1)],m_vertex_array_ptr[*(iptr+2)],m_treat_vertex_data_as_temporary);
          break;
        }
      case(GL_TRIANGLE_STRIP):
        {
          IndexPointer iptr = indices;
          for(GLsizei i=2;i<count;++i,++iptr)
          {
            if ((i%2)) this->operator()(m_vertex_array_ptr[*(iptr)],m_vertex_array_ptr[*(iptr+2)],m_vertex_array_ptr[*(iptr+1)],m_treat_vertex_data_as_temporary);
            else       this->operator()(m_vertex_array_ptr[*(iptr)],m_vertex_array_ptr[*(iptr+1)],m_vertex_array_ptr[*(iptr+2)],m_treat_vertex_data_as_temporary);
          }
          break;
        }
      //case(GL_QUADS):
        //{
          //IndexPointer iptr = indices;
          //for(GLsizei i=3;i<count;i+=4,iptr+=4)
          //{
            //this->operator()(m_vertex_array_ptr[*(iptr)],m_vertex_array_ptr[*(iptr+1)],m_vertex_array_ptr[*(iptr+2)],m_treat_vertex_data_as_temporary);
            //this->operator()(m_vertex_array_ptr[*(iptr)],m_vertex_array_ptr[*(iptr+2)],m_vertex_array_ptr[*(iptr+3)],m_treat_vertex_data_as_temporary);
          //}
          //break;
        //}
      //case(GL_QUAD_STRIP):
        //{
          //IndexPointer iptr = indices;
          //for(GLsizei i=3;i<count;i+=2,iptr+=2)
          //{
            //this->operator()(m_vertex_array_ptr[*(iptr)],m_vertex_array_ptr[*(iptr+1)],m_vertex_array_ptr[*(iptr+2)],m_treat_vertex_data_as_temporary);
            //this->operator()(m_vertex_array_ptr[*(iptr+1)],m_vertex_array_ptr[*(iptr+3)],m_vertex_array_ptr[*(iptr+2)],m_treat_vertex_data_as_temporary);
          //}
          //break;
        //}
      //case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
      case(GL_TRIANGLE_FAN):
        {
          IndexPointer iptr = indices;
          const vec3& vfirst = m_vertex_array_ptr[*iptr];
          ++iptr;
          for(GLsizei i=2;i<count;++i,++iptr)
          {
            this->operator()(vfirst,m_vertex_array_ptr[*(iptr)],m_vertex_array_ptr[*(iptr+1)],m_treat_vertex_data_as_temporary);
          }
          break;
        }
      case(GL_POINTS):
      case(GL_LINES):
      case(GL_LINE_STRIP):
      case(GL_LINE_LOOP):
      default:
        // can't be converted into to triangles.
        break;
    }
  }

protected:

  unsigned int        m_vertex_array_size;
  const vec3*         m_vertex_array_ptr;

  GLenum              m_mode_cache;
};

}

#endif /* GL_GLC_TRIANGLE_FUNCTOR_H */
