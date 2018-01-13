#version 120

varying vec3 lightPos;

void main(void)
{
  gl_Position =  gl_ModelViewProjectionMatrix * gl_Vertex;
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_TexCoord[1] = gl_MultiTexCoord1;
  vec3 T = gl_TexCoord[1].xyz;
  vec3 N = gl_Normal;
  vec3 B = cross(N, T);

  mat3 tbn = mat3(T, B, N);
  mat3 invTbn = transpose(tbn);
  lightPos = invTbn * gl_LightSource[0].position.xyz;
}
