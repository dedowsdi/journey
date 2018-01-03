#version 330 core

uniform mat4 matWVP;

layout(location = 0) in vec4 pos;

void main() {
  gl_Position = matWVP * pos;
}
