#version 100

precision mediump float;

uniform sampler2D texture2;
uniform sampler2D texture1;
uniform lowp float ratio;
varying highp vec4 v_color;
varying	highp vec2 txtCoord;

//ratio:0.0 ~ 2.0

void main()
{
    float R = 1.0 - ratio;
    if(R > 0.0)
    {
        if(txtCoord.x > R)
            gl_FragColor = texture2D(texture2, vec2(txtCoord.x - R, txtCoord.y));
        else
            gl_FragColor = texture2D(texture1, vec2(txtCoord.x + ratio, txtCoord.y));
    }
    else
    {
        if(txtCoord.x < (R + 1.0))
            gl_FragColor = texture2D(texture2, vec2(txtCoord.x - R, txtCoord.y));
        else
            gl_FragColor = texture2D(texture1, vec2(txtCoord.x - (R + 1.0), txtCoord.y));
    }
}
