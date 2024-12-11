//
// Created by Joe.Bi on 2024/11/9.
//

#ifndef __GLProgram_h__
#define __GLProgram_h__
#include "GLProgramUtil.h"
#include "util/GLUtil.h"
#include "GLDataTypeDef.h"
#include <GLES3/gl3.h>
#include <GLES2/gl2.h>
#include <string>
#include <map>
#include <vector>
#include <glm.hpp>

using namespace std;

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    class GLProgram;
    typedef std::shared_ptr<GLProgram>  sharedProgram;

    //------------------------------------------------------------------------------------//
    class API_EXPORTS GLProgram
    {
        friend void deleter<GLProgram>(GLProgram*);
        friend class GLRenderPass;
    public:
        static sharedProgram createGLProgram(const char* pVertexShaderSource, const char* pFragShaderSource);

        static sharedProgram createGLProgram(const char* pVertexShaderSource,
                                            const char* pFragShaderSource,
                                            const GLchar** varying,
                                            int varyingCount);

        GLProgram(const GLProgram&) = delete;
        GLProgram(GLProgram&&);
        GLProgram& operator=(const GLProgram&);
        GLProgram& operator=(GLProgram&&);

        void useProgram(void);

        void unBindProgram(void) { glUseProgram(GL_NONE); }

        void setUniform(const std::string& name, bool* value, int n);

        void setUniform(const std::string& name, int* value, int n);

        void setUniform(const std::string& name, float* value, int n);

        void setUniform(const std::string& name, const glm::vec2* value, int n);

        void setUniform(const std::string& name, float x, float y);

        void setUniform(const std::string& name, const glm::vec3* value, int n);

        void setUniform(const std::string& name, float x, float y, float z);

        void setUniform(const std::string& name, const glm::vec4* value, int n);

        void setUniform(const std::string& name, float x, float y, float z, float w);

        void setUniform(const std::string& name, const glm::mat2* mat, int n);

        void setUniform(const std::string& name, const glm::mat3* mat, int n);

        void setUniform(const std::string& name, const glm::mat4* mat, int n);

        const map<string, ShaderTypeData>& getAttribute(void) const { return mAttribute; }

        void setAttribute(const std::string& name, void* pointer, int component, GLenum type, bool normalized, int stride);

        void setAttribute(const std::string& name, GLint vbo, int component, GLenum type, bool normalized, int stride);

        const vector<string>& getSamplers(void) const { return mSamplersName; }

    private:
        GLProgram(void) = delete;
        GLProgram(const char* pVertexShaderSource, const char* pFragShaderSource);
        GLProgram(const char* pVertexShaderSource,
                  const char* pFragShaderSource,
                  const GLchar** varying,
                  int varyingCount);
        ~GLProgram(void);

        void retrievalAttribute(void);
        void retrievalUniform(void);

        void settingUniformVal(void);

    private:
        GLint                           mProgramId;
        map<string, ShaderTypeData>     mAttribute;
        map<string, ShaderTypeData>     mUniform;
        map<string, ShaderTypeData>     mParameters;
        vector<string>                  mSamplersName;
    };

    typedef map<string, ShaderTypeData>::value_type valueType;
    typedef map<string, ShaderTypeData>::iterator iterator;
    typedef map<string, ShaderTypeData>::const_iterator conIterator;

END

#endif //__GLProgram_h__
