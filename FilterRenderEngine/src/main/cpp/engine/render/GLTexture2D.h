//
// Created by Joe.Bi on 2024/11/9.
//

#ifndef __GLTexture2D_h__
#define __GLTexture2D_h__
#include "GLTexture.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    class API_EXPORTS GLTexture2D : public GLTexture
    {
        friend void deleter<GLTexture2D>(GLTexture2D*);
        friend class GLTexture;
    public:
        GLTexture2D(void) = delete;

        GLTexture2D(const GLTexture2D&);

        GLTexture2D(GLTexture2D&&);

        GLTexture2D& operator=(const GLTexture2D&);

        GLTexture2D& operator=(GLTexture2D&&);

        bool textureDataIsNull(void) const { return !mHadPackedData; }

        const PixelFormat getFormat(void) const { return mFormat; }

        bool isMipmapTexture(void) const { return mHasMipmaps; }

        bool initWithData(const uint8_t* data, int dataLen, PixelFormat pixelFormat, unsigned int pixelsWide, unsigned int pixelsHigh, unsigned int width, unsigned int height);

        bool initWithMipmaps(MipmapInfo *mipmaps, int mipmapsNum, PixelFormat pixelFormat, unsigned int width, unsigned int height);

        bool updateTexture(const void *data, size_t dataLen, PixelFormat pixelFormat, int offsetX, int offsetY, int pixelsWide, int pixelsHigh);

        textureUpdate getTextureUpdateFunc(void);

        void bindTexture(void);

        void unbindTexture(void);

    private:
        GLTexture2D(const TexParam& param, const char* name = nullptr);
        ~GLTexture2D(void);

    protected:
        PixelFormat     mFormat;
        bool            mHasMipmaps;
        bool            mHadPackedData;
    };

END

#endif //__GLTexture2D_h__
