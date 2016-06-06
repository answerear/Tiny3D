

#ifndef __T3D_SKIN_DATA_H__
#define __T3D_SKIN_DATA_H__


#include "Misc/T3DObject.h"
#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"
#include "Math/T3DMatrix4.h"


namespace Tiny3D
{
    class SkinData : public Object
    {
    public:
        static SkinDataPtr create(const String &name, const Matrix4 &m);

        virtual ~SkinData();

        const String &getName() const
        {
            return mName;
        }

        const Matrix4 &getBindPose() const
        {
            return mBindPose;
        }

    protected:
        SkinData(const String &name, const Matrix4 &m);

        String  mName;
        Matrix4 mBindPose;
    };
}



#endif  /*__T3D_SKIN_DATA_H__*/
