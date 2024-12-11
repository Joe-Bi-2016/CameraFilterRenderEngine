//
// Created by Joe.Bi on 2024/11/14.
//

#include "GLQuadModel.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    sharedModel GLQuadModel::mQuadModel = nullptr;

    //------------------------------------------------------------------------------------//
    GLQuadModel::GLQuadModel(void) : GLModel(), mFirstBind(true)
    {
        float vertices[] =
        {
            // positions                // colors          // texture coords
            1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // top right
            1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f, 1.0f,  1.0f, 0.0f, // bottom right
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f,  0.0f, 0.0f, // bottom left
            -1.0f,  1.0f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,  0.0f, 1.0f  // top left
        };

        unsigned int indices[] =
        {
           0, 1, 3, // first triangle
           1, 2, 3  // second triangle
        };

        glGenVertexArrays(1, &mVao);
        glGenBuffers(1, &mVbo);
        glGenBuffers(1, &mEbo);

        glBindBuffer(GL_ARRAY_BUFFER, mVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        mPositionStride = 9 * sizeof(float);
        mNormalStride = -1;
        mColorStride = mPositionStride;
        mTextureStride = mPositionStride;
    }

    //------------------------------------------------------------------------------------//
    GLQuadModel::GLQuadModel(const GLQuadModel& model)
    : GLModel(model)
    , mFirstBind(model.mFirstBind)
    {
    }

    //------------------------------------------------------------------------------------//
    GLQuadModel::GLQuadModel(GLQuadModel&& model)
    : GLModel(std::move(model))
    , mFirstBind(model.mFirstBind)
    {
        model.mFirstBind = true;
    }

    //------------------------------------------------------------------------------------//
    GLQuadModel& GLQuadModel::operator=(const GLQuadModel& model)
    {
        GLModel::operator=(model);
        mFirstBind = model.mFirstBind;
        return *this;
    }

    //------------------------------------------------------------------------------------//
    GLQuadModel& GLQuadModel::operator=(GLQuadModel&& model)
    {
        GLModel::operator=(std::move(model));
        mFirstBind = std::forward<bool>(model.mFirstBind);
        model.mFirstBind = true;
        return *this;
    }

    //------------------------------------------------------------------------------------//
    sharedModel GLQuadModel::getQuadModel(void)
    {
        if(mQuadModel.get() == nullptr)
            mQuadModel = sharedModel(new GLQuadModel(), deleter<GLQuadModel>);

        return mQuadModel;
    }

    //------------------------------------------------------------------------------------//
    void GLQuadModel::bindVAO(void)
    {
        if(mVao == GL_NONE)
            return;

        if(mFirstBind)
        {
            ShaderTypeData* pos = nullptr;
            ShaderTypeData* color = nullptr;
            ShaderTypeData* txt = nullptr;

            if(mAttribute.find("position") != mAttribute.end())
                pos = mAttribute.find("position")->second;

            if(mAttribute.find("color") != mAttribute.end())
                color = mAttribute.find("color")->second;

            if(mAttribute.find("texture") != mAttribute.end())
                txt = mAttribute.find("texture")->second;

            glBindVertexArray(mVao);

            glBindBuffer(GL_ARRAY_BUFFER, mVbo);

            // position attribute
            if(pos)
            {
                glVertexAttribPointer(pos->index, pos->n, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(pos->index);
            }

            // color attribute
            if(color)
            {
                glVertexAttribPointer(color->index, color->n, GL_FLOAT, GL_FALSE, 9 * sizeof(float),(void *) (3 * sizeof(float)));
                glEnableVertexAttribArray(color->index);
            }

            // texture coordinate attribute
            if(txt)
            {
                glVertexAttribPointer(txt->index, txt->n, GL_FLOAT, GL_FALSE, 9 * sizeof(float),(void *) (7 * sizeof(float)));
                glEnableVertexAttribArray(txt->index);
            }

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
            glBindVertexArray(0);

            mFirstBind = false;
			return;
        }

        glBindVertexArray(mVao);
    }

    //------------------------------------------------------------------------------------//
    void GLQuadModel::draw(void)
    {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }


END
