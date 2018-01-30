#version 430 core

in vs_out{
  vec3 normal;
}fi;

out vec4 frag_color;

void main(void)
{
  //render normal as color
  frag_color = vec4((normalize(fi.normal) + 1.0) * 0.5, 1.0);
}
