//
// Created by Joe.Bi on 2024/11/14.
//

#ifndef __GLQuadModel_h__
#define __GLQuadModel_h__
#include "GLModel.h"
#include <GLES3/gl3.h>
#include <GLES2/gl2.h>

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    class API_EXPORTS GLQuadModel : public GLModel
    {
    public:
        ~GLQuadModel(void) { }

        GLQuadModel(const GLQuadModel& model);
        GLQuadModel(GLQuadModel&& model);

        GLQuadModel& operator=(const GLQuadModel& model);
        GLQuadModel& operator=(GLQuadModel&& model);

        static sharedModel getQuadModel(void);
        static void releaseQuadModel(void) { mQuadModel.reset(); }

        void bindVAO(void);

        void draw(void);

    private:
        GLQuadModel(void);

    private:
        static sharedModel  mQuadModel;
        bool                mFirstBind;
    };

END

#endif //__GLQuadModel_h__

