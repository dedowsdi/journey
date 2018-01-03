attribute vec4 vertex;
uniform mat4 modelViewProjMatrix;

varying vec2 texcoord;

void main(void)
{
  gl_Position = modelViewProjMatrix * vec4(vertex.xy, 0, 1);
  texcoord = vertex.zw;
}
