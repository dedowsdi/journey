
attribute vec4 vertex;
attribute vec3 normal;
#ifdef WITH_TEX
attribute vec2 texcoord;
#endif

uniform mat4 mvp_mat;
uniform mat4 mv_mat;
uniform mat4 mv_mat_it;

varying vec3 v_vertex; // view space
varying vec3 v_normal; // view space
#ifdef WITH_TEX
varying vec2 m_texcoord;
#endif
uniform float normal_scale = 1.0f; // set this to -1 if you need back face lighting

void main(void)
{
  v_normal = mat3(mv_mat_it) * normal * normal_scale;
  v_vertex = (mv_mat * vertex).xyz;
#ifdef WITH_TEX
  m_texcoord = texcoord;
#endif
  gl_Position = mvp_mat * vertex;
}
