#version 330 core

in vec4 vertex;
in vec3 normal;

uniform mat4 mv_mat;
uniform mat4 mv_mat_it;

out vs_out
{
  vec3 normal;
}
vo;

void main() {
  gl_Position = mv_mat * vertex;
  vo.normal = mat3(mv_mat_it) * normal;
}
