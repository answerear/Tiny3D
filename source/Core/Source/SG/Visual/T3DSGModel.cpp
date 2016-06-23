

#include "SG/Visual/T3DSGModel.h"
#include "SG/Renderable/T3DSGMesh.h"
#include "SG/Renderable/T3DSGSkeleton.h"
#include "Resource/T3DModel.h"
#include "Resource/T3DModelManager.h"
#include "DataStruct/T3DGeometryData.h"
#include "DataStruct/T3DBone.h"
#include "DataStruct/T3DActionData.h"
#include "Misc/T3DEntrance.h"


namespace Tiny3D
{
    template <typename T, typename P>
    bool searchKeyframe(const T &container, int64_t time, int32_t duration, int32_t &frame, P &keyframe, bool loop)
    {
        bool ret = false;

        int32_t i = frame;

        return ret;
    }

    SGModelPtr SGModel::create(const String &modelName, uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGModelPtr model = new SGModel(unID);
        if (model != nullptr && model->init(modelName))
        {
            model->release();
        }
        else
        {
            T3D_SAFE_RELEASE(model);
        }
        return model;
    }

    SGModel::SGModel(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGShape(unID)
        , mModel(nullptr)
        , mRenderMode(E_RENDER_ENTITY)
        , mSkeleton(nullptr)
        , mStarrTime(0)
        , mCurKeyFrameT(0)
        , mCurKeyFrameR(0)
        , mCurKeyFrameS(0)
    {

    }

    SGModel::~SGModel()
    {
        mSkeleton = nullptr;
        mMeshes.clear();
        T3D_MODEL_MGR.unloadModel(mModel);
    }

    bool SGModel::init(const String &modelName)
    {
        bool ret = false;
        mModel = T3D_MODEL_MGR.loadModel(modelName);

        if (mModel != nullptr)
        {
            ret = true;

            const Model::GeometryDataList &geometries = mModel->getGeometryDataList();
            size_t count = geometries.size();
            size_t i = 0;
            for (i = 0; i < count; ++i)
            {
                SGMeshPtr mesh = SGMesh::create(mModel, i);
                addChild(mesh);
                mMeshes.push_back(mesh);
            }
        }

        return ret;
    }

    Node::Type SGModel::getNodeType() const
    {
        return E_NT_MODEL;
    }

    NodePtr SGModel::clone() const
    {
        SGModelPtr model = create(mModel->getName());

        if (model != nullptr)
        {
            SGShape::cloneProperties(model);
        }
        
        return model;
    }

    void SGModel::updateTransform()
    {
        SGShape::updateTransform();

        updateSkeleton();
    }

    void SGModel::setRenderMode(RenderMode mode)
    {
        Renderer *renderer = T3D_ENTRANCE.getActiveRenderer();
        if (renderer->getRenderMode() == Renderer::E_RM_SOLID)
            return;

        if (mode != mRenderMode)
        {
            switch (mode)
            {
            case E_RENDER_ENTITY:
                {
                    removeAllChildren(false);

                    if (!mMeshes.empty())
                    {
                        auto itr = mMeshes.begin();
                        while (itr != mMeshes.end())
                        {
                            addChild(*itr);
                            ++itr;
                        }
                    }
                }
                break;
            case E_RENDER_SKELETON:
                {
//                     removeAllChildren(false);

                    if (mSkeleton == nullptr)
                    {
                        // 没有生成过骨骼渲染对象，先创建
                        BonePtr bone = smart_pointer_cast<Bone>(mModel->getSkeletonData());
                        bone->updateBone();
                        mSkeleton = SGSkeleton::create(mModel);
                    }

                    addChild(mSkeleton);
                }
                break;
            }

            mRenderMode = mode;
        }
    }

    void SGModel::enumerateActionList(ActionList &actions)
    {
        const Model::AnimationData &animation = mModel->getAnimationData();
        auto itr = animation.begin();
        actions.clear();

        while (itr != animation.end())
        {
            ActionInfo action;
            ActionDataPtr actionData = smart_pointer_cast<ActionData>(itr->second);
            action.mName = actionData->mName;
            ++itr;
        }
    }

    bool SGModel::runAction(const String &name, bool repeat /* = true */, int32_t frame /* = 0 */)
    {
        if (isActionRunning())
        {
            stopAction(mCurActionName);
        }

        const Model::AnimationData &actionList = mModel->getAnimationData();
        auto itr = actionList.find(name);
        if (itr == actionList.end())
            return false;

        mStarrTime = DateTime::currentMSecsSinceEpoch();

        mCurActionData = itr->second;
        mCurKeyFrameT = 0;
        mCurKeyFrameR = 0;
        mCurKeyFrameS = 0;

        mIsActionRunning = true;

        return true;
    }

    bool SGModel::stopAction(const String &name)
    {
        return true;
    }

    void SGModel::updateSkeleton()
    {
        if (isActionRunning())
        {
            int64_t current = DateTime::currentMSecsSinceEpoch();
            int64_t dt = current - mStarrTime;

            updateBone(dt, mModel->getSkeletonData());
        }
    }

    void SGModel::updateBone(int64_t dt, ObjectPtr skeleton)
    {
        BonePtr bone = smart_pointer_cast<Bone>(skeleton);

        ActionDataPtr actionData = smart_pointer_cast<ActionData>(mCurActionData);
        
        auto itrT = actionData->mBonesTranslation.find(bone->getName());
        ActionData::KeyFrames &keyframesT = itrT->second;

        KeyFrameDataPtr keyframe;
//         if (searchKeyFrame(keyframesT, dt, mCurKeyFrameT, keyframe))
//         {
//             KeyFrameDataTPtr keyframeT = smart_pointer_cast<KeyFrameDataT>(keyframe);
//         }
        
        
    }
}
