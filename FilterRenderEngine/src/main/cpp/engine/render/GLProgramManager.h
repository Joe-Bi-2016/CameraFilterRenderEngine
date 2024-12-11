//
// Created by Joe.Bi on 2024/12/11.
//

#ifndef __GLProgramManager_h__
#define __GLProgramManager_h__
#include "GLProgram.h"
#include <string>
#include <map>
#include <mutex>

using namespace std;
//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    class API_EXPORTS GLProgramManager
    {
    public:
        static GLProgramManager* getInstance(void);
        static void releaseGLProgramManager(void);

        bool putProgram(const char* shaderName, const sharedProgram& program);
        bool putProgram(const string& shaderName, const sharedProgram& program)
        { if (!shaderName.empty()) return putProgram(shaderName.c_str(), program); }

        bool deleteProgram(const char* shaderName);
        bool deleteProgram(const string& shaderName)
        { if (!shaderName.empty()) return deleteProgram(shaderName.c_str()); }

        sharedProgram getProgram(const char* shaderName);
        sharedProgram getProgram(const string& shaderName)
        { if (!shaderName.empty()) getProgram(shaderName.c_str()); }

    private:
        GLProgramManager(void) { }
        ~GLProgramManager(void) { }

    private:
        static map<string, sharedProgram> mProgramSets;
    };

END

#endif //__GLProgramManager_h__
