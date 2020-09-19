#version 120
// K a l e i d o s c o p e
// by Matteo Basei
// http://www.matteo-basei.it
// https://www.youtube.com/c/matteobasei

#pragma include shader/ntoy.frag


void main(void)
{
    vec2 st = gl_FragCoord.xy / resolution;
    st = st * 2 - 1;
    // float f = 1 / ( min( abs( st.x - st.y ), abs( st.y + st.x ) ) * 15.5 );
    // float f = 1 / ( max( abs( st.x - st.y ), abs( st.y + st.x ) ) * 15.5 );
    // float f = 1 / ( abs( st.x - st.y ) * abs( st.y + st.x )  * 15.5 );
    // float f = 1 / ( ( abs( st.x - st.y ) + abs( st.y + st.x ) ) * 5.5 );
    float f = 1 / ( abs(st.x * st.y) * 55 );
    vec3 color = vec3( 0.0, 0.3, 0.5 );
    gl_FragColor = vec4( vec3( f * color ), 1.0 );
}
