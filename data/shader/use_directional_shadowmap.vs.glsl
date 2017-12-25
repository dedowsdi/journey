#version 120
// only works for directional light

uniform mat4 lightMatrix;  // bias * scale * proj * view * model
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjMatrix;
uniform mat4 modelViewMatrixInverseTranspose;
varying vec3 normal;  // view
varying vec3 vertex;  // view

void main(void) {
  gl_Position = modelViewProjMatrix * gl_Vertex;
  // assume light proj is orthogonal, w is always 1
  gl_TexCoord[1] = lightMatrix * gl_Vertex;

  normal = mat3(modelViewMatrixInverseTranspose) * gl_Normal;
  vertex = (modelViewMatrix * gl_Vertex).xyz;
}
