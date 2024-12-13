//
// Created by Joe.Bi on 2024/11/20.
//

#ifndef __GLTexture_h__
#define __GLTexture_h__
#include "Uncopyable.hpp"
#include "GLImageDef.h"
#include <GLES3/gl3.h>
#include <GLES2/gl2.h>
#include <string>
#include <functional>
#include <map>

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    typedef enum
    {
        GLTEXTURE  = 0,
        OESTEXTURE = 1,
        EGLTEXTURE = 2,
    }TextureKind;

    typedef enum : uint32_t
    {
        TEXTURE2D       = 1,
        TEXTURECUBE     = 2,
    }TextureType;
    
    typedef enum : uint32_t
    {
        DEFAULT_2D		= 0,
        POSITIVE_X		= 1,
        NEGATIVE_X		= 2,
        POSITIVE_Y		= 3,
        NEGATIVE_Y		= 4,
        POSITIVE_Z		= 5,
        NEGATIVE_Z		= 6,
    }WhatPlane;
    
    enum WrapType
    {
        WRAPEDGE        = GL_CLAMP_TO_EDGE,
        WRAPREPEAT      = GL_REPEAT,
        WRAPMIRROR      = GL_MIRRORED_REPEAT,
#ifdef GL_CAMP_TO_BORDER
        WRAP2BORDER     = GL_CAMP_TO_BORDER,
#endif
    };
    
    typedef struct _TexParam
    {
        WrapType    wrapS;
        WrapType    wrapT;
        bool        antialiasEnabled;
    
        _TexParam(void) = default;
    
        _TexParam(const _TexParam& param)
        {
            wrapS = param.wrapS; wrapT = param.wrapT;
            antialiasEnabled = param.antialiasEnabled;
        }
    
        _TexParam& operator=(const _TexParam& param)
        {
            wrapS = param.wrapS; wrapT = param.wrapT;
            antialiasEnabled = param.antialiasEnabled;
            return *this;
        }
    
    }TexParam;
    
    class GLTexture;
    typedef std::shared_ptr<GLTexture>  sharedTexture;

    typedef std::map<PixelFormat, const PixelFormatInfo> PixelFormatInfoMap;

    typedef bool (GLTexture::*textureUpdate)(const void *data, size_t dataLen, PixelFormat pixelFormat, int offsetX, int offsetY, int pixelsWide, int pixelsHigh);

    typedef std::function<bool (const void *data, size_t dataLen, PixelFormat pixelFormat, int offsetX, int offsetY, int pixelsWide, int pixelsHigh)> textureUpdateFunc;

//------------------------------------------------------------------------------------//
    class API_EXPORTS GLTexture
    {
        friend void deleter<GLTexture>(GLTexture*);
    public:
        template<typename T>
        static sharedTexture createTexture(const TexParam& param, const char* name = nullptr)
        { return sharedTexture(new T(param, name), deleter<T>); }

        GLTexture(void) = delete;

        GLTexture(const GLTexture&);

        GLTexture(GLTexture&&);

        GLTexture& operator=(const GLTexture&);

        GLTexture& operator=(GLTexture&&);

        void setTextureParam(const TexParam& param) { mTexParams = param; }

        virtual bool textureDataIsNull(void) const { return true; }

        virtual bool initWithData(unsigned int width, unsigned int height, PixelFormat pixelFormat, const uint8_t* data = nullptr, int dataLen = 0, unsigned int pixelsWide = 0, unsigned int pixelsHigh = 0)
        { return false; }

        virtual bool initWithMipmaps(MipmapInfo *mipmaps, int mipmapsNum, PixelFormat pixelFormat, unsigned int width, unsigned int height)
        { return false; }

        virtual bool updateTexture(const void *data, size_t dataLen, PixelFormat pixelFormat, int offsetX, int offsetY, int pixelsWide, int pixelsHigh)
        { return false; }

        virtual uint8_t* lockTexture(void)
        { return nullptr; }

        virtual void unlockTexture(void) { }

        virtual unsigned int getBytesPixel(void) { return 0; }

        GLuint getTextureId(void) const { return  mTextureId; }

        char* getTextureName(void) const { return mName; }

        int getTextureWidth(void) const { return mWidth; }

        int getTextureHeight(void) const { return mHeight; }

        int getTextureStride(void) const {return mStride; }

        virtual const PixelFormat getFormat(void) const { return PixelFormat::NONE; }

        const TexParam& getTextureParam(void) const { return mTexParams; }

        virtual bool isMipmapTexture(void) const { return false; }

        virtual textureUpdate getTextureUpdateFunc(void) { return nullptr; }

        textureUpdateFunc getTextureUpdateInterface(void);

        virtual void bindTexture(void) = 0;

        virtual void unbindTexture(void) = 0;

        static PixelFormatInfoMap mPixelFormatInfoTables;

    protected:
        GLTexture(const TexParam& param, const char* name = nullptr);
        virtual ~GLTexture(void);

    private:
        void release(void);

    protected:
        GLuint          mTextureId;
        char*           mName;
        int             mWidth;
        int             mHeight;
        int             mStride;
        TexParam        mTexParams;
    };

END


#endif //__GLTexture_h__
