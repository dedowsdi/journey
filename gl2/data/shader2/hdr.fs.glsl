#version 120

varying vec2 m_texcoord;

uniform int method = 0;
uniform float exposure = 1;

uniform sampler2D diffuse_map;

void main(void) {
  vec4 color = texture2D(diffuse_map, m_texcoord.xy);

  if (method == 0) {
    // reinhard toon map
    gl_FragColor = color / (1 + color);
  } else {
    // exposure tone map
    gl_FragColor = vec4(1.0) - exp(-color * exposure);
  }
}
