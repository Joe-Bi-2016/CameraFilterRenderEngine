//
// Created by Joe.Bi on 2024/11/23.
//

#ifndef __GLFrameBuffer_h__
#define __GLFrameBuffer_h__
#include "GLRender2Texture.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    typedef float vec4f[4];

    class GLFrameBuffer;
    typedef std::shared_ptr<GLFrameBuffer> sharedFrameBuffer;

    //------------------------------------------------------------------------------------//
    class API_EXPORTS GLFrameBuffer : private Uncopyable
    {
        friend void deleter<GLFrameBuffer>(GLFrameBuffer*);
    public:
        static sharedFrameBuffer createFbo(bool needDepth = false, bool needStencil = false);

        GLFrameBuffer(GLFrameBuffer&&);
        GLFrameBuffer& operator=(GLFrameBuffer&&);

        bool init(uint32_t width, uint32_t height);

        GLuint getFBO(void) const { return mFBO; }

        GLuint getOldFBO(void) const { return mOldFBO; }

        void clearFBO(void);

        void useFBO(void);

        void setClearColor(float r, float g, float b, float a);

        void setClearDepth(float depth) { mClearDepth = depth; mCreateDepthBuf = true; }

        void setClearStencil(GLint stencil) { mClearStencilVal = stencil; mCreateStencilBuf = true; }

        void attachRenderTarget(const sharedRenderTarget& renderTarget) { mRenderTarget = renderTarget; mInited = false; }

        const vec4f& getClearColor(void) const { return mClearColor; }

        float getClearDepth(void) const { return mClearDepth; }

        GLint getClearStencil(void) const { return mClearStencilVal; }

        const sharedRenderTarget& getRenderTarget(void) const { return mRenderTarget; }

        uint32_t getWidth(void) const { return mWidth; }

        uint32_t getHeight(void) const { return mHeight; }

        void getPixels(const int x, const int y, const int width, const int height, PixelFormat format, uint8_t* ret);

    private:
        GLFrameBuffer(bool needDepth = false, bool needStencil = false);
        ~GLFrameBuffer(void);
        void release(void);

        bool createDepthStencilBuf(uint32_t width, uint32_t height);

    protected:
        GLuint					mFBO;
        GLint					mOldFBO;
        vec4f			        mClearColor;
        float					mClearDepth;
        GLint					mClearStencilVal;
        sharedRenderTarget		mRenderTarget;
        GLuint                  mDepthBuffer;
        GLuint                  mStencilBuffer;
        uint32_t				mWidth;
        uint32_t				mHeight;
        bool                    mCreateDepthBuf;
        bool                    mCreateStencilBuf;
        bool                    mInited;
    };

END

#endif //__GLFrameBuffer_h__
