//
// Created by Joe.Bi on 2024/11/25.
//

#include <stdint.h>
#ifndef __GLRenderRoot_h__
#define __GLRenderRoot_h__
#include "GLRenderThreadMain.h"
#include <pthread.h>
#include <map>
#include <mutex>

using namespace std;

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    typedef GLRenderThreadBase* (*createRenderThreadObject)(void*);

    typedef struct
    {
        EglCore*                    eglcore;
        void*                       window;
        createRenderThreadObject    objcreator;
        GLRenderThreadBase*         renderbaseptr;
    }threadParam;

    //------------------------------------------------------------------------------------//
    class API_EXPORTS GLRenderRoot : private Uncopyable
    {
    public:
        static GLRenderRoot* getInstance(void);
        static void release(void);

        void initWithEglCore(void);
        void initWithWindow(ANativeWindow *nativeWindow);

        GLRenderThreadBase* getMainThreadRoot(void) const { return mMainThreadRoot; }

        GLRenderThreadBase* forkThread(int width, int height, const createRenderThreadObject& objcreator);

        bool releaseThread(GLRenderThreadBase* thread);

        void setbackgroundColor(const float r, float g, float b, float a);

        void onSurfaceChanged(int width, int height);

        sharedFrameBuffer getDefaultFbo(void) const { return mDefaultFbo; }

        void drawOneFrame(int renderPassIndex = -1);

    private:
        GLRenderRoot(void);
        ~GLRenderRoot(void);

        static void* threadLoop(void* pParam);

    private:
        GLRenderThreadMain*                         mMainThreadRoot;
        sharedFrameBuffer                           mDefaultFbo;
        pthread_t                                   mMainThreadId;
        static map<pthread_t, GLRenderThreadBase*>  mBackThread;

    };

    typedef map<pthread_t, GLRenderThreadBase*>::iterator mapIt;
    typedef map<pthread_t, GLRenderThreadBase*>::value_type mapVal;

END


#endif //__GLRenderRoot_h__
