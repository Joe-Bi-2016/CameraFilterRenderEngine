//
// Created by Joe.Bi on 2024/11/23.
//

#include "GLFrameBuffer.h"
#include "LogUtil.h"
#include <GLES2/gl2ext.h>
#include <GLES3/gl3ext.h>

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    GLFrameBuffer::GLFrameBuffer(bool needDepth/* = false */, bool needStencil/* = false */)
    : mFBO(0)
    , mOldFBO(0)
    , mWidth(0)
    , mHeight(0)
    , mClearDepth(0.0f)
    , mClearStencilVal(0)
    , mRenderTarget(NULL)
    , mDepthBuffer(GL_NONE)
    , mStencilBuffer(GL_NONE)
    , mCreateDepthBuf(needDepth)
    , mCreateStencilBuf(needStencil)
    , mInited(false)
    {
        mClearColor[0] = mClearColor[1] = mClearColor[2] = mClearColor[3] = 1.0f;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&mOldFBO);
    }

    //------------------------------------------------------------------------------------//
    GLFrameBuffer::GLFrameBuffer(GLFrameBuffer&& buffer)
    : mFBO(buffer.mFBO)
    , mOldFBO(buffer.mOldFBO)
    , mWidth(buffer.mWidth)
    , mHeight(buffer.mHeight)
    , mClearDepth(buffer.mClearDepth)
    , mClearStencilVal(buffer.mClearStencilVal)
    , mRenderTarget(std::move(buffer.mRenderTarget))
    , mDepthBuffer(buffer.mDepthBuffer)
    , mStencilBuffer(buffer.mStencilBuffer)
    , mCreateDepthBuf(buffer.mCreateDepthBuf)
    , mCreateStencilBuf(buffer.mCreateStencilBuf)
    , mInited(buffer.mInited)
    {
        memcpy(mClearColor, buffer.mClearColor, sizeof(vec4f));

        buffer.mFBO = 0;
        buffer.mOldFBO = 0;
        buffer.mWidth = 0;
        buffer.mHeight = 0;
        buffer.mClearDepth = 0.0f;
        buffer.mClearStencilVal = 0;
        buffer.mDepthBuffer = GL_NONE;
        buffer.mStencilBuffer = GL_NONE;
        buffer.mCreateDepthBuf = false;
        buffer.mCreateStencilBuf = false;
        buffer.mInited = false;
        memset(buffer.mClearColor, 1.0f, sizeof(vec4f));
    }

    //------------------------------------------------------------------------------------//
    GLFrameBuffer::~GLFrameBuffer(void)
    {
        release();

        mWidth = 0;
        mHeight = 0;
        mClearDepth = 0.0f;
        mClearStencilVal = 0;
        mCreateDepthBuf = false;
        mCreateStencilBuf = false;
    }

    //------------------------------------------------------------------------------------//
    void GLFrameBuffer::release(void)
    {
        mOldFBO = 0;

        if(glIsBuffer(mDepthBuffer))
        {
            glDeleteBuffers(1, &mDepthBuffer);
            mDepthBuffer = GL_NONE;
        }

        if(glIsBuffer(mStencilBuffer))
        {
            glDeleteBuffers(1, &mStencilBuffer);
            mStencilBuffer = GL_NONE;
        }

        if (glIsFramebuffer(mFBO) && mFBO != 0)
        {
            glDeleteFramebuffers(1, &mFBO);
            mFBO = GL_NONE;
        }

        mRenderTarget.reset();

        mInited = false;
    }

    //------------------------------------------------------------------------------------//
    GLFrameBuffer& GLFrameBuffer::operator=(GLFrameBuffer&& buffer)
    {
        mFBO = buffer.mFBO;
        mOldFBO = buffer.mOldFBO;
        mWidth = buffer.mWidth;
        mHeight = buffer.mHeight;
        mClearDepth = buffer.mClearDepth;
        mClearStencilVal = buffer.mClearStencilVal;
        mRenderTarget = std::move(buffer.mRenderTarget);
        mDepthBuffer = buffer.mDepthBuffer;
        mStencilBuffer = buffer.mStencilBuffer;
        mCreateDepthBuf = buffer.mCreateDepthBuf;
        mCreateStencilBuf = buffer.mCreateStencilBuf;
        mInited = buffer.mInited;
        memcpy(mClearColor, buffer.mClearColor, sizeof(vec4f));

        buffer.mFBO = 0;
        buffer.mOldFBO = 0;
        buffer.mWidth = 0;
        buffer.mHeight = 0;
        buffer.mClearDepth = 0.0f;
        buffer.mClearStencilVal = 0;
        buffer.mDepthBuffer = GL_NONE;
        buffer.mStencilBuffer = GL_NONE;
        buffer.mCreateDepthBuf = false;
        buffer.mCreateStencilBuf = false;
        buffer.mInited = false;
        memset(buffer.mClearColor, 1.0f, sizeof(vec4f));

        return *this;
    }

    //------------------------------------------------------------------------------------//
    bool GLFrameBuffer::createDepthStencilBuf(uint32_t width, uint32_t height)
    {
        if(!mCreateDepthBuf && !mCreateStencilBuf)
            return false;

        GLint oldRenderBuffer = 0;
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRenderBuffer);

