#version 430

in vec4 vertex;
in vec3 normal;

uniform mat4 modelViewProjMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewMatrixInverseTranspose;

out VS_OUT{
  vec3 viewNormal; // view space
  vec3 viewVertex; // view space
} vs_out;

void main(void)
{
  vs_out.viewNormal = mat3(modelViewMatrixInverseTranspose) * normal;
  vs_out.viewVertex = (modelViewMatrix * vertex).xyz;
  gl_Position = modelViewProjMatrix * vertex;
}
