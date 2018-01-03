#version 120

uniform sampler2D colorMap;
uniform float threshold = 0.95;

void main(void) {
  vec4 color = texture2D(colorMap, gl_TexCoord[0].xy);
  gl_FragData[0] = color;
  float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
  if (brightness >= threshold) {
    gl_FragData[1] = vec4(color.xyz, 1);
  } else {
    gl_FragData[1] = vec4(0);
  }
}
