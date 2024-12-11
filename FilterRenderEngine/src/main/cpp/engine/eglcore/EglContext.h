//
// Created by Joe.Bi on 2024/11/7.
//

#ifndef __EglContext_h__
#define __EglContext_h__
#include "EglSurface.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    class EglContext;
    typedef std::shared_ptr<EglContext> sharedPtrContext;

    //------------------------------------------------------------------------------------//
    class API_EXPORTS EglContext : private Uncopyable
    {
        friend void deleter<EglContext>(EglContext*);
    public:
        static sharedPtrContext createContext(sharedConfig config, const EglContext* context);

        EGLContext getEGLContext(void) const { return mEGLContext; }

        sharedConfig getConfig(void) const { return mConfig; }

        void release(void);

    private:
        EglContext(sharedConfig config, const EglContext* context);
        ~EglContext(void);

    private:
        sharedConfig    mConfig;
        EGLContext      mEGLContext;

    };

END

#endif //__EglContext_h__
