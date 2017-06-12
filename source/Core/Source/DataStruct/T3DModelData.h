

#ifndef __T3D_MODEL_DATA_H__
#define __T3D_MODEL_DATA_H__


#include "Misc/T3DObject.h"
#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"

#include "T3DMeshData.h"
#include "T3DNodeData.h"
#include "T3DBoneData.h"
#include "T3DActionData.h"


namespace Tiny3D
{
    /**
     * @brief 模型数据
     */
    class ModelData : public Object
    {
    public:
        typedef std::vector<MeshDataPtr>        MeshDataList;
        typedef MeshDataList::iterator          MeshDataListItr;
        typedef MeshDataList::const_iterator    MeshDataListConstItr;

        typedef std::vector<BoneDataPtr>        BoneDataList;
        typedef BoneDataList::iterator          BoneDataListItr;
        typedef BoneDataList::const_iterator    BoneDataListConstItr;

        typedef std::vector<NodeDataPtr>        NodeDataList;
        typedef NodeDataList::iterator          NodeDataListItr;
        typedef NodeDataList::const_iterator    NodeDataListConstItr;

        typedef std::map<String, ObjectPtr>     AnimationData;
        typedef AnimationData::iterator         AnimationItr;
        typedef AnimationData::const_iterator   AnimationConstItr;
        typedef std::pair<String, ObjectPtr>    AnimationValue;

        static ModelDataPtr create();

        bool            mIsVertexShared;        /// 所有网格是否共享顶点缓存
        MeshDataList    mMeshes;                /// 网格数据列表
        BoneDataList    mBones;                 /// 骨骼数据列表
        AnimationData   mAnimations;            /// 动画数据列表
        NodeDataList    mNodes;                 /// 变换结点数据列表

    protected:
        ModelData();

    protected:
        ModelData(const ModelData &rkOther);
        ModelData &operator =(const ModelData &rkOther);
    };
}


#endif  /*__T3D_MODEL_DATA_H__*/
