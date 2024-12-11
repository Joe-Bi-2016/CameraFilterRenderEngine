//
// Created by Joe.Bi on 2024/11/7.
//

#ifndef __EglSurface_h__
#define __EglSurface_h__
#include "EglConfig.h"
#include <android/native_window.h>
#include <memory>

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    class EglSurface;
    typedef std::shared_ptr<EglSurface> sharedSurface;

    //------------------------------------------------------------------------------------//
    class API_EXPORTS EglSurface : private Uncopyable
    {
        friend void deleter<EglSurface>(EglSurface*);
    public:
        static sharedSurface createEglSurface(ANativeWindow *nativeWindow);

        sharedConfig getConfig(void) const { return mEglConfig; }
        EGLSurface getEGLSurface(void) const { return mEglSurface; }
        int getSurfaceWidth(void) const { return mWidth; }
        int getSurfaceHeight(void) const { return mHeight; }

        bool createSurface(ANativeWindow *nativeWindow);
        bool destroySurface(void);

        bool isOffScreen(void) const { return mOffScreen; }

        bool isSameConfig(const sharedSurface& surface) const;

        int querySurface(int what);

    private:
        EglSurface(ANativeWindow *nativeWindow);
        EglSurface(void);
        ~EglSurface();

    private:
        sharedConfig    mEglConfig;
        EGLSurface      mEglSurface;
        int             mWidth;
        int             mHeight;
        bool            mOffScreen;
        EGLint          mConfigId;
    };

END

#endif //__EglSurface_h__
