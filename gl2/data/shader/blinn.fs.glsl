varying vec3 viewVertex; // view space
varying vec3 viewNormal; // view space

void main(void) {
  gl_FragColor = blinn(viewVertex, normalize(viewNormal));
}
