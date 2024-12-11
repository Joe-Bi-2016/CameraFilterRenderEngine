//
// Created by Joe.Bi on 2024/11/22.
//

#include "GLRenderThreadMain.h"
#include <stdint.h>
#include "LogUtil.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    GLRenderThreadMain::GLRenderThreadMain(void)
    : GLRenderThreadBase()
    , mViewWidth(0)
    , mViewHeight(0)
    , mFbo(nullptr)
    , mRenderPassList()
    {

    }

    //------------------------------------------------------------------------------------//
    GLRenderThreadMain::~GLRenderThreadMain(void)
    {
        mViewWidth = mViewHeight = 0;

        mFbo.reset();

        for(size_t i = 0; i < mRenderPassList.size(); ++i)
            mRenderPassList[i].reset();

        mRenderPassList.clear();
    }

    //------------------------------------------------------------------------------------//
    void GLRenderThreadMain::initWithEglCore(const EglCore* eglCore)
    {
        GLRenderThreadBase::initWithEglCore(eglCore);
        mViewWidth = mEglCore->getWindowWidth();
        mViewHeight = mEglCore->getWindowHeight();
    }

    //------------------------------------------------------------------------------------//
    void GLRenderThreadMain::initWithWindow(ANativeWindow *nativeWindow)
    {
        GLRenderThreadBase::initWithWindow(nativeWindow);
        mViewWidth = mEglCore->getWindowWidth();
        mViewHeight = mEglCore->getWindowHeight();
    }

    //------------------------------------------------------------------------------------//
    void GLRenderThreadMain::attacheRenderPass(const sharedRenderPass& renderPass)
    {
        mRenderPassList.push_back(renderPass);
    }

    //------------------------------------------------------------------------------------//
    void GLRenderThreadMain::removeRenderPass(int index)
    {
        if(!mRenderPassList.empty() && mRenderPassList.size() > index)
            mRenderPassList.erase(mRenderPassList.begin() + index);
        else
            LOGINFO("the index %d is larger than the size of render list", index);
    }

    //------------------------------------------------------------------------------------//
    void GLRenderThreadMain::removeRenderPass(sharedRenderPass renderPass)
    {
        if(!mRenderPassList.empty())
        {
            deque<sharedRenderPass>::iterator it = mRenderPassList.begin();
            for(; it != mRenderPassList.end(); )
            {
                if(*it == renderPass)
                {
                    mRenderPassList.erase(it);
                    break;
                }
            }

            LOGINFO("had not fined the render in render list");
        }
        else
            LOGINFO("the render list is null");
    }

    //------------------------------------------------------------------------------------//
    sharedRenderPass GLRenderThreadMain::getRenderPass(int index) const
    {
        if(!mRenderPassList.empty() && index < mRenderPassList.size())
        {
            return mRenderPassList[index];
        }
        else
        {
            LOGINFO("the renderPass list is null");
            return nullptr;
        }
    }

    //------------------------------------------------------------------------------------//
    void GLRenderThreadMain::setDraw2Fbo(int width, int height)
    {
        if(mFbo.get() == nullptr)
            mFbo = GLFrameBuffer::createFbo();
        mFbo->init(width, height);
    }

    //------------------------------------------------------------------------------------//
    sharedTexture GLRenderThreadMain::getFboColorTexture(void)
    {
        if(mFbo.get())
            return mFbo->getRenderTarget()->getTexture();

        return nullptr;
    }

    //------------------------------------------------------------------------------------//
    void GLRenderThreadMain::onDo(int renderPassIndex/* = -1 */)
    {
        LOGINFO("draw one frame start......");

        if(mDraw2Fbo && mFbo.get())
        {
            glViewport(0, 0, mFbo->getWidth(), mFbo->getHeight());
            mFbo->clearFBO();
            mFbo->useFBO();
        }
        else
        {
            glViewport(0, 0, mViewWidth, mViewHeight);
            glClearColor(mClearColor[0], mClearColor[1], mClearColor[2], mClearColor[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        if(renderPassIndex == -1)
        {
            for (int i = 0; i < mRenderPassList.size(); ++i)
                mRenderPassList[i]->draw();
        }
        else
        {
            if(renderPassIndex < mRenderPassList.size())
                mRenderPassList[renderPassIndex]->draw();
            else
            {
                LOGERROR("the %d renderPass is not exist", renderPassIndex);
                LOGINFO("draw end......");
                return;
            }
        }

        mEglCore->swapBuffers();

        LOGINFO("draw end......");
    }


END

