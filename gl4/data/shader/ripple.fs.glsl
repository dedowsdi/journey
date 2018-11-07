#version 430 core
// https://web.archive.org/web/20160116150939/http://freespace.virgin.net/hugo.elias/graphics/x_water.htm

// run in constant speed, nomatter how many ripples there are

in vs_out
{
 vec2 texcoord;
}fi;

out vec4 frag_color;

uniform sampler2D buffer0; // last frame
uniform sampler2D buffer1; // frame before last frame
uniform float dampen = 0.99; 

void main(void)
{
  vec2 texelSize = 1.0/textureSize(buffer0, 0);

  frag_color = ( texture(buffer0, fi.texcoord + vec2(-texelSize.x, 0)) + 
                 texture(buffer0, fi.texcoord + vec2(+texelSize.x, 0)) +
                 texture(buffer0, fi.texcoord + vec2(0, -texelSize.y)) +
                 texture(buffer0, fi.texcoord + vec2(0, +texelSize.y)) ) * 0.5 -
               texture(buffer1, fi.texcoord );

  frag_color *= dampen;
}
