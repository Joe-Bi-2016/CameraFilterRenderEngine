//
// Created by Joe.Bi on 2024/11/9.
//

#ifndef __GLImageDef_h__
#define __GLImageDef_h__
#include "util/GLUtil.h"
#include "GLDataTypeDef.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    typedef struct _PixelFormatInfo
    {
        _PixelFormatInfo(TextureFormat informat, InputFormat pixelformat, DataType type, int aBpp, bool aCompressed, bool anAlpha)
        : internalFormat(informat)
        , format(pixelformat)
        , type(type)
        , bpp(aBpp)
        , compressed(aCompressed)
        , alpha(anAlpha)
        {}

        TextureFormat 	internalFormat;
        InputFormat 	format;
        DataType        type;
        int 	        bpp;
        bool 	        compressed;
        bool 	        alpha;
    }PixelFormatInfo;

    typedef struct _MipmapInfo
    {
        uint8_t* pdata;
        int len;
    }MipmapInfo;

    //------------------------------------------------------------------------------------//
    typedef enum
    {
        UNKNOWN_FORMAT	= -1,
        PVR_FORMAT		= 0,
        ETC_FORMAT		= 1,
        WEBP_FORMAT		= 2,
        STB_FORMAT		= 3,
    }ImageFormat;

    typedef struct
    {
        ImageFormat		format;
        uint8_t*		data;
        int 			width;
        int			    height;
        PixelFormat		pixelFormat;
        int			    bytes;
    }ImageData;

    typedef ImageData* (*loadPicture)(const uint8_t* filePath);
    typedef void (*releasePicture)(ImageData* bitmap);

    struct InputImageInfo
    {
        void*	    data;
        int	        size;
        bool 		cached;
        PixelFormat format;
        int	        width;
        int	        height;
        int	        offsetX;
        int		    offsetY;

        InputImageInfo(void)
        {
            data    = NULL;
            size    = 0;
            cached  = false;
            format  = AUTO;
            width   = 0;
            height  = 0;
            offsetX = 0;
            offsetY = 0;
        }

        ~InputImageInfo(void)
        {
            if(cached && data)
            {
                int8_t* ptr = (int8_t*)data;
                safeDeleteArray(ptr);
            }

            data    = NULL;
            size    = 0;
            cached  = false;
            format  = AUTO;
            width   = 0;
            height  = 0;
            offsetX = 0;
            offsetY = 0;
        }

        InputImageInfo& operator=(const InputImageInfo& other)
        {
            this->size = other.size;
            this->cached = other.cached;
            this->format = other.format;
            this->width = other.width;
            this->height = other.height;
            this->offsetX = other.offsetX;
            this->offsetY = other.offsetY;

            if (other.cached && other.data)
            {
                int8_t* ptr = (int8_t*)data;
                safeDeleteArray(ptr);

                this->data = (void*)safeNewArray(int8_t, this->size);
                memset(this->data, 0x0, this->size);
                memcpy(this->data, other.data, this->size);
            }
            else
                this->data = other.data;

            return *this;
        }
    };

    //------------------------------------------------------------------------------------//
    #define IMAGE_FORMAT_RGBA           0x01
    #define IMAGE_FORMAT_NV21           0x02
    #define IMAGE_FORMAT_NV12           0x03
    #define IMAGE_FORMAT_I420           0x04
    #define IMAGE_FORMAT_YUYV           0x05
    #define IMAGE_FORMAT_GRAY           0x06

    #define IMAGE_FORMAT_RGBA_EXT       "RGB32"
    #define IMAGE_FORMAT_NV21_EXT       "NV21"
    #define IMAGE_FORMAT_NV12_EXT       "NV12"
    #define IMAGE_FORMAT_I420_EXT       "I420"
    #define IMAGE_FORMAT_YUYV_EXT       "YUYV"
    #define IMAGE_FORMAT_GRAY_EXT       "GRAY"

    typedef struct NativeRectF
    {
        float left;
        float top;
        float right;
        float bottom;

        NativeRectF()
        {
            left = top = right = bottom = 0.0f;
        }
    } RectF;

    struct SizeF
    {
        float width;
        float height;

        SizeF()
        {
            width = height = 0;
        }
    };

    struct NativeImage
    {
        int width;
        int height;
        int format;
        uint8_t* ppPlane[3];

        NativeImage()
        {
            width = 0;
            height = 0;
            format = 0;
            ppPlane[0] = nullptr;
            ppPlane[1] = nullptr;
            ppPlane[2] = nullptr;
        }
    };

#ifdef __cplusplus
    extern "C" {
#endif
    void allocNativeImage(NativeImage *pImage);

    void freeNativeImage(NativeImage *pImage);

    void copyNativeImage(NativeImage *pSrcImg, NativeImage *pDstImg);

    void dumpNativeImage(NativeImage *pSrcImg, const char *pPath, const char *pFileName);
#ifdef __cplusplus
    }
#endif

END

#endif //__GLImageDef_h__
