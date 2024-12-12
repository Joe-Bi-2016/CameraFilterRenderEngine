//
// Created by Joe.Bi on 2024/11/13.
//

#ifndef __GLRenderBase_h__
#define __GLRenderBase_h__
#include "GLEGLImageTexture.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    typedef enum
    {
        Render2Buffer		= 0,
        Render2Texture		= 1,
        RenderNone          = 2,
    }RenderType;

    typedef enum
    {
        ColorBuffer			= 0,
        DepthBuffer			= 1,
        StencilBuffer		= 2,
        DepthStencilBuffer	= 3,
        BufferTypeNULL		= -1,
    }BufferType;

    //------------------------------------------------------------------------------------//
    class API_EXPORTS GLRenderBase
    {
    public:
        GLRenderBase(RenderType type)
        { mRenderType = type; mBufferType = BufferTypeNULL; mWidth = mHeight = 0; }

        virtual ~GLRenderBase(void) { mWidth = mHeight = 0; mRenderType = RenderNone; mBufferType = BufferTypeNULL; }

        GLRenderBase(const GLRenderBase& base)
        {
            mRenderType = base.mRenderType;
            mBufferType = base.mBufferType;
            mWidth = base.mWidth;
            mHeight = base.mHeight;
        }

        GLRenderBase(GLRenderBase&& base)
        {
            mRenderType = base.mRenderType;
            mBufferType = base.mBufferType;
            mWidth = base.mWidth;
            mHeight = base.mHeight;

            base.mRenderType = RenderNone;
            base.mBufferType = BufferTypeNULL;
            base.mWidth = base.mHeight = 0;
        }

        GLRenderBase& operator=(const GLRenderBase& base)
        {
            mRenderType = base.mRenderType;
            mBufferType = base.mBufferType;
            mWidth = base.mWidth;
            mHeight = base.mHeight;

            return *this;
        }

        GLRenderBase& operator=(GLRenderBase&& base)
        {
            mRenderType = base.mRenderType;
            mBufferType = base.mBufferType;
            mWidth = base.mWidth;
            mHeight = base.mHeight;

            base.mRenderType = RenderNone;
            base.mBufferType = BufferTypeNULL;
            base.mWidth = base.mHeight = 0;

            return *this;
        }

        virtual bool init(uint32_t width, uint32_t height) = 0;

        uint32_t getWidth(void) const { return mWidth; }
        uint32_t getHeight(void) const { return mHeight; }

        virtual void setTexture(const sharedTexture& texture) = 0;

        virtual const sharedTexture& getTexture(void) const = 0;

        const RenderType& getRenderType(void) const { return mRenderType; }

        const BufferType& getBufferType(void) const { return mBufferType; }

        virtual GLuint getBuffer(const BufferType& type) const = 0;

    protected:
        RenderType  mRenderType;
        BufferType  mBufferType;
        uint32_t	mWidth;
        uint32_t	mHeight;
    };

    typedef std::shared_ptr<GLRenderBase>  sharedRenderTarget;

END

#endif //__GLRenderBase_h__

