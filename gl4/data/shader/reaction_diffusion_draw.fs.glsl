#version 430 core

in vs_out
{
 vec2 texcoord;
}fi;

uniform sampler2D quad_map;

out vec4 frag_color;

void main(void)
{
  frag_color = texture(quad_map, fi.texcoord);
  frag_color.xyz = vec3(frag_color.r - frag_color.g);
  // gif sucks at gradient
  //frag_color.xyz = vec3(step(frag_color.g, 0.1));
}
