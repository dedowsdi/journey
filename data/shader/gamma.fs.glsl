#define gamma 2.2

uniform sampler2D image;

void main(void) {
  gl_FragColor = pow(texture2D(image, gl_TexCoord[0].xy), vec4(1.0/gamma));
}
