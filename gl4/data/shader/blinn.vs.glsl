#ifndef WITH_TEX
#version 430
#endif

in vec4 vertex;
in vec3 normal;
#ifdef WITH_TEX
in vec2 texcoord;
#endif

uniform mat4 mvp_mat;
uniform mat4 mv_mat;
uniform mat4 mv_mat_it;

out vs_out{
  vec3 view_vertex; // view space
  vec3 view_normal; // view space
#ifdef WITH_TEX
  vec2 texcoord;
#endif
} vo;

void main(void)
{
  vo.view_normal = mat3(mv_mat_it) * normal;
  vo.view_vertex = (mv_mat * vertex).xyz;
#ifdef WITH_TEX
  vo.texcoord = texcoord;
#endif
  gl_Position = mvp_mat * vertex;
}
