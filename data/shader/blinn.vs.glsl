#version 120

uniform mat4 modelViewProjMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewMatrixInverseTranspose;

varying vec3 normal; // view space
varying vec3 vertex; // view space

void main(void)
{
  normal = mat3(modelViewMatrixInverseTranspose) * gl_Normal;
  vertex = (modelViewMatrix * gl_Vertex).xyz;
  gl_Position = modelViewProjMatrix * gl_Vertex;
}
