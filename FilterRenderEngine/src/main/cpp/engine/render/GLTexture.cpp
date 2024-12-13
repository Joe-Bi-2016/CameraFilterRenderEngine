//
// Created by Joe.Bi on 2024/11/20.
//

#include "GLTexture.h"
#include "util/GLDeviceInfo.h"
#include "util/GLUtil.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    typedef PixelFormatInfoMap::value_type PixelFormatInfoMapValue;

    static const PixelFormatInfoMapValue TexturePixelFormatInfoTablesValue[] =
    {
        PixelFormatInfoMapValue(PixelFormat::RGBA8888, PixelFormatInfo(RGBA_TEXTURE, RGBA_TEXTURE, UNSIGNED_BYTE_TYPE, 32, false, true)),
        PixelFormatInfoMapValue(PixelFormat::RGBA4444, PixelFormatInfo(RGBA_TEXTURE, RGBA_TEXTURE, UNSIGNED_SHORT_4_4_4_4_TYPE, 16, false, true)),
        PixelFormatInfoMapValue(PixelFormat::RGB5A1, PixelFormatInfo(RGB_TEXTURE, RGB_TEXTURE, UNSIGNED_SHORT_5_5_5_1_TYPE, 16, false, true)),
        PixelFormatInfoMapValue(PixelFormat::RGB565, PixelFormatInfo(RGB_TEXTURE, RGB_TEXTURE, UNSIGNED_SHORT_5_6_5_TYPE, 16, false, false)),
        PixelFormatInfoMapValue(PixelFormat::RGB888, PixelFormatInfo(RGB_TEXTURE, RGB_TEXTURE, UNSIGNED_BYTE_TYPE, 24, false, false)),
        PixelFormatInfoMapValue(PixelFormat::A8, PixelFormatInfo(ALPHA_TEXTURE, ALPHA_TEXTURE, UNSIGNED_BYTE_TYPE, 8, false, false)),
        PixelFormatInfoMapValue(PixelFormat::I8, PixelFormatInfo(LUMINANCE_TEXTURE, LUMINANCE_TEXTURE, UNSIGNED_BYTE_TYPE, 8, false, false)),
        PixelFormatInfoMapValue(PixelFormat::AI88, PixelFormatInfo(LUMINANCEALPHA_TEXTURE, LUMINANCEALPHA_TEXTURE, UNSIGNED_BYTE_TYPE, 16, false, true)),

#if defined( GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG)
        PixelFormatInfoMapValue(PixelFormat::PVRTC2, PixelFormatInfo(COMPRESSED_RGB_PVRTC_2BPPV1_IMG_TEXTURE, NONE_TEXTURE, NONE_TYPE, 2, true, false)),
        PixelFormatInfoMapValue(PixelFormat::PVRTC2A, PixelFormatInfo(COMPRESSED_RGBA_PVRTC_2BPPV1_IMG_TEXTURE, NONE_TEXTURE, NONE_TYPE, 2, true, true)),
        PixelFormatInfoMapValue(PixelFormat::PVRTC4, PixelFormatInfo(COMPRESSED_RGB_PVRTC_4BPPV1_IMG_TEXTURE, NONE_TEXTURE, NONE_TYPE, 4, true, false)),
        PixelFormatInfoMapValue(PixelFormat::PVRTC4A, PixelFormatInfo(COMPRESSED_RGBA_PVRTC_4BPPV1_IMG_TEXTURE, NONE_TEXTURE, NONE_TYPE, 4, true, true)),
#endif

#if defined(GL_ETC1_RGB8_OES)
        PixelFormatInfoMapValue(PixelFormat::ETC, PixelFormatInfo(ETC1_RGB8_OES_TEXTURE, NONE_TEXTURE, NONE_TYPE, 4, true, false)),
#endif

#if defined(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
        PixelFormatInfoMapValue(PixelFormat::S3TC_DXT1, PixelFormatInfo(COMPRESSED_RGBA_S3TC_DXT1_EXT_TEXTURE, NONE_TEXTURE, NONE_TYPE, 4, true, false)),
#endif

#if defined(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT)
        PixelFormatInfoMapValue(PixelFormat::S3TC_DXT3, PixelFormatInfo(COMPRESSED_RGBA_S3TC_DXT3_EXT_TEXTURE, NONE_TEXTURE, NONE_TYPE, 8, true, false)),
#endif

#if defined(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)
        PixelFormatInfoMapValue(PixelFormat::S3TC_DXT5, PixelFormatInfo(COMPRESSED_RGBA_S3TC_DXT5_EXT_TEXTURE, NONE_TEXTURE, NONE_TYPE, 8, true, false)),
#endif

#ifdef GL_ATC_RGB_AMD
        PixelFormatInfoMapValue(backend::PixelFormat::ATC_RGB, Texture2D::PixelFormatInfo(ATC_RGB_AMD_TEXTURE, NONE_TEXTURE, NONE_TYPE, 4, true, false)),
#endif

#ifdef GL_ATC_RGBA_EXPLICIT_ALPHA_AMD
        PixelFormatInfoMapValue(backend::PixelFormat::ATC_EXPLICIT_ALPHA, Texture2D::PixelFormatInfo(ATC_RGBA_EXPLICIT_ALPHA_AMD_TEXTURE, NONE_TEXTURE, NONE_TYPE, 8, true, false)),
#endif

#ifdef GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD
        PixelFormatInfoMapValue(backend::PixelFormat::ATC_INTERPOLATED_ALPHA, Texture2D::PixelFormatInfo(ATC_RGBA_INTERPOLATED_ALPHA_AMD_TEXTURE, NONE_TEXTURE, NONE_TYPE, 8, true, false)),
#endif
    };

    PixelFormatInfoMap GLTexture::mPixelFormatInfoTables(TexturePixelFormatInfoTablesValue,
        TexturePixelFormatInfoTablesValue + sizeof(TexturePixelFormatInfoTablesValue) / sizeof(TexturePixelFormatInfoTablesValue[0]));

    //------------------------------------------------------------------------------------//
    GLTexture::GLTexture(const TexParam& param, const char* name/* = nullptr*/)
    : mTextureId(GL_NONE)
    , mName(nullptr)
    , mWidth(0)
    , mHeight(0)
    , mStride(0)
    , mTexParams(param)
    {
        glGenTextures(1, &mTextureId);

        if (name)
        {
            size_t len = strlen(name);
            mName = new char[len+1];
            memset(mName, 0x0, len+1);
            memcpy(mName, name, len);
        }
        else
        {
            size_t len = strlen("TextureId_") + 4;
            mName = new char[len];
            memset(mName, 0x0, len);
            snprintf(mName, len, "%s%d", "TextureId_", mTextureId);
        }
    }

    //------------------------------------------------------------------------------------//
    GLTexture::GLTexture(const GLTexture& texture2D)
    : mTextureId(texture2D.mTextureId)
    , mName(nullptr)
    , mWidth(texture2D.mWidth)
    , mHeight(texture2D.mHeight)
    , mStride(texture2D.mStride)
    , mTexParams(texture2D.mTexParams)
    {
        if (texture2D.mName)
        {
            size_t len = strlen(texture2D.mName);
            mName = new char[len+1];
            memset(mName, 0x0, len+1);
            memcpy(mName, texture2D.mName, len);
        }
    }

    //------------------------------------------------------------------------------------//
    GLTexture::GLTexture(GLTexture&& texture2D)
    : mTextureId(texture2D.mTextureId)
    , mName(texture2D.mName)
    , mWidth(texture2D.mWidth)
    , mHeight(texture2D.mHeight)
    , mStride(texture2D.mStride)
    , mTexParams(texture2D.mTexParams)
    {
        texture2D.mTextureId = GL_NONE;
        texture2D.mName = nullptr;
        texture2D.mWidth = 0;
        texture2D.mHeight = 0;
        texture2D.mStride = 0;
    }

    //------------------------------------------------------------------------------------//
    GLTexture& GLTexture::operator=(const GLTexture& texture2D)
    {
        release();

        mTextureId = texture2D.mTextureId;
        mWidth = texture2D.mWidth;
        mHeight = texture2D.mHeight;
        mStride = texture2D.mStride;
        mTexParams = texture2D.mTexParams;

        if (texture2D.mName)
        {
            size_t len = strlen(texture2D.mName);
            mName = new char[len+1];
            memset(mName, 0x0, len+1);
            memcpy(mName, texture2D.mName, len);
        }

        return *this;
    }

    //------------------------------------------------------------------------------------//
    GLTexture& GLTexture::operator=(GLTexture&& texture2D)
    {
        release();

        mTextureId = texture2D.mTextureId;
        mName = texture2D.mName;
        mWidth = texture2D.mWidth;
        mHeight = texture2D.mHeight;
        mStride = texture2D.mStride;
        mTexParams = texture2D.mTexParams;

        texture2D.mTextureId = GL_NONE;
        texture2D.mName = nullptr;
        texture2D.mWidth = 0;
        texture2D.mHeight = 0;
        texture2D.mStride = 0;

        return *this;
    }

    //------------------------------------------------------------------------------------//
    GLTexture::~GLTexture(void)
    {
        release();
    }

    //------------------------------------------------------------------------------------//
    void GLTexture::release(void)
    {
        if(glIsTexture(mTextureId))
            glDeleteTextures(1, &mTextureId);

        mTextureId = GL_NONE;
        mWidth = 0;
        mHeight = 0;
        mStride = 0;

        if (mName)
        {
            delete [] mName; mName = nullptr;
        }
    }

    //------------------------------------------------------------------------------------//
    textureUpdateFunc GLTexture::getTextureUpdateInterface(void)
    {
        auto fn = [&](const void *data, size_t dataLen, PixelFormat pixelFormat, int offsetX, int offsetY, int pixelsWide, int pixelsHigh)
        {
            return this->updateTexture(data, dataLen, pixelFormat, offsetY, offsetX, pixelsWide, pixelsHigh);
        };

        return fn;
    }


END
