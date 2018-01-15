#version 430 core

in vec4 vertex;
in vec4 color;

uniform mat4 modelViewProjMatrix;

out VS_OUT{
  flat vec4 color;
}vs_out;

void main(void)
{
  gl_Position = modelViewProjMatrix * vertex;
  vs_out.color = color;
}
