//
// Created by Joe.Bi on 2024/11/15.
//

#include "GLModel.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    GLModel::GLModel(const GLModel& model)
    {
        mVao = model.mVao;
        mVbo = model.mVbo;
        mEbo = model.mEbo;
        mAttribute.insert(model.mAttribute.begin(), model.mAttribute.end());
        mPositionStride = model.mPositionStride;
        mNormalStride = model.mNormalStride;
        mColorStride = model.mColorStride;
        mTextureStride = model.mTextureStride;
    }

    //------------------------------------------------------------------------------------//
    GLModel::GLModel(GLModel&& model)
    {
        mVao = model.mVao;
        mVbo = model.mVbo;
        mEbo = model.mEbo;
        mAttribute.swap(model.mAttribute);
        mPositionStride = model.mPositionStride;
        mNormalStride = model.mNormalStride;
        mColorStride = model.mColorStride;
        mTextureStride = model.mTextureStride;

        model.mVao = GL_NONE;
        model.mVbo = GL_NONE;
        model.mEbo = GL_NONE;
        model.mAttribute.clear();
        model.mPositionStride = 0;
        model.mNormalStride = 0;
        model.mColorStride = 0;
        model.mTextureStride = 0;
    }

    //------------------------------------------------------------------------------------//
    GLModel::~GLModel(void)
    {
        if(glIsBuffer(mEbo))
            glDeleteBuffers(1, &mEbo);
        if(glIsBuffer(mVbo))
            glDeleteBuffers(1, &mVbo);
        if(glIsVertexArray(mVao))
            glDeleteVertexArrays(1, &mVao);

        mAttribute.clear();
    }

    //------------------------------------------------------------------------------------//
    void GLModel::setAttributeParam(string name, const ShaderTypeData* data)
    {
        mAttribute.insert(valType(name, const_cast<ShaderTypeData*>(data)));
    }

    //------------------------------------------------------------------------------------//
    GLModel& GLModel::operator=(const GLModel& model)
    {
        mVao = model.mVao;
        mVbo = model.mVbo;
        mEbo = model.mEbo;
        mAttribute.clear();
        mAttribute.insert(model.mAttribute.begin(), model.mAttribute.end());
        mPositionStride = model.mPositionStride;
        mNormalStride = model.mNormalStride;
        mColorStride = model.mColorStride;
        mTextureStride = model.mTextureStride;

        return *this;
    }

    //------------------------------------------------------------------------------------//
    GLModel& GLModel::operator=(GLModel&& model)
    {
        mVao = model.mVao;
        mVbo = model.mVbo;
        mEbo = model.mEbo;
        mAttribute.clear();
        mAttribute.swap(model.mAttribute);
        mPositionStride = model.mPositionStride;
        mNormalStride = model.mNormalStride;
        mColorStride = model.mColorStride;
        mTextureStride = model.mTextureStride;

        model.mVao = GL_NONE;
        model.mVbo = GL_NONE;
        model.mEbo = GL_NONE;
        model.mAttribute.clear();
        model.mPositionStride = 0;
        model.mNormalStride = 0;
        model.mColorStride = 0;
        model.mTextureStride = 0;

        return *this;
    }

END
