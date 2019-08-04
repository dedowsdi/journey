#include "bezier.h"
#include "glmath.h"

namespace zxd
{

//--------------------------------------------------------------------
void bezier::build_vertex()
{
  vec3_array& vertices = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&vertices));
  vertices.reserve(m_partitions + 1);

  float dt = (m_end - m_begin) / m_partitions;
  for (uint i = 0; i <= m_partitions; ++i)
  {
    vertices.push_back(get(m_begin + dt * i));
  }

  m_primitive_sets.clear();
  add_primitive_set(new draw_arrays(GL_LINE_STRIP, 0, num_vertices()));
}

//--------------------------------------------------------------------
void bezier::build_texcoord()
{
  float_array& texcoords = *(new float_array());
  attrib_array(num_arrays(), array_ptr(&texcoords));
  texcoords.reserve(num_vertices());
  for (int i = 0; i < num_vertices(); ++i)
  {
    texcoords.push_back(static_cast<GLfloat>(i) / m_partitions);
  }
}

//--------------------------------------------------------------------
glm::vec3 bezier::get(GLfloat t)
{
  return get(m_ctrl_points.begin(), m_ctrl_points.end(), t);
}

//--------------------------------------------------------------------
glm::vec3 bezier::get(GLuint i, GLuint j, GLfloat t)
{
  // iteration of points will be an equilateral
  GLuint n = this->n();
  glm::vec3 p(0);

  if (i > n)
  {
    std::cerr << "iteration over flow" << std::endl;
    return p;
  }
  if (i + j > n)
  {
    std::cerr << "index over flow" << std::endl;
    return p;
  }

  return get(m_ctrl_points.begin() + j, m_ctrl_points.begin() + j + i + 1, t);
}

//--------------------------------------------------------------------
void bezier::elevate(bool positive /* = true*/)
{
  GLuint n = this->n();
  if (!positive && n <= 1) return;

  vec3_vector points;

  points.push_back(m_ctrl_points.front());

  if (positive)
  {
    GLuint r = n + 1;
    vec3_array& points = *(new vec3_array());
    attrib_array(num_arrays(), array_ptr(&points));
    points.reserve(r + 1);
    GLfloat rcp_nplus1 = 1.0 / r;

    for (unsigned int i = 1; i < r; ++i)
    {
      points.push_back(m_ctrl_points[i - 1] * (i * rcp_nplus1) +
                       (m_ctrl_points[i] * (1 - i * rcp_nplus1)));
    }
  } else
  {
    // degree drop, unstable. Graph might be changed as lower degree can't
    // entirely represent higher degree bezier
    GLuint r = n - 1;
    vec3_array& points = *(new vec3_array());
    attrib_array(num_arrays(), array_ptr(&points));
    points.reserve(r + 1);
    GLfloat rcpN = 1.0 / n;

    if (r >= 2)
    {
      points.push_back((m_ctrl_points[1] - points.front() * rcpN) / (1 - rcpN));

      for (unsigned int i = 2; i < r; ++i)
      {
        points.push_back(
          (m_ctrl_points[i] + m_ctrl_points[i - 1] -
            points[i - 2] * ((i - 1) * rcpN) - points[i - 1] * (1 + rcpN)) /
          (1 - i * rcpN));
      }
    }
  }

  points.push_back(m_ctrl_points.back());
  m_ctrl_points = points;
}

//--------------------------------------------------------------------
bezier bezier::derivative(GLuint level /* = 1*/)
{
  bezier curve(*this);

  if (level == 0) return curve;
  if (level >= n())
  {
    std::cerr << "too high level of derivative" << std::endl;
    return curve;
  }
  vec3_vector points;

  GLuint k = n() - level;
  points.reserve(k + 1);

  GLfloat c = pi(n() - k + 1, 1, k);

  for (GLuint i = 0; i < k; ++i)
  {
    points.push_back(d(i, k) * c);
  }

  curve.ctrl_points(points);
  return curve;
}

//--------------------------------------------------------------------
glm::vec3 bezier::tangent(GLfloat t)
{
  return tangent(m_ctrl_points.begin(), m_ctrl_points.end(), t);
}

