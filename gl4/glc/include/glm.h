#ifndef GL_GLC_GLM_H
#define GL_GLC_GLM_H

#include <vector>

#include "gl.h"

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_META_PROG_HELPERS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/fwd.hpp>
#include <glm/ext/vector_uint1.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

// mat4x4.hpp includes too many files.
// #include <glm/mat4x4.hpp>

namespace zxd
{
// The only global using!!!.
using namespace glm;

// no static, const variable in namespace has internal linkage
const GLfloat fpi = 3.1415926535897932384626433832795;
const GLfloat f2pi = fpi * 2;
const GLfloat fpi2 = fpi / 2;
const GLfloat fpi4 = fpi / 4;
const GLfloat fpi8 = fpi / 8;
const GLfloat fpi16 = fpi / 16;
const GLfloat fpi32 = fpi / 32;
const GLdouble dpi = 3.1415926535897932384626433832795;
const GLfloat d2pi = dpi * 2;
const GLfloat dpi2 = dpi / 2;
const GLfloat dpi4 = dpi / 4;
const GLfloat dpi8 = dpi / 8;
const GLfloat dpi16 = dpi / 16;
const GLfloat dpi32 = dpi / 32;

const glm::vec3 pxa(1, 0, 0);
const glm::vec3 nxa(-1, 0, 0);
const glm::vec3 pya(0, 1, 0);
const glm::vec3 nya(0, -1, 0);
const glm::vec3 pza(0, 0, 1);
const glm::vec3 nza(0, 0, -1);
const glm::vec4 hpxa(1, 0, 0, 1);
const glm::vec4 hnxa(-1, 0, 0, 1);
const glm::vec4 hpya(0, 1, 0, 1);
const glm::vec4 hnya(0, -1, 0, 1);
const glm::vec4 hpza(0, 0, 1, 1);
const glm::vec4 hnza(0, 0, -1, 1);
const glm::vec3 zp(0, 0, 0);
const glm::vec4 hzp(0, 0, 0, 1);

using mat2_vector = std::vector<mat2>;
using mat3_vector = std::vector<mat3>;
using mat4_vector = std::vector<mat4>;
using int_vector = std::vector<GLint>;
using uint_vector = std::vector<GLuint>;
using short_vector = std::vector<GLshort>;
using ushort_vector = std::vector<GLushort>;
using float_vector = std::vector<GLfloat>;
using uvec1_vector = std::vector<uvec1>;
using vec1_vector = std::vector<vec1>;
using vec2_vector = std::vector<vec2>;
using vec3_vector = std::vector<vec3>;
using vec4_vector = std::vector<vec4>;
using uvec3_vector = std::vector<uvec3>;

using float_vector2 = std::vector<std::vector<GLfloat> >;
using vec2_vector2 = std::vector<std::vector<vec2> >;
using vec3_vector2 = std::vector<std::vector<vec3> >;
using vec4_vector2 = std::vector<std::vector<vec4> >;

using fv_i = float_vector::iterator;
using v2v_i = vec2_vector::iterator;
using v3v_i = vec3_vector::iterator;
using v4v_i = vec4_vector::iterator;
using fv_ci = float_vector::const_iterator;
using v2v_ci = vec2_vector::const_iterator;
using v3v_ci = vec3_vector::const_iterator;
using v4v_ci = vec4_vector::const_iterator;

}

#endif /* GL_GLC_GLM_H */
