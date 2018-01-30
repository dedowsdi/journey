#version 430 core

in vec4 vertex;
in vec3 normal;

uniform mat4 mvp_mat;
uniform mat4 mv_mat;
uniform mat4 mv_mat_it;

out vs_out{
  vec3 view_vertex;
  vec3 view_normal;
} vo;

void main(void)
{
  vo.view_normal = mat3(mv_mat_it) * normal;
  vo.view_vertex = (mv_mat * vertex).xyz;
  gl_Position = mvp_mat * vertex;
}
