#version 430 core

in vec4 vertex;
in vec3 normal;
in vec3 tangent;
in vec2 texcoord;

uniform mat4 mvp_mat;
uniform mat4 mv_mat;
uniform mat4 mv_mat_it;

out vs_out{
  vec3 view_vertex;
  vec2 texcoord;
  mat3 tbn;
} vo;

void main(void)
{
  vo.view_vertex = (mv_mat * vertex).xyz;
  vo.texcoord = texcoord;

  vec3 B = normalize(cross(normal, tangent));
  vo.tbn = mat3(tangent, B, normal);

  gl_Position = mvp_mat * vertex;
}
