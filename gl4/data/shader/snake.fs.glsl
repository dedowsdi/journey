#version 430 core

out vec4 frag_color;
uniform vec3 color;

void main(void)
{
  frag_color = vec4(color, 1);
}
