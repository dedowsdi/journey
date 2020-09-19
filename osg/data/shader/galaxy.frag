// https://www.shadertoy.com/view/MdXSzS
// The Big Bang - just a small explosion somewhere in a massive Galaxy of Universes.
// Outside of this there's a massive galaxy of 'Galaxy of Universes'... etc etc. :D

// To fake a perspective it takes advantage of the screen being wider than it is tall.

#version 120

#pragma include shader/ntoy.frag

void main()
{
    vec2 uv = ( gl_FragCoord.xy / resolution ) - .5;
    float time = osg_SimulationTime;
    // time = 10;
    float t =
        time * .1 +
        ( ( .25 + .05 * sin( time * .1 ) ) / ( length( uv.xy ) + .07 ) ) * 2.2;
    float si = sin( t );
    float co = cos( t );
    mat2 ma = mat2( co, si, -si, co );

    vec3 color;
    float len = length( uv );

    float s = 0.0;
    vec2 uv2 = uv * ma;
    for ( int i = 0; i < 90; i++ )
    {
        vec3 p = vec3( s * uv2, s - 1.5 - sin( time * .13 ) * .1 );
        p.xy += vec2( 0.22, 0.3 );

        for ( int j = 0; j < 8; j++ ) p = abs( p ) / dot( p, p ) - 0.659;

        // v1 +=
        //     dot( p, p ) * .0015;
        float dotp = dot( p, p );
        color +=
            vec3( dotp * 0.015 * ( 1.8 + sin( len * 13 + .5 - time * 0.2 ) ),
                dotp * 0.013 * ( 1.5 + sin( len * 14.5 +1.2 - time * 0.3 ) ),
                length( p.xy * 10.0 ) * .0003 );
        s += 0.35;
    }

    // set v1 to 0 if it's greater than 0.7
    color.x *= smoothstep( 0.5, .0, len );
    color.y *= smoothstep( 0.7, .0, len );
    color.z *= smoothstep( 0.9, .0, len );

    color = vec3( color.z * ( 1.5 + sin( time * .2 ) * .4 ),
                ( color.x + color.z ) * .3, color.y ) +
            smoothstep( 0.2, .0, len ) * .85 +
            smoothstep( .0, .6, color.z ) * .3;

    color = color + smoothstep( 0.05, 0, len ) * 0.5;

    gl_FragColor = vec4( color, 1.0 );
}
