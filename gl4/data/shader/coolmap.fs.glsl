#version 430 core

in vs_out
{
 vec2 texcoord;
}fi;

uniform sampler2D cool_map;

out vec4 frag_color;

void main(void)
{
  vec2 ts = 1.0 / textureSize(cool_map, 0);

  // smooth spread fire
  vec4 color = texture(cool_map, fi.texcoord + vec2(ts.x, 0)) + 
               texture(cool_map, fi.texcoord + vec2(-ts.x, 0)) + 
               texture(cool_map, fi.texcoord + vec2(0, ts.y)) + 
               texture(cool_map, fi.texcoord + vec2(0, -ts.y));
  color *= 0.25;

  frag_color = color;
}
