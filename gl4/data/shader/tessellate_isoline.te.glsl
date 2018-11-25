//#version 430 core

#ifndef LAYOUT
#define LAYOUT isolines, equal_spacing, ccw
#endif

layout(LAYOUT)in;
uniform mat4 mvp_mat;

void main()
{
  gl_Position = mix(
    mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x),
    mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x),
    gl_TessCoord.y
  );
  gl_Position = mvp_mat * gl_Position;
}
