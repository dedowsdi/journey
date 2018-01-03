uniform float nearPlane;
uniform float farPlane;
varying float zDistance; // in eye space

void main(void)
{
  //write linear depth
  gl_FragDepth = (zDistance - nearPlane) / (farPlane - nearPlane);
}
