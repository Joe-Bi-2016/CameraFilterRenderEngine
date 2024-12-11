//
// Created by Joe.Bi on 2024/11/26.
//

#include "GLOESTexture.h"
#include <GLES2/gl2ext.h>
#include <GLES3/gl3ext.h>

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    GLOESTexture::GLOESTexture(const TexParam& param, const char* name/* = nullptr*/)
    : GLTexture(param, name)
    {
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, mTextureId);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER,
                            mTexParams.antialiasEnabled ? GL_LINEAR : GL_NEAREST);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER,
                            mTexParams.antialiasEnabled ? GL_LINEAR : GL_NEAREST);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, mTexParams.wrapS);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, mTexParams.wrapT);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, GL_NONE);
    }

    //------------------------------------------------------------------------------------//
    GLOESTexture::~GLOESTexture(void)
    {
        unbindTexture();
    }

    //------------------------------------------------------------------------------------//
    void GLOESTexture::bindTexture(void)
    {
        if(glIsTexture(mTextureId))
            glBindTexture(GL_TEXTURE_EXTERNAL_OES, mTextureId);
    }

    //------------------------------------------------------------------------------------//
    void GLOESTexture::unbindTexture(void)
    {
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, GL_NONE);
    }

END
