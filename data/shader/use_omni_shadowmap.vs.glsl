#version 120

uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjMatrix;
uniform mat4 modelViewMatrixInverseTranspose;
uniform mat4 modelMatrix;

varying vec3 viewVertex;  
varying vec3 worldVertex; 
varying vec3 normal;  // local

void main(void) {
  gl_Position = modelViewProjMatrix * gl_Vertex;

  worldVertex = (modelMatrix * gl_Vertex).xyz;
  
  normal = mat3(modelViewMatrixInverseTranspose) * gl_Normal;
  viewVertex = (modelViewMatrix * gl_Vertex).xyz;
}
