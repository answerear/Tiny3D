

#ifndef __T3D_BONE_DATA_H__
#define __T3D_BONE_DATA_H__


#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"
#include "T3DNodeData.h"


namespace Tiny3D
{
    /**
     * @brief 骨骼数据
     */
    class BoneData : public NodeData
    {
    public:
        static BoneDataPtr create(const String &name);

        Matrix4     mOffsetMatrix;          /// 骨骼偏移矩阵
        bool        mIsMatrixDirty;         /// 是否要重新计算偏移矩阵

    protected:
        BoneData(const String &name);

    private:
        BoneData(const BoneData &rkOther);
        BoneData &operator =(const BoneData &rkOther);
    };
}


#endif  /*__T3D_BONE_DATA_H__*/
