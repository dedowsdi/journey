#version 330 core

in vec4 vertex;

uniform mat4 mvp_mat;

void main() {
  gl_Position = mvp_mat * vertex;
}
