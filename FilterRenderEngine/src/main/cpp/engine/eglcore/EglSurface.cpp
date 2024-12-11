//
// Created by Joe.Bi on 2024/11/7.
//

#include "EglSurface.h"
#include "util/LogUtil.h"
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#include <assert.h>

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    EglSurface::EglSurface(ANativeWindow *nativeWindow)
    : mEglConfig(nullptr)
    , mEglSurface(EGL_NO_SURFACE)
    , mWidth(0)
    , mHeight(0)
    , mOffScreen(false)
    {
        LOGINFO("eglCreateWindowSurface start");

        EglConfig* config = new EglConfig(8, 8, 8, 8, 16, 8, WINDOWS, true);
        assert(config);
        mEglConfig = sharedConfig(config, deleter<EglConfig>);

        createSurface(nativeWindow);

        eglQuerySurface(mEglConfig->getDisplay(), mEglSurface, EGL_WIDTH, &mWidth);
        eglQuerySurface(mEglConfig->getDisplay(), mEglSurface, EGL_HEIGHT, &mHeight);
        eglQuerySurface(mEglConfig->getDisplay(), mEglSurface, EGL_CONFIG_ID, &mConfigId);
    }

    //------------------------------------------------------------------------------------//
    EglSurface::EglSurface(void)
    : mEglConfig(nullptr)
    , mEglSurface(EGL_NO_SURFACE)
    , mWidth(1)
    , mHeight(1)
    , mOffScreen(true)
    {
        LOGINFO("eglCreateWindowSurface start");

        EglConfig* config = new EglConfig(8, 8, 8, 8, 16, 8, PBUFFER, true);
        assert(config);
        mEglConfig = sharedConfig(config, deleter<EglConfig>);

        createSurface(nullptr);

        eglQuerySurface(mEglConfig->getDisplay(), mEglSurface, EGL_CONFIG_ID, &mConfigId);
    }

    //------------------------------------------------------------------------------------//
    EglSurface::~EglSurface()
    {
        destroySurface();
        mEglConfig.reset();
    }

    //------------------------------------------------------------------------------------//
    bool EglSurface::createSurface(ANativeWindow *nativeWindow)
    {
        if (nativeWindow)
        {
            int surfaceAttribs[] = { EGL_NONE };
            mEglSurface = eglCreateWindowSurface(mEglConfig->getDisplay(), mEglConfig->getConfig(), nativeWindow, surfaceAttribs);
            if (mEglSurface == EGL_NO_SURFACE)
            {
                EGLint err = EglConfig::checkEglError("eglCreateWindowSurface");
                EglConfig::eglStrError(err);
                return false;
            }
        }
        else
        {
            int surfaceAttribs[] =
            {
                EGL_WIDTH, mWidth,
                EGL_HEIGHT, mHeight,
                EGL_NONE
            };

            mEglSurface = eglCreatePbufferSurface(mEglConfig->getDisplay(), mEglConfig->getConfig(), surfaceAttribs);
            if (mEglSurface == EGL_NO_SURFACE)
            {
                EGLint err = EglConfig::checkEglError("eglCreatePbufferSurface");
                EglConfig::eglStrError(err);
                return false;
            }
        }

        return true;
    }

    //------------------------------------------------------------------------------------//
    bool EglSurface::destroySurface(void)
    {
        assert(mEglConfig->getDisplay() != EGL_NO_DISPLAY);
        if(mEglSurface != EGL_NO_SURFACE)
        {
            EGLBoolean b = eglDestroySurface(mEglConfig->getDisplay(), mEglSurface);
            if(b == EGL_TRUE)
            {
                mEglSurface = EGL_NO_SURFACE;
                mWidth = mHeight = 0;
            }
            else
            {
                LOGERROR("destroy surface error!");
                EGLint err = EglConfig::checkEglError("eglDestroySurface");
                EglConfig::eglStrError(err);
            }

            return b;
        }

        LOGINFO("surface is not been created!");
        return true;
    }

    //------------------------------------------------------------------------------------//
    bool EglSurface::isSameConfig(const sharedSurface& surface) const
    {
        assert(mEglConfig.get() && surface->mEglConfig.get());
        return ((mEglConfig->getDisplay() != EGL_NO_DISPLAY && mEglConfig->getDisplay() == surface->mEglConfig->getDisplay()) &&
                (mEglSurface != EGL_NO_SURFACE && surface->mEglSurface != EGL_NO_SURFACE) &&
                (mConfigId == surface->mConfigId) &&
                mOffScreen == surface->mOffScreen);
    }

    //------------------------------------------------------------------------------------//
    int EglSurface::querySurface(int what)
    {
        int value;
        eglQuerySurface(mEglConfig->getDisplay(), mEglSurface, what, &value);
        return value;
    }

    //------------------------------------------------------------------------------------//
    sharedSurface EglSurface::createEglSurface(ANativeWindow *nativeWindow)
    {
        if (nativeWindow)
            return sharedSurface(new EglSurface(nativeWindow), deleter<EglSurface>);
        else
            return sharedSurface(new EglSurface(), deleter<EglSurface>);
    }


END
