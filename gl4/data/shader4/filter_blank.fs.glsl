#version 330 core

out vec4 frag_color;
void main(void)
{
  // you must output something, if you discard or don't set frag_color, no blend
  // operation will take effect on this fragment
  frag_color = vec4(0, 1, 0, 1);
}
