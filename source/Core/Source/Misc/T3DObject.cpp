
#include "Misc/T3DObject.h"


namespace Tiny3D
{
    Object::Object()
        : mReferCount(1)
    {

    }

    Object::~Object()
    {

    }

    Object *Object::acquire()
    {
        ++mReferCount;
        return this;
    }

    void Object::release()
    {
        if (--mReferCount == 0)
        {
            delete this;
        }
    }
}
