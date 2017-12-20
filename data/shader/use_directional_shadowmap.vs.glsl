// only works for directional light

uniform mat4 lightMatrix;  // bias * scale * proj * view * model
uniform mat4 modelViewProjMatrix;
varying vec3 vertex;  // local
varying vec3 normal;  // local

void main(void) {
  gl_Position = modelViewProjMatrix * gl_Vertex;
  // assume light proj is orthogonal, w is always 1
  gl_TexCoord[1] = lightMatrix * gl_Vertex;

  vertex = gl_Vertex.xyz;
  normal = gl_Normal;
}
