#version 430 core

in vertex_data
{
  vec4 color;
  noperspective float stipple_count;
} fi;

layout(location = 2) uniform uint pattern;
layout(location = 3) uniform float scale;

out vec4 frag_color;

void main()
{
  uint bit = uint(fi.stipple_count / scale) & 0xf;
  if( (pattern & (1 << bit) ) == 0u )
    discard;

  frag_color = fi.color;
}
