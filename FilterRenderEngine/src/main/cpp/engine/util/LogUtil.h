//
// Created by Joe.Bi on 2024/11/7.
//

#ifndef __LogUtil_h__
#define __LogUtil_h__
#include<android/log.h>
#include <sys/time.h>

__inline static long long GetSysCurrentTime()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    long long curTime = ((long long)(time.tv_sec))*1000+time.tv_usec/1000;
    return curTime;
}

#define  LOG_TAG "Filter-GLESRenderEngine"

#define  LOGERROR(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGWARNING(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGBVERBORSE(...)  __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG,__VA_ARGS__)
#define  LOGDEBUG(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGINFO(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#define FUN_BEGIN_TIME(FUN) {\
    LOGINFO("%s:%s func start", __FILE__, FUN); \
    long long t0 = GetSysCurrentTime();

#define FUN_END_TIME(FUN) \
    long long t1 = GetSysCurrentTime(); \
    LOGINFO("%s:%s func cost time %ldms", __FILE__, FUN, (long)(t1-t0));}

#define BEGIN_TIME(FUN) {\
    LOGINFO("%s func start", FUN); \
    long long t0 = GetSysCurrentTime();

#define END_TIME(FUN) \
    long long t1 = GetSysCurrentTime(); \
    LOGINFO("%s func cost time %ldms", FUN, (long)(t1-t0));}

#define DEBUG_LOGCATE(...) LOGDEBUG("DEBUG %s line = %d",  __FUNCTION__, __LINE__)



#endif //__LogUtil_h__
