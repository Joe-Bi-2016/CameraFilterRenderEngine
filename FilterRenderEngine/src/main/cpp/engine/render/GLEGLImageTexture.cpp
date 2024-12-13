//
// Created by Joe.Bi on 2024/12/12.
//

#include "GLEGLImageTexture.h"
#include <GLES2/gl2ext.h>
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <KHR/khrplatform.h>

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    GLEGLImageTexture::GLEGLImageTexture(const TexParam& param, const char* name/* = nullptr*/)
    : GLTexture2D(param, name)
    {
#if (defined(ANDROID) || defined(__ANDROID__))
        mInBuffer = nullptr;
        mBufferPtr = nullptr;
#endif
    }

    //------------------------------------------------------------------------------------//
    GLEGLImageTexture::GLEGLImageTexture(const GLEGLImageTexture& texture)
    : GLTexture2D(texture)
    {
#if (defined(ANDROID) || defined(__ANDROID__))
        mInBuffer = texture.mInBuffer;
        mBufferPtr = texture.mBufferPtr;
#endif
    }

    //------------------------------------------------------------------------------------//
    GLEGLImageTexture::GLEGLImageTexture(GLEGLImageTexture&& texture)
    : GLTexture2D(texture)
    {
#if (defined(ANDROID) || defined(__ANDROID__))
        mInBuffer = texture.mInBuffer;
        mBufferPtr = texture.mBufferPtr;
        texture.mInBuffer = nullptr;
        texture.mBufferPtr = nullptr;
#endif
    }

    //------------------------------------------------------------------------------------//
    GLEGLImageTexture& GLEGLImageTexture::operator=(const GLEGLImageTexture& texture)
    {
        GLTexture2D::operator=(texture);
#if (defined(ANDROID) || defined(__ANDROID__))
        release(*this);
        mInBuffer = texture.mInBuffer;
        mBufferPtr = texture.mBufferPtr;
#endif
        return *this;
    }

    //------------------------------------------------------------------------------------//
    GLEGLImageTexture& GLEGLImageTexture::operator=(GLEGLImageTexture&& texture)
    {
        GLTexture2D::operator=(texture);
#if (defined(ANDROID) || defined(__ANDROID__))
        release(*this);
        texture.mInBuffer = nullptr;
        release(texture);
#endif
        return *this;
    }

    //------------------------------------------------------------------------------------//
    GLEGLImageTexture::~GLEGLImageTexture()
    {
        release(*this);
    }

    //------------------------------------------------------------------------------------//
    void GLEGLImageTexture::release(GLEGLImageTexture& texture)
    {
        texture.unbindTexture();
#if (defined(ANDROID) || defined(__ANDROID__))
        if (texture.mInBuffer)
        {
            AHardwareBuffer_release(texture.mInBuffer);
        }
        texture.mBufferPtr = nullptr;
#endif
    }

    //------------------------------------------------------------------------------------//
    int GLEGLImageTexture::pixelFormat2HardwareBufferFormat(PixelFormat pixelFormat)
    {
        // Note: some only bits is equal, did not mean the same format
        int ret = 1;
        switch (pixelFormat) {
            case BGRA8888:  { ret = 2; break; }
            case RGBA8888:  { ret = 1; break; }
            case RGB888:    { ret = 3; break; }
            case RGB565:    { ret = 4; break; }
            case A8:        { ret = 0x35; break; }
            case I8:        { ret = 0x35; break; }
            case AI88:      { ret = 0x30; break; }
            default: break;
        }

        return ret;
    }

    //------------------------------------------------------------------------------------//
    bool GLEGLImageTexture::initWithMipmaps(MipmapInfo *mipmaps, int mipmapsNum, PixelFormat pixelFormat, unsigned int pixelsWide, unsigned int pixelsHigh)
    {
#if (defined(ANDROID) || defined(__ANDROID__))
        // first, get function pointer of EGL and extension
        PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC eglGetNativeClientBufferANDROID = (PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC)eglGetProcAddress("eglGetNativeClientBufferANDROID");
        if (!eglGetNativeClientBufferANDROID)
        {
            LOGERROR("Get eglGetNativeClientBufferANDROID function failed!\n");
            return false;
        }

        PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC)eglGetProcAddress("eglCreateImageKHR");
        if (!eglCreateImageKHR)
        {
            LOGERROR("Get eglCreateImageKHR function failed!\n");
            return false;
        }

        typedef void (*PFNGLBINDTEXIMAGE)(GLenum target, GLeglImageOES image);
        PFNGLBINDTEXIMAGE glEGLImageTargetTexture2DOES = (PFNGLBINDTEXIMAGE)eglGetProcAddress("glEGLImageTargetTexture2DOES");
        if (!glEGLImageTargetTexture2DOES)
        {
            LOGERROR("Get glEGLImageTargetTexture2DOES function failed!\n");
            return false;
        }

        AHardwareBuffer_Format format = (AHardwareBuffer_Format)pixelFormat2HardwareBufferFormat(pixelFormat);
        AHardwareBuffer_Desc desc = {pixelsWide, pixelsHigh,
                                     1,
                                     format,
                                     AHARDWAREBUFFER_USAGE_CPU_WRITE_OFTEN|AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE,
                                     pixelsWide,
                                     0 ,
                                     0};
        int ret = AHardwareBuffer_allocate(&desc, &mInBuffer);
        LOGINFO("AHardwareBuffer_allocate ret: %d", ret);

        AHardwareBuffer_Planes planes_info = {0};
        ret = AHardwareBuffer_lockPlanes(mInBuffer,
                                         AHARDWAREBUFFER_USAGE_CPU_WRITE_MASK,
                                         -1,
                                         nullptr,
                                         &planes_info);
        if (ret != 0)
        {
            LOGERROR("Failed to AHardwareBuffer_lockPlanes");
            ret = AHardwareBuffer_unlock(mInBuffer, nullptr);
            return false;
        }
        else
        {
            const PixelFormatInfo& info = mPixelFormatInfoTables.at(pixelFormat);
            mBytesPixels = info.bpp / 8;
            unsigned int bytesPerRow = pixelsWide * mBytesPixels;
            memcpy(planes_info.planes[0].data, mipmaps[0].pdata, bytesPerRow * pixelsHigh);

            ret = AHardwareBuffer_unlock(mInBuffer, nullptr);
            if (ret != 0)
            {
                LOGERROR("Failed to AHardwareBuffer_unlock");
                return false;
            }
        }

        mBufferPtr = planes_info.planes[0].data;
        mWidth = pixelsWide;
        mHeight = pixelsHigh;
        mFormat = pixelFormat;
        mHasMipmaps = mipmapsNum > 1;
        mHadPackedData = true;

        EGLint eglImageAttributes[] = {EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE};
        EGLClientBuffer cb = eglGetNativeClientBufferANDROID(mInBuffer);
        LOGINFO("eglGetNativeClientBufferANDROID cb: %p", cb);
        EGLImageKHR eglImageHandle = eglCreateImageKHR(eglGetDisplay(EGL_DEFAULT_DISPLAY), EGL_NO_CONTEXT,
                                                       EGL_NATIVE_BUFFER_ANDROID,
                                                       cb,
                                                       reinterpret_cast<const EGLint*>(eglImageAttributes));
        LOGINFO("eglCreateImageKHR eglImageHandle: %p", eglImageHandle);
        LOGINFO("error: %d", eglGetError());

        glBindTexture(GL_TEXTURE_2D, mTextureId);
        glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, eglImageHandle);

        if (mipmapsNum == 1)
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mTexParams.antialiasEnabled ? GL_LINEAR : GL_NEAREST);
        else
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mTexParams.antialiasEnabled ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST);

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mTexParams.antialiasEnabled ? GL_LINEAR : GL_NEAREST);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mTexParams.wrapS);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mTexParams.wrapT);

        CHECK_GL_ERROR(); // clean possible GL error

        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
