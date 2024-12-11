//
// Created by Joe.Bi on 2024/11/23.
//

#include <stdint.h>
#include "GLQuadRenderPass.h"
#include "GLDefaultShader.h"
#include "GLProgramManager.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    GLQuadRenderPass::GLQuadRenderPass(void)
    : GLRenderPass()
    {
        mQuadModel = GLQuadModel::getQuadModel();
    }

    //------------------------------------------------------------------------------------//
    GLQuadRenderPass::~GLQuadRenderPass()
    {
    }

    //------------------------------------------------------------------------------------//
    void GLQuadRenderPass::draw(void)
    {
        if(mProgramObj.get() == nullptr)
        {
            if(mTextureList.size() == 0)
            {
                mProgramObj = GLProgram::createGLProgram(quadVertGLSL, quadColorFragGLSL);
                GLProgramManager::getInstance()->putProgram("inner_quadColorProgram", mProgramObj);
            }
            else if(mTextureList.size() == 1)
            {
                mProgramObj = GLProgram::createGLProgram(quadVertGLSL, quadFragGLSL);
                GLProgramManager::getInstance()->putProgram("inner_quadProgram", mProgramObj);
            }
            else
            {
                LOGERROR("Error: program is not match with texture");
                return;
            }
        }

        GLRenderPass::draw();
    }


END