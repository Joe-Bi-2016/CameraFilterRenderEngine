#version 100

precision highp float;
precision highp int;

attribute highp vec3 position;
attribute highp vec4 color;
attribute highp vec2 texture;
varying highp vec4 v_color;
varying	highp vec2 txtCoord;

void main()
{
    gl_Position = vec4(position, 1.0);
    v_color = color;
    txtCoord = texture;
}