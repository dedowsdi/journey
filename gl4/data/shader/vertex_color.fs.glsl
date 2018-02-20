#version 330 core

in vs_out
{
 vec3 color;
}fi;

out vec4 frag_color;

void main(void)
{
  frag_color = vec4(fi.color, 1);
}
