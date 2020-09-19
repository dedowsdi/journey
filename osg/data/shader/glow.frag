#version 120

#pragma include shader/ntoy.frag

void main()
{
    vec2 st = gl_TexCoord[0].xy;

    st *= 32;
    ivec2 ist = ivec2( floor( st ) );
    ist %= 4;
    if ( ist.x < 3 || ist.y < 3 )
    {
        gl_FragColor = vec4( 0, 0, 0, 1 );
        return;
    }

    st = 2 * fract( st ) - 1;

    float l = length( st );

    // float a = 0.2 + 0.2 * exp( -abs( sin( osg_SimulationTime * 0.5 ) ) * 8 );
    float blink_time_percent = 0.25;
    float time_scale = 0.25;
    float t = fract( osg_SimulationTime * time_scale );
    float a = 0.1 + 0.3 * sin( t * PI / blink_time_percent ) *
                        step( t, blink_time_percent );

    // circle
    float f = step( l, 1 ) * a * 1.2 / l;

    // make sure edge is always 0
    f *= smoothstep( 1, 0, l );

    // draw border
    vec2 edge = step( vec2( -1 ), st ) * step( st, vec2( -0.9 ) );
    edge += step( vec2( 0.9 ), st ) * step( st, vec2( 1 ) );
    f += edge.x + edge.y;

    vec4 color = vec4( f, f, f, 1 );
    gl_FragColor = color;
}
