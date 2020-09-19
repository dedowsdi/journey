#version 120

varying float lightIntensity;
varying vec2 pos;

uniform vec2 density = vec2( 3, 3 );

void main( void )
{
    // minimal diffuse head light
    vec3 L = vec3( 0, 0, 1 );
    vec3 N = normalize( gl_NormalMatrix * gl_Normal );
    lightIntensity = clamp( dot( L, N ), 0, 1 );

    pos = gl_Vertex.xz / gl_Vertex.w;
    pos *= density;

    gl_Position = ftransform();
}
