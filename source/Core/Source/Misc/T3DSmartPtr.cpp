
#include "Misc/T3DSmartPtr.h"
#include "Misc/T3DObject.h"


namespace Tiny3D
{
    SmartBasePtr::~SmartBasePtr()
    {
        if (mReferObject != nullptr)
        {
            mReferObject->release();
        }
    }
}