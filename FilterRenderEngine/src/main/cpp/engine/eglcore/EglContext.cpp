//
// Created by Joe.Bi on 2024/11/7.
//

#include "EglContext.h"
#include "util/LogUtil.h"
#include <assert.h>

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------
    EglContext::EglContext(sharedConfig config, const EglContext* context)
    : mEGLContext(EGL_NO_CONTEXT)
    , mConfig(config)
    {
        assert(mConfig.get());
        // EGL context attributes
        const EGLint ctxAttr[] =
        {
            EGL_CONTEXT_CLIENT_VERSION, mConfig->getGLVersion(),
            EGL_NONE
        };

        mEGLContext = eglCreateContext(mConfig->getDisplay(), mConfig->getConfig(), context ? context->getEGLContext() : EGL_NO_CONTEXT, ctxAttr);
        if (mEGLContext == EGL_NO_CONTEXT) {
            EGLint error = EglConfig::checkEglError("eglCreateContext");
            EglConfig::eglStrError(error);
        }
    }

    //------------------------------------------------------------------------------------
    EglContext::~EglContext(void)
    {
        release();
    }

    //------------------------------------------------------------------------------------
    void EglContext::release(void)
    {
        assert(mConfig.get());
        if (mConfig->getDisplay() != EGL_NO_DISPLAY)
        {
            eglMakeCurrent(mConfig->getDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            eglDestroyContext(mConfig->getDisplay(), mEGLContext);
            eglReleaseThread();
            eglTerminate(mConfig->getDisplay());
        }

        mConfig.reset();

        mEGLContext = EGL_NO_CONTEXT;
    }

    //------------------------------------------------------------------------------------
    sharedPtrContext EglContext::createContext(sharedConfig config, const EglContext* context)
    {
        return sharedPtrContext(new EglContext(config, context), deleter<EglContext>);
    }

END