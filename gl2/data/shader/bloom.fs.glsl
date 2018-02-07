#version 120

varying vec2 m_texcoord;

uniform float exposure = 1;
uniform sampler2D hdr_map;
uniform sampler2D brightness_map;

void main(void)
{
  vec4 color = texture2D(hdr_map, m_texcoord.xy) +
    texture2D(brightness_map, m_texcoord.xy);
  
  gl_FragColor = vec4(1.0) - exp(-color * exposure);
}
