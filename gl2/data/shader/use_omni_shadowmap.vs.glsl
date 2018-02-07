#version 120

attribute vec4 vertex;
attribute vec3 normal;

uniform mat4 mv_mat;
uniform mat4 mvp_mat;
uniform mat4 mv_mat_it;
uniform mat4 m_mat;

varying vec3 v_vertex;  
varying vec3 w_vertex; 
varying vec3 v_normal; 

void main(void) {
  gl_Position = mvp_mat * vertex;

  w_vertex = (m_mat * vertex).xyz;
  
  v_normal = mat3(mv_mat_it) * normal;
  v_vertex = (mv_mat * vertex).xyz;
}
