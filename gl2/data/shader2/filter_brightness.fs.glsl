#version 120

varying vec2 m_texcoord;

uniform sampler2D quad_map;
uniform float threshold = 0.95;

void main(void) {
  vec4 color = texture2D(quad_map, m_texcoord.xy);
  // color
  gl_FragData[0] = color;
  float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));

  // brightness
  if (brightness >= threshold) {
    gl_FragData[1] = vec4(color.xyz, 1);
  } else {
    gl_FragData[1] = vec4(0);
  }
}
