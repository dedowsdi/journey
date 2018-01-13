//#version 430 core
in VS_OUT{
  vec3 viewVertex; // view space
  vec3 viewNormal; // view space
  vec3 viewTangent;
  vec2 texcoord;
} fs_in;

uniform mat4 modelViewMatrixInverseTranspose;
uniform sampler2D normalMap;

out vec4 fragColor;

void main(void)
{

  vec3 T = normalize(fs_in.viewTangent);
  vec3 B = normalize(cross(fs_in.viewNormal, fs_in.viewTangent));
  vec3 N = normalize(fs_in.viewNormal);
  mat3 tbn = mat3(T,B,N);

  vec3 normal = texture(normalMap, fs_in.texcoord).xyz * 2 - 1; 
  // tbn is an orthogonal basis
  normal = normalize(mat3(modelViewMatrixInverseTranspose) * (tbn * normal));

  fragColor = blinn(fs_in.viewVertex, normal);
}
