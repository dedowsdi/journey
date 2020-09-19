#version 120

#pragma include ntoy.frag
#pragma include snoise.frag

// https://thebookofshaders.com/edit.php#11/splatter.frag

vec2 cycle(float a0,float b0, float c0, float a1, float b1, float c1, float t )
{
    return vec2( sin( a0 * t + cos( b0 * t ) ) * cos( c0 * t ),
        cos( a1 * t + sin( b1 * t ) ) * sin( c1 * t ) );
}

void main()
{
    vec2 st = gl_FragCoord.xy / resolution;
    st -= 0.5f;

    // animation
    // st = rotate2d( snoise( st ) ) * st +
    //      cycle( 1.3, 2.5, 0.1, 3.6, 5.9, -0.3, osg_SimulationTime * 0.01f ) * 2;

    // big drop
    float color = smoothstep( 0.5, 0.51, snoise( st ) );

    // splatter
    color += smoothstep( 0.6, 0.61, snoise( st * 8 ) );

    // hole
    color -= smoothstep( 0.8, 0.81, snoise( st * 8 ) );

    gl_FragColor = vec4(  1 - color );
}
