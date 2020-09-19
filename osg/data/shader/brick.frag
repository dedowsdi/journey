#version 120

varying float lightIntensity;
varying vec2 pos;

uniform vec4 mortarColor = vec4( 1, 1, 1, 1 );
uniform vec4 brickColor = vec4( 1, 0.5, 0, 1 );

// mortar size is 1 - brickSize.
uniform vec2 brickSize = vec2( 0.8, 0.7 );


void main( void )
{
    vec2 brickPos = pos;

    // shift neighbor bricks
    // if ( fract( brickPos.x * 0.5 ) > 0.5 )
    //     brickPos.y += 0.5;

    vec2 fpos = fract( brickPos );

    // start brick from left bottom
    // vec2 isBrick = step( fpos, brickSize );

    // centered brick, it's symmetry in all direction if you remove shift
    vec2 centerBrickStart = 0.5 - 0.5 * brickSize;
    vec2 centerBrickEnd = brickSize + centerBrickStart;
    vec2 isBrick = step( centerBrickStart, fpos ) * step( fpos, centerBrickEnd );

    gl_FragColor = mix( mortarColor, brickColor, isBrick.x * isBrick.y ) * lightIntensity;
}
