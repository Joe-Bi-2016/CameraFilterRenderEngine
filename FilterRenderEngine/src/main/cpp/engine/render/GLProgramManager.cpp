//
// Created by Joe.Bi on 2024/12/11.
//

#include "GLProgramManager.h"
#include <string>
#include <assert.h>

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    map<string, sharedProgram> GLProgramManager::mProgramSets = map<string, sharedProgram>();

    //------------------------------------------------------------------------------------//
    GLProgramManager* GLProgramManager::getInstance(void)
    {
        static GLProgramManager glProgramManager;
        return &glProgramManager;
    }

    //------------------------------------------------------------------------------------//
    bool GLProgramManager::putProgram(const char* shaderName, const sharedProgram& program)
    {
        assert(shaderName != nullptr);
        if (mProgramSets.find(shaderName) != mProgramSets.end())
        {
            LOGWARNING("%s%s%s", "program ", shaderName, " had been created!!!");
            return false;
        }
        mProgramSets[shaderName] = program;
        return true;
    }

    //------------------------------------------------------------------------------------//
    bool GLProgramManager::deleteProgram(const char* shaderName)
    {
        assert(shaderName != nullptr);
        map<string, sharedProgram>::iterator it = mProgramSets.find(shaderName);
        if (it != mProgramSets.end())
        {
            mProgramSets.erase(it);
            return true;
        }
        else
            LOGWARNING("%s%s%s", "program ", shaderName, " had not been created!!!");
        return false;
    }

    //------------------------------------------------------------------------------------//
    sharedProgram GLProgramManager::getProgram(const char* shaderName)
    {
        assert(shaderName != nullptr);
        if (mProgramSets.find(shaderName) == mProgramSets.end())
        {
            LOGWARNING("%s%s%s", "program ", shaderName, " had not been created!!!");
            return nullptr;
        }

        return mProgramSets[shaderName];
    }

    //------------------------------------------------------------------------------------//
    void GLProgramManager::releaseGLProgramManager(void)
    {
        mProgramSets.clear();
        map<string, sharedProgram>().swap(mProgramSets);
    }

END
