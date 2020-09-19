#version 120

// see https://www.shadertoy.com/view/lsf3WH
// It's blocky, not so useful

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

float hash( vec2 p ) // replace this by something better
{
    p = 50.0 * fract( p * 0.3183099 + vec2( 0.71, 0.113 ) );
    return -1.0 + 2.0 * fract( p.x * p.y * ( p.x + p.y ) );
}

float noise( in vec2 st )
{
    vec2 i = floor( st );
    vec2 f = fract( st );

    // Four corners of a tile
    float a = hash( i );
    float b = hash( i + vec2( 1.0, 0.0 ) );
    float c = hash( i + vec2( 0.0, 1.0 ) );
    float d = hash( i + vec2( 1.0, 1.0 ) );

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f * f * ( 3.0 - 2.0 * f );

    // Mix 4 coorners percentages
    return mix( mix( a, b, u.x ), mix( c, d, u.x ), u.y );
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
        st *= 8.0;

        // m need not be orthogonal
        mat2 m = mat2( 1.6, 1.2, -1.2, 1.6 );
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
