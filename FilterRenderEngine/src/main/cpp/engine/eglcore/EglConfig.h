//
// Created by Joe.Bi on 2024/11/9.
//

#ifndef __EglConfig_h__
#define __EglConfig_h__
#include "base/Uncopyable.hpp"
#include <EGL/egl.h>
#include <memory>

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    typedef enum : uint32_t
    {
        WINDOWS     = 0x00,
        PBUFFER     = 0x01,
        PIXBITMAP   = 0x02,
    }SurfaceType;

    class EglConfig;
    typedef std::shared_ptr<EglConfig> sharedConfig;

    //------------------------------------------------------------------------------------//
    class API_EXPORTS EglConfig : private Uncopyable
    {
        friend class EglSurface;
        friend void deleter<EglConfig>(EglConfig*);
    public:
        EGLDisplay getDisplay(void) const { return mDisplay; }

        EGLConfig getConfig(void) const { return mConfig; }

        EGLint getGLVersion(void) const { return mGLVersion; }

        bool operator !=(const sharedConfig& config);
        bool operator ==(const sharedConfig& config);

        static EGLint checkEglError(const char *msg);

        static const char* eglStrError(EGLint err);

    private:
        EglConfig(void) = delete;
        EglConfig(int redBits, int greenBits, int blueBits, int alphaBits, int depthBits, int stencilBits, SurfaceType type, bool enableAndroidRecord = false);
        ~EglConfig(void);

        EGLConfig createConfig(int redBits, int greenBits, int blueBits, int alphaBits, int depthBits, int stencilBits, EGLint rendertype, EGLint surfacetype, bool enableAndroidRecord);

        static sharedConfig getConfig(int redBits, int greenBits, int blueBits, int alphaBits, int depthBits, int stencilBits, SurfaceType type, bool enableAndroidRecord = false);

    private:
        EGLDisplay  mDisplay;
        EGLConfig   mConfig;

    public:
        static EGLint mGLVersion;
    };

END

#endif //TESTNDK_EGLCONFIG_H
