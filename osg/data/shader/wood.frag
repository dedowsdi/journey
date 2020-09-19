#version 120

#define PI 3.1415926535897932384626433832795
#define PI_2 ( PI * 0.5 )
#define PI_4 ( PI * 0.25 )
#define PI_8 ( PI * 0.125 )

#pragma include shader/snoise.frag
float snoise(vec2 v);

#define patan( y, x ) ( atan( -y, -x ) + PI )
#define to_angle( radian ) ( 180 * radian / PI )
#define to_radian( angle ) ( PI * angle / 180 )

mat2 rotate2d( float theta )
{
    float c = cos( theta );
    float s = sin( theta );
    return mat2( c, s, -s, c );
}

mat2 scale2d( vec2 v ) { return mat2( v.x, 0, 0, v.y ); }

uniform int osg_FrameNumber;
uniform float osg_FrameTime;
uniform float osg_DeltaFrameTime;
uniform float osg_SimulationTime;
uniform float osg_DeltaSimulationTime;
uniform vec2 mouse;
uniform vec2 resolution;

// -----percent-----
float lines( float x, float percent )
{
    float f = abs( fract( x ) - 0.5 ); // [0, 0.5]
    float smooth_range = 0.01;

    // [0-0.5*percent] is valid range
    return 1 - smoothstep( 0.5 * percent - smooth_range, 0.5 * percent, f );
}

void main( void )
{
    vec2 st = gl_FragCoord.xy / resolution;

    st = rotate2d( snoise( st ) ) * st;
    float f = lines( st.x * 10.14, 0.45 );
    gl_FragColor = vec4( f );
}
