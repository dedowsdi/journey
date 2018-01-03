// only works for directional light

uniform mat4 modelViewProjMatrix;

void main(void) {
  gl_Position = modelViewProjMatrix * gl_Vertex;
}
