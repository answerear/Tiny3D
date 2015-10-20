
#include "Misc/T3DObject.h"


namespace Tiny3D
{
    Object::Object()
        : mReferCount(0)
    {

    }

    Object::~Object()
    {

    }

    uint32_t Object::acquire()
    {
        return ++mReferCount;
    }

    void Object::release()
    {
        if (--mReferCount == 0)
        {
            delete this;
        }
    }
}
