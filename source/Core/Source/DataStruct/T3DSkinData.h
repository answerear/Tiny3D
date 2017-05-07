

#ifndef __T3D_SKIN_DATA_H__
#define __T3D_SKIN_DATA_H__


#include "Misc/T3DObject.h"
#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"


namespace Tiny3D
{
    class SkinData : public Object
    {
    public:
        static SkinDataPtr create(const String &name);

        String      mName;
        Matrix4     mOffsetMatrix;

    protected:
        SkinData(const String &name);

    private:
        SkinData(const SkinData &rkOther);
        SkinData &operator =(const SkinData &rkOther);
    };
}


#endif  /*__T3D_SKIN_DATA_H__*/
