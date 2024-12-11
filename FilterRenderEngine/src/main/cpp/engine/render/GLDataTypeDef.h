//
// Created by Joe.Bi on 2024/11/15.
//

#ifndef __GLDataTypedef_h__
#define __GLDataTypedef_h__
#include "util/GLUtil.h"
#include <GLES3/gl3.h>
#include <GLES2/gl2.h>
#include <memory>
#include <map>
#include <cassert>

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    typedef enum
    {
        RGB_TEXTURE                                 = GL_RGB,
        RGBA_TEXTURE                                = GL_RGBA,
        LUMINANCEALPHA_TEXTURE                      = GL_LUMINANCE_ALPHA,
        LUMINANCE_TEXTURE                           = GL_LUMINANCE,
        ALPHA_TEXTURE                               = GL_ALPHA,
        DEPTHSTENCIL_TEXTURE                        = GL_DEPTH_STENCIL,
    #if defined( GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG)
        COMPRESSED_RGB_PVRTC_2BPPV1_IMG_TEXTURE     = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,
        COMPRESSED_RGBA_PVRTC_2BPPV1_IMG_TEXTURE    = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,
        COMPRESSED_RGB_PVRTC_4BPPV1_IMG_TEXTURE     = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,
        COMPRESSED_RGBA_PVRTC_4BPPV1_IMG_TEXTURE    = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,
    #endif
    #ifdef GL_ETC1_RGB8_OES
        ETC1_RGB8_OES_TEXTURE                       = GL_ETC1_RGB8_OES,
    #endif
    #ifdef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
        COMPRESSED_RGBA_S3TC_DXT1_EXT_TEXTURE       = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
    #endif
    #ifdef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
        COMPRESSED_RGBA_S3TC_DXT3_EXT_TEXTURE       = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
    #endif
    #ifdef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
        COMPRESSED_RGBA_S3TC_DXT5_EXT_TEXTURE       = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
    #endif
    #ifdef GL_ATC_RGB_AMD
        ATC_RGB_AMD_TEXTURE                         = GL_ATC_RGB_AMD,
    #endif

    #ifdef GL_ATC_RGBA_EXPLICIT_ALPHA_AMD
        ATC_RGBA_EXPLICIT_ALPHA_AMD_TEXTURE         = GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,
    #endif

    #ifdef GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD
        ATC_INTERPOLATED_ALPHA_TEXTURE              = GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD,
    #endif
        NONE_TEXTURE                                = 0xFFFFFFFF,
    }TextureFormat;

    typedef TextureFormat InputFormat;

    //------------------------------------------------------------------------------------//
    enum PixelFormat : int32_t
    {
        AUTO = 0,
        BGRA8888,
        RGBA8888,
        RGB888,
        RGB565,
        A8,
        I8,
        AI88,
        RGBA4444,
        RGB5A1,
        PVRTC4,
        PVRTC4A,
        PVRTC2,
        PVRTC2A,
        ETC,
        S3TC_DXT1,
        S3TC_DXT3,
        S3TC_DXT5,
        ATC_RGB,
        ATC_EXPLICIT_ALPHA,
        ATC_INTERPOLATED_ALPHA,
        MTL_ABGR4,
        MTL_B5G6R5,
        MTL_BGR5A1,
        DEFAULT = AUTO,
        NONE = -1
    };

    //------------------------------------------------------------------------------------//
    enum DataType
    {
        UNSIGNED_BYTE_TYPE                  = GL_UNSIGNED_BYTE,
        BYTE_TYPE                           = GL_BYTE,
        UNSIGNED_SHORT_TYPE                 = GL_UNSIGNED_SHORT,
        SHORT_TYPE                          = GL_SHORT,
        UNSIGNED_INT_TYPE                   = GL_UNSIGNED_INT,
        INT_TYPE                            = GL_INT,
        HALF_FLOAT_TYPE                     = GL_HALF_FLOAT,
        FLOAT_TYPE                          = GL_FLOAT,
        UNSIGNED_SHORT_5_6_5_TYPE           = GL_UNSIGNED_SHORT_5_6_5,
        UNSIGNED_SHORT_4_4_4_4_TYPE         = GL_UNSIGNED_SHORT_4_4_4_4,
        UNSIGNED_SHORT_5_5_5_1_TYPE         = GL_UNSIGNED_SHORT_5_5_5_1,
        UNSIGNED_INT_2_10_10_10_REV_TYPE    = GL_UNSIGNED_INT_2_10_10_10_REV,
        UNSIGNED_INT_10F_11F_11F_REV_TYPE   = GL_UNSIGNED_INT_10F_11F_11F_REV,
        UNSIGNED_INT_5_9_9_9_REV_TYPE       = GL_UNSIGNED_INT_5_9_9_9_REV,
        UNSIGNED_INT_24_8_TYPE              = GL_UNSIGNED_INT_24_8,
        FLOAT_32_UNSIGNED_INT_24_8_REV_TYPE = GL_FLOAT_32_UNSIGNED_INT_24_8_REV,
    #if 0
        UNSIGNED_BYTE_3_3_2_TYPE            = GL_UNSIGNED_BYTE_3_3_2,
            UNSIGNED_BYTE_2_3_3_REV_TYPE        = GL_UNSIGNED_BYTE_2_3_3_REV,
            UNSIGNED_SHORT_5_6_5_REV_TYPE       = GL_UNSIGNED_SHORT_5_6_5_REV,
            UNSIGNED_SHORT_4_4_4_4_REV_TYPE     = GL_UNSIGNED_SHORT_4_4_4_4_REV,
            UNSIGNED_SHORT_1_5_5_5_REV_TYPE     = GL_UNSIGNED_SHORT_1_5_5_5_REV,
            UNSIGNED_INT_8_8_8_8_TYPE           = GL_UNSIGNED_INT_8_8_8_8,
            UNSIGNED_INT_8_8_8_8_REV_TYPE       = GL_UNSIGNED_INT_8_8_8_8_REV,
            UNSIGNED_INT_10_10_10_2_TYPE        = GL_UNSIGNED_INT_10_10_10_2,
    #endif
        NONE_TYPE                           = 0xFFFFFFFF,
    };

    //------------------------------------------------------------------------------------//
    struct ShaderTypeData
    {
        void*   val;
        GLenum  type;
        GLint   bytesize;
        GLint   n;
        GLuint  index;

        ShaderTypeData(void) { val = nullptr; bytesize = 0; n = 0; index = -1; }

        ShaderTypeData(const ShaderTypeData& data)
        {
            val = nullptr; type = data.type; bytesize = data.bytesize; n = data.n; index = data.index;
            if(bytesize > 0)
            {
                val = safeGlobalAlloc(bytesize);
                memcpy(val, data.val, bytesize);
            }
        }

        ShaderTypeData(ShaderTypeData&& data)
        {
            val = data.val; type = data.type; bytesize = data.bytesize; n = data.n; index = data.index;
            data.val = nullptr; data.bytesize = 0; data.n = 0; data.index = -1;
        }

        ~ShaderTypeData(void)
        {
            safeGlobalFree(val);
            bytesize = 0; n = 0; index = -1;
        }

        ShaderTypeData& operator=(const ShaderTypeData& data)
        {
            safeGlobalFree(val);
            type = data.type; bytesize = data.bytesize; n = data.n; index = data.index;
            if(data.val) {
                assert(bytesize > 0);
                val = safeGlobalAlloc(bytesize);
                memcpy(val, data.val, bytesize);
            }

            return *this;
        }

        ShaderTypeData& operator=(ShaderTypeData&& data)
        {
            safeGlobalFree(val);
            val = data.val; type = data.type; bytesize = data.bytesize; n = data.n; index = data.index;
            data.bytesize = 0; data.index = -1;

            return *this;
        }
    };

    //------------------------------------------------------------------------------------//
    extern const std::map<GLenum, GLenum> typeMapTable;

END

#endif //__GLDataTypedef_h__
