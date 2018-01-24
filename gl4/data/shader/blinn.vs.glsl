#ifndef WITH_TEX
#version 430
#endif

in vec4 vertex;
in vec3 normal;
#ifdef WITH_TEX
in vec2 texcoord;
#endif

uniform mat4 modelViewProjMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewMatrixInverseTranspose;

out VS_OUT{
  vec3 viewVertex; // view space
  vec3 viewNormal; // view space
#ifdef WITH_TEX
  vec2 texcoord;
#endif
} vs_out;

void main(void)
{
  vs_out.viewNormal = mat3(modelViewMatrixInverseTranspose) * normal;
  vs_out.viewVertex = (modelViewMatrix * vertex).xyz;
#ifdef WITH_TEX
  vs_out.texcoord = texcoord;
#endif
  gl_Position = modelViewProjMatrix * vertex;
}
