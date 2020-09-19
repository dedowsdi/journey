#version 120

//https://thebookofshaders.com/12/

#pragma include shader/ntoy.frag

void main()
{
    vec2 st = gl_FragCoord.xy / resolution;
    st *= 8;
    vec2 ist = floor( st );
    vec2 fst = fract( st );

    float min_dist = 1.0;

    // check 3x3 cells for distance filed
    for ( int x = -1; x < 2; x++ )
    {
        for ( int y = -1; y < 2; y++ )
        {
            vec2 neighbor = vec2( x, y );

            // get random point by tile index
            vec2 point = random2( vec2( ist + neighbor ) );

            // animate the point
            point = 0.5 + 0.5 * sin( point * 6 + osg_SimulationTime );

            // get distance2 to this point
            vec2 toPoint = neighbor + point - fst;
            min_dist = min( min_dist, dot( toPoint, toPoint ) );
        }
    }

    min_dist = sqrt( min_dist );

    vec4 color = vec4( 1 - min_dist );

    // draw cell center
    color += step( min_dist, 0.02 );

    // draw grid
    color.x += step( 0.98, fst.x ) + step( 0.98, fst.y );

    // draw isolines
    color -= step( 0.7, sin( min_dist * 32 ) ) * 0.5;;
    gl_FragColor = vec4( color );
}
