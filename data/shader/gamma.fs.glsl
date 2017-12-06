#define gamma 2.2

uniform sampler2D image;

void main(void) {
  gl_FragColor = pow(texture2D(image, vec2(0.1,0.1)), vec4(1.0/gamma));
}
