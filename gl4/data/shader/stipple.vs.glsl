#version 430 core

uniform mat4 mvp_mat;

in vec4 vertex;

void main() {
  gl_Position = mvp_mat * vertex;
}
