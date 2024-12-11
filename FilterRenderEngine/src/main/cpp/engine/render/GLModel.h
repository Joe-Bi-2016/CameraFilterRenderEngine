//
// Created by Joe.Bi on 2024/11/15.
//

#ifndef __GLModel_h__
#define __GLModel_h__
#include "base/Uncopyable.hpp"
#include "GLDataTypeDef.h"
#include "GLTexture2D.h"
#include <GLES3/gl3.h>
#include <GLES2/gl2.h>
#include <map>
#include <string>
#include <glm.hpp>

using namespace std;

//----------------------------------------------------------------------------------------//
BEGIN

    class GLModel;
    typedef std::shared_ptr<GLModel>  sharedModel;

    //------------------------------------------------------------------------------------//
    class API_EXPORTS GLModel
    {
        friend void deleter<GLModel>(GLModel*);
    public:
        GLModel(void) : mAttribute(), mPositionStride(0), mNormalStride(0), mColorStride(0), mTextureStride(0) { mVao = mVbo = mEbo = GL_NONE; }
        virtual ~GLModel(void);

        GLModel(const GLModel& model);
        GLModel(GLModel&& model);

        GLModel& operator=(const GLModel& model);
        GLModel& operator=(GLModel&& model);

        GLuint getVAO(void) const { return mVao; }
        GLuint getVBO(void) const { return mVbo; }
        GLuint getEBO(void) const { return mEbo; }

        virtual void setAttributeParam(string name, const ShaderTypeData* data);

        virtual void bindVAO(void) = 0;

        void unBindVAO(void) { glBindVertexArray(0); }

        void bindEbo(void) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo); }
        void unbindEbo(void) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

        virtual void draw(void) = 0;

        virtual GLint getPositonStride(void) const { return mPositionStride; }
        virtual GLint getNormalStride(void)  const { return mNormalStride; }
        virtual GLint getColorStride(void)  const { return mColorStride; }
        virtual GLint getTextureStride(void)  const { return mTextureStride; }

        virtual void setVerticesPositon(const glm::vec3* position, int num, bool cache = false, GLenum usage = GL_STATIC_DRAW) { }
        virtual void setVerticesNormal(const glm::vec3* normal, int num, bool cache = false, GLenum usage = GL_STATIC_DRAW) { }
        virtual void setVerticesColor(const void* color, int num, int channel = 3, bool cache = false, GLenum usage = GL_STATIC_DRAW) { }
        virtual void setVerticesTextureUV0(const glm::vec2* txtcoord, int num, bool cache = false, GLenum usage = GL_STATIC_DRAW) { }
        virtual void setVerticesTextureUV1(const glm::vec2* txtcoord, int num, bool cache = false, GLenum usage = GL_STATIC_DRAW) { }
        virtual void setVerticesTextureUV2(const glm::vec2* txtcoord, int num, bool cache = false, GLenum usage = GL_STATIC_DRAW) { }
        virtual void setVerticesTextureUV3(const glm::vec2* txtcoord, int num, bool cache = false, GLenum usage = GL_STATIC_DRAW) { }
        virtual void setVerticesTextureUV4(const glm::vec2* txtcoord, int num, bool cache = false, GLenum usage = GL_STATIC_DRAW) { }
        virtual void setVerticesTextureUV5(const glm::vec2* txtcoord, int num, bool cache = false, GLenum usage = GL_STATIC_DRAW) { }
        virtual void setVerticesTextureUV6(const glm::vec2* txtcoord, int num, bool cache = false, GLenum usage = GL_STATIC_DRAW) { }
        virtual void setVerticesTextureUV7(const glm::vec2* txtcoord, int num, bool cache = false, GLenum usage = GL_STATIC_DRAW) { }
        virtual void setVerticesIndex(const void* index, int num, DataType type = UNSIGNED_SHORT_TYPE, bool cache = false, GLenum usage = GL_STATIC_DRAW) { }

        virtual glm::vec3*	getVerticesPosition(void) const { return nullptr; }
        virtual glm::vec3*	getVerticesNormal(void) const { return nullptr; }
        virtual glm::vec3*	getVerticesColor3(void) const { return nullptr; }
        virtual glm::vec4*	getVerticesColor4(void) const { return nullptr; }
        virtual glm::vec2*	getVerticesTextureUV0(void) const { return nullptr; }
        virtual glm::vec2*	getVerticesTextureUV1(void) const { return nullptr; }
        virtual glm::vec2*	getVerticesTextureUV2(void) const { return nullptr; }
        virtual glm::vec2*	getVerticesTextureUV3(void) const { return nullptr; }
        virtual glm::vec2*	getVerticesTextureUV4(void) const { return nullptr; }
        virtual glm::vec2*	getVerticesTextureUV5(void) const { return nullptr; }
        virtual glm::vec2*	getVerticesTextureUV6(void) const { return nullptr; }
        virtual glm::vec2*	getVerticesTextureUV7(void) const { return nullptr; }
        virtual uint16_t* getVerticesIndex16(void) const { return nullptr; }
        virtual uint32_t* getVerticesIndex32(void) const { return nullptr; }

        virtual uint32_t getVerticesPosition_VBO(void) const { return 0; }
        virtual uint32_t getVerticesNormal_VBO(void) const { return 0; }
        virtual uint32_t getVerticesColor_VBO(void) const { return 0; }
        virtual uint32_t getVerticesTextureUV0_VBO(void) const { return 0; }
        virtual uint32_t getVerticesTextureUV1_VBO(void) const { return 0; }
        virtual uint32_t getVerticesTextureUV2_VBO(void) const { return 0; }
        virtual uint32_t getVerticesTextureUV3_VBO(void) const { return 0; }
        virtual uint32_t getVerticesTextureUV4_VBO(void) const { return 0; }
        virtual uint32_t getVerticesTextureUV5_VBO(void) const { return 0; }
        virtual uint32_t getVerticesTextureUV6_VBO(void) const { return 0; }
        virtual uint32_t getVerticesTextureUV7_VBO(void) const { return 0; }
        virtual uint32_t getVerticesIndex_VBO(void) const { return 0; }
        virtual int getVerticesIndexCount(void) const { return 0; }

        virtual void updateVerticesPosition(const glm::vec3* postion, int num, int offset) { }
        virtual void updateVerticesNormal(const glm::vec3* normal, int num, int offset) { }
        virtual void updateVerticesColor(const void* color, int channel, int num, int offset) { }
        virtual void updateVerticesTxtureUV0(const glm::vec2* txtcoord, int num, int offset) { }
        virtual void updateVerticesTxtureUV1(const glm::vec2* txtcoord, int num, int offset) { }
        virtual void updateVerticesTxtureUV2(const glm::vec2* txtcoord, int num, int offset) { }
        virtual void updateVerticesTxtureUV3(const glm::vec2* txtcoord, int num, int offset) { }
        virtual void updateVerticesTxtureUV4(const glm::vec2* txtcoord, int num, int offset) { }
        virtual void updateVerticesTxtureUV5(const glm::vec2* txtcoord, int num, int offset) { }
        virtual void updateVerticesTxtureUV6(const glm::vec2* txtcoord, int num, int offset) { }
        virtual void updateVerticesTxtureUV7(const glm::vec2* txtcoord, int num, int offset) { }
        virtual void updateVerticesIndex(const void* index, DataType type, int num, int offset){ }

        virtual void setModelTexture(const sharedTexture& texture, int txtIndex) { }
        virtual sharedTexture getModelTexture(int index) const { return nullptr; }

    protected:
        GLuint                          mVao;
        GLuint                          mVbo;
        GLuint                          mEbo;
        map<string, ShaderTypeData*>    mAttribute;
        GLint                           mPositionStride;
        GLint                           mNormalStride;
        GLint                           mColorStride;
        GLint                           mTextureStride;

    };

    typedef map<string, ShaderTypeData*>::value_type valType;

END

#endif //__GLModel_h__
