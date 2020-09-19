// https://thebookofshaders.com/10/

float random( float v ) { return fract( sin( v ) * 100000 ); }

float random( vec2 v )
{
    return fract( sin( dot( v, vec2( 12.9898, 78.233 ) ) ) * 43758.5453123 );
}

