uniform mat4 modelViewProjMatrix;
//uniform mat4 modelViewMatrix;
uniform float nearPlane;
uniform float farPlane;
varying float zDistance; // in eye space
void main(void)
{
  gl_Position = modelViewProjMatrix * gl_Vertex;
  
  // get z distance from ndc, must faster than a mat4*vec4
  float zn = gl_Position.z/gl_Position.w;
  zDistance = -2.0 * nearPlane * farPlane / (zn * (farPlane - nearPlane) -
      farPlane - nearPlane );
}
