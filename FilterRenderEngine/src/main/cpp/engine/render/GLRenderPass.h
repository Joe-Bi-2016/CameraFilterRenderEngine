//
// Created by Joe.Bi on 2024/11/8.
//

#ifndef __GLRenderPass_h__
#define __GLRenderPass_h__
#include "GLProgram.h"
#include "GLQuadModel.h"
#include "GLFrameBuffer.h"
#include <vector>
#include "eglcore/EglCore.h"

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    class GLRenderPass;
    typedef std::shared_ptr<GLRenderPass>  sharedRenderPass;

    //------------------------------------------------------------------------------------//
    class API_EXPORTS GLRenderPass
    {
    public:
        explicit GLRenderPass(void);
        explicit GLRenderPass(const GLRenderPass&);
        explicit GLRenderPass(GLRenderPass&&);
        explicit GLRenderPass(const GLRenderPass*);
        virtual ~GLRenderPass(void);

        GLRenderPass& operator=(GLRenderPass&&);
        GLRenderPass& operator=(const GLRenderPass&);

        virtual void setFbo(int width, int height);

        virtual void isDraw2fbo(bool usingfbo) { mDraw2Fbo = usingfbo; }

        virtual void setDrawFbo(sharedFrameBuffer fbo) { mFbo = fbo; }

        const sharedFrameBuffer getFbo(void) const { return mFbo; }

        const sharedTexture getFboColorBuffer(void) const;

        virtual void setProgram(sharedProgram program) { mProgramObj = program; }

        virtual void setProgram(const char* programName, const char* vertShader, const char* fragShader);

        sharedProgram& getProgram(void) { return mProgramObj; }

        const sharedTexture getTexture(int index) const;

        const sharedTexture getTexture(const char* name) const;

        virtual void addTexture(const sharedTexture& texture) { mTextureList.push_back(texture); }

        virtual void addTexture(TextureKind kind, const char* name = nullptr);

        virtual void setModel(const sharedModel& model) { mQuadModel = model; }

        void setClearColor(float r, float g, float b, float a);

        virtual void draw(void);

    protected:
        sharedFrameBuffer           mFbo;
        sharedProgram               mProgramObj;
        std::vector<sharedTexture>  mTextureList;
        sharedModel                 mQuadModel;
        vec4f			            mClearColor;
        bool                        mColorbeUsing;
        bool                        mhadSettedAttrib;
        bool                        mDraw2Fbo;
    };



END

#endif //__GLRenderPass_h__
