varying vec2 m_texcoord;

uniform sampler2D quad_map;

void main(void)
{
  gl_FragColor = texture2D(quad_map, m_texcoord);
}
