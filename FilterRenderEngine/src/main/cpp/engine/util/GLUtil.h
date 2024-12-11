//
// Created by Joe.Bi on 2024/11/10.
//

#ifndef __GLUtil_h__
#define __GLUtil_h__
#include "base/Macro.h"
#include "LogUtil.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    #ifndef MIN
    #define MIN(x,y) (((x) > (y)) ? (y) : (x))
    #endif

    #ifndef MAX
    #define MAX(x,y) (((x) < (y)) ? (y) : (x))
    #endif

    //------------------------------------------------------------------------------------//
    #define CHECK_GL_ERROR() \
    do                         \
    { \
        GLenum __error = glGetError(); \
        if(__error)                \
            LOGERROR("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
    } while (false)

    //------------------------------------------------------------------------------------//
    //macro - memory alloc definiation
    #ifndef safeGlobalAlloc
        #define safeGlobalAlloc(bytesize)	(void*)malloc(bytesize)
    #endif

    //------------------------------------------------------------------------------------//
    #ifndef safeGlobalFree
        #define safeGlobalFree(ptr)	\
		    {\
		    	if(ptr)	\
		    	{\
		    		free((void*)ptr);\
		    		ptr = NULL;\
		    	}\
		    }\

    #endif

    //------------------------------------------------------------------------------------//
    #ifndef safeLocalAlloc
        #if !defined(_WIN32)
            #include <alloca.h>
        #endif
        #define safeLocalAlloc(bytesize) (void*)alloca(bytesize)
    #endif

    //------------------------------------------------------------------------------------//
    #ifndef safeNewArray
        #define safeNewArray(T, n) ((T*)(new T[n]))
    #endif

    //------------------------------------------------------------------------------------//
    #ifndef safeDeleteArray
        #define safeDeleteArray(ptr)	\
		{\
			if(ptr)\
			{\
				delete [] ptr; ptr = NULL;\
			}\
		}\

    #endif

    //------------------------------------------------------------------------------------//
    #ifndef safeNewObject
        #define safeNewObject(T)	(new T())
    #endif

    //------------------------------------------------------------------------------------//
    #ifndef safeDeleteObject
        #define safeDeleteObject(ptr)	\
		{\
			if(ptr)\
			{\
				delete ptr; ptr = NULL;\
			}\
		}\

    #endif

    //------------------------------------------------------------------------------------//
    int ccNextPOT(int x);

    //------------------------------------------------------------------------------------//
    void convertI8ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertAI88ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertI8ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertAI88ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertI8ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertAI88ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertI8ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertAI88ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertI8ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertAI88ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertI8ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertAI88ToA8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertAI88ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertRGB888ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertRGBA8888ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertRGB888ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertRGBA8888ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertRGB888ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertRGBA8888ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertRGBA8888ToA8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertRGB888ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertRGBA8888ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertRGB888ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertRGBA8888ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertRGB888ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

    //------------------------------------------------------------------------------------//
    void convertRGBA8888ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

END

#endif //__GLUtil_h__

