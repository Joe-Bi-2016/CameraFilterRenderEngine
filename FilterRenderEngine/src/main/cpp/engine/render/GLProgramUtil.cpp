//
// Created by Joe.Bi on 2024/11/8.
//

#include "GLProgramUtil.h"
#include "eglcore/EglConfig.h"
#include "util/LogUtil.h"
#include <assert.h>
#include <stdlib.h>
#include <cstring>
#include <GLES2/gl2ext.h>

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    GLuint GLProgramUtil::loadShader(GLenum shaderType, const char* pSource)
    {
        GLuint shader = 0;
        FUN_BEGIN_TIME("GLProgramUtil::LoadShader")
        shader = glCreateShader(shaderType);
        if (shader)
        {
            glShaderSource(shader, 1, &pSource, NULL);
            glCompileShader(shader);
            GLint compiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
            if (!compiled)
            {
                GLint infoLen = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
                if (infoLen)
                {
                    char* buf = (char*) malloc((size_t)infoLen);
                    if (buf)
                    {
                        glGetShaderInfoLog(shader, infoLen, NULL, buf);
                        LOGERROR("GLProgramUtil::LoadShader Could not compile shader %s:\n%s\n", shaderType == GL_VERTEX_SHADER ? "Vertex " : "Frag ", buf);
                        free(buf);
                    }
                    glDeleteShader(shader);
                    shader = 0;
                }
            }
        }
        FUN_END_TIME("GLProgramUtil::LoadShader")
        return shader;
    }

    //------------------------------------------------------------------------------------//
    GLuint GLProgramUtil::createProgram(const char* pVertexShaderSource, const char* pFragShaderSource)
    {
        GLuint program = 0;
        GLuint vertexShaderHandle = 0;
        GLuint fragShaderHandle = 0;

        FUN_BEGIN_TIME("GLProgramUtil::CreateProgram")
        vertexShaderHandle = loadShader(GL_VERTEX_SHADER, pVertexShaderSource);
        if (!vertexShaderHandle)
            return program;

        fragShaderHandle = loadShader(GL_FRAGMENT_SHADER, pFragShaderSource);
        if (!fragShaderHandle)
            return program;

        program = glCreateProgram();
        if (program)
        {
            glAttachShader(program, vertexShaderHandle);
            checkGLError("glAttachShader");

            glAttachShader(program, fragShaderHandle);
            checkGLError("glAttachShader");

            glLinkProgram(program);

            GLint linkStatus = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

            glDetachShader(program, vertexShaderHandle);
            glDeleteShader(vertexShaderHandle);
            vertexShaderHandle = 0;

            glDetachShader(program, fragShaderHandle);
            glDeleteShader(fragShaderHandle);
            fragShaderHandle = 0;

            if (linkStatus != GL_TRUE)
            {
                GLint bufLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
                if (bufLength)
                {
                    char* buf = (char*) malloc((size_t)bufLength);
                    if (buf)
                    {
                        glGetProgramInfoLog(program, bufLength, NULL, buf);
                        LOGERROR("GLProgramUtil::CreateProgram Could not link program:\n%s\n", buf);
                        free(buf);
                    }
                }
                glDeleteProgram(program);
                program = 0;
            }
        }
        FUN_END_TIME("GLProgramUtil::CreateProgram")
        LOGINFO("GLProgramUtil::CreateProgram program = %d", program);
        return program;
    }

    //------------------------------------------------------------------------------------//
    GLuint GLProgramUtil::createProgramWithFeedback(const char* pVertexShaderSource, const char* pFragShaderSource, GLchar const** varying, int varyingCount)
    {
        GLuint program = 0;
        GLuint vertexShaderHandle = 0;
        GLuint fragShaderHandle = 0;

        FUN_BEGIN_TIME("GLProgramUtil::CreateProgramWithFeedback")
        vertexShaderHandle = loadShader(GL_VERTEX_SHADER, pVertexShaderSource);
        if (!vertexShaderHandle)
            return program;

        fragShaderHandle = loadShader(GL_FRAGMENT_SHADER, pFragShaderSource);
        if (!fragShaderHandle)
            return program;

        program = glCreateProgram();
        if (program)
        {
            glAttachShader(program, vertexShaderHandle);
            checkGLError("glAttachShader");

            glAttachShader(program, fragShaderHandle);
            checkGLError("glAttachShader");

            //transform feedback
            if(EglConfig::mGLVersion == 3)
                glTransformFeedbackVaryings(program, varyingCount, varying, GL_INTERLEAVED_ATTRIBS);

            glLinkProgram(program);

            GLint linkStatus = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

            glDetachShader(program, vertexShaderHandle);
            glDeleteShader(vertexShaderHandle);
            vertexShaderHandle = 0;

            glDetachShader(program, fragShaderHandle);
            glDeleteShader(fragShaderHandle);
            fragShaderHandle = 0;

            if (linkStatus != GL_TRUE)
            {
                GLint bufLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
                if (bufLength)
                {
                    char* buf = (char*) malloc((size_t)bufLength);
                    if (buf)
                    {
                        glGetProgramInfoLog(program, bufLength, NULL, buf);
                        LOGERROR("GLProgramUtil::CreateProgramWithFeedback Could not link program:\n%s\n", buf);
                        free(buf);
                    }
                }
                glDeleteProgram(program);
                program = 0;
            }
        }
        FUN_END_TIME("GLProgramUtil::CreateProgramWithFeedback")
        LOGINFO("GLProgramUtil::CreateProgramWithFeedback program = %d", program);
        return program;
    }

    //------------------------------------------------------------------------------------//
    void GLProgramUtil::deleteProgram(GLuint& program)
    {
        LOGINFO("GLProgramUtil::DeleteProgram");
        if (program)
        {
            glUseProgram(0);
            glDeleteProgram(program);
            program = 0;
        }
    }

    //------------------------------------------------------------------------------------//
    void GLProgramUtil::checkGLError(const char* pGLOperation)
    {
        for (GLint error = glGetError(); error; error = glGetError())
        {
            LOGERROR("GLProgramUtil::CheckGLError GL Operation %s() glError (0x%x)\n", pGLOperation, error);
        }

    }

END