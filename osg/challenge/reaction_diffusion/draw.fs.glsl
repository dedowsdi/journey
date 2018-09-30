#version 120

uniform sampler2D quad_map;

void main(void)
{
  gl_FragColor = texture2D(quad_map, gl_TexCoord[0].xy);
  gl_FragColor.xyz = vec3(gl_FragColor.r - gl_FragColor.g);
}
