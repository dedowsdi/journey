#version 120
/*
 * Original idea published by kubiak - this shader is my rendition. Thank you!
 * https://www.shadertoy.com/view/Wsy3zm <------ Original shader
 *  Infinite Supernova from  sevanetrebchenko   ; ST
 *
 * Computed noise instead noise texture;
 * gigatron >> glslsandbox !
 */

#pragma include shader/ntoy.frag
#pragma include shader/cnoise.frag

// Readability
#define globalTime osg_SimulationTime * 0.001
// #define time osg_SimulationTime
#define time 1

// Returns the color value at 'position' coordinate in the provided texture.
float TextureCoordinate( vec2 position )
{
    /*
     * By how much to zoom the texture in. A higher zooming factor will give way
     * to more of the detail of the texture being lost, and will ultimately give
     * way to a smoother, less rippled surface. If using a highly detailed
     * texture (with lots of changes in texture colors on a very small scale,
     * i.e. noisy textures), decreasing the zoom factor will cause more bright
     * spots to be visible.
     */
    float zoomingFactor = 180.0;

    /*
     * The last parameter to texture is the LOD (level of detail), which
     * dictates which mipmap texture level to use for the texture coordinate
     * sampling. A higher LOD results in a lower quality texture. In this case,
     * the maximum quality texture is used.
     */
    float levelOfDetail = 0.0;

    // Get the texture coordinates from the provided channel at the given zoom
    // factor set above.
    return cnoise( position );
}

//  Fractal Brownian Motion (fbm) function
// * Resources used:
// *
// 	- https://thebookofshaders.com/13/
// *
// 	- https://www.iquilezles.org/www/articles/fbm/fbm.htm
// *
// 																											 *
// 	Function aims to mimic fractal noise by increasing the frequencies of
// textures within a given space and  * decreasing the brightness and subsequent
// impact of each sub-texture on the overall color of the pixel.   * Summing up
// the contributions of the several scaled textures gives the appearance of
// randomness and noise.*
float FBM( vec2 uv )
{
// Represents the number of levels sub-textures present in each texture (fractal
// power)
#define fractalDimension 5

    // Represents the number of sub-textures present one one edge of the texture
    // at the next fractal power. If you look at one instance of the texture,
    // there will be (innerPower ^ 2) sub-textures contained within.
    //
    // This effect is easier to see by decreasing zoomingFactor (in
    // textureCoordinate function above) to 1.0, the fractalDimension (above) to
    // 2, and using a more recognizable texture (iChannel1) to see the effect
    // that changing this number has.
    float innerPower = 2.0;

    float noiseValue = 0.0; // Final noise to return at this pixel
    float brightness =
        2.0; // Starting brightness of the first fractal power.
             // Subsequent powers will have lower brightness contributions.
    float dampeningFactor = 2.0; // How much of an impact subsequent fractal
                                 // powers have on the result

    //
    // This value influences what range of colors become brighter than others.
    // This color range is only valid between 0.0 and 1.0. At 0.0, colors in the
    // original texture are inversed - those that are closest to white show up
    // black (inverse). At 1.0, colors stay the same.
    //
    // Having a color offset within [0.0, 1.0] shifts the range of colors'
    // darkness that become bright white spots in the output.
    float offset = 0.5;

    /*
     * This changes how clear the difference between two opposite brightness
     * pixels are. The higher this number is, the clearer the difference is.
     */
    float difference = 2.0;

    for ( int i = 0; i < fractalDimension; ++i )
    {

        /*
         * At this stage we get the color at the texture coordinate and apply
         * the color manipulation that was specified above.
         *
         * Note that the absolute value of the fbm function is used. This is to
         * create a turbulent effect by creating sharp changes in the resulting
         * noise where it would otherwise have little effect.
         */
        noiseValue += abs( ( TextureCoordinate( uv ) - offset ) * difference ) /
                      brightness;

        /*
         * Wave amplitude increases so that fractals at a higher power (smaller
         * textures) have a lesser impact on the resulting noise at the provided
         * uv coordinate.
         */
        brightness *= dampeningFactor;

        // Go one fractal power deeper
        uv *= innerPower;
    }

    return noiseValue;
}

/*************************************************************************************************************
 * Generates noise at the provided UV coordinates by overlapping multiple copies
 *of the same texture.        *
 *************************************************************************************************************/
