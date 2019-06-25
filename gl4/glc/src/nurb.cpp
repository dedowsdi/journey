#include "nurb.h"

namespace zxd
{

//--------------------------------------------------------------------
void nurb::build_vertex()
{
  if (!valid())
  {
    std::cout << "invalid b spline. n : " << n() << ", p : " << m_degree
              << ", m : " << m() << std::endl;
  }

  vec4_array& vertices = *(new vec4_array());
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
void nurb::build_texcoord()
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
vec4 nurb::get(GLfloat u)
{
  u = std::max(std::min(u, m_knots.back()), m_knots.front());
  // early check
  if (u == m_knots.front())
    return m_ctrl_points.front();
  else if (u == m_knots.back())
    return m_ctrl_points.back();

  GLuint k = knot_span(u);                                // knot span
  GLuint s = u == m_knots[k] ? knot_multiplicity(k) : 0;  // multiplicity
  GLuint p = m_degree;
  // GLuint pMinusS = p - s;
  GLuint kMinusS = k - s;
  GLuint kMinusP = k - p;

  if (s >= p)
  {
    // when s>=p it's just  P_k,  P_k-1 P_k-2......P_k-p+1, P_k-p,   P_k-p
    return m_ctrl_points[k - p];
  } else
  {
    // use de Boor's algoritm to create a knot of multiplicity p to get point

    // init columns 0
    vec4_vector points(
      m_ctrl_points.begin() + kMinusP, m_ctrl_points.begin() + (kMinusS + 1));
    GLuint h = points.size() - 1;  // p-s, number of insertions
    // iterate p-s times. leave only 1 element at last column
    for (unsigned int r = 1; r <= h; ++r)
    {
      // col size is (k-s) - (k-p + r) + 1 = p - s - r + 1 = h - r + 1
      for (unsigned int i = 0; i < h - r + 1; ++i)
      {
        GLfloat a = knot_ratio(u, kMinusP + i + r, r);
        points[i] = points[i] * (1 - a) + points[i + 1] * a;
      }
    }
    return points[0];
  }
}

//--------------------------------------------------------------------
vec3 nurb::tangent(GLfloat u)
{
  nurb nurb_d = derivative();
  return normalize(nurb_d.get(u).xyz());

  // failed to use de Boos's algorithm to caucluate tangent.
  u = std::max(std::min(u, m_knots.back()), m_knots.front());
  // early check
  if (u == m_knots.front())
  {
    const vec4& p0 = m_ctrl_points[0];
    const vec4& p1 = m_ctrl_points[1];
    return normalize(p1.xyz() / p1.w - p0.xyz() / p0.w);
  } else if (u == m_knots.back())
  {
    const vec4& p0 = m_ctrl_points[m_ctrl_points.size() - 2];
    const vec4& p1 = m_ctrl_points.back();
    return normalize(p1.xyz() / p1.w - p0.xyz() / p0.w);
  }

  GLuint k = knot_span(u);                                // knot span
  GLuint s = u == m_knots[k] ? knot_multiplicity(k) : 0;  // multiplicity
  GLuint p = m_degree;
  GLuint kMinusS = k - s;
  GLuint kMinusP = k - p;
  // use de Boor's algoritm to get 2points of last second iteration

  // init columns 0
  vec4_vector points(
    m_ctrl_points.begin() + kMinusP, m_ctrl_points.begin() + (kMinusS + 1));
  GLuint h = points.size() - 2;  // p - s - 1

  // iterate p-s-1 times. leave only 2 element at last column
  for (unsigned int r = 1; r <= h; ++r)
  {
    for (unsigned int i = 0; i < h - r + 1; ++i)
    {
      GLfloat a = knot_ratio(u, kMinusP + i + r, r);
      points[i] = points[i] * (1 - a) + points[i + 1] * a;
    }
  }

  return normalize(
    points[1].xyz() / points[1].w - points[0].xyz() / points[0].w);
}

//--------------------------------------------------------------------
nurb nurb::derivative()
{
  nurb curve;
  curve.knots(m_knots.begin() + 1, m_knots.end() - 1);

  vec4_vector q;
  q.reserve(n());
  for (int i = 0; i < n(); ++i)
  {
    q.push_back(p() / (m_knots[i + p() + 1] - m_knots[i + 1]) *
                (m_ctrl_points[i + 1] - m_ctrl_points[i]));
  }
  curve.ctrl_points(q);
  curve.degree(p() - 1);

  return curve;
}

//--------------------------------------------------------------------
vec4 nurb::get_by_coefs(GLfloat u)
{
  float_vector coefs = coefficients(u);
  vec4 p;
  for (unsigned int i = 0; i < m_ctrl_points.size(); ++i)
  {
    p += m_ctrl_points[i] * coefs[i];
  }
  return p;
}

//--------------------------------------------------------------------
void nurb::insert_knot(GLfloat u, GLuint times /* = 1*/)
{
  vec4_vector points;

  GLuint k = knot_span(u);                                // knot span
  GLuint s = u == m_knots[k] ? knot_multiplicity(k) : 0;  // multiplicity
  GLuint p = m_degree;
  GLuint kMinusP = k - p;
  // GLuint pMinusS = p - s;
  GLuint kMinusS = k - s;

  if (s >= p)
  {
    // most simple case, just repeat P_k_p multiple times
    vec4_vector::iterator iter = points.begin() + kMinusP;
    points.insert(iter, times, *iter);
  } else
  {
    // seperate insert to two partes if s + times is greater than p
    GLuint h = s + times >= p ? p - s : times;
    GLuint beg = k - p;
    GLuint end = k - s + 1;

    // iterate related points to generate new points
    std::vector<vec4_vector> vec = iterate(beg, end, u, h);

    // construct new control points by 5 parts
    // part 1, original [0,k-p]
    points.insert(
      points.end(), m_ctrl_points.begin(), m_ctrl_points.begin() + kMinusP + 1);

    // part 2, upper edge of point iterations
    for (unsigned int i = 1; i < vec.size() - 1; ++i)
      points.push_back(vec[i].front());

    // part 3, right edge of point iterations
    points.insert(points.end(), vec.back().begin(), vec.back().end());

    // part 4, down edge of point iterations
    for (unsigned int i = vec.size() - 2; i >= 1; --i)
      points.push_back(vec[i].back());

    // part 5, original [k-s,n]
    points.insert(
      points.end(), m_ctrl_points.begin() + kMinusS, m_ctrl_points.end());

    // second part of this condition
    if (s + times >= p)
    {
      vec4_vector::iterator iter = points.begin() + kMinusP;
      points.insert(iter, times - (p - s), *iter);
    }

    m_ctrl_points.assign(points.begin(), points.end());
  }

  m_knots.insert(m_knots.begin() + k, times, u);
}

//--------------------------------------------------------------------
GLuint nurb::knot_span(GLfloat u)
{
  // special case. Is it right to put um at the last valid span [u_m-p-1, u_m)
  // ????
  if (u == m_knots.back()) return m_knots.size() - m_degree - 2;

  float_vector::iterator iter =
    std::upper_bound(m_knots.begin(), m_knots.end(), u);
  if (iter == m_knots.end())
  {
    std::cerr << "illigal u" << std::endl;
  }
  return iter == m_knots.end() ? -1 : (iter - m_knots.begin()) - 1;
}

//--------------------------------------------------------------------
GLuint nurb::knot_multiplicity(GLuint i)
{
  GLuint s = 1;
  int i2 = i;
  GLfloat u = m_knots[i];
  GLuint size = m_knots.size();

  while (++i < size && m_knots[i] == u) ++s;
  while (--i2 >= 0 && m_knots[i2] == u) ++s;

  return s;
}

//--------------------------------------------------------------------
void nurb::subdivide(GLfloat u, nurb& lc, nurb& rc)
{
  GLuint k = knot_span(u);                                // knot span
  GLuint s = u == m_knots[k] ? knot_multiplicity(k) : 0;  // multiplicity
  GLuint p = m_degree;
  GLuint kMinusP = k - p;
  // GLuint pMinusS = p - s;
  GLuint kMinusS = k - s;

  std::vector<vec4_vector> vec = iterate(kMinusP, kMinusS + 1, u, p - s);
  if (vec.back().size() != 1)
  {
    std::cout << "error, not enough iterations!" << std::endl;
    return;
  }

  vec4_vector points;
  float_vector knots;

  // part 1, original [0,k-p]
  points.insert(
    points.end(), m_ctrl_points.begin(), m_ctrl_points.begin() + kMinusP + 1);

  // part 2, upper edge of point iterations
  for (unsigned int i = 1; i < vec.size(); ++i)
    points.push_back(vec[i].front());

  knots.assign(m_knots.begin(), m_knots.begin() + (k - s + 1));
  knots.insert(knots.end(), p + 1, u);

  lc.ctrl_points(points);
  lc.knots(knots);


  // part 1, down edge of point iterations
  for (int i = vec.size() - 1; i > 0; --i) points.push_back(vec[i].back());

  // part 2, original [k-s,n]
  points.insert(
    points.end(), m_ctrl_points.begin() + kMinusS, m_ctrl_points.end());
  knots.insert(knots.end(), p + 1, u);
  knots.insert(knots.end(), m_knots.begin() + k + 1, m_knots.end());

  rc.ctrl_points(points);
  rc.knots(knots);
}

//--------------------------------------------------------------------
std::vector<vec4_vector> nurb::iterate(
  GLuint beg, GLuint end, GLfloat u, GLuint times)
{
  std::vector<vec4_vector> vv;

  // init columns 0
  vv.push_back(vec4_vector());
  vv[0].assign(m_ctrl_points.begin() + beg, m_ctrl_points.begin() + end);

  GLuint size = end - beg;

  for (GLuint r = 1; r <= times; ++r)
  {
    // init current column
    vv.push_back(vec4_vector());

    for (unsigned int i = 0; i < size - r; ++i)
    {
      GLfloat a = knot_ratio(u, beg + i + r, r);
      vec4 p = vv[r - 1][i] * (1 - a) + vv[r - 1][i + 1] * a;
      vv[r].push_back(p);
    }
  }
  return vv;
}

//--------------------------------------------------------------------
void nurb::uniform_knots()
{
  GLuint m = n() + p() + 1;

  m_knots.reserve(m + 1);
  GLuint s = p() + 1;
  GLint c = m + 1 - 2 * s;

  for (GLuint i = 0; i < s; ++i) m_knots.push_back(0);
  for (GLuint i = 0; i < c; ++i)
    m_knots.push_back(static_cast<GLfloat>((i + 1)) / (c + 1));
  for (GLuint i = 0; i < s; ++i) m_knots.push_back(1);
}

//--------------------------------------------------------------------
GLfloat nurb::coefficient(GLint i, GLint p, GLfloat u)
{
  return coefficient(m_knots.begin(), m_knots.end(), i, p, u);
}

//--------------------------------------------------------------------
vec4 nurb::get(
  v4v_ci cbeg, v4v_ci cend, fv_ci kbeg, fv_ci kend, GLuint degree, GLfloat u)
{
  nurb curve(cbeg, cend, kbeg, kend, degree);
  return curve.get(u);
}

//--------------------------------------------------------------------
vec3 nurb::tangent(
  v4v_ci cbeg, v4v_ci cend, fv_ci kbeg, fv_ci kend, GLuint degree, GLfloat u)
{
  nurb curve(cbeg, cend, kbeg, kend, degree);
  return curve.tangent(u);
}

//--------------------------------------------------------------------
float nurb::coefficient(fv_ci kbeg, fv_ci kend, GLuint i, GLuint p, GLfloat u)
{
  if (i + p + 1 >= std::distance(kbeg, kend))
  {
    std::cerr << "knot index " << i << " overflow" << std::endl;
    return 0;
  }
  GLfloat ui = *(kbeg + i);
  GLfloat uiplus1 = *(kbeg + i + 1);
  GLfloat uiplusp = *(kbeg + i + p);
  GLfloat uipluspplus1 = *(kbeg + i + p + 1);

  return (u - ui) / (uiplusp - ui) * coefficient(kbeg, kend, i, p - 1, u) +
         (uipluspplus1 - u) / (uipluspplus1 - uiplus1) *
           coefficient(kbeg, kend, i + 1, p - 1, u);
}

//--------------------------------------------------------------------
float_vector nurb::coefficients(fv_ci kbeg, fv_ci kend, GLuint p, GLfloat u)
{
  GLuint m = std::distance(kbeg, kend) - 1;
  GLuint n = m - p - 1;
  GLfloat u0 = *kbeg;
  GLfloat um = *(kend - 1);

  float_vector coefs;
  // init to 0
  coefs.resize(n + 1, 0);

  // only at most p+1 non zero basis function exists in this span, our job is
  // to find them, only them, others are just 0;
  // rule out special case when k belong to [0,p] or [m-p, m]
  if (u == u0)
  {
    coefs.front() = 1;
    return coefs;
  } else if (u == um)
  {
    coefs.back() = 1;
    return coefs;
  }

  GLuint k = nurb::knot_span(kbeg, kend, p, u);

  coefs[k] = 0;  // degree 0 coefficients
  GLuint kplus1 = k + 1;
  GLfloat uk = *(kbeg + k);
  GLfloat ukplus1 = *(kbeg + kplus1);
  GLfloat ukplus1_minusu = ukplus1 - u;
  GLfloat uminusuk = u - uk;

  // loop from degree 1 to degree p
  for (unsigned int d = 1; d <= p; ++d)
  {
    // right (south-west corner) term only
    coefs[k - d] =
      ukplus1_minusu * coefs[k - d + 1] / (ukplus1 - *(kbeg + k - d + 1));

    // both terms
    for (unsigned int i = k - d + 1; i < k - 1; ++i)
    {
      GLfloat ui = *(kbeg + i);
      GLfloat uiplusd = *(kbeg + i + d);
      GLfloat uiplus1 = *(kbeg + i + 1);
      GLfloat uiplusdplus1 = *(kbeg + i + d + 1);

      coefs[i] = (u - ui) * coefs[i] / (uiplusd - ui) +
                 (uiplusdplus1 - u) * coefs[i + 1] / (uiplusdplus1 - uiplus1);
    }

    // left (north-west corner) term only
    coefs[k] = uminusuk * coefs[k] / (*(kbeg + k + d) - k);
  }
  // according to partion of unity
  // GLfloat c = 0;
  // std::for_each(coefs.begin() + (k - p), coefs.begin() + k,
  //[&](decltype(*coefs.begin()) v)
  //{
  // c+= v;
  //});
  // coefs[k] = 1 - c;
  return coefs;
}

//--------------------------------------------------------------------
GLuint nurb::knot_span(fv_ci kbeg, fv_ci kend, GLuint p, GLfloat u)
{
  GLuint m = std::distance(kbeg, kend) - 1;
  // special case. Is it right to put um at the last valid span [u_m-p-1, u_m)
  // ????
  if (u == *(kend - 1)) return m - p - 1;

  fv_ci iter = std::upper_bound(kbeg, kend, u);
  if (iter == kend)
  {
    std::cerr << "illigal u" << std::endl;
  }
  return iter == kend ? -1 : (iter - kbeg) - 1;
}

//--------------------------------------------------------------------
float_vector nurb::coefficients(GLfloat u)
{
  return coefficients(m_knots.begin(), m_knots.end(), p(), u);
}

//--------------------------------------------------------------------
GLfloat nurb::knot_ratio(GLfloat t, GLuint i, GLuint r /* = 1*/)
{
  return (t - m_knots[i]) / (m_knots[i + m_degree - r + 1] - m_knots[i]);
}
}
