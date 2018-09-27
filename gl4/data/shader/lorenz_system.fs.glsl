#version 330 core

in VS_OUT
{
  vec4 color;
}fi;

out vec4 frag_color;

void main(void)
{
  frag_color = fi.color;
}
