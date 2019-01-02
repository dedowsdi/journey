#version 430 core

uniform vec3 color;

out vec4 frag_color;

void main(void)
{
  frag_color = vec4(color, 1);
}
