#version 430 core

#define PI 3.1415926535897932384626433832795
#define TWO_PI 6.283185
#define patan( y, x ) ( atan( -y, -x ) + PI )

uniform vec2 resolution;
uniform float time;
out vec4 frag_color;

// x*(1-x) is a special function that's symmetry at x = 0.5, it also get it's
// max value at x = 0.5
vec2 rect_test( vec2 st ) { return st * ( 1 - st ); }

// create rect in screen center.
float rect( vec2 st, vec2 size, float smoothedge )
{
    vec2 t0 = rect_test( st );
    vec2 max_rect_st = ( 1 - size ) / 2;
    vec2 t1 = rect_test(max_rect_st);
    vec2 uv = smoothstep( t1, t1 + smoothedge, t0 );
    return uv.x * uv.y;
}

// create rect in random position and size
float rect( vec2 st, vec4 p, float smoothedge )
{
    vec2 t0 = smoothstep( p.xy, p.xy + smoothedge, st );
    vec2 t1 = 1 - smoothstep( p.zw - smoothedge, p.zw, st );
    return t0.x * t0.y * t1.x * t1.y;
}

float circle( vec2 st, float radius, float smoothedge2 )
{
    vec2 ray = st - vec2( 0.5 );
    float radius2 = radius * radius;
    float ray_length2 = dot( ray, ray );
    return smoothstep( ray_length2, ray_length2 + smoothedge2, radius2 );
}

float polygon(
    vec2 st, int num_sides, float center_edge_dist, float smoothedge )
{
    // angle of current pixel
    float a = patan( st.y, st.x );

    // radian per side
    float r = TWO_PI / float( num_sides );

    // Shaping function that modulate the distance
    float d = cos( floor( .5 + a / r ) * r - a ) * length( st );

    return 1 - smoothstep( center_edge_dist - smoothedge, center_edge_dist, d );
}

float fan( vec2 cst, float radius, float theta, float smoothedge )
{
    float t = atan( -cst.y, -cst.x ) + PI;
    float l = length( cst );
    return step( t, theta ) * smoothstep( l, l + smoothedge, radius );
}

mat2 rotate2d( float a )
{
    float c = cos( a );
    float s = sin( a );
    return mat2( c, s, -s, c );
}

vec2 tile( vec2 st, int x, int y )
{
    st.x *= x;
    st.y *= y;
    return fract( st );
}

float random( vec2 st )
{
    return fract( sin( dot( st, vec2( 12.9898, 78.233 ) ) ) * 43758.5453123 );
}

float random( float f ) { return fract( sin( f ) * 10000 ); }

float noise( float v )
{
    float i = floor( v );
    float f = fract( v );
    return mix( random( i ), random( i + 1 ), smoothstep( 0, 1, f ) );
}

float noise( vec2 st )
{
    vec2 i = floor( st );
    vec2 f = fract( st );

    // Four corners in 2D of a tile
    float a = random( i );
    float b = random( i + vec2( 1.0, 0.0 ) );
    float c = random( i + vec2( 0.0, 1.0 ) );
    float d = random( i + vec2( 1.0, 1.0 ) );

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f * f * ( 3.0 - 2.0 * f );
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    // return mix( mix( a, b, u.x ), mix(c, d, u.x), u.y );
    return mix( a, b, u.x ) + ( c - a ) * u.y * ( 1.0 - u.x ) +
           ( d - b ) * u.x * u.y;
}

float plot( vec2 st, float y, float width )
{
    float half_width = width * 0.5f;
    return smoothstep( y - half_width, y, st.y ) -
           smoothstep( y, y + half_width, st.y );
}

void main( void ) {}

