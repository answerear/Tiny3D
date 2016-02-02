

#ifndef __T3D_CREATOR_H__
#define __T3D_CREATOR_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    template <typename T>
    class Creator
    {
    public:
        virtual String getType() const = 0;
        virtual T* createObject() const = 0;
    };
}


#endif  /*__T3D_CREATOR_H__*/
