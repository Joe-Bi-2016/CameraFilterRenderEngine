//
// Created by Joe.Bi on 2024/11/8.
//

#include "GLRenderPass.h"
#include "util/LogUtil.h"
#include "GLEGLImageTexture.h"
#include "GLOESTexture.h"
#include "GLProgramManager.h"
#include <assert.h>

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    GLRenderPass::GLRenderPass(void)
    : mFbo(NULL)
    , mProgramObj(NULL)
    , mTextureList(0)
    , mQuadModel(nullptr)
    , mhadSettedAttrib(false)
    , mDraw2Fbo(false)
    , mColorbeUsing(false)
    {
        mClearColor[0] = mClearColor[1] = mClearColor[2] = mClearColor[3] = 1.0f;
    }

    //------------------------------------------------------------------------------------//
    GLRenderPass::GLRenderPass(const GLRenderPass& renderPass)
    : mFbo(renderPass.mFbo)
    , mProgramObj(renderPass.mProgramObj)
    , mTextureList(renderPass.mTextureList)
    , mQuadModel(renderPass.mQuadModel)
    , mhadSettedAttrib(renderPass.mhadSettedAttrib)
    , mDraw2Fbo(renderPass.mDraw2Fbo)
    , mColorbeUsing(renderPass.mColorbeUsing)
    {
        memcpy(mClearColor, renderPass.mClearColor, sizeof(vec4f));
    }

    //------------------------------------------------------------------------------------//
    GLRenderPass::GLRenderPass(GLRenderPass&& renderPass)
    : mFbo(std::move(renderPass.mFbo))
    , mProgramObj(std::move(renderPass.mProgramObj))
    , mTextureList(std::move(renderPass.mTextureList))
    , mQuadModel(std::move(renderPass.mQuadModel))
    , mhadSettedAttrib(renderPass.mhadSettedAttrib)
    , mDraw2Fbo(renderPass.mDraw2Fbo)
    , mColorbeUsing(renderPass.mColorbeUsing)
    {
        memcpy(mClearColor, renderPass.mClearColor, sizeof(vec4f));
        renderPass.mColorbeUsing = false;
        renderPass.mhadSettedAttrib = false;
        renderPass.mDraw2Fbo = false;
    }

    //------------------------------------------------------------------------------------//
    GLRenderPass::GLRenderPass(const GLRenderPass* renderPass)
    : mFbo(NULL)
    , mProgramObj(NULL)
    , mTextureList(0)
    , mQuadModel(renderPass ? renderPass->mQuadModel : nullptr)
    , mhadSettedAttrib(renderPass ? renderPass->mhadSettedAttrib : false)
    , mDraw2Fbo(renderPass ? renderPass->mDraw2Fbo : false)
    {
        if(renderPass)
        {
            mFbo = renderPass->mFbo;
            mProgramObj = renderPass->mProgramObj;
            mTextureList.assign(renderPass->mTextureList.begin(), renderPass->mTextureList.end());
            memcpy(mClearColor, renderPass->mClearColor, sizeof(vec4f));
            mColorbeUsing = renderPass->mColorbeUsing;
        }
    }

    //------------------------------------------------------------------------------------//
    GLRenderPass::~GLRenderPass(void)
    {
        mFbo.reset();
        mProgramObj.reset();
        mQuadModel.reset();

        for(size_t i = 0; i < mTextureList.size(); ++i)
            mTextureList[i].reset();

        mTextureList.clear();
    }

    //------------------------------------------------------------------------------------//
    GLRenderPass& GLRenderPass::operator=(GLRenderPass&& renderPass)
    {
        if(this != &renderPass)
        {
            mFbo = std::move(renderPass.mFbo);
            mProgramObj = std::move(renderPass.mProgramObj);
            mTextureList.clear();
            mTextureList = std::move(renderPass.mTextureList);
            mQuadModel = std::move(renderPass.mQuadModel);
            mhadSettedAttrib = renderPass.mhadSettedAttrib;
            mDraw2Fbo = renderPass.mDraw2Fbo;
            renderPass.mhadSettedAttrib = false;
            renderPass.mDraw2Fbo = false;
            memcpy(mClearColor, renderPass.mClearColor, sizeof(vec4f));
            mColorbeUsing = renderPass.mColorbeUsing;
            renderPass.mColorbeUsing = false;
        }

        return *this;
    }

    //------------------------------------------------------------------------------------//
    GLRenderPass& GLRenderPass::operator=(const GLRenderPass& renderPass)
    {
        mFbo = renderPass.mFbo;
        mProgramObj = renderPass.mProgramObj;
        mTextureList.clear();
        mTextureList.assign(renderPass.mTextureList.begin(), renderPass.mTextureList.end());
        mQuadModel = renderPass.mQuadModel;
        mhadSettedAttrib = renderPass.mhadSettedAttrib;
        mDraw2Fbo = renderPass.mDraw2Fbo;
        memcpy(mClearColor, renderPass.mClearColor, sizeof(vec4f));
        mColorbeUsing = renderPass.mColorbeUsing;

        return *this;
    }

    //------------------------------------------------------------------------------------//
    void GLRenderPass::setFbo(int width, int height)
    {
        if(mFbo.get() == nullptr)
            mFbo = GLFrameBuffer::createFbo();
        mFbo->init(width, height);
    }

    //------------------------------------------------------------------------------------//
    const sharedTexture GLRenderPass::getFboColorBuffer(void) const
    {
        if(mFbo.get())
            return mFbo->getRenderTarget()->getTexture();
        return nullptr;
    }

    //------------------------------------------------------------------------------------//
    const sharedTexture GLRenderPass::getTexture(int index) const
    {
        if(mTextureList.empty() || index >= mTextureList.size())
            return nullptr;

        return mTextureList[index];
    }

    //------------------------------------------------------------------------------------//
    const sharedTexture GLRenderPass::getTexture(const char* name) const
    {
        if(mTextureList.empty())
            return nullptr;

        auto it = std::find_if(
                mTextureList.begin(), mTextureList.end(),
                [&](const sharedTexture& t) -> bool {
                    return strcmp(t->getTextureName(), name) == 0;
                });
        if (it != mTextureList.end())
            return *it;

        return nullptr;
    }

    //------------------------------------------------------------------------------------//
    void GLRenderPass::setProgram(const char* programName, const char* vertShader, const char* fragShader)
    {
        mProgramObj = GLProgramManager::getInstance()->getProgram(programName);
        if (mProgramObj.get() == nullptr)
        {
            mProgramObj = GLProgram::createGLProgram(vertShader, fragShader);
            GLProgramManager::getInstance()->putProgram(programName, mProgramObj);
        }
    }

    //------------------------------------------------------------------------------------//
    void GLRenderPass::addTexture(TextureKind kind, const char* name/* = nullptr*/)
    {
        TexParam param;
        param.antialiasEnabled = true;
        param.wrapS = WRAPEDGE;
        param.wrapT = WRAPEDGE;
        if (kind == GLTEXTURE)
        {
            sharedTexture texture = GLTexture::createTexture<GLTexture2D>(param, name);
            mTextureList.push_back(texture);
        }
        else if (kind == OESTEXTURE)
        {
            sharedTexture texture = GLTexture::createTexture<GLOESTexture>(param, name);
            mTextureList.push_back(texture);
        }
        else
        {
            sharedTexture texture = GLTexture::createTexture<GLEGLImageTexture>(param, name);
            mTextureList.push_back(texture);
        }
    }

    //------------------------------------------------------------------------------------//
    void GLRenderPass::setClearColor(float r, float g, float b, float a)
    {
        if(mFbo)
            mFbo->setClearColor(r, g, b, a);
        else
        {
            mClearColor[0] = r;
            mClearColor[1] = g;
            mClearColor[2] = b;
            mClearColor[3] = a;
            mColorbeUsing = true;
        }
    }

    //------------------------------------------------------------------------------------//
    void GLRenderPass::draw(void)
    {
        if(mDraw2Fbo && mFbo.get())
        {
            glViewport(0, 0, mFbo->getWidth(), mFbo->getHeight());
            mFbo->clearFBO();
            mFbo->useFBO();
        }
        else
        {
            if(mColorbeUsing)
            {
                glClearColor(mClearColor[0], mClearColor[1], mClearColor[2], mClearColor[3]);
                glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            }
        }

        std::vector<string> samplers = mProgramObj->getSamplers();
        if(mTextureList.size() < samplers.size())
            LOGBVERBORSE("Warning: texture's number is smaller than program's sampler");

        mProgramObj->useProgram();

        // setting attribute
        if(!mhadSettedAttrib)
        {
            mhadSettedAttrib = true;
            const map<string, ShaderTypeData>& data = mProgramObj->getAttribute();
            conIterator it = data.begin();
            if(mQuadModel->getVAO() != GL_NONE)
            {
                for (; it != data.end(); ++it)
                {
                    if (it->first.find("pos") != string::npos)
                        mQuadModel->setAttributeParam("position", &(it->second));
                    else if (it->first.find("normal") != string::npos)
                        mQuadModel->setAttributeParam("normal", &(it->second));
                    else if (it->first.find("color") != string::npos)
                        mQuadModel->setAttributeParam("color", &(it->second));
                    else if (it->first.find("texture") != string::npos)
                        mQuadModel->setAttributeParam("texture", &(it->second));
                    else
                        mQuadModel->setAttributeParam(it->first, &(it->second));
                }
            }
            else
            {
                for (; it != data.end() ; ++it)
                {
                    GLenum type = typeMapTable.find(it->second.type)->second;
                    if(it->first.find("pos") != string::npos)
                        mProgramObj->setAttribute(it->first, mQuadModel->getVBO(), it->second.n, type, mQuadModel->getPositonStride(), 0);
                    else if(it->first.find("normal") != string::npos)
                        mProgramObj->setAttribute(it->first, mQuadModel->getVBO(), it->second.n, type, mQuadModel->getNormalStride(), 0);
                    else if(it->first.find("color") != string::npos)
                        mProgramObj->setAttribute(it->first, mQuadModel->getVBO(), it->second.n, type, mQuadModel->getColorStride(), 0);
                    else if(it->first.find("texture") != string::npos)
                        mProgramObj->setAttribute(it->first, mQuadModel->getVBO(), it->second.n, type, mQuadModel->getTextureStride(), 0);
                    else
                        mProgramObj->setAttribute(it->first, mQuadModel->getVBO(), it->second.n, type, 0, 0);
                }
            }
        }

        for (int i = 0; i < samplers.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            mTextureList[i]->bindTexture();
            mProgramObj->setUniform(samplers[i], &i, 1);
        }

        mProgramObj->settingUniformVal();

        if(mQuadModel->getVAO() != GL_NONE)
        {
            mQuadModel->bindVAO();
            mQuadModel->draw();
            mQuadModel->unBindVAO();
        }
        else
        {
            mQuadModel->bindEbo();
            mQuadModel->draw();
            mQuadModel->unbindEbo();
        }

        mProgramObj->unBindProgram();
    }

END