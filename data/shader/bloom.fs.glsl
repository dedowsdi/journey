#version 120

uniform float exposure = 1;
uniform sampler2D hdrMap;
uniform sampler2D brightnessMap;

void main(void)
{
  vec4 color = texture2D(hdrMap, gl_TexCoord[0].xy) +
    texture2D(brightnessMap, gl_TexCoord[0].xy);
  
  gl_FragColor = vec4(1.0) - exp(-color * exposure);
}
