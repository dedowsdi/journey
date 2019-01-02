#version 430 core

in vs_out{
  vec2 texcoord;
} fi;

out vec4 frag_color;

uniform sampler2D font_map;
uniform vec4 text_color = vec4(1);

void main(void)
{
  vec4 color = vec4(1, 1, 1, texture(font_map, fi.texcoord).r);
  frag_color = text_color * color;
}
