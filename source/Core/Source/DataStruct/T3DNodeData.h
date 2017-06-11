

#ifndef __T3D_NODE_DATA_H__
#define __T3D_NODE_DATA_H__


#include "Misc/T3DObject.h"
#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"


namespace Tiny3D
{
    class NodeData : public Object
    {
    public:
        static NodeDataPtr create(const String &name);

        String      mName;
        Matrix4     mLocalMatrix;

    protected:
        NodeData(const String &name);

    private:
        NodeData(const NodeData &rkOther);
        NodeData &operator =(const NodeData &rkOther);
    };
}


#endif  /*__T3D_BONE_DATA_H__*/
