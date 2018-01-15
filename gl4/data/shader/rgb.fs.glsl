#version 430 core

in VS_OUT{
  flat vec4 color;
}fs_in;

uniform bool gamma = false;

out vec4 fragColor;

void main(void)
{
  fragColor = fs_in.color;
  if(gamma)
  {
    fragColor.xyz = pow(fragColor.xyz, vec3(1/2.2));
  }
}
