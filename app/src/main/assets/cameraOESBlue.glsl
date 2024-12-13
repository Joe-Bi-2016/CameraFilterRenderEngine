#version 100

#extension GL_OES_EGL_image_external : require
uniform samplerExternalOES sampler;
varying	mediump vec2 txtCoord;

void main()
{
	vec4 sample0, sample1, sample2, sample3;
    float blurStep = 0.5;
    float step = blurStep / 100.0;
    sample0 = texture2D(sampler, vec2(txtCoord.x - step, txtCoord.y - step));
    sample1 = texture2D(sampler, vec2(txtCoord.x + step, txtCoord.y + step));
    sample2 = texture2D(sampler, vec2(txtCoord.x + step, txtCoord.y - step));
    sample3 = texture2D(sampler, vec2(txtCoord.x - step, txtCoord.y + step));
    gl_FragColor = (sample0 + sample1 + sample2 + sample3) / 4.0;
}