#version 430 core

uniform mat4 modelViewProjMatrix;

in vec4 vertex;

void main() {
  gl_Position = modelViewProjMatrix * vertex;
}
