uniform mat4 modelViewProjMatrix;
uniform mat4 modelMatrix;

varying vec3 localVertex;  
varying vec3 worldVertex; 
varying vec3 normal;  // local

void main(void) {
  gl_Position = modelViewProjMatrix * gl_Vertex;

  localVertex = gl_Vertex.xyz;
  vec4 pos = modelMatrix * gl_Vertex;
  worldVertex = pos.xyz / pos.w;
  
  normal = gl_Normal;
}
