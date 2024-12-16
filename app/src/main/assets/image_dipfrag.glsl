#version 100
precision mediump float;

uniform sampler2D renderTexture;
const vec2 texSize = vec2(256., 256.);
varying highp vec4 v_color;
varying	highp vec2 txtCoord;

vec4 dip_filter(mat3 myfilter, vec2 filter_pos[9], sampler2D image, vec2 xy, vec2 texSize)
{
   vec4 final_color = vec4(0., 0., 0., 0.);
   for(int i = 0; i < 3; i++)
   {
        for(int j  = 0; j < 3; j++)
        {
            vec2 newxy = vec2(xy.x + filter_pos[3*i+j].x, xy.y + filter_pos[3*i+j].y);
            vec2 newuv = vec2(newxy.x / texSize.x, newxy.y / texSize.y);
            final_color += texture2D(image, newuv) * myfilter[i][j];
        }
   }
   return final_color;
}

void main()
{
    vec2 filter_pos_delta[9];
    filter_pos_delta[0] = vec2(-1., -1.);
    filter_pos_delta[1] = vec2(0., -1.);
    filter_pos_delta[2] = vec2(1., -1.);
    filter_pos_delta[3] = vec2(-1., 0.);
    filter_pos_delta[4] = vec2(0., 0.);
    filter_pos_delta[5] = vec2(1., 0.);
    filter_pos_delta[6] = vec2(-1., 1.);
    filter_pos_delta[7] = vec2(0., 1.);
    filter_pos_delta[8] = vec2(1., 1.);

    mat3 myfilter = mat3(
    1./16., 1./8.,1./16.,
    1./8.,1./4.,1./8.,
    1./16.,1./8.,1./16.);

    vec2 xy = vec2(txtCoord.x * texSize.x, txtCoord.y * texSize.y);
    gl_FragColor = dip_filter(myfilter, filter_pos_delta, renderTexture, xy, texSize);
}