#version 120

#pragma include shader/ntoy.frag
#pragma include shader/snoise.frag

void main()
{
    vec2 nst = gl_FragCoord.xy / resolution * 2 - 1;

    // move left
    float offset = 0.85;
    vec2 st = vec2( nst.x + offset, nst.y );

    // animation
    st += snoise( st.xx - osg_SimulationTime ) * 0.05;

    float max_height = 0.98;
    float smooth_begin = 0.67;
    float visibility = 0;

    if ( st.x > 0 )
    {
        // draw tail
        float height = mix( max_height, 0, st.x / ( 1 + offset ) );
        visibility = smoothstep( height, height * smooth_begin, abs( st.y ) );
    }
    else
    {
        // draw squeezed round head
        visibility = smoothstep( max_height, max_height * smooth_begin,
            length( vec2( st.x * 8, st.y ) ) );
    }

    // make it glow
    visibility *= 1 + 1 / length( st * 4 );

    // draw some particles, scale x to match aspect ratio, we need round particle.
    int num_particles = 64;
    float duration = 3.5;
    float aspect_ratio = resolution.x / resolution.y;
    vec2 ast = vec2( st.x * aspect_ratio, nst.y );

    // make sure the x part reach tail, y part still visible at the end of it's
    // life time
    vec2 max_acc =
        vec2( ( 1 + offset ) * aspect_ratio, 1 ) * 2 / ( duration * duration );

    for ( int i = 0; i < num_particles; i++ )
    {
        vec2 acc = max_acc * vec2( 1, snoise( vec2( i, i ) ) );
        float t = mod(
            osg_SimulationTime + i * ( duration / num_particles ), duration );
        vec2 pos = 0.5 * acc * t * t ;
        visibility = max( visibility, 1 / ( 8 * length( ast - pos ) ) );
    }

    // fade out tail
    visibility *= smoothstep( 1.3, 0, st.x / ( 1 + offset ) );

    // one of them has to be 0, otherwise there will be white line.
    vec3 color = vec3( 0.00, 0.6, 1.24 ) * visibility;

    gl_FragColor = vec4( color, 1 );
}
