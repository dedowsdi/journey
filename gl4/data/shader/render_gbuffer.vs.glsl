#version 430 core

in vec4 vertex;
in vec3 normal;

uniform mat4 modelViewProjMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewMatrixInverseTranspose;

out VS_OUT{
  vec3 viewVertex;
  vec3 viewNormal;
} vs_out;

void main(void)
{
  vs_out.viewNormal = mat3(modelViewMatrixInverseTranspose) * normal;
  vs_out.viewVertex = (modelViewMatrix * vertex).xyz;
  gl_Position = modelViewProjMatrix * vertex;
}
