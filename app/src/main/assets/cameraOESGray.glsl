#version 100

#extension GL_OES_EGL_image_external : require
uniform samplerExternalOES sampler;
varying	mediump vec2 txtCoord;

void main()
{
    mediump vec4 txtColor = texture2D(sampler, txtCoord);
    float fGrayColor = 0.3*txtColor .r + 0.59*txtColor .g + 0.11*txtColor .b;
    gl_FragColor = vec4(fGrayColor, fGrayColor, fGrayColor, 1.0);
}