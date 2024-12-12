//
// Created by Joe.Bi on 2024/12/12.
//

#ifndef __GLEGLImageTexture_h__
#define __GLEGLImageTexture_h__
#include "GLTexture2D.h"
#if (defined(ANDROID) || defined(__ANDROID__))
#include <android/native_window.h>
#endif

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    class API_EXPORTS GLEGLImageTexture : public GLTexture2D
    {
        friend void deleter<GLEGLImageTexture>(GLEGLImageTexture*);
        friend class GLTexture;
    public:
        static int pixelFormat2HardwareBufferFormat(PixelFormat pixelFormat);

        GLEGLImageTexture(void) = delete;

        GLEGLImageTexture(const GLEGLImageTexture&);

        GLEGLImageTexture(GLEGLImageTexture&&);

        GLEGLImageTexture& operator=(const GLEGLImageTexture&);

        GLEGLImageTexture& operator=(GLEGLImageTexture&&);

        bool initWithMipmaps(MipmapInfo *mipmaps, int mipmapsNum, PixelFormat pixelFormat, unsigned int width, unsigned int height);

        bool updateTexture(const void *data, size_t dataLen, PixelFormat pixelFormat, int offsetX, int offsetY, int pixelsWide, int pixelsHigh);

        textureUpdate getTextureUpdateFunc(void);

        uint8_t* lockTexture(void);

        void unlockTexture(void);

    private:
        GLEGLImageTexture(const TexParam& param, const char* name = nullptr);
        ~GLEGLImageTexture(void);

        void release(GLEGLImageTexture& texture);

    private:
#if (defined(ANDROID) || defined(__ANDROID__))
        AHardwareBuffer*    mInBuffer;
        void*               mBufferPtr;
#endif

};


END


#endif //__GLEGLImageTexture_h__
