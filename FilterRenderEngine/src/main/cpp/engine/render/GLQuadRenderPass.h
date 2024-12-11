//
// Created by Joe.Bi on 2024/11/23.
//

#ifndef __GLQuadRenderPass_h__
#define __GLQuadRenderPass_h__
#include "GLRenderPass.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    class API_EXPORTS GLQuadRenderPass : public GLRenderPass
    {
    public:
        GLQuadRenderPass(void);
        ~GLQuadRenderPass(void);

        void draw(void);
    };

END

#endif //__GLQuadRenderPass_h__
