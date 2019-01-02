#version 120

attribute vec4 vertex;
attribute vec3 normal;
attribute vec3 tangent;
attribute vec2 texcoord;

uniform mat4 mvp_mat;
uniform mat4 mv_mat;

varying vec3 v_vertex;
varying vec2 m_texcoord;
varying mat3 tbn;

void main(void)
{
  v_vertex = (mv_mat * vertex).xyz;
  m_texcoord = texcoord;

  vec3 B = normalize(cross(normal, tangent));
  tbn = mat3(tangent, B, normal);

  gl_Position = mvp_mat * vertex;
}
