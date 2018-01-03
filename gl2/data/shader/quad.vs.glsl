attribute vec4 vertex;
attribute vec2 texcoord;

varying vec2 _texcoord;

void main(void)
{
  gl_Position = vertex;
  _texcoord = texcoord;
}
