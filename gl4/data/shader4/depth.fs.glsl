#version 430 core

in vs_out
{
 vec2 texcoord;
}fi;

out vec4 frag_color;
uniform sampler2D quad_map;

void main(void)
{
  gl_FragDepth = texture(quad_map, fi.texcoord).x;
}
