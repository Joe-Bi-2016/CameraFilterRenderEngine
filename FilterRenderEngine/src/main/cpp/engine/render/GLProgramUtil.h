//
// Created by Joe.Bi on 2024/11/8.
//

#ifndef __GLProgramUtil_h__
#define __GLProgramUtil_h__
#include "base/Uncopyable.hpp"
#include <GLES3/gl3.h>
#include <GLES2/gl2.h>
#include <string>
#include <glm.hpp>
//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    #define MATH_PI 3.1415926535897932384626433832802

    //------------------------------------------------------------------------------------//
    class API_EXPORTS GLProgramUtil
    {
    public:
        GLProgramUtil(void) { }
        ~GLProgramUtil(void) { }

        static GLuint loadShader(GLenum shaderType, const char* pSource);

        static GLuint createProgram(const char* pVertexShaderSource, const char* pFragShaderSource);

        static GLuint createProgramWithFeedback(
                const char* pVertexShaderSource,
                const char* pFragShaderSource,
                const GLchar** varying,
                int varyingCount);

        static void deleteProgram(GLuint& program);

        static void checkGLError(const char* pGLOperation);

        static void setBool(GLuint programId, const std::string& name, bool value)
        {
            glUniform1i(glGetUniformLocation(programId, name.c_str()), (int) value);
        }

        static void setInt(GLuint programId, const std::string& name, int value)
        {
            glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
        }

        static void setFloat(GLuint programId, const std::string& name, float value)
        {
            glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
        }

        static void setVec2(GLuint programId, const std::string& name, const glm::vec2 &value)
        {
            glUniform2fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
        }

        static void setVec2(GLuint programId, const std::string& name, float x, float y)
        {
            glUniform2f(glGetUniformLocation(programId, name.c_str()), x, y);
        }

        static void setVec3(GLuint programId, const std::string& name, const glm::vec3 &value)
        {
            glUniform3fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
        }

        static void setVec3(GLuint programId, const std::string& name, float x, float y, float z)
        {
            glUniform3f(glGetUniformLocation(programId, name.c_str()), x, y, z);
        }

        static void setVec4(GLuint programId, const std::string& name, const glm::vec4 &value)
        {
            glUniform4fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
        }

        static void setVec4(GLuint programId, const std::string& name, float x, float y, float z, float w)
        {
            glUniform4f(glGetUniformLocation(programId, name.c_str()), x, y, z, w);
        }

        static void setMat2(GLuint programId, const std::string& name, const glm::mat2 &mat)
        {
            glUniformMatrix2fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

        static void setMat3(GLuint programId, const std::string& name, const glm::mat3 &mat)
        {
            glUniformMatrix3fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

        static void setMat4(GLuint programId, const std::string& name, const glm::mat4 &mat)
        {
            glUniformMatrix4fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

    };

END

#endif //__GLProgramUtil_h__
