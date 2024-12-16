#version 100
precision mediump float;

uniform sampler2D OpenGL;
uniform sampler2D noiseTexture;
uniform float uQuantLevel;   // 2-6
uniform float uWaterPower;   // 8-64
const vec2 texSize = vec2(256., 256.);
varying highp vec4 v_color;
varying	highp vec2 txtCoord;

vec4 quant(vec4 cl, float n)
{
   cl.x = floor(cl.x * 255./n)*n/255.;
   cl.y = floor(cl.y * 255./n)*n/255.;
   cl.z = floor(cl.z * 255./n)*n/255.;
   return cl;
}

void main()
{
   vec4 noiseColor = uWaterPower * texture2D(noiseTexture, txtCoord);
   vec2 newUV = vec2(txtCoord.x + noiseColor.x / texSize.x, txtCoord.y + noiseColor.y / texSize.y);
   vec4 fColor = texture2D(OpenGL, newUV);
   vec4 color = quant(fColor, 255./pow(2., uQuantLevel));
   gl_FragColor = color;
}