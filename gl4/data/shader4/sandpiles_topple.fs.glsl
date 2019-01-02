#version 430 core

in vs_out
{
  vec2 texcoord;
}fi;

uniform usampler2D diffuse_map;
out uint frag_color;

void main(void)
{
  vec2 texel_size = 1.0 / textureSize(diffuse_map, 0);

  uint r = texture(diffuse_map, fi.texcoord).r;
  // add grain toppled fram neighbour
  uint n0 = texture(diffuse_map, fi.texcoord + vec2(texel_size.x, 0)).r;
  uint n1 = texture(diffuse_map, fi.texcoord + vec2(-texel_size.x, 0)).r;
  uint n2 = texture(diffuse_map, fi.texcoord + vec2(0, texel_size.y)).r;
  uint n3 = texture(diffuse_map, fi.texcoord + vec2(0, -texel_size.y)).r;

  r -= uint(mix(0, 4, step(4, r)));
  r += uint(step(4, n0));
  r += uint(step(4, n1));
  r += uint(step(4, n2));
  r += uint(step(4, n3));

  frag_color = r;
}
