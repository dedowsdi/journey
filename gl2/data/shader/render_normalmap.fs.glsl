#version 120

varying vec3 m_normal;

void main(void)
{
  //render normal as color
  gl_FragColor = vec4((normalize(m_normal) + 1.0) * 0.5, 1.0);
}
