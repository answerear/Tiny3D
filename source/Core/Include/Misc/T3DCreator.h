

#ifndef __T3D_CREATOR_H__
#define __T3D_CREATOR_H__


#include "T3DPrerequisites.h"
#include "Misc/T3DSmartPtr.h"


namespace Tiny3D
{
    template <typename T>
    class Creator
    {
    public:
        virtual String getType() const = 0;
        virtual SmartPtr<T> createObject(int32_t argc, ...) const = 0;
    };
}


#endif  /*__T3D_CREATOR_H__*/
