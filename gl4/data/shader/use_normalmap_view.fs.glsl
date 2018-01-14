//#version 430 core
in VS_OUT{
  vec3 viewVertex;
  vec2 texcoord;
  mat3 tbn;
} fs_in;

uniform mat4 modelViewMatrixInverseTranspose;
uniform sampler2D normalMap;

out vec4 fragColor;

void main(void)
{
  vec3 normal = texture(normalMap, fs_in.texcoord).xyz * 2 - 1; 
  // tbn is an orthogonal basis
  normal = normalize(mat3(modelViewMatrixInverseTranspose) * (fs_in.tbn * normal));

  fragColor = blinn(fs_in.viewVertex, normal);
}
