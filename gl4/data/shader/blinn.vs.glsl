//#version 430 core

in vec4 vertex;
in vec3 normal;
#ifdef WITH_TEX
in vec2 texcoord;
#endif

#ifdef INSTANCE
in mat4 mvp_mat;
in mat4 mv_mat;
in mat4 mv_mat_it;
#else
uniform mat4 mvp_mat;
uniform mat4 mv_mat;
uniform mat4 mv_mat_it;
#endif
uniform float normal_scale = 1.0f; // set this to -1 if you need back face lighting

out vs_out{
  vec3 v_vertex; // view space
  vec3 v_normal; // view space
#ifdef WITH_TEX
  vec2 texcoord;
#endif
} vo;

void main(void)
{
  vo.v_normal = mat3(mv_mat_it) * normal * normal_scale;
  vo.v_vertex = (mv_mat * (vertex / vertex.w)).xyz;
#ifdef WITH_TEX
  vo.texcoord = texcoord;
#endif
  gl_Position = mvp_mat * vertex;
}
