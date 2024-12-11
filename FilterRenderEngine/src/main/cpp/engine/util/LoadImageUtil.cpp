//
// Created by Joe.Bi on 2024/11/9.
//

#include "GLImageDef.h"
#include "LogUtil.h"
#include <GLES3/gl3.h>
#include <GLES2/gl2.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdint.h>

//----------------------------------------------------------------------------------------//
BEGIN

#ifdef __cplusplus
extern "C" {
#endif
    //------------------------------------------------------------------------------------//
    void allocNativeImage(NativeImage *pImage)
    {
        if (pImage->height == 0 || pImage->width == 0)
            return;

        switch (pImage->format)
        {
            case IMAGE_FORMAT_RGBA:
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 4));
                break;
            case IMAGE_FORMAT_YUYV:
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 2));
                break;
            case IMAGE_FORMAT_NV12:
            case IMAGE_FORMAT_NV21:
            {
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(
                        pImage->width * pImage->height * 1.5));
                pImage->ppPlane[1] = pImage->ppPlane[0] + pImage->width * pImage->height;
            }
                break;
            case IMAGE_FORMAT_I420:
            {
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(
                        pImage->width * pImage->height * 1.5));
                pImage->ppPlane[1] = pImage->ppPlane[0] + pImage->width * pImage->height;
                pImage->ppPlane[2] = pImage->ppPlane[1] + pImage->width * (pImage->height >> 2);
            }
                break;
            case IMAGE_FORMAT_GRAY:
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height));
                break;
            default:
                LOGERROR("NativeImageUtil::AllocNativeImage do not support the format. Format = %d",
                         pImage->format);
                break;
        }
    }

    //------------------------------------------------------------------------------------//
    void freeNativeImage(NativeImage *pImage)
    {
        if (pImage == nullptr || pImage->ppPlane[0] == nullptr) return;

        free(pImage->ppPlane[0]);
        pImage->ppPlane[0] = nullptr;
        pImage->ppPlane[1] = nullptr;
        pImage->ppPlane[2] = nullptr;
    }

    //------------------------------------------------------------------------------------//
    void copyNativeImage(NativeImage *pSrcImg, NativeImage *pDstImg)
    {
        if (pSrcImg == nullptr || pSrcImg->ppPlane[0] == nullptr) return;

        if (pSrcImg->format != pDstImg->format ||
            pSrcImg->width != pDstImg->width ||
            pSrcImg->height != pDstImg->height)
            return;

        if (pDstImg->ppPlane[0] == nullptr)
            allocNativeImage(pDstImg);

        switch (pSrcImg->format)
        {
            case IMAGE_FORMAT_I420:
            case IMAGE_FORMAT_NV21:
            case IMAGE_FORMAT_NV12:
                memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0],pSrcImg->width * pSrcImg->height * 1.5);
                break;
            case IMAGE_FORMAT_YUYV:
                memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0],pSrcImg->width * pSrcImg->height * 2);
                break;
            case IMAGE_FORMAT_RGBA:
                memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0],pSrcImg->width * pSrcImg->height * 4);
                break;
            case IMAGE_FORMAT_GRAY:
                memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pSrcImg->width * pSrcImg->height);
                break;
            default:
                LOGERROR("NativeImageUtil::CopyNativeImage do not support the format. Format = %d", pSrcImg->format);
                break;
        }
    }

    //------------------------------------------------------------------------------------//
    void dumpNativeImage(NativeImage *pSrcImg, const char *pPath, const char *pFileName)
    {
        if (pSrcImg == nullptr || pPath == nullptr || pFileName == nullptr)
            return;

        if (access(pPath, 0) == -1)
            mkdir(pPath, 0666);

        char imgPath[256] = {0};
        const char *pExt = nullptr;
        switch (pSrcImg->format)
        {
            case IMAGE_FORMAT_I420:
                pExt = IMAGE_FORMAT_I420_EXT;
                break;
            case IMAGE_FORMAT_NV12:
                pExt = IMAGE_FORMAT_NV12_EXT;
                break;
            case IMAGE_FORMAT_NV21:
                pExt = IMAGE_FORMAT_NV21_EXT;
                break;
            case IMAGE_FORMAT_RGBA:
                pExt = IMAGE_FORMAT_RGBA_EXT;
                break;
            case IMAGE_FORMAT_YUYV:
                pExt = IMAGE_FORMAT_YUYV_EXT;
                break;
            case IMAGE_FORMAT_GRAY:
                pExt = IMAGE_FORMAT_GRAY_EXT;
                break;
            default:
                pExt = "Default";
                break;
        }

        sprintf(imgPath, "%s/IMG_%dx%d_%s.%s", pPath, pSrcImg->width, pSrcImg->height, pFileName,
                pExt);

        FILE *fp = fopen(imgPath, "wb");

        LOGERROR("DumpNativeImage fp=%p, file=%s", fp, imgPath);

        if (fp)
        {
            switch (pSrcImg->format)
            {
                case IMAGE_FORMAT_I420:
                {
                    fwrite(pSrcImg->ppPlane[0],static_cast<size_t>(pSrcImg->width * pSrcImg->height), 1, fp);
                    fwrite(pSrcImg->ppPlane[1],static_cast<size_t>((pSrcImg->width >> 1) * (pSrcImg->height >> 1)), 1, fp);
                    fwrite(pSrcImg->ppPlane[2],static_cast<size_t>((pSrcImg->width >> 1) * (pSrcImg->height >> 1)), 1, fp);
                }
                    break;
                case IMAGE_FORMAT_NV21:
                case IMAGE_FORMAT_NV12:
                {
                    fwrite(pSrcImg->ppPlane[0],static_cast<size_t>(pSrcImg->width * pSrcImg->height), 1, fp);
                    fwrite(pSrcImg->ppPlane[1],static_cast<size_t>(pSrcImg->width * (pSrcImg->height >> 1)), 1, fp);
                }
                    break;
                case IMAGE_FORMAT_RGBA:
                    fwrite(pSrcImg->ppPlane[0],static_cast<size_t>(pSrcImg->width * pSrcImg->height * 4), 1, fp);
                    break;
                case IMAGE_FORMAT_YUYV:
                    fwrite(pSrcImg->ppPlane[0],static_cast<size_t>(pSrcImg->width * pSrcImg->height * 2), 1, fp);
                    break;
                default:
                {
                    fwrite(pSrcImg->ppPlane[0],static_cast<size_t>(pSrcImg->width * pSrcImg->height), 1, fp);
                    LOGERROR("DumpNativeImage default");
                }
                    break;
            }

            fclose(fp);
            fp = NULL;
        }
    }

#ifdef __cplusplus
    }
#endif

END