#version 120

#pragma include ntoy.frag
#pragma include snoise.frag

varying float lightIntensity;
varying vec3 pos;

uniform vec3 marbleColor = vec3( 0.7 );
uniform vec3 veinColor = vec3( 0, 0.15, 0 );

#define OCTAVES 4
float fnoise( vec3 p )
{
    float f = 0;
    float amp = 0.5;
    float scale = 2;

    // you can animate scale to make it waving. Althouth there is the sin artifact
    // float t= osg_SimulationTime;
    // scale = 2 + 0.25 * sin(osg_SimulationTime);

    // extra parameter, just tweak it.
    float offset = -0;
    for ( int i = 0; i < OCTAVES; i++ )
    {
        f += amp * snoise( p ) + offset;
        p *= scale;
        amp *= 0.5;
        offset *= 0.5;
    }
    return f;
}

void main( void )
{
    float f = fnoise( pos );

    // interesting result 0
    // f = sin( 64 * f );

    // interesting result 1
    f = sin( sin(pos.z * 4) + 1.6 * f );

    f = f * 0.5 + 0.5;

    // make the vein grow
    // f = pow(f, -0.1);

    // control function
    f = pow(f, 0.6);


    vec3 color = mix( veinColor, marbleColor, f );

    gl_FragColor = vec4( color * lightIntensity, 1 );
}
