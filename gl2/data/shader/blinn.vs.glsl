#version 120

uniform mat4 modelViewProjMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewMatrixInverseTranspose;

varying vec3 viewNormal; // view space
varying vec3 viewVertex; // view space

void main(void)
{
  viewNormal = mat3(modelViewMatrixInverseTranspose) * gl_Normal;
  viewVertex = (modelViewMatrix * gl_Vertex).xyz;
  gl_Position = modelViewProjMatrix * gl_Vertex;
}
