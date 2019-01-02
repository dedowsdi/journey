#version 120
// only works for directional light

attribute vec4 vertex;
attribute vec3 normal;

uniform mat4 light_mat;  // bias * scale * proj * light_view * model
uniform mat4 mv_mat;
uniform mat4 mvp_mat;
uniform mat4 mv_mat_it;

varying vec3 v_normal;
varying vec3 v_vertex;
varying vec4 l_vertex; // bias, scaled wnd vertex of light view

void main(void) {
  gl_Position = mvp_mat * vertex;
  // assume light proj is orthogonal, w is always 1
  l_vertex = light_mat * vertex;

  v_normal = mat3(mv_mat_it) * normal;
  v_vertex = (mv_mat * vertex).xyz;
}
