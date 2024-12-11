//
// Created by Joe.Bi on 2024/11/14.
//

#include "GLDefaultShader.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    const char* quadVertGLSL = STR(
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
    );

    const char* quadFragGLSL = STR(
        uniform sampler2D sampler;
        varying highp vec4 v_color;
        varying	highp vec2 txtCoord;
        uniform bool usingColor;

        void main()
        {
            if (usingColor)
                gl_FragColor = v_color;
            else
            {
                highp vec4 txtColor = texture2D(sampler, txtCoord);
                gl_FragColor = txtColor;
            }
        }
    );

    const char* quadColorFragGLSL = STR(
        varying highp vec4 v_color;
        void main()
        {
            gl_FragColor = v_color;
        }
    );

END
