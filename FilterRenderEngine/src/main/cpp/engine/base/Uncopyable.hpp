//
// Created by Joe.Bi on 2024/11/18.
//

#ifndef __Uncopyable_h__
#define __Uncopyable_h__
#include "Macro.h"

//---------------------------------------------------------------------------//
BEGIN

    //-----------------------------------------------------------------------//
    class API_EXPORTS Uncopyable
    {
    protected:
        Uncopyable(void) { }
        ~Uncopyable(void) { }
    
    private:
        Uncopyable(const Uncopyable& other);
        Uncopyable& operator=(const Uncopyable& other);
    };

END

#endif // __Uncopyable_h__
