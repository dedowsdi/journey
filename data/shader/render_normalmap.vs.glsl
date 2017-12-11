varying vec3 normal;

void main(void)
{
  normal = gl_Normal;
  //inorder to render normal at resolution * tex, generate ndc from tex
  gl_Position = vec4(gl_MultiTexCoord0.xy * 2 - 1, 0, 1);
}
