#version 330 core

in vec4 vertex; // x y s t

out vs_out{
  vec2 texcoord;
}vo;

uniform mat4 mvp_mat;

void main() {
  vo.texcoord = vertex.yz;
  gl_Position = mvp_mat * vec4(vertex.xy, 0, 1);
}
