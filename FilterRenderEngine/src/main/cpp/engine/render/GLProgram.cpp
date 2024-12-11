//
// Created by Joe.Bi on 2024/11/9.
//

#include "GLProgram.h"
#include "util/LogUtil.h"
#include <GLES2/gl2ext.h>
#include <assert.h>

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    static int getTypeSize(GLenum type, int typeN)
    {
        int n = typeN;
        switch (type)
        {
            case GL_BOOL:
            case GL_SAMPLER_2D:
            case GL_SAMPLER_EXTERNAL_OES:
            case GL_FLOAT:
            case GL_INT:
            case GL_UNSIGNED_INT:
                n = typeN * 1;
                break;

            case GL_FLOAT_VEC2:
            case GL_INT_VEC2:
            case GL_UNSIGNED_INT_VEC2:
                n = typeN * 2;
                break;

            case GL_FLOAT_VEC3:
            case GL_INT_VEC3:
            case GL_UNSIGNED_INT_VEC3:
                n = typeN * 3;
                break;

            case GL_FLOAT_VEC4:
            case GL_INT_VEC4:
            case GL_UNSIGNED_INT_VEC4:
            case GL_FLOAT_MAT2:
                n = typeN * 4;
                break;

            case GL_FLOAT_MAT3:
                n = typeN * 9;
                break;

            case GL_FLOAT_MAT4:
                n = typeN * 16;
                break;

            case GL_FLOAT_MAT2x3:
            case GL_FLOAT_MAT3x2:
                n = typeN * 6;
                break;

            case GL_FLOAT_MAT2x4:
            case GL_FLOAT_MAT4x2:
                n = typeN * 8;
                break;

            case GL_FLOAT_MAT3x4:
            case GL_FLOAT_MAT4x3:
                n = typeN * 12;
                break;
        }

        return n;
    }

    //------------------------------------------------------------------------------------//
    GLProgram::GLProgram(const char* pVertexShaderSource, const char* pFragShaderSource)
    : mProgramId(0)
    , mAttribute()
    , mUniform()
    , mParameters()
    , mSamplersName()
    {
        mProgramId = GLProgramUtil::createProgram(pVertexShaderSource, pFragShaderSource);
        retrievalAttribute();
        retrievalUniform();
    }

    //------------------------------------------------------------------------------------//
    GLProgram::GLProgram(const char* pVertexShaderSource,
                         const char* pFragShaderSource,
                         const GLchar** varying,
                         int varyingCount)
    : mProgramId(0)
    , mAttribute()
    , mUniform()
    , mParameters()
    , mSamplersName()
    {
        mProgramId = GLProgramUtil::createProgramWithFeedback(pVertexShaderSource, pFragShaderSource, varying, varyingCount);
        retrievalAttribute();
        retrievalUniform();
    }

    //------------------------------------------------------------------------------------//
    GLProgram::GLProgram(GLProgram&& program)
    {
        mProgramId = program.mProgramId;
        mAttribute.swap(program.mAttribute);
        mUniform.swap(program.mUniform);
        mParameters.swap(program.mParameters);
        mSamplersName.swap(program.mSamplersName);

        program.mProgramId = 0;
        program.mAttribute.clear();
        program.mUniform.clear();
        program.mParameters.clear();
        program.mSamplersName.clear();
    }

    //------------------------------------------------------------------------------------//
    GLProgram::~GLProgram(void)
    {
        if(glIsProgram(mProgramId))
            glDeleteProgram(mProgramId);

        mProgramId = 0;

        mAttribute.clear();
        mUniform.clear();
        mParameters.clear();

        mSamplersName.clear();
    }

    //------------------------------------------------------------------------------------//
    GLProgram& GLProgram::operator=(const GLProgram& program)
    {
        if(glIsProgram(program.mProgramId))
        {
            if (glIsProgram(mProgramId))
                glDeleteProgram(mProgramId);

            mProgramId = program.mProgramId;
            mAttribute = program.mAttribute;
            mUniform = program.mUniform;
            mParameters = program.mParameters;
        }
        else
            LOGERROR("parameter is wrong!");

        return *this;
    }

    //------------------------------------------------------------------------------------//
    GLProgram& GLProgram::operator=(GLProgram&& program)
    {
        if(glIsProgram(program.mProgramId))
        {
            if(glIsProgram(mProgramId))
                glDeleteProgram(mProgramId);

            mProgramId = program.mProgramId;
            mAttribute.swap(program.mAttribute);
            mUniform.swap(program.mUniform);
            mParameters.swap(program.mParameters);
            mSamplersName.swap(program.mSamplersName);
            program.mProgramId = 0;
            program.mAttribute.clear();
            program.mUniform.clear();
            program.mParameters.clear();
            program.mSamplersName.clear();
        }
        else
            LOGERROR("parameter is wrong!");

        return *this;
    }

    //------------------------------------------------------------------------------------//
    sharedProgram GLProgram::createGLProgram(const char* pVertexShaderSource, const char* pFragShaderSource)
    {
        return sharedProgram(new GLProgram(pVertexShaderSource, pFragShaderSource), deleter<GLProgram>);
    }

    //------------------------------------------------------------------------------------//
    sharedProgram GLProgram::createGLProgram(const char* pVertexShaderSource,
                                            const char* pFragShaderSource,
                                            const GLchar** varying,
                                            int varyingCount)
    {
        return sharedProgram(new GLProgram(pVertexShaderSource, pFragShaderSource, varying, varyingCount), deleter<GLProgram>);
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::retrievalAttribute(void)
    {
        GLint total = 0, i = 0;
        glGetProgramiv(mProgramId, GL_ACTIVE_ATTRIBUTES, &total);
        if (total > 0)
        {
            while (i != total)
            {
                char name[64] = {0};
                ShaderTypeData data;
                glGetActiveAttrib(mProgramId, i++, 64, NULL, &data.n, &data.type, name);
                data.index = glGetAttribLocation(mProgramId, name);
                data.n = getTypeSize(data.type, data.n);
                mAttribute.insert(valueType(name, std::move(data)));
            }
        }
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::retrievalUniform(void)
    {
        GLint total = 0, i = 0;
        glGetProgramiv(mProgramId, GL_ACTIVE_UNIFORMS, &total);
        if (total > 0)
        {
            while (i != total)
            {
                char name[64] = {0};
                ShaderTypeData data;
                glGetActiveUniform(mProgramId, i++, 64, NULL, &data.n, &data.type, name);
                data.index = glGetUniformLocation(mProgramId, name);
                mUniform.insert(valueType(name, std::move(data)));

                if(data.type == GL_SAMPLER_2D || data.type == GL_SAMPLER_EXTERNAL_OES)
                    mSamplersName.push_back(name);
            }
        }
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::setUniform(const std::string& name, bool* value, int n)
    {
        conIterator it = mParameters.find(name);
        if(it == mParameters.end())
        {
            ShaderTypeData data;
            data.bytesize = sizeof(int) * n;
            data.n = n;
            data.type = GL_BOOL;
            data.val = safeGlobalAlloc(data.bytesize);
            for (int i = 0; i < n; ++i)
                ((int *) data.val)[i] = value[i];

            mParameters.insert(valueType(name, std::move(data)));
        }
        else
        {
            for (int i = 0; i < n; ++i)
                ((int *) it->second.val)[i] = value[i];
        }
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::setUniform(const std::string& name, int* value, int n)
    {
        conIterator it = mParameters.find(name);
        if(it == mParameters.end())
        {
            ShaderTypeData data;
            data.bytesize = sizeof(int) * n;
            data.n = n;
            data.type = GL_INT;
            data.val = safeGlobalAlloc(data.bytesize);
            memcpy(data.val, value, data.bytesize);
            mParameters.insert(valueType(name, std::move(data)));
        }
        else
            memcpy(it->second.val, value, sizeof(int) * n);
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::setUniform(const std::string& name, float* value, int n)
    {
        conIterator it = mParameters.find(name);
        if(it == mParameters.end())
        {
            ShaderTypeData data;
            data.bytesize = sizeof(float) * n;
            data.n = n;
            data.type = GL_FLOAT;
            data.val = safeGlobalAlloc(data.bytesize);
            memcpy(data.val, value, data.bytesize);
            mParameters.insert(valueType(name, std::move(data)));
        }
        else
            memcpy(it->second.val, value, sizeof(float) * n);
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::setUniform(const std::string& name, const glm::vec2* value, int n)
    {
        conIterator it = mParameters.find(name);
        if(it == mParameters.end())
        {
            ShaderTypeData data;
            data.bytesize = sizeof(glm::vec2) * n;
            data.n = n;
            data.type = GL_FLOAT_VEC2;
            data.val = safeGlobalAlloc(data.bytesize);
            memcpy(data.val, &(value[0].x), data.bytesize);
            mParameters.insert(valueType(name, std::move(data)));
        }
        else
            memcpy(it->second.val, &(value[0].x), sizeof(glm::vec2) * n);
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::setUniform(const std::string& name, float x, float y)
    {
        conIterator it = mParameters.find(name);
        if(it == mParameters.end())
        {
            ShaderTypeData data;
            data.bytesize = sizeof(float) * 2;
            data.n = 1;
            data.type = GL_FLOAT_VEC2;
            data.val = safeGlobalAlloc(data.bytesize);
            ((float *) data.val)[0] = x;
            ((float *) data.val)[1] = y;
            mParameters.insert(valueType(name, std::move(data)));
        }
        else
        {
            ((float*)it->second.val)[0] = x;
            ((float*)it->second.val)[1] = y;
        }
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::setUniform(const std::string& name, const glm::vec3* value, int n)
    {
        conIterator it = mParameters.find(name);
        if(it == mParameters.end())
        {
            ShaderTypeData data;
            data.bytesize = sizeof(glm::vec3) * n;
            data.n = n;
            data.type = GL_FLOAT_VEC3;
            data.val = safeGlobalAlloc(data.bytesize);
            memcpy(data.val, &(value[0].x), data.bytesize);
            mParameters.insert(valueType(name, std::move(data)));
        }
        else
            memcpy(it->second.val, &(value[0].x), sizeof(glm::vec3) * n);
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::setUniform(const std::string& name, float x, float y, float z)
    {
        conIterator it = mParameters.find(name);
        if(it == mParameters.end())
        {
            ShaderTypeData data;
            data.bytesize = sizeof(float) * 3;
            data.n = 1;
            data.type = GL_FLOAT_VEC3;
            data.val = safeGlobalAlloc(data.bytesize);
            ((float *) data.val)[0] = x;
            ((float *) data.val)[1] = y;
            ((float *) data.val)[2] = z;
            mParameters.insert(valueType(name, std::move(data)));
        }
        else
        {
            ((float*)it->second.val)[0] = x;
            ((float*)it->second.val)[1] = y;
            ((float*)it->second.val)[2] = z;
        }
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::setUniform(const std::string& name, const glm::vec4* value, int n)
    {
        conIterator it = mParameters.find(name);
        if(it == mParameters.end())
        {
            ShaderTypeData data;
            data.bytesize = sizeof(glm::vec4) * n;
            data.n = n;
            data.type = GL_FLOAT_VEC4;
            data.val = safeGlobalAlloc(data.bytesize);
            memcpy(data.val, &(value[0].x), data.bytesize);
            mParameters.insert(valueType(name, std::move(data)));
        }
        else
            memcpy(it->second.val, &(value[0].x), sizeof(glm::vec4) * n);
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::setUniform(const std::string& name, float x, float y, float z, float w)
    {
        conIterator it = mParameters.find(name);
        if(it == mParameters.end())
        {
            ShaderTypeData data;
            data.bytesize = sizeof(float) * 4;
            data.n = 1;
            data.type = GL_FLOAT_VEC4;
            data.val = safeGlobalAlloc(data.bytesize);
            ((float *) data.val)[0] = x;
            ((float *) data.val)[1] = y;
            ((float *) data.val)[2] = z;
            ((float *) data.val)[3] = w;
            mParameters.insert(valueType(name, std::move(data)));
        }
        else
        {
            ((float*)it->second.val)[0] = x;
            ((float*)it->second.val)[1] = y;
            ((float*)it->second.val)[2] = z;
            ((float*)it->second.val)[3] = w;
        }
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::setUniform(const std::string& name, const glm::mat2* mat, int n)
    {
        conIterator it = mParameters.find(name);
        if(it == mParameters.end())
        {
            ShaderTypeData data;
            data.bytesize = sizeof(glm::mat2) * n;
            data.n = n;
            data.type = GL_FLOAT_MAT2;
            data.val = safeGlobalAlloc(data.bytesize);
            memcpy(data.val, &(mat[0][0].x), data.bytesize);
            mParameters.insert(valueType(name, std::move(data)));
        }
        else
            memcpy(it->second.val, &(mat[0][0].x), sizeof(glm::mat2) * n);
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::setUniform(const std::string& name, const glm::mat3* mat, int n)
    {
        conIterator it = mParameters.find(name);
        if(it == mParameters.end())
        {
            ShaderTypeData data;
            data.bytesize = sizeof(glm::mat3) * n;
            data.n = n;
            data.type = GL_FLOAT_MAT3;
            data.val = safeGlobalAlloc(data.bytesize);
            memcpy(data.val, &(mat[0][0].x), data.bytesize);
            mParameters.insert(valueType(name, std::move(data)));
        }
        else
            memcpy(it->second.val, &(mat[0][0].x), sizeof(glm::mat3) * n);
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::setUniform(const std::string& name, const glm::mat4* mat, int n)
    {
        conIterator it = mParameters.find(name);
        if(it == mParameters.end())
        {
            ShaderTypeData data;
            data.bytesize = sizeof(glm::mat4) * n;
            data.n = n;
            data.type = GL_FLOAT_MAT4;
            data.val = safeGlobalAlloc(data.bytesize);
            memcpy(data.val, &(mat[0][0].x), data.bytesize);
            mParameters.insert(valueType(name, std::move(data)));
        }
        else
            memcpy(it->second.val, &(mat[0][0].x), sizeof(glm::mat4) * n);
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::setAttribute(const std::string& name, void* pointer, int component, GLenum type, bool normalized, int stride)
    {
        iterator it = mAttribute.find(name);
        if(it != mAttribute.end())
        {
            ShaderTypeData& data = it->second;
            glEnableVertexAttribArray(data.index);
            glVertexAttribPointer(data.index, component, type, normalized, stride, pointer);
        }
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::setAttribute(const std::string& name, GLint vbo, int component, GLenum type, bool normalized, int stride)
    {
        iterator it = mAttribute.find(name);
        if(it != mAttribute.end())
        {
            ShaderTypeData& data = it->second;
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glVertexAttribPointer(data.index, component, type, normalized, stride, 0);
            glEnableVertexAttribArray(data.index);
        }
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::useProgram(void)
    {
        glUseProgram(mProgramId);
    }

    //------------------------------------------------------------------------------------//
    void GLProgram::settingUniformVal(void)
    {
        iterator it = mUniform.begin();
        for (; it != mUniform.end(); ++it)
        {
            ShaderTypeData& uniformData = it->second;
            iterator paramit = mParameters.find(it->first);
            if(paramit == mParameters.end())
                continue;
            ShaderTypeData& paramData = paramit->second;
            switch (uniformData.type)
            {
                case GL_BOOL:
                case GL_BYTE:
                case GL_UNSIGNED_BYTE:
                case GL_INT:
                case GL_SAMPLER_2D:
                case GL_SAMPLER_EXTERNAL_OES:
                {
                    if (uniformData.n == 1)
                        glUniform1i(uniformData.index, *(GLint*)paramData.val);
                    else
                        glUniform1iv(uniformData.index, uniformData.n, (GLint*)paramData.val);
                }
                break;

                case GL_FLOAT:
                {
                    if (uniformData.n == 1)
                        glUniform1f(uniformData.index, *(GLfloat*)paramData.val);
                    else
                        glUniform1fv(uniformData.index, uniformData.n, (GLfloat*)paramData.val);
                }
                break;

                case GL_INT_VEC2:
                {
                    if (uniformData.n == 1)
                        glUniform2i(uniformData.index, ((GLint*)paramData.val)[0], ((GLint*)paramData.val)[1]);
                    else
                        glUniform2iv(uniformData.index, uniformData.n, (GLint*)paramData.val);
                }
                break;

                case GL_FLOAT_VEC2:
                {
                    if (uniformData.n == 1)
                        glUniform2f(uniformData.index, ((GLfloat*)paramData.val)[0], ((GLfloat*)paramData.val)[1]);
                    else
                        glUniform2fv(uniformData.index, uniformData.n, (GLfloat*)paramData.val);
                }
                break;

                case GL_INT_VEC3:
                {
                    if (uniformData.n == 1)
                        glUniform3i(uniformData.index, ((GLint*)paramData.val)[0], ((GLint*)paramData.val)[1], ((GLint*)paramData.val)[2]);
                    else
                        glUniform3iv(uniformData.index, uniformData.n, (GLint*)paramData.val);
                }
                break;

                case GL_FLOAT_VEC3:
                {
                    if (uniformData.n == 1)
                        glUniform3f(uniformData.index, ((GLfloat*)paramData.val)[0], ((GLfloat*)paramData.val)[1], ((GLfloat*)paramData.val)[2]);
                    else
                        glUniform3fv(uniformData.index, uniformData.n, (GLfloat*)paramData.val);
                }
                break;

                case GL_INT_VEC4:
                {
                    if (uniformData.n == 1)
                        glUniform4i(uniformData.index, ((GLint*)paramData.val)[0], ((GLint*)paramData.val)[1], ((GLint*)paramData.val)[2], ((GLint*)paramData.val)[3]);
                    else
                        glUniform4iv(uniformData.index, uniformData.n, (GLint*)paramData.val);
                }
                break;

                case GL_FLOAT_VEC4:
                {
                    if (uniformData.n == 1)
                        glUniform4f(uniformData.index, ((GLfloat*)paramData.val)[0], ((GLfloat*)paramData.val)[1], ((GLfloat*)paramData.val)[2], ((GLfloat*)paramData.val)[3]);
                    else
                        glUniform4fv(uniformData.index, uniformData.n, (GLfloat*)paramData.val);
                }
                break;

                case GL_FLOAT_MAT2:
                        glUniformMatrix2fv(uniformData.index, uniformData.n, GL_FALSE, (GLfloat*)paramData.val);
                break;

                case GL_FLOAT_MAT3:
                    glUniformMatrix3fv(uniformData.index, uniformData.n, GL_FALSE, (GLfloat*)paramData.val);
                break;

                case GL_FLOAT_MAT4:
                    glUniformMatrix4fv(uniformData.index, uniformData.n, GL_FALSE, (GLfloat*)paramData.val);
                break;

                default:
                    break;
            }
        }
    }

END
