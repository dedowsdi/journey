#version 430 core

in vec4 vertex;
in vec3 normal;
in vec3 tangent;
in vec2 texcoord;

uniform mat4 modelViewProjMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewMatrixInverseTranspose;

out VS_OUT{
  vec3 viewVertex;
  vec2 texcoord;
  mat3 tbn;
} vs_out;

void main(void)
{
  vs_out.viewVertex = (modelViewMatrix * vertex).xyz;
  vs_out.texcoord = texcoord;

  vec3 B = normalize(cross(normal, tangent));
  vs_out.tbn = mat3(tangent, B, normal);

  gl_Position = modelViewProjMatrix * vertex;
}