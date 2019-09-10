#ifndef GL4_GLC_GLM_TRAITS_H
#define GL4_GLC_GLM_TRAITS_H

#include <glm.h>

namespace zxd
{

template <typename T> constexpr GLenum glm_type_enum = GL_FLOAT;
template<> constexpr GLenum glm_type_enum<GLchar> = GL_BYTE;
template<> constexpr GLenum glm_type_enum<GLubyte> = GL_UNSIGNED_BYTE;
template<> constexpr GLenum glm_type_enum<GLshort> = GL_SHORT;
template<> constexpr GLenum glm_type_enum<GLushort> = GL_UNSIGNED_SHORT;
template<> constexpr GLenum glm_type_enum<GLint> = GL_INT;
template<> constexpr GLenum glm_type_enum<GLuint> = GL_UNSIGNED_INT;
template<> constexpr GLenum glm_type_enum<GLdouble> = GL_DOUBLE;

template <length_t L, typename T, qualifier Q>
constexpr GLenum glm_type_enum<vec<L, T, Q>> = glm_type_enum<T>;

template <GLenum T> struct glm_enum_type { using type = float; };
template<> struct glm_enum_type<GL_BYTE> { using type = GLchar; };
template<> struct glm_enum_type<GL_UNSIGNED_BYTE> { using type = GLubyte; };
template<> struct glm_enum_type<GL_SHORT> { using type = GLshort; };
template<> struct glm_enum_type<GL_UNSIGNED_SHORT> { using type = GLushort; };
template<> struct glm_enum_type<GL_INT> { using type = GLint; };
template<> struct glm_enum_type<GL_UNSIGNED_INT> { using type = GLuint; };
template<> struct glm_enum_type<GL_DOUBLE> { using type = GLdouble; };

template <typename T> constexpr GLuint glm_type_components = 1;

template <length_t L, typename T, qualifier Q>
constexpr GLuint glm_type_components<vec<L, T, Q>> = L;

}

#endif /* GL4_GLC_GLM_TRAITS_H */
