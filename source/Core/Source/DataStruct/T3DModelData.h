

#ifndef __T3D_MODEL_DATA_H__
#define __T3D_MODEL_DATA_H__


#include "Misc/T3DObject.h"
#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"

#include "T3DMeshData.h"
#include "T3DSkinData.h"
#include "T3DBoneData.h"
#include "T3DActionData.h"


namespace Tiny3D
{
    class ModelData : public Object
    {
    public:
        typedef std::vector<MeshDataPtr>        MeshDataList;
        typedef MeshDataList::iterator          MeshDataListItr;
        typedef MeshDataList::const_iterator    MeshDataListConstItr;

        typedef std::vector<BoneDataPtr>        BoneDataList;
        typedef BoneDataList::iterator          BoneDataListItr;
        typedef BoneDataList::const_iterator    BoneDataListConstItr;

        typedef std::map<String, ObjectPtr>     AnimationData;
        typedef AnimationData::iterator         AnimationItr;
        typedef AnimationData::const_iterator   AnimationConstItr;
        typedef std::pair<String, ObjectPtr>    AnimationValue;

        static ModelDataPtr create();

        bool            mIsVertexShared;
        MeshDataList    mMeshes;
        BoneDataList    mBones;
        AnimationData   mAnimations;
        Matrix4         mVertexMatrix;

    protected:
        ModelData();

    protected:
        ModelData(const ModelData &rkOther);
        ModelData &operator =(const ModelData &rkOther);
    };
}


#endif  /*__T3D_MODEL_DATA_H__*/
