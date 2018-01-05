in VS_OUT{
  vec3 viewVertex; // view space
  vec3 viewNormal; // view space
} fs_in;

out vec4 flagColor;

void main(void) {
  vec3 normal = normalize(fs_in.viewNormal);
  flagColor = blinn(fs_in.viewVertex, normal);
}
