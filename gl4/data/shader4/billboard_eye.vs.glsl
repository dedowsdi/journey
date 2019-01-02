/*
 * look at camera from billboard, use camera up as up
 */
#version 330 core

in vec4 vertex; // x y s t
in vec3 translation; // instance translation

out vs_out{
  vec2 texcoord;
}vo;

uniform mat4 vp_mat;
uniform vec3 camera_up;
uniform vec3 camera_pos;

void main() {
  vec3 ev = camera_pos - translation;
  ev = normalize(ev);

  vec3 up = camera_up;
  vec3 side = normalize(cross(up, ev));
  up = cross(ev, side);

  mat4 m_mat = mat4(mat3(side, up, ev));
  m_mat[3].xyz = translation;

  vo.texcoord = vertex.zw;
  gl_Position = vp_mat * m_mat * vec4(vertex.xy, 0, 1);
}
