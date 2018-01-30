#version 430 core

in vs_out{
  flat vec4 color;
}fi;

uniform bool gamma = false;

out vec4 frag_color;

void main(void)
{
  frag_color = fi.color;
  if(gamma)
  {
    frag_color.xyz = pow(frag_color.xyz, vec3(1/2.2));
  }
}
