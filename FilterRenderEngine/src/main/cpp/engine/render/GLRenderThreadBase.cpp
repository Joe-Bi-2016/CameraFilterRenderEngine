//
// Created by Joe.Bi on 2024/11/23.
//

#include "GLRenderThreadBase.h"
#include <stdint.h>
#include "LogUtil.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    GLRenderThreadBase::GLRenderThreadBase(void)
    : mEglCore(nullptr)
    , mExit(false)
    {
        mClearColor[0] = 1.0f; mClearColor[1] = 1.0f; mClearColor[2] = 1.0f; mClearColor[3] = 1.0f;
    }

    //------------------------------------------------------------------------------------//
    GLRenderThreadBase::~GLRenderThreadBase(void)
    {
        safeDeleteObject(mEglCore);
    }

    //------------------------------------------------------------------------------------//
    void GLRenderThreadBase::initWithEglCore(const EglCore* eglCore)
    {
        mEglCore = new EglCore(eglCore ? eglCore->getEGLContext() : sharedPtrContext(nullptr));
        mEglCore->makeCurrent();
    }

    //------------------------------------------------------------------------------------//
    void GLRenderThreadBase::initWithWindow(ANativeWindow *nativeWindow)
    {
        mEglCore = new EglCore(nativeWindow);
        mEglCore->makeCurrent();
    }

    //------------------------------------------------------------------------------------//
    void GLRenderThreadBase::destroyEGLSurface(void)
    {
        if (mEglCore)
        {
            mEglCore->setEGLSurface(nullptr, nullptr);
        }
        else
            LOGWARNING("you had not create EGLSurface!!!");
    }

END


