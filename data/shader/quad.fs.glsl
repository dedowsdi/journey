varying vec2 _texcoord;

uniform sampler2D quadMap;

void main(void)
{
  gl_FragColor = texture2D(quadMap, _texcoord);
}
