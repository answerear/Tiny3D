

#ifndef __T3D_OBJECT_H__
#define __T3D_OBJECT_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Object
    {
        friend class AutoreleasePool;

    public:
        Object();
        virtual ~Object();

        virtual Object *acquire();
        virtual void release();
        
        uint32_t referCount() const
        {
            return mReferCount;
        }

    private:
        uint32_t    mReferCount;
        uint32_t    mAutoReleaseCount;
    };
}


#endif  /*__T3D_OBJECT_H__*/
