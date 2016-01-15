

#ifndef __T3D_OBJECT_H__
#define __T3D_OBJECT_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Object
    {
    public:
        Object();
        virtual ~Object();

        Object *acquire();
        void release();
        
        uint32_t referCount() const
        {
            return mReferCount;
        }

    private:
        uint32_t    mReferCount;
    };
}


#endif  /*__T3D_OBJECT_H__*/
