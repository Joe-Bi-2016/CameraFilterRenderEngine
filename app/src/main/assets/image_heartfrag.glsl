#version 100

precision highp float;

uniform float u_time;//时间偏移量
const vec2 u_screenSize = vec2(1282, 1925);//屏幕尺寸
const float PI = 3.141593;

void main()
{
    // move to center
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 p = (2.0*fragCoord-u_screenSize.xy)/min(u_screenSize.y,u_screenSize.x);

    // background color
    vec3 bcol = vec3(1.0,0.8,0.8)*(1.0-0.38*length(p));

    // animate
    float tt = u_time;
	float ss = pow(tt,.2)*0.5 + 0.5;
	ss = 1.0 + ss*0.5*sin(tt*6.2831*3.0 + p.y*0.5)*exp(-tt*4.0);
	p *= vec2(0.5,1.5) + ss*vec2(0.5,-0.5);

    // shape
    p.y -= 0.25;
    float a = atan(p.x,p.y) / PI;
    float r = length(p);
    float h = abs(a);
    float d = (13.0*h - 22.0*h*h + 10.0*h*h*h)/(6.0-5.0*h);

    // color
    float s = 0.75 + 0.75*p.x;
    s *= 1.0-0.4*r;
    s = 0.3 + 0.7*s;
    s *= 0.5+0.5*pow( 1.0-clamp(r/d, 0.0, 1.0 ), 0.1 );
    vec3 hcol = vec3(1.0,0.5*r,0.3)*s;
    vec3 col = mix( bcol, hcol, smoothstep( -0.06, 0.06, d-r) );

    gl_FragColor = vec4(col,1.0);
}