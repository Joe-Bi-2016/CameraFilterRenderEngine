//
// Created by Joe.Bi on 2024/11/7.
//

#ifndef __EglCore_h__
#define __EglCore_h__
#include "EglContext.h"
#include "EglSurface.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    typedef EGLBoolean (EGLAPIENTRYP EGL_PRESENTATION_TIME_ANDROIDPROC)(EGLDisplay display, EGLSurface surface, khronos_stime_nanoseconds_t time);

    //------------------------------------------------------------------------------------//
    class API_EXPORTS EglCore : private Uncopyable
    {
        friend class GLRenderThreadBase;
    public:
        EglCore(const EglCore&) = delete;
        EglCore& operator=(const EglCore&) = delete;
        ~EglCore(void);

        void release(void);

        int getWindowWidth(void) const { return mWindowWidth; }
        int getWindowHeight(void) const { return mWindowHeight; }

        void setEGLContext(const sharedPtrContext & context) { mContext = context; }
        void setEGLSurface(const sharedSurface& drawSurface, const sharedSurface& readSurface);

        sharedPtrContext getEGLContext(void) const { return mContext; }
        sharedSurface getDrawEGLSurface(void) const { return mDrawSurface; }
        sharedSurface getReadEGLSurface(void) const { return mReadSurface; }

        void makeCurrent(void);

        void makeCurrent(const sharedSurface& eglSurface);

        void makeCurrent(const sharedSurface& drawSurface, const sharedSurface& readSurface);

        void makeNothingCurrent(void);

        bool swapBuffers(void);

        bool swapBuffers(const sharedSurface& eglSurface);

        void setPresentationTime(const sharedSurface& eglSurface, long nsecs);

        bool isCurrent(const sharedSurface& eglSurface);

        const char* queryString(int what);

    private:
        EglCore(void);
        EglCore(ANativeWindow *nativeWindow);
        EglCore(const sharedPtrContext& context, ANativeWindow *nativeWindow);
        EglCore(const sharedPtrContext& context);
        EglCore(const sharedPtrContext& context, const sharedSurface& eglSurface);
        EglCore(const sharedPtrContext& context, const sharedSurface& drawSurface, const sharedSurface& readSurface);

    private:
        sharedPtrContext    mContext;
        sharedSurface       mDrawSurface;
        sharedSurface       mReadSurface;
        EGLDisplay          mEGLDisplay;
        EGLContext          mEGLContext;
        int                 mWindowWidth;
        int                 mWindowHeight;
        EGL_PRESENTATION_TIME_ANDROIDPROC eglPresentationTimeANDROID;

        bool init(EglContext* context, EglSurface* drawSurface, EglSurface* readSurface, ANativeWindow *nativeWindow);
    };

END

#endif //__EglCore_h__
