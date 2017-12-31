#version 120

varying vec2 texcoord;

uniform sampler2D fontMap;
uniform vec4 textColor = vec4(1);

void main(void)
{
  vec4 color = vec4(1, 1, 1, texture2D(fontMap, texcoord).r);
  gl_FragColor = textColor * color;
}
