#version 430 core

in vs_out
{
 vec2 texcoord;
}fi;


uniform sampler2D quad_map;

uniform vec3 threshold;

out vec4 frag_color;

bool greater(vec3 color)
{
  return color.r > threshold.r && color.g > threshold.g && color.b >
    threshold.b;
}

void main(void)
{
  frag_color = texture(quad_map, fi.texcoord);
  if(!greater(frag_color.xyz))
    discard;
}
