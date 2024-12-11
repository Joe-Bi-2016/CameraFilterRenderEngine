//
// Created by Joe.Bi on 2024/11/13.
//

#ifndef __GLRender2Texture_h__
#define __GLRender2Texture_h__
#include "GLRenderBase.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    class API_EXPORTS GLRender2Texture : public GLRenderBase
    {
        friend void deleter<GLRender2Texture>(GLRender2Texture*);
    public:
        static sharedRenderTarget generate2DRenderTarget(void);

        GLRender2Texture(const GLRender2Texture& texture);
        GLRender2Texture(GLRender2Texture&& texture);
        GLRender2Texture& operator=(const GLRender2Texture& texture);
        GLRender2Texture& operator=(GLRender2Texture&& texture);

        bool init(uint32_t width, uint32_t height);

        void setTexture(const sharedTexture& texture) { mTexture = texture; }

        const sharedTexture& getTexture(void) const { return mTexture; }

        GLuint getBuffer(const BufferType& type) const { return 0; }

    private:
        GLRender2Texture(void);
        ~GLRender2Texture(void);

    protected:
        sharedTexture	mTexture;
    };

END

#endif //__GLRender2Texture_h__
