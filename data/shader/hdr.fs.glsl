#version 120

uniform int method = 0;
uniform float exposure = 1;

uniform sampler2D imageMap;

void main(void) {
  vec4 color = texture2D(imageMap, gl_TexCoord[0].xy);

  if (method == 0) {
    // Reinhard toon map
    gl_FragColor = color / (1 + color);
  } else {
    // exposure tone map
    gl_FragColor = vec4(1.0) - exp(-color * exposure);
  }
}
