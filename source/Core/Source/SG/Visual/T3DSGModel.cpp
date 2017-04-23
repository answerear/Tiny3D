

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
    template <typename T, typename K>
    bool searchKeyframe(const T &container, int64_t time, int32_t duration, int32_t &frame, K &keyframe1, K &keyframe2, bool loop)
    {
        bool ret = false;

        int32_t i = frame;

        while (i < container.size() && i + 1 < container.size())
        {
            K kf1 = container[i];
            K kf2 = container[i+1];

            if (time > kf1->mTimestamp && time < kf2->mTimestamp)
            {
                keyframe1 = kf1;
                keyframe2 = kf2;
                frame = i;
                ret = true;
                break;
            }

            ++i;
        }

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
        , mStartTime(0)
        , mCurKeyFrameT(0)
        , mCurKeyFrameR(0)
        , mCurKeyFrameS(0)
        , mIsActionRunning(false)
        , mIsLoop(false)
        , mCurActionData(nullptr)
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
                    removeAllChildren(false);

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

        mStartTime = DateTime::currentMSecsSinceEpoch();

        mCurActionData = itr->second;
        mCurKeyFrameT = 0;
        mCurKeyFrameR = 0;
        mCurKeyFrameS = 0;

        mIsLoop = repeat;
        mIsActionRunning = true;

        return true;
    }

    bool SGModel::stopAction(const String &name)
    {
        mIsActionRunning = false;
        return true;
    }

    void SGModel::updateSkeleton()
    {
        if (isActionRunning())
        {
            int64_t current = DateTime::currentMSecsSinceEpoch();
            int64_t time = current - mStartTime;
            ActionDataPtr actionData = smart_pointer_cast<ActionData>(mCurActionData);
            int64_t dt = time % actionData->mDuration;
            T3D_LOG_INFO("time : %lld, dt = %lld, duration : %d", time, dt, actionData->mDuration);
            updateBone(dt, mModel->getSkeletonData());
            mSkeleton->updateVertices();
        }
    }

    void SGModel::updateBone(int64_t time, ObjectPtr skeleton)
    {
        BonePtr bone = smart_pointer_cast<Bone>(skeleton);

        ActionDataPtr actionData = smart_pointer_cast<ActionData>(mCurActionData);
        
        KeyFrameDataPtr kf1, kf2;

        // 平移变换数据
        Vector3 translation;
        auto itrT = actionData->mBonesTranslation.find(bone->getName());
        if (itrT != actionData->mBonesTranslation.end())
        {
            ActionData::KeyFrames &keyframesT = itrT->second;

            if (searchKeyframe(keyframesT, time, actionData->mDuration, mCurKeyFrameT, kf1, kf2, mIsLoop))
            {
                KeyFrameDataTPtr keyframe1 = smart_pointer_cast<KeyFrameDataT>(kf1);
                KeyFrameDataTPtr keyframe2 = smart_pointer_cast<KeyFrameDataT>(kf2);
                double t = double(time - keyframe1->mTimestamp) / double(keyframe2->mTimestamp - keyframe1->mTimestamp);
                Vector3 &base = keyframe1->mTranslation;
                translation = (base + (keyframe2->mTranslation - base) * t);
                T3D_LOG_INFO("Keyframe #1 T(%f, %f, %f)", keyframe1->mTranslation[0], keyframe1->mTranslation[1], keyframe1->mTranslation[2]);
                T3D_LOG_INFO("Keyframe #2 T(%f, %f, %f)", keyframe2->mTranslation[0], keyframe2->mTranslation[1], keyframe2->mTranslation[2]);
                T3D_LOG_INFO("Bone : %s [%f], T(%f, %f, %f)", bone->getName().c_str(), t, translation[0], translation[1], translation[2]);

                bone->setTranslation(translation);
            }
        }
        
        // 旋转变换数据
        Quaternion orientation;
        auto itrR = actionData->mBonesRotation.find(bone->getName());
        if (itrR != actionData->mBonesRotation.end())
        {
            ActionData::KeyFrames &keyframesR = itrR->second;

            int32_t frame = 0;
            if (searchKeyframe(keyframesR, time, actionData->mDuration, frame, kf1, kf2, mIsLoop))
            {
                KeyFrameDataRPtr keyframe1 = smart_pointer_cast<KeyFrameDataR>(kf1);
                KeyFrameDataRPtr keyframe2 = smart_pointer_cast<KeyFrameDataR>(kf2);
                double t = double(time - keyframe1->mTimestamp) / double(keyframe2->mTimestamp - keyframe1->mTimestamp);
                orientation.lerp(keyframe1->mOrientation, keyframe2->mOrientation, t/* / 1000*/);
                T3D_LOG_INFO("Keyframe #1 R(%f, %f, %f, %f)", keyframe1->mOrientation[0], keyframe1->mOrientation[1], keyframe1->mOrientation[2], keyframe1->mOrientation[3]);
                T3D_LOG_INFO("Keyframe #2 R(%f, %f, %f, %f)", keyframe2->mOrientation[0], keyframe2->mOrientation[1], keyframe2->mOrientation[2], keyframe2->mOrientation[3]);
                Degree deg;
                Vector3 axis;
                orientation.toAngleAxis(deg, axis);
                T3D_LOG_INFO("Bone : %s [%lld], keyframe=%d, kf1t=%lld, kf2t=%lld, R(%f, %f, %f, %f), deg=%f, Axis(%f, %f, %f)", 
                    bone->getName().c_str(), time, frame, keyframe1->mTimestamp, keyframe2->mTimestamp,
                    orientation[0], orientation[1], orientation[2], orientation[3], deg.valueDegrees(), axis[0], axis[1], axis[2]);
                bone->setOrientation(orientation);
            }
            else
            {
                KeyFrameDataRPtr keyframe = smart_pointer_cast<KeyFrameDataR>(keyframesR.back());
                Degree deg;
                Vector3 axis;
                orientation = keyframe->mOrientation;
                orientation.toAngleAxis(deg, axis);
                T3D_LOG_INFO("Bone : %s [%lld], keyframe=%d, R(%f, %f, %f, %f), deg=%f, Axis(%f, %f, %f)", bone->getName().c_str(), time, mCurKeyFrameR,
                    orientation[0], orientation[1], orientation[2], orientation[3], deg.valueDegrees(), axis[0], axis[1], axis[2]);
                bone->setOrientation(orientation);
            }
        }

        // 缩放变换数据
        Vector3 scaling;
        auto itrS = actionData->mBonesScaling.find(bone->getName());
        if (itrS != actionData->mBonesScaling.end())
        {
            ActionData::KeyFrames &keyframesS = itrS->second;

            if (searchKeyframe(keyframesS, time, actionData->mDuration, mCurKeyFrameS, kf1, kf2, mIsLoop))
            {
                KeyFrameDataSPtr keyframe1 = smart_pointer_cast<KeyFrameDataS>(kf1);
                KeyFrameDataSPtr keyframe2 = smart_pointer_cast<KeyFrameDataS>(kf2);
                double t = double(time - keyframe1->mTimestamp) / double(keyframe2->mTimestamp - keyframe1->mTimestamp);
                Vector3 &base = keyframe1->mScaling;
                scaling = (base * (keyframe2->mScaling - base) * t);
                T3D_LOG_INFO("Keyframe #1 S(%f, %f, %f)", keyframe1->mScaling[0], keyframe1->mScaling[1], keyframe1->mScaling[2]);
                T3D_LOG_INFO("Keyframe #2 S(%f, %f, %f)", keyframe2->mScaling[0], keyframe2->mScaling[1], keyframe2->mScaling[2]);
                T3D_LOG_INFO("Bone : %s [%f], S(%f, %f, %f)", bone->getName().c_str(), t, scaling[0], scaling[1], scaling[2]);

                bone->setScaling(scaling);
            }
        }

        bone->updateBone();

        auto itr = bone->getChildren().begin();
        while (itr != bone->getChildren().end())
        {
            updateBone(time, *itr);
            ++itr;
        }
    }
}
