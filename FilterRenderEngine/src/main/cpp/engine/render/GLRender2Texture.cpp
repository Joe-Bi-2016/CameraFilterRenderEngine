//
// Created by Joe.Bi on 2024/11/13.
//

#include "GLRender2Texture.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    GLRender2Texture::GLRender2Texture(void) : GLRenderBase(Render2Texture)
    {
        TexParam param;
        param.wrapS = WRAPEDGE;
        param.wrapT = WRAPEDGE;
        param.antialiasEnabled = false;

        mTexture = GLTexture::createTexture<GLTexture2D>(param);
    }

    //------------------------------------------------------------------------------------//
    GLRender2Texture::~GLRender2Texture(void)
    {
        mTexture.reset();
    }

    //------------------------------------------------------------------------------------//
    GLRender2Texture::GLRender2Texture(const GLRender2Texture& texture)
    : GLRenderBase(texture)
    {
        mTexture = texture.mTexture;
    }

    //------------------------------------------------------------------------------------//
    GLRender2Texture::GLRender2Texture(GLRender2Texture&& texture)
    : GLRenderBase(std::move(texture))
    {
        mTexture = std::move(texture.mTexture);
        texture.mRenderType = RenderNone;
        texture.mWidth = texture.mHeight = 0;
    }

    //------------------------------------------------------------------------------------//
    GLRender2Texture& GLRender2Texture::operator=(const GLRender2Texture& texture)
    {
        GLRenderBase::operator=(texture);
        mTexture = texture.mTexture;
        return *this;
    }

    //------------------------------------------------------------------------------------//
    GLRender2Texture& GLRender2Texture::operator=(GLRender2Texture&& texture)
    {
        GLRenderBase::operator=(std::move(texture));
        mTexture = std::move(texture.mTexture);
        return *this;
    }

    //------------------------------------------------------------------------------------//
    sharedRenderTarget GLRender2Texture::generate2DRenderTarget(void)
    {
        return sharedRenderTarget(new GLRender2Texture(), deleter<GLRender2Texture>);
    }

    //------------------------------------------------------------------------------------//
    bool GLRender2Texture::init(uint32_t width, uint32_t height)
    {
        mWidth = width;
        mHeight = height;

        mTexture->initWithData(nullptr, width * height * 4, PixelFormat(RGBA8888), width, height, width, height);

        return true;
    }

END
