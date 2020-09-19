#version 120

varying vec3 pos;


void main( void )
{
    pos = gl_Vertex.xyz/ gl_Vertex.w;
    gl_Position = ftransform();
}
