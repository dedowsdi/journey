#version 120 

attribute vec4 vertex;
attribute vec3 normal;
// If you have too many instances, you will run out of uniform, it'd better to
// use attribute to pass instance data such as transform
attribute mat4 modelViewProjMatrix;
attribute mat4 modelViewMatrix;
attribute mat4 modelViewMatrixInverseTranspose;

varying vec3 viewNormal; // view space
varying vec3 viewVertex; // view space

void main(void)
{
  viewNormal = mat3(modelViewMatrixInverseTranspose) * normal;
  viewVertex = (modelViewMatrix * vertex).xyz;
  gl_Position = modelViewProjMatrix * vertex;
}
