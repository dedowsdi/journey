#version 120

uniform samplerCube cubemap;

void main(void)
{
  vec4 reflectColor = textureCube(cubemap, gl_TexCoord[0].xyz);
  vec4 refractColor = textureCube(cubemap, gl_TexCoord[1].xyz);
  // gl_FragColor = reflectColor * refractColor;
  gl_FragColor = mix(reflectColor, refractColor, 0.3);
}
