#version 430 core

in vec4 vertex;

uniform mat4 mvp_mat;

void main(void)
{
  gl_Position =  mvp_mat * vertex;
}
