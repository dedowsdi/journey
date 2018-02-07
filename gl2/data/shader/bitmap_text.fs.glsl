#version 120

varying vec2 texcoord;

uniform sampler2D font_map;
uniform vec4 text_color = vec4(1);

void main(void)
{
  vec4 color = vec4(1, 1, 1, texture2D(font_map, texcoord).r);
  gl_FragColor = text_color * color;
}
