

#ifndef __T3D_BONE_DATA_H__
#define __T3D_BONE_DATA_H__


#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"
#include "SG/Node/T3DSGTransformNode.h"
#include "Math/T3DMatrix4.h"


namespace Tiny3D
{
    class Bone : public SGTransformNode
    {
    public:
        virtual ~Bone();

        static BonePtr create(const String &name, const Matrix4 &m);

        const String &getName() const
        {
            return mName;
        }

    protected:
        Bone(const String &name, const Matrix4 &m);

    protected:
        String  mName;
    };
}

#endif  /*__T3D_BONE_DATA_H__*/
