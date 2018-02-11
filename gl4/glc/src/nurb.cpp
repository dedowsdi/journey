#include "nurb.h"

namespace zxd {

//--------------------------------------------------------------------
void nurb::build_vertex() {
  if (!valid()) {
    std::cout << "invalid b spline. n : " << n() << ", p : " << m_degree
              << ", m : " << m() << std::endl;
    return;
  }

  m_vertices.clear();
  m_vertices.reserve(m_partitions + 1);

  float dt = (m_end - m_begin) / m_partitions;
  for (uint i = 0; i <= m_partitions; ++i) {
    m_vertices.push_back(get(m_begin + dt * i));
  }
}

//--------------------------------------------------------------------
void nurb::build_texcoord() {
  m_texcoords.clear();
  m_texcoords.reserve(m_vertices.size());
  for (int i = 0; i < m_vertices.size(); ++i) {
    m_texcoords.push_back(static_cast<GLfloat>(i) / m_partitions);
  }
}

//--------------------------------------------------------------------
void nurb::draw(GLuint primcount) {
  bind_vertex_array_object();
  draw_arrays(GL_LINE_STRIP, 0, m_vertices.size(), primcount);
}

//--------------------------------------------------------------------
vec4 nurb::get(GLfloat u) {
  u = std::max(std::min(u, m_knots.back()), m_knots.front());
  // early check
  if (u == m_knots.front())
    return m_ctrl_points.front();
  else if (u == m_knots.back())
    return m_ctrl_points.back();

  GLuint k = knot_span(u);                                   // knot span
  GLuint s = u == m_knots.at(k) ? knot_multiplicity(k) : 0;  // multiplicity
  GLuint p = m_degree;
  // GLuint pMinusS = p - s;
  GLuint kMinusS = k - s;
  GLuint kMinusP = k - p;

  if (s >= p) {
    // when s>=p it's just  P_k,  P_k-1 P_k-2......P_k-p+1, P_k-p,   P_k-p
    return m_ctrl_points.at(k - p);
  } else {
    // use de Boor's algoritm to create a knot of multiplicity p to get point

    // init columns 0
    vec4_vector points(
      m_ctrl_points.begin() + kMinusP, m_ctrl_points.begin() + (kMinusS + 1));
    GLuint h = points.size();
    // iterate p-s times. leave only 1 element at last column
    for (unsigned int r = 1; r < h; ++r) {
      for (unsigned int i = 0; i < h - r; ++i) {
        GLfloat a = knot_ratio(u, kMinusP + i + r, r);
        points.at(i) = points.at(i) * (1 - a) + points.at(i + 1) * a;
      }
    }
    return points.at(0);
  }
}

//--------------------------------------------------------------------
vec4 nurb::get_by_coefs(GLfloat u) {
  float_vector coefs = coefficents(u);
  vec4 p;
  for (unsigned int i = 0; i < m_ctrl_points.size(); ++i) {
    p += m_ctrl_points.at(i) * coefs.at(i);
  }
  return p;
}

//--------------------------------------------------------------------
void nurb::insert_knot(GLfloat u, GLuint times /* = 1*/) {
  vec4_vector points;

  GLuint k = knot_span(u);                                   // knot span
  GLuint s = u == m_knots.at(k) ? knot_multiplicity(k) : 0;  // multiplicity
  GLuint p = m_degree;
  GLuint kMinusP = k - p;
  // GLuint pMinusS = p - s;
  GLuint kMinusS = k - s;

  if (s >= p) {
    // most simple case, just repeat P_k_p multiple times
    vec4_vector::iterator iter = points.begin() + kMinusP;
    points.insert(iter, times, *iter);
  } else {
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
    if (s + times >= p) {
      vec4_vector::iterator iter = points.begin() + kMinusP;
      points.insert(iter, times - (p - s), *iter);
    }

    m_ctrl_points.assign(points.begin(), points.end());
  }

  m_knots.insert(m_knots.begin() + k, times, u);
}

//--------------------------------------------------------------------
GLuint nurb::knot_span(GLfloat u) {
  // special case. Is it right to put um at the last valid span [u_m-p-1, u_m)
  // ????
  if (u == m_knots.back()) return m_knots.size() - m_degree - 2;

  float_vector::iterator iter =
    std::upper_bound(m_knots.begin(), m_knots.end(), u);
  if (iter == m_knots.end()) {
    std::cerr << "illigal u" << std::endl;
  }
  return iter == m_knots.end() ? -1 : (iter - m_knots.begin()) - 1;
}

//--------------------------------------------------------------------
GLuint nurb::knot_multiplicity(GLuint i) {
  GLuint s = 1;
  int i2 = i;
  GLfloat u = m_knots.at(i);
  GLuint size = m_knots.size();

  while (++i < size && m_knots.at(i) == u) ++s;
  while (--i2 >= 0 && m_knots.at(i2) == u) ++s;

  return s;
}

//--------------------------------------------------------------------
void nurb::subdivide(GLfloat u, nurb& lc, nurb& rc) {
  GLuint k = knot_span(u);                                   // knot span
  GLuint s = u == m_knots.at(k) ? knot_multiplicity(k) : 0;  // multiplicity
  GLuint p = m_degree;
  GLuint kMinusP = k - p;
  // GLuint pMinusS = p - s;
  GLuint kMinusS = k - s;

  std::vector<vec4_vector> vec = iterate(kMinusP, kMinusS + 1, u, p - s);
  if (vec.back().size() != 1) {
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

  points.clear();
  knots.clear();

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
  GLuint beg, GLuint end, GLfloat u, GLuint times) {
  std::vector<vec4_vector> vv;

  // init columns 0
  vv.push_back(vec4_vector());
  vv[0].assign(m_ctrl_points.begin() + beg, m_ctrl_points.begin() + end);

  GLuint size = end - beg;

  for (GLuint r = 1; r <= times; ++r) {
    // init current column
    vv.push_back(vec4_vector());

    for (unsigned int i = 0; i < size - r; ++i) {
      GLfloat a = knot_ratio(u, beg + i + r, r);
      vec4 p = vv[r - 1].at(i) * (1 - a) + vv[r - 1].at(i + 1) * a;
      vv[r].push_back(p);
    }
  }
  return vv;
}

//--------------------------------------------------------------------
void nurb::uniform_knots() {
  GLuint m = n() + p() + 1;

  m_knots.clear();
  m_knots.reserve(m + 1);
  GLuint s = p() + 1;
  GLint c = m + 1 - 2 * s;

  for (GLuint i = 0; i < s; ++i) m_knots.push_back(0);
  for (GLuint i = 0; i < c; ++i) m_knots.push_back((i + 1) / (c + 1));
  for (GLuint i = 0; i < s; ++i) m_knots.push_back(1);
}

//--------------------------------------------------------------------
float_vector nurb::coefficents(GLfloat u) {
  float_vector coefs;

  // init to 0
  coefs.insert(coefs.end(), m_degree + 1, 0);

  // only at most p+1 non zero basis function exists in this span, our job is
  // to find them, only them, others are just 0;

  // rule out special case when k belong to [0,p] or [m-p, m]
  if (u == m_knots.front()) {
    coefs[0] = 1;
    return coefs;
  } else if (u == m_knots.back()) {
    coefs[n()] = 1;
    return coefs;
  }

  GLuint p = this->p();
  GLuint k = knot_span(u);

  coefs[k] = 0;  // degree 0 coefficients
  GLuint kplus1 = k + 1;
  GLfloat uk = m_knots.at(k);
  GLfloat ukplus1 = m_knots.at(kplus1);
  GLfloat ukplus1MinusU = ukplus1 - u;
  GLfloat uMinusUk = u - uk;

  // loop from degree 1 to degree p
  for (unsigned int d = 1; d <= p; ++d) {
    // right (south-west corner) term only
    coefs[k - d] =
      ukplus1MinusU * coefs[k - d + 1] / (ukplus1 - m_knots.at(k - d + 1));

    // both temes
    for (unsigned int i = k - d + 1; i < k - 1; ++i) {
      GLfloat ui = m_knots.at(i);
      GLfloat uiplusd = m_knots.at(i + d);
      GLfloat uiplus1 = m_knots.at(i + 1);
      GLfloat uiplusdplus1 = m_knots.at(i + d + 1);

      coefs[i] = (u - ui) * coefs[i] / (uiplusd - ui) +
                 (uiplusdplus1 - u) * coefs[i + 1] / (uiplusdplus1 - uiplus1);
    }

    // left (north-west corner) term only
    coefs[k] = uMinusUk * coefs[k] / (m_knots.at(k + d) - k);
  }
  // according to partion of unity
  // GLfloat c = 0;
  // std::for_each(coefs.begin() + (k - p), coefs.begin() + k,
  //[&](decltype(*coefs.begin()) v) {
  // c+= v;
  //});
  // coefs[k] = 1 - c;
  return coefs;
}

//--------------------------------------------------------------------
GLfloat nurb::knot_ratio(GLfloat t, GLuint i, GLuint r /* = 1*/) {
  return (t - m_knots.at(i)) /
         (m_knots.at(i + m_degree - r + 1) - m_knots.at(i));
}
}
