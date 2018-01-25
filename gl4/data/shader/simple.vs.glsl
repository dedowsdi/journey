#version 330 core

in vec4 vertex;

uniform mat4 modelViewProjMatrix;

void main() {
  gl_Position = modelViewProjMatrix * vertex;
}
