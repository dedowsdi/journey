attribute vec4 vertex;

varying vec2 texcoord;

uniform mat4 modelViewProjMatrix;

void main(void)
{
  gl_Position = modelViewProjMatrix * vec4(vertex.xy, 0, 1);
  texcoord = vertex.zw;
}
