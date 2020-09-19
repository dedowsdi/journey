#version 120

// https://www.shadertoy.com/view/XdXGW8

uniform int osg_FrameNumber;
uniform float osg_FrameTime;
uniform float osg_DeltaFrameTime;
uniform float osg_SimulationTime;
uniform float osg_DeltaSimulationTime;
uniform vec2 mouse;
uniform vec2 resolution;

float random( float v ) { return fract( sin( v ) * 100000 ); }

float random( vec2 v )
{
    return fract( sin( dot( v, vec2( 12.9898, 78.233 ) ) ) * 43758.5453123 );
}

float noise( float v )
{
    float i = floor( v );
    float f = fract( v );
    return mix( random( i ), random( i + 1 ), smoothstep( 0, 1, f ) );
}

vec2 random2( vec2 st )
{
    st = vec2(
        dot( st, vec2( 127.1, 311.7 ) ), dot( st, vec2( 269.5, 183.3 ) ) );
    return -1.0 + 2.0 * fract( sin( st ) * 43758.5453123 );
}

vec2 hash( vec2 x )  // replace this by something better
{
    const vec2 k = vec2( 0.3183099, 0.3678794 );
    x = x*k + k.yx;
    return -1.0 + 2.0*fract( 16.0 * k*fract( x.x*x.y*(x.x+x.y)) );
}

float noise( in vec2 st )
{
    vec2 i = floor( st );
    vec2 f = fract( st );

    // Four gradients in corners of g00 tile
    vec2 g00 = hash( i );
    vec2 g10 = hash( i + vec2( 1., 0. ) );
    vec2 g01 = hash( i + vec2( 0., 1. ) );
    vec2 g11 = hash( i + vec2( 1., 1. ) );

    vec2 u = f * f * ( 3.0 - 2.0 * f );

    float x = mix( dot( g00, f ), dot( g10, f - vec2( 1., 0. ) ), u.x );
    float y = mix(
        dot( g01, f - vec2( 0., 1. ) ), dot( g11, f - vec2( 1., 1. ) ), u.x );
    return mix( x, y, u.y );
}

mat2 rotate(float theta)
{
    float c = cos(theta);
    float s = sin(theta);
    return mat2(c, s, -s, c);
}

mat2 scale(float s)
{
    return mat2(s, 0, 0, s);
}

void main( void )
{
    vec2 nst = gl_FragCoord.xy / resolution;
    vec2 st = nst * vec2( resolution.x / resolution.y, 1 );
    float f;

    if ( nst.x < 0.5 )
    {
        f = noise( 32.0 * st );
    }
    // right: fbm - fractal noise (4 octaves)
    else
    {
        st *= 16.0;

        // m need not be orthogonal
        mat2 m = rotate( osg_SimulationTime * 0.1 );

        f = 0.5000 * noise( st );
        st = m * st;
        f += 0.2500 * noise( st );
        st = m * st;
        f += 0.1250 * noise( st );
        st = m * st;
        f += 0.0625 * noise( st );
        st = m * st;
    }

    f = f * 0.5 + 0.5;

    // draw separator
    f *= smoothstep( 0, 0.005, abs( nst.x - 0.5 ) );

    gl_FragColor = vec4( f );
}
