attribute vec4 vertex;
attribute vec2 texcoord;

varying vec2 m_texcoord;

void main(void)
{
  gl_Position = vertex;
  m_texcoord = texcoord;
}
