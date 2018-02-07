#define gamma 2.2

uniform sampler2D color_map;

void main(void) {
  gl_FragColor = pow(texture2D(color_map, gl_TexCoord[0].xy), vec4(1.0/gamma));
}
