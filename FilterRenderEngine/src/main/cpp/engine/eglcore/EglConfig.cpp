//
// Created by Joe.Bi on 2024/11/9.
//

#include "EglConfig.h"
#include "util/LogUtil.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------
    #define EGL_RECORDABLE_ANDROID 0x3142

    EGLint EglConfig::mGLVersion = 0;

    //------------------------------------------------------------------------------------//
    EglConfig::EglConfig(int redBits, int greenBits, int blueBits, int alphaBits, int depthBits, int stencilBits, SurfaceType type, bool enableAndroidRecord/* = false */)
    : mDisplay(EGL_NO_DISPLAY)
    , mConfig(NULL)
    {
        mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (mDisplay == EGL_NO_DISPLAY)
        {
            LOGERROR("unable to get EGL display.\n");
            return;
        }

        EGLint eglMajVers, eglMinVers;
        if (!eglInitialize(mDisplay, &eglMajVers, &eglMinVers))
        {
            mDisplay = EGL_NO_DISPLAY;
            LOGERROR("unable to initialize EGL14");
            return;
        }

        EGLint surfaceType = 0;
        if(type == WINDOWS)
            surfaceType = EGL_WINDOW_BIT;
        else if(type == PBUFFER)
            surfaceType = EGL_PBUFFER_BIT;
        else
            surfaceType = EGL_PIXMAP_BIT;

       mConfig = createConfig(redBits, greenBits, blueBits, alphaBits, depthBits, stencilBits, EGL_OPENGL_ES3_BIT, surfaceType, enableAndroidRecord);
       if(mConfig == NULL)
       {
           mConfig = createConfig(redBits, greenBits, blueBits, alphaBits, depthBits, stencilBits, EGL_OPENGL_ES2_BIT, surfaceType, enableAndroidRecord);
           mGLVersion = 2;
       }
       else
           mGLVersion = 3;
    }

    //------------------------------------------------------------------------------------//
    EGLConfig EglConfig::createConfig(int redBits, int greenBits, int blueBits, int alphaBits, int depthBits, int stencilBits, EGLint rendertype, EGLint surfacetype, bool enableAndroidRecord)
    {
        EGLint attrs[] =
        {
            EGL_RENDERABLE_TYPE, rendertype,
            EGL_SURFACE_TYPE,   surfacetype,
            EGL_RED_SIZE,       redBits,
            EGL_GREEN_SIZE,     greenBits,
            EGL_BLUE_SIZE,      blueBits,
            EGL_ALPHA_SIZE,     alphaBits,
            EGL_DEPTH_SIZE,     depthBits,
            EGL_STENCIL_SIZE,   stencilBits,
            EGL_NONE, 0,      // placeholder for recordable [@-3]
            EGL_NONE
        };

        int length = sizeof(attrs) / sizeof(attrs[0]);
        if (enableAndroidRecord)
        {
            attrs[length - 3] = EGL_RECORDABLE_ANDROID;
            attrs[length - 2] = 1;
        }

        EGLConfig config = NULL;
        int numConfigs;
        if (!eglChooseConfig(mDisplay, attrs, &config, 1, &numConfigs))
            LOGERROR("unable to find suitable EGLConfig");

        return config;
    }

    //------------------------------------------------------------------------------------//
    sharedConfig EglConfig::getConfig(int redBits, int greenBits, int blueBits, int alphaBits, int depthBits, int stencilBits, SurfaceType type, bool enableAndroidRecord/* = false */)
    {
        return sharedConfig(new EglConfig(redBits, greenBits, blueBits, alphaBits, depthBits, stencilBits, type, enableAndroidRecord), deleter<EglConfig>);
    }

    //------------------------------------------------------------------------------------//
    EglConfig::~EglConfig(void)
    {
        mDisplay = EGL_NO_DISPLAY;
        mConfig = NULL;
    }

    //------------------------------------------------------------------------------------//
    EGLint EglConfig::checkEglError(const char *msg)
    {
        EGLint error = EGL_SUCCESS;
        if ((error = eglGetError()) != EGL_SUCCESS)
            LOGERROR("%s: EGL error: %s", msg, eglStrError(error));

        return error;
    }

    //------------------------------------------------------------------------------------//
    bool EglConfig::operator !=(const sharedConfig& config)
    {
        return (mDisplay != config->mDisplay || mConfig != config->mConfig);
    }

    //------------------------------------------------------------------------------------//
    bool EglConfig::operator ==(const sharedConfig& config)
    {
        return (mDisplay == config->mDisplay && mConfig == config->mConfig);
    }

    //------------------------------------------------------------------------------------//
    const char* EglConfig::eglStrError(EGLint err)
    {
        switch (err)
        {
            case EGL_SUCCESS:               return "EGL_SUCCESS";
            case EGL_NOT_INITIALIZED:       return "EGL_NOT_INITIALIZED";
            case EGL_BAD_ACCESS:            return "EGL_BAD_ACCESS";
            case EGL_BAD_ALLOC:             return "EGL_BAD_ALLOC";
            case EGL_BAD_ATTRIBUTE:         return "EGL_BAD_ATTRIBUTE";
            case EGL_BAD_CONFIG:            return "EGL_BAD_CONFIG";
            case EGL_BAD_CONTEXT:           return "EGL_BAD_CONTEXT";
            case EGL_BAD_CURRENT_SURFACE:   return "EGL_BAD_CURRENT_SURFACE";
            case EGL_BAD_DISPLAY:           return "EGL_BAD_DISPLAY";
            case EGL_BAD_MATCH:             return "EGL_BAD_MATCH";
            case EGL_BAD_NATIVE_PIXMAP:     return "EGL_BAD_NATIVE_PIXMAP";
            case EGL_BAD_NATIVE_WINDOW:     return "EGL_BAD_NATIVE_WINDOW";
            case EGL_BAD_PARAMETER:         return "EGL_BAD_PARAMETER";
            case EGL_BAD_SURFACE:           return "EGL_BAD_SURFACE";
            case EGL_CONTEXT_LOST:          return "EGL_CONTEXT_LOST";
            default:                        return "UNKOWN";
        }
    }

END