//
// Created by Joe.Bi on 2024/11/26.
//

#ifndef __GLOESTexture_h__
#define __GLOESTexture_h__
#include "GLTexture.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    class API_EXPORTS GLOESTexture : public GLTexture
    {
        friend void deleter<GLOESTexture>(GLOESTexture*);
        friend class GLTexture;
    public:
        GLOESTexture(void) = delete;

        GLOESTexture(const GLOESTexture&) = default;

        GLOESTexture(GLOESTexture&&) = default;

        GLOESTexture& operator=(const GLOESTexture&) = default;

        GLOESTexture& operator=(GLOESTexture&&) = default;

        void bindTexture(void);

        void unbindTexture(void);

    private:
        GLOESTexture(const TexParam& param, const char* name = nullptr);
        ~GLOESTexture(void);
    };

END


#endif //__GLOESTexture_h__
