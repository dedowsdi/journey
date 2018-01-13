#version 430 core

in VS_OUT{
  vec3 normal;
}fs_in;

out vec4 fragColor;

void main(void)
{
  //render normal as color
  fragColor = vec4((normalize(fs_in.normal) + 1.0) * 0.5, 1.0);
}
