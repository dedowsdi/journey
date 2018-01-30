#version 330 core

in vec4 vertex; // x y s t
in vec4 xyzs;  // instance x y z size 
in vec4 color; // instance color

out vs_out{
  vec4 color;
  vec2 texcoord;
}vo;

uniform mat4 vp_mat;
uniform vec3 camera_up;
uniform vec3 camera_pos;

void main() {
  vec3 ev = camera_pos - xyzs.xyz;
  ev = normalize(ev);

  vec3 up = camera_up;
  vec3 right = normalize(cross(up, ev));
  up = cross(ev, right);

  mat4 m_mat = mat4(mat3(right, up, ev));
  m_mat[3].xyz = xyzs.xyz;

  vo.texcoord = vertex.zw;
  vo.color = color;
  gl_Position = vp_mat * m_mat * vec4(vertex.xy * xyzs.w, 0, 1);
}
