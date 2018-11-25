//#version 430 core

#ifndef LAYOUT
#define LAYOUT triangles, equal_spacing, ccw
#endif

layout(LAYOUT)in;

uniform mat4 mvp_mat;

void main()
{
  gl_Position = gl_in[0].gl_Position * gl_TessCoord.x +
                gl_in[1].gl_Position * gl_TessCoord.y + 
                gl_in[2].gl_Position * gl_TessCoord.z;
  gl_Position = mvp_mat * gl_Position;
}
