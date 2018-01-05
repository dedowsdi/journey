in VS_OUT{
  vec2 texcoord;
} fs_in;

uniform sampler2D gVertex;
uniform sampler2D gNormal;
uniform sampler2D gEmission;
uniform sampler2D gAmbient;
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;
uniform sampler2D gShininess;
uniform sampler2D gDepth;

out vec4 flagColor;

void main(void) {
  vec3 vertex = texture(gVertex, fs_in.texcoord).xyz;
  vec3 normal = normalize(texture(gNormal, fs_in.texcoord).xyz);

  Material material;

  material.emission = texture(gEmission, fs_in.texcoord);
  material.ambient = texture(gAmbient, fs_in.texcoord);
  material.diffuse = texture(gDiffuse, fs_in.texcoord);
  material.specular = texture(gSpecular, fs_in.texcoord);
  material.shininess = texture(gShininess, fs_in.texcoord).r * 255;

  flagColor = blinn(vertex, normal, material);
  gl_FragDepth = texture(gDepth, fs_in.texcoord).r;
}
