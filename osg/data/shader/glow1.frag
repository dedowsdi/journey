#version 120

#pragma include shader/ntoy.frag

void main()
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = gl_FragCoord.xy/resolution;

    
    // Zooms out by a factor of 2.0
    uv *= 2.0;
    // Shifts every axis by -1.0
    uv -= 1.0;

    // Base color for the effect
    vec3 finalColor = vec3 (1., 1., 1. );

    // finalColor *= abs(
    //     0.09 /
    //     ( sin( uv.x + sin( uv.y + osg_SimulationTime * 0 ) * 0.5 ) * 4.0 ) );

    // finalColor *= abs( 0.025 / ( sin( uv.x ) ) );
    finalColor *= abs( 0.025 /  uv.x  ) ;

    gl_FragColor = vec4( finalColor, 1.0 );   
}
