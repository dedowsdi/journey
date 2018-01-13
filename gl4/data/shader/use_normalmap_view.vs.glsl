#version 430 core

in vec4 vertex;
in vec3 normal;
in vec3 tangent;
in vec2 texcoord;

uniform mat4 modelViewProjMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewMatrixInverseTranspose;

out VS_OUT{
  vec3 viewVertex; // view space
  vec3 viewNormal; // view space
  vec3 viewTangent;
  vec2 texcoord;
} vs_out;

void main(void)
{
  vs_out.viewNormal = mat3(modelViewMatrixInverseTranspose) * normal;
  vs_out.viewVertex = (modelViewMatrix * vertex).xyz;
  vs_out.viewTangent = mat3(modelViewMatrix) * tangent;
  vs_out.texcoord = texcoord;

  gl_Position = modelViewProjMatrix * vertex;
}
