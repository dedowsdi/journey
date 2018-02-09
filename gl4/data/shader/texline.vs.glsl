#version 330 core

in vec4 vertex;
in float texcoord;

uniform mat4 mvp_mat;

out vs_out{
 float texcoord;
} vo;

void main() {
  vo.texcoord = texcoord;
  gl_Position = mvp_mat * vertex;
}
