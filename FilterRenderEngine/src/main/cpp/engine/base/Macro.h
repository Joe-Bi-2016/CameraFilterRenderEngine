//
// Created by Joe.Bi on 2024/11/8.
//

#ifndef __Macro_h__
#define __Macro_h__

//---------------------------------------------------------------------------//
#if (defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__))
    #ifdef API_CONDUCT
        #ifdef __GNUC__
            #define API_EXPORTS __attribute__ ((dllexport))
        #else
            #define API_EXPORTS __declspec(dllexport)
        #endif
    #else
        #ifdef __GNUC__
            #define API_EXPORTS __attribute__ ((dllimport))
        #else
            #define API_EXPORTS __declspec(dllimport)
        #endif
    #endif
#else
    #if __GNUC__ >= 4
        #define API_EXPORTS __attribute__ ((visibility ("default")))
        #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
    #else
        #define API_EXPORTS
        #define DLL_LOCAL
    #endif
#endif

//---------------------------------------------------------------------------//
//define string
#define _STR(x)		#x
#define	STR(x)		_STR(x)

//---------------------------------------------------------------------------//
#define BEGIN   namespace filterRenderEngine{
#define END }

//---------------------------------------------------------------------------//
template<typename T>
void deleter(T* p)
{
    if (p) delete p;
}

#endif //__Macro_h__

