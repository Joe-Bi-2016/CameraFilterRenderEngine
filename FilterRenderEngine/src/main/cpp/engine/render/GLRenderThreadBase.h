//
// Created by Joe.Bi on 2024/11/23.
//

#ifndef __GLRenderThreadBase_h__
#define __GLRenderThreadBase_h__
#include "GLRenderPass.h"
#include "GLFrameBuffer.h"
#include "eglcore/EglCore.h"

using namespace std;

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//

    class API_EXPORTS GLRenderThreadBase : private Uncopyable
    {
    public:
        GLRenderThreadBase(void);
        virtual ~GLRenderThreadBase(void);

        EglCore* getEGLContext(void) { return mEglCore; }

        virtual void attacheRenderPass(const sharedRenderPass& renderpass) = 0;
        virtual void removeRenderPass(int index) = 0;
        virtual void removeRenderPass(sharedRenderPass renderpass) = 0;

        virtual sharedRenderPass getRenderPass(int index) const = 0;

        void setBackgroundColor(float r, float g, float b, float a)
        { mClearColor[0] = r; mClearColor[1] = g; mClearColor[2] = b; mClearColor[3] = a; }

        virtual void setDraw2Fbo(int width, int height) = 0;
        virtual void setDraw2Fbo(const sharedFrameBuffer& fbo) = 0;
        virtual void enableDraw2Fbo(void) = 0;
        virtual void disableDraw2Fbo(void) = 0;
        virtual sharedTexture getFboColorTexture(void) = 0;

        virtual void setMessage(void* msg) = 0;

        virtual void initWithEglCore(const EglCore* eglCore);
        virtual void initWithWindow(ANativeWindow *nativeWindow);

        void destroyEGLSurface(void);

        bool isShouldExit(void) const { return mExit; }

        virtual void onDo(int renderPassIndex = -1) = 0;

    protected:
        EglCore*    mEglCore;
        vec4f		mClearColor;
        bool        mExit;
    };

END

#endif //__GLRenderThreadBase_h__

