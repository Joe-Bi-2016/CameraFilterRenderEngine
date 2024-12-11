//
// Created by Joe.Bi on 2024/11/10.
//

#ifndef __GLDeviceInfo_h__
#define __GLDeviceInfo_h__
#include "base/Uncopyable.hpp"
#include <GLES3/gl3.h>
#include <GLES2/gl2.h>
#include <string>

//----------------------------------------------------------------------------------------//
BEGIN

    enum class FeatureType : uint32_t
    {
        ETC1,
        S3TC,
        AMD_COMPRESSED_ATC,
        PVRTC,
        IMG_FORMAT_BGRA8888,
        DISCARD_FRAMEBUFFER,
        PACKED_DEPTH_STENCIL,
        VAO,
        MAPBUFFER,
        DEPTH24,
        ASTC
    };

    //------------------------------------------------------------------------------------//
    class GLDeviceInfo : private Uncopyable
    {
    public:
        GLDeviceInfo(void) = default;
        ~GLDeviceInfo(void) = default;

        static const char* getVendor();

        static const char* getRenderer();

        static const char* getVersion();

        static const char* getExtension();

        static bool checkForFeatureSupported(FeatureType feature);

        static int getMaxTextureSize() { return maxTextureSize; }

        static int getMaxAttributes() { return maxAttributes; }

        static int getMaxTextureUnits() { return maxTextureUnits; }

        static int getMaxSamplesAllowed() { return maxSamplesAllowed; }

    private:
        static bool init(void);

        static bool checkForGLExtension(const std::string &searchName);

    protected:
        static int maxAttributes;
        static int maxTextureSize;
        static int maxTextureUnits;
        static int maxSamplesAllowed;
        static std::string mGLExtensions;
    };

END

#endif //__GLDeviceInfo_h__
