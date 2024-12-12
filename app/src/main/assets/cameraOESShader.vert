#version 100

attribute mediump vec3 position;
attribute mediump vec2 texture;
uniform mediump mat4 modelMat;
uniform mediump mat4 txtureMat;
varying mediump vec4 v_color;
varying	mediump vec2 txtCoord;

void main()
{
    gl_Position = modelMat * vec4(position, 1.0);
    txtCoord = (txtureMat * vec4(texture, 1.0, 1.0)).xy; 	 
}