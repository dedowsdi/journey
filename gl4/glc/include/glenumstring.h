#ifndef GL_GLC_GLENUMSTRING_H
#define GL_GLC_GLENUMSTRING_H

#include "gl.h"

const char* gl_tex_wrap_to_string(GLenum e);
GLenum g_tex_wrapl_from_string(const char* s);

const char* gl_tex_filter_to_string(GLenum e);
GLenum gl_tex_filter_from_string(const char* s);

const char* glu_nurb_sample_method_to_string(GLenum e);
GLenum glu_nurb_sample_methodl_from_string(const char* s);

const char* gl_shader_type_to_string(GLenum e);
GLenum gl_shader_type_from_string(const char* s);

const char* gl_debug_source_to_string(GLenum e);
GLenum gl_debug_source_from_string(const char* s);

const char* gl_debug_type_to_string(GLenum e);
GLenum gl_debug_type_from_string(const char* s);

const char* gl_debug_severity_to_string(GLenum e);
GLenum gl_debug_severity_from_string(const char* s);

const char* gl_polygon_mode_to_string(GLenum e);
GLenum gl_polygon_mode_from_string(const char* s);

const char* gl_cull_face_mode_to_string(GLenum e);
GLenum gl_cull_face_mode_from_string(const char* s);

const char* gl_hint_to_string(GLenum e);
GLenum gl_hint_from_string( char* s);

const char* gl_provoke_mode_to_string(GLenum e);
GLenum gl_provoke_mode_from_string( char* s);

#ifndef GL_VERSION_3_0

const char* gl_tex_env_mode_to_string(GLenum e);
GLenum gl_tex_env_mode_from_string(const char* s);

const char* glu_nurb_sample_method_to_string(GLenum e);
GLenum glu_nurb_sample_methodl_from_string(const char* s);
#endif

#endif /* GL_GLC_GLENUMSTRING_H */
