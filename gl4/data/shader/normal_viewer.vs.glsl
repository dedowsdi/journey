#version 330 core

in vec4 vertex;
in vec3 normal;

uniform mat4 modelViewMatrix;
uniform mat4 modelViewMatrixInverseTranspose;

out VS_OUT 
{ 
  vec3 normal; 
}
vs_out;

void main() {
  gl_Position = modelViewMatrix * vertex;
  vs_out.normal = mat3(modelViewMatrixInverseTranspose) * normal;
}
