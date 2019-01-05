#version 430 core

in vs_out{
  vec2 texcoord;
  vec4 color;
} fi;

out vec4 frag_color;

uniform sampler2D font_map;

void main(void)
{
  vec4 color = vec4(1, 1, 1, texture(font_map, fi.texcoord).r);
  frag_color = fi.color * color;
}
