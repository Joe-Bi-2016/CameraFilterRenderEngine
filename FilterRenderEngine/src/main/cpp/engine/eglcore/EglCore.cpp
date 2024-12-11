//
// Created by Joe.Bi on 2024/11/7.
//

#include "EglCore.h"
#include "util/LogUtil.h"
#include <assert.h>

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    EglCore::EglCore(void)
    : mContext(nullptr)
    , mDrawSurface(nullptr)
    , mReadSurface(nullptr)
    , mEGLDisplay(EGL_NO_DISPLAY)
    , mEGLContext(EGL_NO_CONTEXT)
    , eglPresentationTimeANDROID(nullptr)
    , mWindowWidth(1)
    , mWindowHeight(1)
    {
        eglPresentationTimeANDROID = (EGL_PRESENTATION_TIME_ANDROIDPROC)eglGetProcAddress("eglPresentationTimeANDROID");
        if (!eglPresentationTimeANDROID)
        {
            LOGERROR("eglPresentationTimeANDROID is not available!");
        }
        init(nullptr, nullptr, nullptr, nullptr);
    }

    //------------------------------------------------------------------------------------//
    EglCore::EglCore(ANativeWindow *nativeWindow)
    : mContext(nullptr)
    , mDrawSurface(nullptr)
    , mReadSurface(nullptr)
    , mEGLDisplay(EGL_NO_DISPLAY)
    , mEGLContext(EGL_NO_CONTEXT)
    , eglPresentationTimeANDROID(nullptr)
    , mWindowWidth(1)
    , mWindowHeight(1)
    {
        init(nullptr, nullptr, nullptr, nativeWindow);
    }

    //------------------------------------------------------------------------------------//
    EglCore::EglCore(const sharedPtrContext& context, ANativeWindow *nativeWindow)
    : mContext(nullptr)
    , mDrawSurface(nullptr)
    , mReadSurface(nullptr)
    , mEGLDisplay(EGL_NO_DISPLAY)
    , mEGLContext(EGL_NO_CONTEXT)
    , eglPresentationTimeANDROID(nullptr)
    , mWindowWidth(1)
    , mWindowHeight(1)
    {
        init(context.get(), nullptr, nullptr, nativeWindow);
    }

    //------------------------------------------------------------------------------------//
    EglCore::EglCore(const sharedPtrContext& context)
    : mContext(nullptr)
    , mDrawSurface(nullptr)
    , mReadSurface(nullptr)
    , mEGLDisplay(EGL_NO_DISPLAY)
    , mEGLContext(EGL_NO_CONTEXT)
    , eglPresentationTimeANDROID(nullptr)
    , mWindowWidth(1)
    , mWindowHeight(1)
    {
        init(context.get(), nullptr, nullptr, nullptr);
    }
    //------------------------------------------------------------------------------------//
    EglCore::EglCore(const sharedPtrContext& context, const sharedSurface& eglSurface)
    : mContext(nullptr)
    , mDrawSurface(nullptr)
    , mReadSurface(nullptr)
    , mEGLDisplay(EGL_NO_DISPLAY)
    , mEGLContext(EGL_NO_CONTEXT)
    , eglPresentationTimeANDROID(nullptr)
    , mWindowWidth(1)
    , mWindowHeight(1)
    {
        init(context.get(), eglSurface.get(), eglSurface.get(), nullptr);
    }

    //------------------------------------------------------------------------------------//
    EglCore::EglCore(const sharedPtrContext& context, const sharedSurface& drawSurface, const sharedSurface& readSurface)
    : mContext(nullptr)
    , mDrawSurface(nullptr)
    , mReadSurface(nullptr)
    , mEGLDisplay(EGL_NO_DISPLAY)
    , mEGLContext(EGL_NO_CONTEXT)
    , eglPresentationTimeANDROID(nullptr)
    , mWindowWidth(1)
    , mWindowHeight(1)
    {
        init(context.get(), drawSurface.get(), readSurface.get(), nullptr);
    }

    //------------------------------------------------------------------------------------//
    EglCore::~EglCore(void)
    {
        release();
    }

    //------------------------------------------------------------------------------------//
    bool EglCore::init(EglContext *context, EglSurface *drawSurface, EglSurface *readSurface, ANativeWindow *nativeWindow)
    {
        bool isNull = (drawSurface == nullptr && readSurface == nullptr);

        if(nativeWindow)
        {
            mWindowWidth = ANativeWindow_getWidth(nativeWindow);
            mWindowHeight = ANativeWindow_getHeight(nativeWindow);
        }
        else
        {
            mWindowWidth = 1;
            mWindowHeight = 1;
        }

        if(drawSurface)
            mDrawSurface = sharedSurface(drawSurface, deleter<EglSurface>);
        else
        {
            if (!mDrawSurface)
                mDrawSurface = EglSurface::createEglSurface(nativeWindow);
            else
                mDrawSurface->createSurface(nativeWindow);
        }

        if(readSurface)
            mReadSurface = sharedSurface(readSurface, deleter<EglSurface>);
        else
        {
            if(isNull)
                mReadSurface = mDrawSurface;
            else
            {
                if (!mReadSurface)
                    mReadSurface = EglSurface::createEglSurface(nativeWindow);
                else
                    mReadSurface->createSurface(nativeWindow);
            }
        }

        if(!mDrawSurface->isSameConfig(mReadSurface))
        {
            LOGERROR("read surface is not compatible with draw surface");
            return false;
        }

        mContext = EglContext::createContext(mDrawSurface->getConfig(), context);

        mEGLDisplay = mDrawSurface->getConfig()->getDisplay();
        mEGLContext = mContext->getEGLContext();
        eglPresentationTimeANDROID = (EGL_PRESENTATION_TIME_ANDROIDPROC)eglGetProcAddress("eglPresentationTimeANDROID");
        if (!eglPresentationTimeANDROID)
            LOGERROR("eglPresentationTimeANDROID is not available!");

        return true;
    }

    //------------------------------------------------------------------------------------//
    void EglCore::release(void)
    {
        eglWaitGL();
        makeNothingCurrent();
        mDrawSurface.reset();
        mReadSurface.reset();
        mContext.reset();
        mEGLDisplay = EGL_NO_DISPLAY;
        mEGLContext = EGL_NO_CONTEXT;
        eglPresentationTimeANDROID = nullptr;
    }

    //------------------------------------------------------------------------------------//
    void EglCore::setEGLSurface(const sharedSurface& drawSurface, const sharedSurface& readSurface)
    {
        if (drawSurface)
            mDrawSurface = drawSurface;
        else
            mDrawSurface->destroySurface();

        if (readSurface)
            mReadSurface = readSurface;
        else
            mReadSurface->destroySurface();
    }

    //------------------------------------------------------------------------------------//
    void EglCore::makeCurrent(void)
    {
        if (mEGLDisplay == EGL_NO_DISPLAY)
        {
            LOGERROR("Note: makeCurrent w/o display.\n");
            return;
        }

        if(mDrawSurface.get() != mReadSurface.get() ||
            (mDrawSurface->getConfig()->operator!=(mContext->getConfig())) ||
            (mReadSurface->getConfig()->operator!=(mContext->getConfig())))
        {
            LOGERROR("Error: draw surface didn't equal to read surface");
            return;
        }

        if (!eglMakeCurrent(mEGLDisplay, mDrawSurface->getEGLSurface(), mReadSurface->getEGLSurface(), mEGLContext))
        {
            EGLint err = EglConfig::checkEglError("eglMakeCurrent");
            EglConfig::eglStrError(err);
        }
    }

    //------------------------------------------------------------------------------------//
    void EglCore::makeCurrent(const sharedSurface& eglSurface)
    {
        if (mEGLDisplay == EGL_NO_DISPLAY || eglSurface.get() == nullptr)
        {
            LOGERROR("Note: makeCurrent w/o display.\n");
            return;
        }

        if((eglSurface->getConfig()->operator!=(mContext->getConfig())))
        {
            LOGERROR("Error: surface's config didn't match with context");
            return;
        }

        if (!eglMakeCurrent(mEGLDisplay, eglSurface->getEGLSurface(), eglSurface->getEGLSurface(), mEGLContext))
        {
            EGLint err = EglConfig::checkEglError("eglMakeCurrent");
            EglConfig::eglStrError(err);
        }
    }

    //------------------------------------------------------------------------------------//
    void EglCore::makeCurrent(const sharedSurface& drawSurface, const sharedSurface& readSurface)
    {
        if (mEGLDisplay == EGL_NO_DISPLAY)
        {
            LOGERROR("Note: makeCurrent w/o display.\n");
            return;
        }

        assert(drawSurface.get() && readSurface.get());
        if(!drawSurface->isSameConfig(readSurface))
        {
            LOGERROR("tow surface's config is not same");
            return;
        }

        if((drawSurface->getConfig()->operator!=(mContext->getConfig())) ||
           (readSurface->getConfig()->operator!=(mContext->getConfig())))
        {
            LOGERROR("Error: draw surface didn't equal to read surface");
            return;
        }

        if (!eglMakeCurrent(mEGLDisplay, drawSurface->getEGLSurface(), readSurface->getEGLSurface(), mEGLContext))
        {
            EGLint err = EglConfig::checkEglError("eglMakeCurrent");
            EglConfig::eglStrError(err);
        }
    }

    //------------------------------------------------------------------------------------//
    void EglCore::makeNothingCurrent(void)
    {
        if (mEGLDisplay == EGL_NO_DISPLAY)
        {
            LOGERROR("Note: makeCurrent w/o display.\n");
            return;
        }

        if (!eglMakeCurrent(mEGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
        {
            EGLint err = EglConfig::checkEglError("eglMakeCurrent");
            EglConfig::eglStrError(err);
        }
    }

    //------------------------------------------------------------------------------------//
    bool EglCore::swapBuffers(void)
    {
        if (mEGLDisplay == EGL_NO_DISPLAY)
        {
            LOGERROR("Note: makeCurrent w/o display.\n");
            return false;
        }

        if(mDrawSurface.get() == nullptr)
        {
            LOGERROR("draw buffer is null");
            return false;
        }

        if(mDrawSurface->isOffScreen())
        {
            LOGERROR("Pixel buffer is not supporting swapBuffers");
            return false;
        }

        return eglSwapBuffers(mEGLDisplay, mDrawSurface->getEGLSurface());
    }

    //------------------------------------------------------------------------------------//
    bool EglCore::swapBuffers(const sharedSurface& eglSurface)
    {
        if (mEGLDisplay == EGL_NO_DISPLAY)
        {
            LOGERROR("Note: makeCurrent w/o display.\n");
            return false;
        }

        if(eglSurface.get() == nullptr)
        {
            LOGERROR("draw buffer is null");
            return false;
        }

        if(eglSurface->isOffScreen())
        {
            LOGERROR("Pixel buffer is not supporting swapBuffers");
            return false;
        }

        return eglSwapBuffers(mEGLDisplay, eglSurface->getEGLSurface());
    }

    //------------------------------------------------------------------------------------//
    void EglCore::setPresentationTime(const sharedSurface& eglSurface, long nsecs)
    {
        eglPresentationTimeANDROID(mEGLDisplay, eglSurface->getEGLSurface(), nsecs);
    }

    //------------------------------------------------------------------------------------//
    bool EglCore::isCurrent(const sharedSurface& eglSurface)
    {
        if(eglSurface.get() == nullptr)
        {
            LOGERROR("draw buffer is null");
            return false;
        }

        return mEGLContext == eglGetCurrentContext() && eglSurface->getEGLSurface() == eglGetCurrentSurface(EGL_DRAW);
    }

    //------------------------------------------------------------------------------------//
    const char* EglCore::queryString(int what)
    {
        return eglQueryString(mEGLDisplay, what);
    }

END