#version 120

uniform mat4 textureMatrix;

vec3 refract(vec3 I, vec3 N, float eta)
{
   float k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));
   if (k < 0.0)
      return vec3(0.0);       // or genDType(0.0)
   else
      return eta * I - (eta * dot(N, I) + sqrt(k)) * N;
}

void main(void)
{
  vec3 eye = gl_ModelViewMatrixInverse[3].xyz;
  vec3 N = gl_Normal;
  vec3 I = gl_Vertex.xyz - eye;
  vec3 R0 = reflect(I, normalize(N));
  vec3 R1 = refract(I, N, 1.04);

  gl_TexCoord[0] = textureMatrix * vec4(R0, 1);
  gl_TexCoord[1] = textureMatrix * vec4(R1, 1);
  gl_Position = ftransform();
}
