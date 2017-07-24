

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

        String      mName;              /// 结点名称
        uint16_t    mParent;            /// 父节点索引
        Matrix4     mLocalMatrix;       /// 结点本地变换

        bool        mHasLink;           /// 是否有挂渲染网格
        String      mLinkMesh;          /// 本结点下挂接的网格
        String      mLinkSubMesh;       /// 网格下挂接的子网格

    protected:
        NodeData(const String &name);

    private:
        NodeData(const NodeData &rkOther);
        NodeData &operator =(const NodeData &rkOther);
    };
}


#endif  /*__T3D_BONE_DATA_H__*/