#if (defined(__APPLE__) || defined(ANDROID) || defined(__ANDROID__))
        if(mDepthBuffer == GL_NONE)
        {
            glGenRenderbuffers(1, &mDepthBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL_OES, width, height);
            mStencilBuffer = mDepthBuffer;
        }
#else
        if (mCreateDepthBuf && mDepthBuffer == GL_NONE)
        {
            glGenRenderbuffers(1, &mDepthBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        }

        if (mCreateStencilBuf && mStencilBuffer == GL_NONE)
        {
            glGenRenderbuffers(1, &mStencilBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, mStencilBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);
        }
#endif

        glBindRenderbuffer(GL_RENDERBUFFER, oldRenderBuffer);

        return true;
    }

    //------------------------------------------------------------------------------------//
    bool GLFrameBuffer::init(uint32_t width, uint32_t height)
    {
        if(mWidth != width || mHeight != height)
        {
            release();

            glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&mOldFBO);

            mWidth = width;
            mHeight = height;

            glGenFramebuffers(1, &mFBO);
        }

        return true;
    }

    //------------------------------------------------------------------------------------//
    void GLFrameBuffer::clearFBO(void)
    {
        useFBO();
        glClearColor(mClearColor[0], mClearColor[1], mClearColor[2], mClearColor[3]);

        if(mClearDepth != 0.0f)
            glClearDepthf(mClearDepth);

        if(mClearStencilVal != 0)
            glClearStencil(mClearStencilVal);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, mOldFBO);
    }

    //------------------------------------------------------------------------------------//
    void GLFrameBuffer::useFBO(void)
    {
        if (mFBO == 0)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, mOldFBO);
            return;
        }

        if(mInited)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
            return;
        }
		
		if(mWidth == 0 || mHeight == 0)
        {
            LOGERROR("width or height is not right, one is 0");
            return;
        }

        if(mRenderTarget.get() == nullptr)
        {
            mRenderTarget = GLRender2Texture::generate2DRenderTarget();
            mRenderTarget->init(mWidth, mHeight);
        }

        createDepthStencilBuf(mWidth, mHeight);

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mOldFBO);

        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

        assert(mRenderTarget->getRenderType() == Render2Texture);

        GLRender2Texture* renderPtr = (GLRender2Texture*)(mRenderTarget.get());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderPtr->getTexture()->getTextureId(), 0);

        if(mCreateDepthBuf && mDepthBuffer != GL_NONE)
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);

        if(mCreateStencilBuf && mStencilBuffer != GL_NONE)
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mStencilBuffer);

        if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
        {
            char str[128] = { 0 };
#if (__STDC_WANT_LIB_EXT1__ == 1)
            sprintf_s(str, "%s%d\n", "Error: FrameBuffer status error", (int32_t)glCheckFramebufferStatus(GL_FRAMEBUFFER));
#else
            snprintf(str, 128, "%s%d\n", "Error: FrameBuffer status error", (int32_t)glCheckFramebufferStatus(GL_FRAMEBUFFER));
#endif
            LOGERROR("%s", str);
        }

        mInited = true;
    }

    //------------------------------------------------------------------------------------//
    void GLFrameBuffer::setClearColor(float r, float g, float b, float a)
    {
        mClearColor[0] = r;
        mClearColor[1] = g;
        mClearColor[2] = b;
        mClearColor[3] = a;
    }

    //------------------------------------------------------------------------------------//
    void GLFrameBuffer::getPixels(const int x, const int y, const int width, const int height, PixelFormat format, uint8_t* ret)
    {
        const PixelFormatInfo& info = GLTexture2D::mPixelFormatInfoTables.at(format);
        if (mRenderTarget)
        {
            GLRender2Texture* renderPtr = (GLRender2Texture*)(mRenderTarget.get());
            uint8_t* ptr = renderPtr->getTexture()->lockTexture();
            if (ptr)
            {
                int stride = renderPtr->getTexture()->getTextureStride();
                unsigned int bytesPixel = renderPtr->getTexture()->getBytesPixel();
                uint32_t srcStrideBytes = stride * bytesPixel;
                uint8_t* src = ptr + (y * stride + x) * bytesPixel;
                int dstRowBytes = width * (info.bpp / 8);
                for (int r = 0; r < height; ++r)
                {
                    memcpy(ret + r * dstRowBytes, src + r * srcStrideBytes, dstRowBytes);
                }
                renderPtr->getTexture()->unlockTexture();
            }
            else
                glReadPixels(x, y, width, height, info.format, GL_UNSIGNED_BYTE, ret);
        }
    }

    //------------------------------------------------------------------------------------//
    sharedFrameBuffer GLFrameBuffer::createFbo(bool needDepth/* = false */, bool needStencil/* = false */)
    {
        return sharedFrameBuffer(new GLFrameBuffer(needDepth, needStencil), deleter<GLFrameBuffer>);
    }


END
