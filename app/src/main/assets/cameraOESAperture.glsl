#version 100

#extension GL_OES_EGL_image_external : require
precision highp float;
const float PI = 3.1415926535;
uniform samplerExternalOES sampler;
varying	mediump vec2 txtCoord;

void main()
{
	float aperture = 158.0;
    float apertureHalf = 0.5 * aperture * (PI / 180.0);
    float maxFactor = sin(apertureHalf);
    vec2 uv;
    vec2 xy = 2.0 * txtCoord.xy - 1.0;
    float d = length(xy);
    if (d < (2.0 - maxFactor)) 
	{
        d = length(xy * maxFactor);
        float z = sqrt(1.0 - d * d);
        float r = atan(d, z) / PI;
        float phi = atan(xy.y, xy.x);
        uv.x = r * cos(phi) + 0.5;
        uv.y = r * sin(phi) + 0.5;
    } 
	else 
	{
        uv = txtCoord.xy;
    }
	
    gl_FragColor = texture2D(sampler, uv);
}