//
// Created by Joe.Bi on 2024/11/10.
//

#include "GLUtil.h"
#include "render/GLDataTypeDef.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    const std::map<GLenum, GLenum> typeMapTable =
    {
        {GL_FLOAT,              GL_FLOAT},
        {GL_FLOAT_VEC2,         GL_FLOAT},
        {GL_FLOAT_VEC3,         GL_FLOAT},
        {GL_FLOAT_VEC4,         GL_FLOAT},
        {GL_FLOAT_MAT2,         GL_FLOAT},
        {GL_FLOAT_MAT3,         GL_FLOAT},
        {GL_FLOAT_MAT4,         GL_FLOAT},
        {GL_INT,                GL_INT},
        {GL_INT_VEC2,           GL_INT},
        {GL_INT_VEC3,           GL_INT},
        {GL_INT_VEC4,           GL_INT},
        {GL_UNSIGNED_INT,       GL_UNSIGNED_INT},
        {GL_UNSIGNED_INT_VEC2,  GL_UNSIGNED_INT},
        {GL_UNSIGNED_INT_VEC3,  GL_UNSIGNED_INT},
        {GL_UNSIGNED_INT_VEC4,  GL_UNSIGNED_INT}
    };

    //------------------------------------------------------------------------------------//
    int ccNextPOT(int x)
    {
        x = x - 1;
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >>16);
        return x + 1;
    }

    //------------------------------------------------------------------------------------//
    void convertI8ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        for (ssize_t i=0; i < dataLen; ++i)
        {
            *outData++ = data[i];     //R
            *outData++ = data[i];     //G
            *outData++ = data[i];     //B
        }
    }

    //------------------------------------------------------------------------------------//
    // IIIIIIIIAAAAAAAA -> RRRRRRRRGGGGGGGGBBBBBBBB
    void convertAI88ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
        {
            *outData++ = data[i];     //R
            *outData++ = data[i];     //G
            *outData++ = data[i];     //B
        }
    }

    //------------------------------------------------------------------------------------//
    // IIIIIIII -> RRRRRRRRGGGGGGGGGBBBBBBBBAAAAAAAA
    void convertI8ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0; i < dataLen; ++i)
        {
            *outData++ = data[i];     //R
            *outData++ = data[i];     //G
            *outData++ = data[i];     //B
            *outData++ = 0xFF;        //A
        }
    }

    //------------------------------------------------------------------------------------//
    // IIIIIIIIAAAAAAAA -> RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA
    void convertAI88ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
        {
            *outData++ = data[i];     //R
            *outData++ = data[i];     //G
            *outData++ = data[i];     //B
            *outData++ = data[i + 1]; //A
        }
    }

    //------------------------------------------------------------------------------------//
    // IIIIIIII -> RRRRRGGGGGGBBBBB
    void convertI8ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (int i = 0; i < dataLen; ++i)
        {
            *out16++ = (data[i] & 0x00F8) << 8    //R
                       | (data[i] & 0x00FC) << 3         //G
                       | (data[i] & 0x00F8) >> 3;        //B
        }
    }

    //------------------------------------------------------------------------------------//
    // IIIIIIIIAAAAAAAA -> RRRRRGGGGGGBBBBB
    void convertAI88ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
        {
            *out16++ = (data[i] & 0x00F8) << 8    //R
                       | (data[i] & 0x00FC) << 3         //G
                       | (data[i] & 0x00F8) >> 3;        //B
        }
    }

    //------------------------------------------------------------------------------------//
    // IIIIIIII -> RRRRGGGGBBBBAAAA
    void convertI8ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0; i < dataLen; ++i)
        {
            *out16++ = (data[i] & 0x00F0) << 8    //R
                       | (data[i] & 0x00F0) << 4             //G
                       | (data[i] & 0x00F0)                  //B
                       | 0x000F;                             //A
        }
    }

    //------------------------------------------------------------------------------------//
    // IIIIIIIIAAAAAAAA -> RRRRGGGGBBBBAAAA
    void convertAI88ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
        {
            *out16++ = (data[i] & 0x00F0) << 8    //R
                       | (data[i] & 0x00F0) << 4             //G
                       | (data[i] & 0x00F0)                  //B
                       | (data[i+1] & 0x00F0) >> 4;          //A
        }
    }

    //------------------------------------------------------------------------------------//
    // IIIIIIII -> RRRRRGGGGGBBBBBA
    void convertI8ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (int i = 0; i < dataLen; ++i)
        {
            *out16++ = (data[i] & 0x00F8) << 8    //R
                       | (data[i] & 0x00F8) << 3         //G
                       | (data[i] & 0x00F8) >> 2         //B
                       | 0x0001;                         //A
        }
    }

    //------------------------------------------------------------------------------------//
    // IIIIIIIIAAAAAAAA -> RRRRRGGGGGBBBBBA
    void convertAI88ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
        {
            *out16++ = (data[i] & 0x00F8) << 8    //R
                       | (data[i] & 0x00F8) << 3         //G
                       | (data[i] & 0x00F8) >> 2         //B
                       | (data[i + 1] & 0x0080) >> 7;    //A
        }
    }

    //------------------------------------------------------------------------------------//
    // IIIIIIII -> IIIIIIIIAAAAAAAA
    void convertI8ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0; i < dataLen; ++i)
        {
            *out16++ = 0xFF00     //A
                       | data[i];            //I
        }
    }

    //------------------------------------------------------------------------------------//
    // IIIIIIIIAAAAAAAA -> AAAAAAAA
    void convertAI88ToA8(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 1; i < dataLen; i += 2)
        {
            *outData++ = data[i]; //A
        }
    }

    //------------------------------------------------------------------------------------//
    // IIIIIIIIAAAAAAAA -> IIIIIIII
    void convertAI88ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
        {
            *outData++ = data[i]; //R
        }
    }

    //------------------------------------------------------------------------------------//
    // RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA
    void convertRGB888ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
        {
            *outData++ = data[i];         //R
            *outData++ = data[i + 1];     //G
            *outData++ = data[i + 2];     //B
            *outData++ = 0xFF;            //A
        }
    }

    //------------------------------------------------------------------------------------//
    // RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> RRRRRRRRGGGGGGGGBBBBBBBB
    void convertRGBA8888ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
        {
            *outData++ = data[i];         //R
            *outData++ = data[i + 1];     //G
            *outData++ = data[i + 2];     //B
        }
    }

    //------------------------------------------------------------------------------------//
    // RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRRGGGGGGBBBBB
    void convertRGB888ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
        {
            *out16++ = (data[i] & 0x00F8) << 8    //R
                       | (data[i + 1] & 0x00FC) << 3     //G
                       | (data[i + 2] & 0x00F8) >> 3;    //B
        }
    }

    //------------------------------------------------------------------------------------//
    // RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> RRRRRGGGGGGBBBBB
    void convertRGBA8888ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
        {
            *out16++ = (data[i] & 0x00F8) << 8    //R
                       | (data[i + 1] & 0x00FC) << 3     //G
                       | (data[i + 2] & 0x00F8) >> 3;    //B
        }
    }

    //------------------------------------------------------------------------------------//
    // RRRRRRRRGGGGGGGGBBBBBBBB -> IIIIIIII
    void convertRGB888ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
        {
            *outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //I =  (R*299 + G*587 + B*114 + 500) / 1000
        }
    }

    //------------------------------------------------------------------------------------//
    // RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> IIIIIIII
    void convertRGBA8888ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
        {
            *outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //I =  (R*299 + G*587 + B*114 + 500) / 1000
        }
    }

    //------------------------------------------------------------------------------------//
    // RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> AAAAAAAA
    void convertRGBA8888ToA8(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen -3; i < l; i += 4)
        {
            *outData++ = data[i + 3]; //A
        }
    }

    //------------------------------------------------------------------------------------//
    // RRRRRRRRGGGGGGGGBBBBBBBB -> IIIIIIIIAAAAAAAA
    void convertRGB888ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
        {
            *outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //I =  (R*299 + G*587 + B*114 + 500) / 1000
            *outData++ = 0xFF;
        }
    }

    //------------------------------------------------------------------------------------//
    // RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> IIIIIIIIAAAAAAAA
    void convertRGBA8888ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
        {
            *outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //I =  (R*299 + G*587 + B*114 + 500) / 1000
            *outData++ = data[i + 3];
        }
    }

    //------------------------------------------------------------------------------------//
    // RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRGGGGBBBBAAAA
    void convertRGB888ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
        {
            *out16++ = ((data[i] & 0x00F0) << 8           //R
                        | (data[i + 1] & 0x00F0) << 4     //G
                        | (data[i + 2] & 0xF0)            //B
                        |  0x0F);                         //A
        }
    }

    //------------------------------------------------------------------------------------//
    // RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> RRRRGGGGBBBBAAAA
    void convertRGBA8888ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
        {
            *out16++ = (data[i] & 0x00F0) << 8    //R
                       | (data[i + 1] & 0x00F0) << 4         //G
                       | (data[i + 2] & 0xF0)                //B
                       |  (data[i + 3] & 0xF0) >> 4;         //A
        }
    }

    //------------------------------------------------------------------------------------//
    // RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRRGGGGGBBBBBA
    void convertRGB888ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
        {
            *out16++ = (data[i] & 0x00F8) << 8    //R
                       | (data[i + 1] & 0x00F8) << 3     //G
                       | (data[i + 2] & 0x00F8) >> 2     //B
                       |  0x01;                          //A
        }
    }

    //------------------------------------------------------------------------------------//
    // RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRRGGGGGBBBBBA
    void convertRGBA8888ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 2; i < l; i += 4)
        {
            *out16++ = (data[i] & 0x00F8) << 8    //R
                       | (data[i + 1] & 0x00F8) << 3     //G
                       | (data[i + 2] & 0x00F8) >> 2     //B
                       |  (data[i + 3] & 0x0080) >> 7;   //A
        }
    }

END

