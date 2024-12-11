//
// Created by Joe.Bi on 2024/11/10.
//

#include "GLDeviceInfo.h"
#include "util/LogUtil.h"
#include <assert.h>

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    int GLDeviceInfo::maxAttributes = 0;
    int GLDeviceInfo::maxTextureSize = 0;
    int GLDeviceInfo::maxTextureUnits = 0;
    int GLDeviceInfo::maxSamplesAllowed = 0;
    std::string GLDeviceInfo::mGLExtensions = std::string("");

    //------------------------------------------------------------------------------------//
    bool GLDeviceInfo::init(void)
    {
        if(maxAttributes == 0)
        {
            glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributes);
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
            glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
            mGLExtensions = (const char *) glGetString(GL_EXTENSIONS);
        }
        return true;
    }

    //------------------------------------------------------------------------------------//
    const char* GLDeviceInfo::getVendor()
    {
        return (const char*)glGetString(GL_VENDOR);
    }

    //------------------------------------------------------------------------------------//
    const char* GLDeviceInfo::getRenderer()
    {
        return (const char*)glGetString(GL_RENDERER);
    }

    //------------------------------------------------------------------------------------//
    const char* GLDeviceInfo::getVersion()
    {
        return (const char*)glGetString(GL_VERSION);
    }

    //------------------------------------------------------------------------------------//
    const char* GLDeviceInfo::getExtension()
    {
        init();
        return mGLExtensions.c_str();
    }

    //------------------------------------------------------------------------------------//
    bool GLDeviceInfo::checkForFeatureSupported(FeatureType feature)
    {
        bool featureSupported = false;
        switch (feature)
        {
            case FeatureType::ETC1:
    #ifdef GL_ETC1_RGB8_OES //GL_ETC1_RGB8_OES is not defined in old opengl version
                featureSupported = checkForGLExtension("GL_OES_compressed_ETC1_RGB8_texture");
    #endif
                break;
            case FeatureType::S3TC:
    #ifdef GL_EXT_texture_compression_s3tc
                featureSupported = checkForGLExtension("GL_EXT_texture_compression_s3tc");
    #endif
                break;
            case FeatureType::AMD_COMPRESSED_ATC:
                featureSupported = checkForGLExtension("GL_AMD_compressed_ATC_texture");
                break;
            case FeatureType::PVRTC:
                featureSupported = checkForGLExtension("GL_IMG_texture_compression_pvrtc");
                break;
            case FeatureType::IMG_FORMAT_BGRA8888:
                featureSupported = checkForGLExtension("GL_IMG_texture_format_BGRA8888");
                break;
            case FeatureType::DISCARD_FRAMEBUFFER:
                featureSupported = checkForGLExtension("GL_EXT_discard_framebuffer");
                break;
            case FeatureType::PACKED_DEPTH_STENCIL:
                featureSupported = checkForGLExtension("GL_OES_packed_depth_stencil");
                break;
            case FeatureType::VAO:
    #ifdef CC_PLATFORM_PC
                featureSupported = checkForGLExtension("vertex_array_object");
    #else
                featureSupported = checkForGLExtension("GL_OES_vertex_array_object");
    #endif
                break;
            case FeatureType::MAPBUFFER:
                featureSupported = checkForGLExtension("GL_OES_mapbuffer");
                break;
            case FeatureType::DEPTH24:
                featureSupported = checkForGLExtension("GL_OES_depth24");
                break;
            default:
                break;
        }
        return featureSupported;
    }

    bool GLDeviceInfo::checkForGLExtension(const std::string &searchName)
    {
        init();
        return mGLExtensions.find(searchName) != std::string::npos;
    }

END
