//
// Created by Joe.Bi on 2024/11/9.
//

#include "GLTexture2D.h"
#include "util/GLDeviceInfo.h"
#include "util/GLUtil.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    GLTexture2D::GLTexture2D(const TexParam& param, const char* name/* = nullptr*/)
    : GLTexture(param, name)
    , mFormat(NONE)
    , mHasMipmaps(false)
    , mHadPackedData(false)
    , mBytesPixels(0)
    {
    }

    //------------------------------------------------------------------------------------//
    GLTexture2D::GLTexture2D(const GLTexture2D& texture2D)
    : GLTexture(texture2D)
    , mFormat(texture2D.mFormat)
    , mHasMipmaps(texture2D.mHasMipmaps)
    , mHadPackedData(texture2D.mHadPackedData)
    , mBytesPixels(texture2D.mBytesPixels)
    {
    }

    //------------------------------------------------------------------------------------//
    GLTexture2D::GLTexture2D(GLTexture2D&& texture2D)
    : GLTexture(texture2D)
    , mFormat(texture2D.mFormat)
    , mHasMipmaps(texture2D.mHasMipmaps)
    , mHadPackedData(texture2D.mHadPackedData)
    , mBytesPixels(texture2D.mBytesPixels)
    {
        texture2D.mFormat = NONE;
        texture2D.mHasMipmaps = false;
        texture2D.mHadPackedData = false;
        texture2D.mBytesPixels = 0;
    }

    //------------------------------------------------------------------------------------//
    GLTexture2D& GLTexture2D::operator=(const GLTexture2D& texture2D)
    {
        GLTexture::operator=(texture2D);
        mFormat = texture2D.mFormat;
        mHasMipmaps = texture2D.mHasMipmaps;
        mHadPackedData = texture2D.mHadPackedData;
        mBytesPixels = texture2D.mBytesPixels;

        return *this;
    }

    //------------------------------------------------------------------------------------//
    GLTexture2D& GLTexture2D::operator=(GLTexture2D&& texture2D)
    {
        GLTexture::operator=(texture2D);
        mFormat = texture2D.mFormat;
        mHasMipmaps = texture2D.mHasMipmaps;
        mHadPackedData = texture2D.mHadPackedData;
        mBytesPixels = texture2D.mBytesPixels;

        texture2D.mFormat = NONE;
        texture2D.mHasMipmaps = false;
        texture2D.mHadPackedData = false;
        texture2D.mBytesPixels = 0;

        return *this;
    }

    //------------------------------------------------------------------------------------//
    GLTexture2D::~GLTexture2D(void)
    {
        unbindTexture();
        mFormat = NONE;
        mHasMipmaps = false;
        mHadPackedData = false;
        mBytesPixels = 0;
    }

    //------------------------------------------------------------------------------------//
    bool GLTexture2D::initWithData(unsigned int width, unsigned int height, PixelFormat pixelFormat, const uint8_t* data/* = nullptr */, int dataLen/* = 0 */, unsigned int pixelsWide/* = 0 */, unsigned int pixelsHigh/* = 0 */)
    {
        MipmapInfo info;
        info.pdata = const_cast<uint8_t*>(data);
        info.len = dataLen;

        return initWithMipmaps(&info, 1, pixelFormat, pixelsWide, pixelsHigh);
    }

    //------------------------------------------------------------------------------------//
    bool GLTexture2D::initWithMipmaps(MipmapInfo *mipmaps, int mipmapsNum, PixelFormat pixelFormat, unsigned int pixelsWide, unsigned int pixelsHigh)
    {
        if (pixelFormat == PixelFormat::NONE || pixelFormat == PixelFormat::AUTO)
        {
            LOGERROR("the pixelFormat param must be a certain value!");
            return false;
        }

        assert(pixelsWide > 0 && pixelsHigh > 0);

        if (mipmapsNum <= 0)
        {
            LOGERROR("WARNING: mipmap number is less than 1");
            return false;
        }

        if(mPixelFormatInfoTables.find(pixelFormat) == mPixelFormatInfoTables.end())
        {
            LOGERROR("WARNING: unsupported pixelformat: %lx", (unsigned long)pixelFormat);
            return false;
        }

        const PixelFormatInfo& info = mPixelFormatInfoTables.at(pixelFormat);
        if (info.compressed && !GLDeviceInfo::checkForFeatureSupported(FeatureType::PVRTC)
            && !GLDeviceInfo::checkForFeatureSupported(FeatureType::ETC1)
            && !GLDeviceInfo::checkForFeatureSupported(FeatureType::S3TC)
            && !GLDeviceInfo::checkForFeatureSupported(FeatureType::AMD_COMPRESSED_ATC))
        {
            LOGERROR("WARNING: PVRTC/ETC images are not supported");
            return false;
        }

        //Set the row align only when mipmapsNum == 1 and the data is uncompressed
        if (mipmapsNum == 1 && !info.compressed)
        {
            unsigned int bytesPerRow = pixelsWide * info.bpp / 8;

            if(bytesPerRow % 8 == 0)
                glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
            else if(bytesPerRow % 4 == 0)
                glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            else if(bytesPerRow % 2 == 0)
                glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
            else
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        }
        else
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glBindTexture(GL_TEXTURE_2D, mTextureId);

        if (mipmapsNum == 1)
        {
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mTexParams.antialiasEnabled ? GL_LINEAR : GL_NEAREST);
        }else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mTexParams.antialiasEnabled ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST);
        }

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mTexParams.antialiasEnabled ? GL_LINEAR : GL_NEAREST);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mTexParams.wrapS);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mTexParams.wrapT);

        CHECK_GL_ERROR(); // clean possible GL error

        // Specify OpenGL texture image
        int width = pixelsWide;
        int height = pixelsHigh;

        for (int i = 0; i < mipmapsNum; ++i)
        {
            unsigned char *data = mipmaps[i].pdata;
            GLsizei datalen = mipmaps[i].len;

            if (info.compressed)
            {
                glCompressedTexImage2D(GL_TEXTURE_2D, i, info.internalFormat, (GLsizei)width, (GLsizei)height, 0, datalen, data);
            }
            else
            {
                glTexImage2D(GL_TEXTURE_2D, i, info.internalFormat, (GLsizei)width, (GLsizei)height, 0, info.format, info.type, data);
            }

            if (i > 0 && (width != height || ccNextPOT(width) != width ))
            {
                LOGINFO("Texture2D. WARNING. Mipmap level %u is not squared. Texture won't render correctly. width=%d != height=%d", i, width, height);
            }

            GLenum err = glGetError();
            if (err != GL_NO_ERROR)
            {
                LOGERROR("Texture2D: Error uploading compressed texture level: %u . glError: 0x%04X", i, err);
                return false;
            }

            width = MAX(width >> 1, 1);
            height = MAX(height >> 1, 1);
        }

        mWidth = pixelsWide;
        mHeight = pixelsHigh;
        mFormat = pixelFormat;

        mHasMipmaps = mipmapsNum > 1;

        mHadPackedData = true;

        mBytesPixels = info.bpp / 8;

        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        return true;
    }

    //------------------------------------------------------------------------------------//
    bool GLTexture2D::updateTexture(const void *data, size_t dataLen, PixelFormat pixelFormat, int offsetX, int offsetY, int pixelsWide, int pixelsHigh)
    {
        if (!mHadPackedData)
        {
            LOGERROR("%s", "Error: GL texture is null");
            return initWithData(pixelsWide, pixelsHigh, pixelFormat, (uint8_t*)data, dataLen, pixelsWide, pixelsHigh);
        }

        if (data == NULL || offsetX < 0 || (offsetX >= mWidth) || offsetY < 0 || (offsetY >= mHeight) || (mWidth - offsetX) < pixelsWide || (mHeight - offsetY) < pixelsHigh)
        {
            LOGERROR("%s", "Error: parameter is not right");
            return false;
        }

        glBindTexture(GL_TEXTURE_2D, mTextureId);
        const PixelFormatInfo& info = mPixelFormatInfoTables.at(mFormat);
        if (info.compressed)
            glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, offsetY, pixelsWide, pixelsHigh, info.format, info.type, data);
        else
            glTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, offsetY, pixelsWide, pixelsHigh, info.format, info.type, data);

        return true;
    }

    //------------------------------------------------------------------------------------//
    textureUpdate GLTexture2D::getTextureUpdateFunc(void)
    {
        textureUpdate func = reinterpret_cast<textureUpdate>(&GLTexture2D::updateTexture);
        return func;
    }

    //------------------------------------------------------------------------------------//
    void GLTexture2D::bindTexture(void)
    {
        if(glIsTexture(mTextureId))
            glBindTexture(GL_TEXTURE_2D, mTextureId);
    }

    //------------------------------------------------------------------------------------//
    void GLTexture2D::unbindTexture(void)
    {
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

END