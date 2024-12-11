//
// Created by Joe.Bi on 2024/11/22.
//

#ifndef __GLRenderThreadMain_h__
#define __GLRenderThreadMain_h__
#include "GLRenderThreadBase.h"
#include <deque>

using namespace std;

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    class API_EXPORTS GLRenderThreadMain : public GLRenderThreadBase
    {
        friend class GLRenderRoot;
    public:
        ~GLRenderThreadMain(void);

        void attacheRenderPass(const sharedRenderPass& renderPass);
        void removeRenderPass(int index);
        void removeRenderPass(sharedRenderPass renderPass);

        sharedRenderPass getRenderPass(int index) const;

        void setDraw2Fbo(int width, int height);
        void setDraw2Fbo(const sharedFrameBuffer& fbo) { mFbo = fbo; }
        void enableDraw2Fbo(void) { mDraw2Fbo = true; }
        void disableDraw2Fbo(void) { mDraw2Fbo = false; }
        sharedTexture getFboColorTexture(void);

        int getViewWidth(void) const { return mViewWidth; }
        int getViewHeight(void) const { return mViewHeight; }

        void resizeView(int width, int height) { mViewWidth = width; mViewHeight = height; }

        void setMessage(void* msg) { }

        void initWithEglCore(const EglCore* eglCore);
        void initWithWindow(ANativeWindow *nativeWindow);

        void onDo(int renderPassIndex = -1);

    private:
        GLRenderThreadMain(void);

    private:
        int                     mViewWidth;
        int                     mViewHeight;
        sharedFrameBuffer       mFbo;
        bool                    mDraw2Fbo;
        deque<sharedRenderPass> mRenderPassList;
    };

END

#endif //__GLRenderThreadMain_h__