//--------------------------------------------------------------------
void bezier::subdivide(GLfloat t, bezier& lc, bezier& rc)
{
  vec3_vector lp = lc.ctrl_points();
  vec3_vector rp = rc.ctrl_points();
  GLuint n = this->n();
  lp.reserve(n + 1);
  rp.reserve(n + 1);

  // for (uint i = 0; i < lp.capacity(); ++i)
  // {
  // lp.push_back(get(i, 0, t));
  // rp.push_back(get(i, n - i, t));
  //}

  // get all iterations is faster than use get one by one
  vec3_vector2 vv = iterate_all(m_ctrl_points, t);
  for (uint i = 0; i < lp.capacity(); ++i)
  {
    lp.push_back(vv[i][0]);
    rp.push_back(vv[i][n - i]);
  }
}

//--------------------------------------------------------------------
vec3_vector2 bezier::iterate_all(vec3_vector& ctrl_points, float t)
{
  return iterate_all(ctrl_points.begin(), ctrl_points.end(), t);
}

//--------------------------------------------------------------------
vec3_vector2 bezier::iterate_all(
  vec3_vector::const_iterator beg, vec3_vector::const_iterator end, float t)
{
  vec3_vector2 vv;

  vec3_vector va(beg, end);
  vv.push_back(va);
  while (va.size() > 1)
  {
    va = iterate(va, t);
    vv.push_back(va);
  }
  return vv;
}

//--------------------------------------------------------------------
vec3_vector bezier::iterate(vec3_vector& ctrl_points, float t)
{
  return iterate(ctrl_points.begin(), ctrl_points.end(), t);
}

//--------------------------------------------------------------------
vec3_vector bezier::iterate(
  vec3_vector::const_iterator beg, vec3_vector::const_iterator end, float t)
{
  vec3_vector vertices;

  if (beg == end)
  {
    std::cerr << "not enough control points : " << std::endl;
    return vertices;
  }

  if (std::distance(beg, end) == 1)
  {
    vertices.push_back(*beg);
    return vertices;
  }

  GLuint order = std::distance(beg, end);

  t = glm::clamp(t, 0.0f, 1.0f);
  vertices.reserve(order - 1);
  vec3_vector::const_iterator end_minus1 = end - 1;

  for (vec3_vector::const_iterator iter = beg; iter != end_minus1; ++iter)
  {
    vertices.push_back(*iter * (1 - t) + *(iter + 1) * t);
  }

  return vertices;
}

//--------------------------------------------------------------------
glm::vec3 bezier::get(
  vec3_vector::const_iterator beg, vec3_vector::const_iterator end, GLfloat t)
{
  if (beg == end) return vec3(0);

  t = std::max(std::min(t, 1.0f), 0.0f);

  GLuint p = end - beg - 1;
  float oneMinusT = 1 - t;

  // get by definition, binomial might cause problem as degree gets too bigger
  // for (uint i = 0; i <= degree; ++i)
  // {
  // p += *(beg + i) * ((std::pow(oneMinusT, degree - i) * std::pow(t, i) *
  // zxd::Math::binomial(degree, i)));
  //}

  // get by de Casteljau's algorithm
  vec3_vector points(beg, end);

  for (unsigned int i = 0; i < p; ++i)
  {
    for (unsigned int j = 0; j < p - i; ++j)
    {
      points[j] = points[j] * oneMinusT + points[j + 1] * t;
    }
  }

  return points.front();
}

//--------------------------------------------------------------------
glm::vec3 bezier::tangent(
  vec3_vector::const_iterator beg, vec3_vector::const_iterator end, GLfloat t)
{
  vec3_vector2 vv = iterate_all(beg, end, t);
  GLuint s = vv.size();

  glm::vec3 v = vv[s - 2][1] - vv[s - 2][0];
  return glm::normalize(v);
}


//--------------------------------------------------------------------
glm::vec3 bezier::d(GLuint i, GLuint k)
{
  if (k == 0)
    return m_ctrl_points[i + 1] - m_ctrl_points[i];
  else
    return d(i + 1, k - 1) - d(i, k - 1);
}
}