float turbulence( vec2 uv )
{
    // How fast the tendrils of electricity move around
    float activityLevel = 3.0;

    // A basis is created with two calls to turbulence with texture offsets.
    // These two values move the texture in perpendicular directions and added
    // to the original UV coordinates to create the illusion of movement. The
    // values are added to create more randomness in the generated noise.
    //
    // The first basis shifts the textures along each diagonal.
    // The second basis shifts the textures along the x axis.
    // The third basis shifts the textures along the y axis.
    //
    vec2 noiseBasisDiag = vec2( FBM( uv - globalTime * activityLevel ),
        FBM( uv + globalTime * activityLevel ) );
    // vec2 noiseBasisX = vec2(FBM(vec2(uv.x - globalTime * activityLevel,
    // 0.0)),
    //						   FBM(vec2(uv.x + globalTime *
    // activityLevel, 0.0)));
    // vec2 noiseBasisY = vec2(FBM(vec2(0.0, uv.y - globalTime *
    // activityLevel)),
    //						   FBM(vec2(0.0, uv.y +
    // globalTime
    //* activityLevel)));

    // The resulting vector is offset randomly by the calculated turbulence
    // bases
    uv += noiseBasisDiag;
    // uv -= noiseBasisX;
    // uv += noiseBasisY;

    // Final call to turbulence rotates the entire screen, though this effect is
    // most visible towards the edge
    return FBM( uv * rotate2d( time * 0.5 )  );
}

/*************************************************************************************************************
 * The goal of this function was to create a circle without using sin/cos waves
 *to                           *
 *************************************************************************************************************/
float Ring( vec2 uv )
{
    // float circleRadius = sqrt( length( uv ) );
    float circleRadius = length( uv );

    //  Note: this is easier to visualize if the function is graphed.
    //  Function: y = abs(mod(x, 2.4) - 2.4 / 2) * 7.0 + 0.2
    //
    //  It is easy to approximate this function with a simple cosine/sine wave,
    //  such as the one below: y = 4.3 * cos(2.625 * x) + 4.3 However, as these
    //  functions reach their peaks, the resulting values will result in the
    //  peak white lasting for longer than desired and giving a thicker ring. A
    //  linear up and down gives more flexibility.
    //
    //  - Range:
    //  		The range specifies the distance between circles. A
    //  higher range moves the circle further away from the center.
    //  - Function Slope:
    //  		The function slope specifies how quickly the peak
    //  (white) color is reached and how quickly it falls back to normal. The
    //  higher the slope, the quicker it will be reached, and the thinner the
    //  white portion of the circle will be
    //  - Offset:
    //  		Offsets the function up, reducing how much of the
    //  function can be visible in the peak color. The higher this number is,
    //  the thinner the ring of the circle will be.
    float range = 2.8;
    float functionSlope = 16.14;
    float offset = 0.26;

    return abs( mod( circleRadius, range ) - range / 2. ) * functionSlope +
           offset;
}

void main()
{
    vec2 st = gl_FragCoord.xy / resolution.xy - 0.5;

    float distanceFromCenter = length( st );
    float radius = 0.4;             // Maximum radius of the effect
    float alpha = 1.0;              // Alpha starting value (full brightness)
    float alphaFalloffSpeed = 0.18; // How quickly alpha values fade to 0.0

    /*
     * If the distance of the UV coordinate is further than the desired radius,
     * decrease the alpha to fade the effect out to black along the edges.
     */
    if ( distanceFromCenter > radius )
    {
        alpha = max(
            0.0, 1.0 - ( distanceFromCenter - radius ) / alphaFalloffSpeed );
    }

    if ( alpha == 0.0 )
    {
        discard;
    }

    /*
     * UV Coordinates are zoomed out so that more textures can be visible at one
     * time (since textures are wrapped by repetition). This slows down the
     * speed of the turbulence and gives more precise control over the effect.
     */
    float zoom = 4.0;
    vec2 uvZoomed = st * zoom;

    // Get the cumulative contributions of the fractal at this st coordinate.
    float fractalColor = turbulence( uvZoomed );

    // Scale the color of the current st coordinate based on the distance it is
    // away from the ring.
    // fractalColor *= Ring( uvZoomed );

    // fractalColor = Ring( uvZoomed );

    // The closer the coordinate is to the ring, the brighter the color will be
    // (fractalColor approaches 0).
    vec3 col = normalize( vec3( 0.721, 0.311, 0.165 ) ) / fractalColor;
    col *= alpha;

    // FINAL COLOR
    gl_FragColor = vec4( col, 1. );
}