#else
        return GLTexture2D::initWithMipmaps(mipmaps, mipmapsNum, pixelFormat, pixelsWide, pixelsHigh);
#endif
    }

    //------------------------------------------------------------------------------------//
    bool GLEGLImageTexture::updateTexture(const void *data, size_t dataLen, PixelFormat pixelFormat, int offsetX, int offsetY, int pixelsWide, int pixelsHigh)
    {
#if (defined(ANDROID) || defined(__ANDROID__))
        if (!mHadPackedData)
        {
            return initWithData(pixelsWide, pixelsHigh, pixelFormat, (uint8_t*)data, dataLen, pixelsWide, pixelsHigh);
        }

        if (data == NULL || offsetX < 0 || (offsetX >= mWidth) || offsetY < 0 || (offsetY >= mHeight) || (mWidth - offsetX) < pixelsWide || (mHeight - offsetY) < pixelsHigh)
        {
            LOGERROR("%s", "Error: parameter is not right");
            return false;
        }

        const PixelFormatInfo& info = mPixelFormatInfoTables.at(mFormat);
        unsigned int bytesPixel = info.bpp / 8;
        unsigned int bytesPerRow = pixelsWide * bytesPixel;
        unsigned int offset = (offsetY * mWidth + offsetX) * mBytesPixels;
        memcpy((uint8_t*)mBufferPtr + offset, data, bytesPerRow * pixelsHigh);

        return true;
#else
        return GLTexture2D::updateTexture(data, dataLen, pixelFormat, offsetY, offsetY, pixelsWide, pixelsHigh);
#endif
    }

    //------------------------------------------------------------------------------------//
    textureUpdate GLEGLImageTexture::getTextureUpdateFunc(void)
    {
#if (defined(ANDROID) || defined(__ANDROID__))
        textureUpdate func = reinterpret_cast<textureUpdate>(&GLEGLImageTexture::updateTexture);
        return func;
#else
        return GLTexture2D::getTextureUpdateFunc();
#endif
    }

    //------------------------------------------------------------------------------------//
    uint8_t* GLEGLImageTexture::lockTexture(void)
    {
#if (defined(ANDROID) || defined(__ANDROID__))
        AHardwareBuffer_lock(mInBuffer, AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN, -1, nullptr,
                             (void **) &mBufferPtr);
        return (uint8_t*)mBufferPtr;
    }
#else
        return GLTexture2D::lockTexture();
#endif

    //------------------------------------------------------------------------------------//
    void GLEGLImageTexture::unlockTexture(void)
    {
#if (defined(ANDROID) || defined(__ANDROID__))
        int ret = AHardwareBuffer_unlock(mInBuffer, nullptr);
        if (ret != 0)
        {
            LOGERROR("Failed to AHardwareBuffer_unlock");
        }
#endif
    }

END