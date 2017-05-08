

#include "SG/Visual/T3DSGModel.h"
#include "SG/Renderable/T3DSGMesh.h"
#include "SG/Renderable/T3DSGSkeleton.h"
#include "Resource/T3DModel.h"
#include "Resource/T3DModelManager.h"
#include "DataStruct/T3DModelData.h"
#include "DataStruct/T3DBone.h"
#include "Misc/T3DEntrance.h"
#include "Render/T3DHardwareBufferManager.h"


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
        T3D_MODEL_MGR.unloadModel(mModel);
    }

    bool SGModel::init(const String &modelName)
    {
        bool ret = false;
        mModel = T3D_MODEL_MGR.loadModel(modelName);

        if (mModel != nullptr)
        {
            ret = true;

            ModelDataPtr modelData = smart_pointer_cast<ModelData>(mModel->getModelData());

            if (modelData->mIsVertexShared)
            {
                // 共享顶点模式，只有一个mesh，多个submesh
                T3D_ASSERT(modelData->mMeshes.size() == 1);

                // 创建共享的顶点数据对象
                MeshDataPtr meshData = modelData->mMeshes[0];
                VertexDataPtr vertexData = createVertexData(meshData);
                mVertexDataList.push_back(vertexData);

                // 根据子网格数据逐个创建渲染用的网格对象
                size_t submeshCount = meshData->mSubMeshes.size();
                size_t i = 0;

                auto itr = meshData->mSubMeshes.begin();
                for (i = 0; i < submeshCount; ++i)
                {
                    SubMeshDataPtr submeshData = *itr;
                    SGMeshPtr mesh = SGMesh::create(vertexData, submeshData);
                    addChild(mesh);
                    mMeshes.push_back(mesh);
                    ++itr;
                }
            }
            else
            {
                // 不共享顶点模式，有多个mesh和多个submesh
                size_t meshCount = modelData->mMeshes.size();
                size_t i = 0;
                mVertexDataList.resize(meshCount);

                for (i = 0; i < meshCount; ++i)
                {
                    // 根据网格数据来逐个创建渲染用网格对象
                    MeshDataPtr meshData = modelData->mMeshes[i];
                    VertexDataPtr vertexData = createVertexData(meshData);
                    mVertexDataList[i] = vertexData;

                    // 根据子网格数据逐个创建渲染用的网格对象
                    size_t j = 0;
                    size_t submeshCount = meshData->mSubMeshes.size();

                    for (j = 0; j < submeshCount; ++j)
                    {
                        SGMeshPtr mesh = SGMesh::create(vertexData, meshData);
                        addChild(mesh);
                        mMeshes.push_back(mesh);
                    }
                }
            }

            // 创建骨骼层次和骨骼偏移矩阵数组
            ret = createSkeletons();

            if (modelData->mAnimations.size() > 0)
            {
                auto animations = modelData->mAnimations;
                auto itr = animations.begin();

                mCurActionData = itr->second;
                mCurKeyFrameT = 0;
                mCurKeyFrameR = 0;
                mCurKeyFrameS = 0;

                BonePtr bone = smart_pointer_cast<Bone>(mRootBone);
                bone->updateBone();
                updateVertices();
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
        if (isActionRunning())
        {
            updateSkeleton();
        }

        SGShape::updateTransform();
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
                        BonePtr bone = smart_pointer_cast<Bone>(mRootBone);
                        bone->updateBone();
                        mSkeleton = SGSkeleton::create(mRootBone);
                    }

                    addChild(mSkeleton);
                }
                break;
            }

            mRenderMode = mode;
        }
    }

    VertexDataPtr SGModel::createVertexData(ObjectPtr data)
    {
        MeshDataPtr meshData = smart_pointer_cast<MeshData>(data);

        VertexDeclarationPtr vertexDecl = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
        auto itr = meshData->mBuffers.begin();
        while (itr != meshData->mBuffers.end())
        {
            auto buffer = *itr;
            auto i = buffer->mAttributes.begin();

            while (i != buffer->mAttributes.end())
            {
                auto vertexElement = *i;
                vertexDecl->addElement(vertexElement);
                ++i;
            }

            ++itr;
        }

        VertexDataPtr vertexData = VertexData::create(vertexDecl);

        itr = meshData->mBuffers.begin();
        while (itr != meshData->mBuffers.end())
        {
            auto buffer = *itr;

            size_t vertexCount = buffer->mVertices.size() / buffer->mVertexSize;
            HardwareVertexBufferPtr vertexBuffer = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(buffer->mVertexSize, vertexCount, HardwareBuffer::E_HBU_WRITE_ONLY, false);

            if (vertexDecl != nullptr && vertexBuffer != nullptr)
            {
                if (vertexBuffer->writeData(0, buffer->mVertices.size(), &buffer->mVertices[0]))
                {
                    vertexData->addVertexBuffer(vertexBuffer);
                }
            }

            ++itr;
        }

        return vertexData;
    }

    bool SGModel::createSkeletons()
    {
        ModelDataPtr modelData = smart_pointer_cast<ModelData>(mModel->getModelData());
        mBones.resize(modelData->mBones.size());
        size_t i = 0;

        for (i = 0; i < modelData->mBones.size(); ++i)
        {
            auto boneData = modelData->mBones[i];
            BonePtr bone = Bone::create(boneData->mName, boneData->mOffsetMatrix, boneData->mLocalMatrix);
            mBones[i] = bone;
        }

        for (i = 0; i < mBones.size(); ++i)
        {
            auto bone = smart_pointer_cast<Bone>(mBones[i]);
            auto boneData = modelData->mBones[i];
            if (boneData->mParentBone == 0xFFFF)
            {
                mRootBone = bone;
            }
            else
            {
                T3D_ASSERT(boneData->mParentBone < mBones.size());
                auto parentBone = smart_pointer_cast<Bone>(mBones[boneData->mParentBone]);
                parentBone->addChild(bone);
            }
        }

        return true;
    }

    void SGModel::enumerateActionList(ActionList &actions)
    {
        ModelDataPtr modelData = smart_pointer_cast<ModelData>(mModel->getModelData());
        auto animations = modelData->mAnimations;
        auto itr = animations.begin();
        actions.clear();

        while (itr != animations.end())
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

        ModelDataPtr modelData = smart_pointer_cast<ModelData>(mModel->getModelData());
        auto animations = modelData->mAnimations;
        auto itr = animations.find(name);
        if (itr == animations.end())
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
        int64_t current = DateTime::currentMSecsSinceEpoch();
        int64_t time = current - mStartTime;
        ActionDataPtr actionData = smart_pointer_cast<ActionData>(mCurActionData);
        int64_t dt = time % actionData->mDuration;
        T3D_LOG_INFO("time : %lld, dt = %lld, duration : %d", time, dt, actionData->mDuration);
        updateBone(dt, mRootBone);
        BonePtr bone = smart_pointer_cast<Bone>(mRootBone);
        bone->updateBone();

        updateVertices();
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
//         Vector3 scaling;
//         auto itrS = actionData->mBonesScaling.find(bone->getName());
//         if (itrS != actionData->mBonesScaling.end())
//         {
//             ActionData::KeyFrames &keyframesS = itrS->second;
// 
//             if (searchKeyframe(keyframesS, time, actionData->mDuration, mCurKeyFrameS, kf1, kf2, mIsLoop))
//             {
//                 KeyFrameDataSPtr keyframe1 = smart_pointer_cast<KeyFrameDataS>(kf1);
//                 KeyFrameDataSPtr keyframe2 = smart_pointer_cast<KeyFrameDataS>(kf2);
//                 double t = double(time - keyframe1->mTimestamp) / double(keyframe2->mTimestamp - keyframe1->mTimestamp);
//                 Vector3 &base = keyframe1->mScaling;
//                 scaling = (base * (keyframe2->mScaling - base) * t);
//                 T3D_LOG_INFO("Keyframe #1 S(%f, %f, %f)", keyframe1->mScaling[0], keyframe1->mScaling[1], keyframe1->mScaling[2]);
//                 T3D_LOG_INFO("Keyframe #2 S(%f, %f, %f)", keyframe2->mScaling[0], keyframe2->mScaling[1], keyframe2->mScaling[2]);
//                 T3D_LOG_INFO("Bone : %s [%f], S(%f, %f, %f)", bone->getName().c_str(), t, scaling[0], scaling[1], scaling[2]);
// 
//                 bone->setScaling(scaling);
//             }
//         }

        auto itr = bone->getChildren().begin();
        while (itr != bone->getChildren().end())
        {
            updateBone(time, *itr);
            ++itr;
        }
    }

    void SGModel::updateVertices()
    {
        ModelDataPtr modelData = smart_pointer_cast<ModelData>(mModel->getModelData());

        if (modelData->mIsVertexShared)
        {
            // 共享顶点模式，只有一个mesh，多个submesh
            T3D_ASSERT(modelData->mMeshes.size() == 1);

            auto itr = mVertexDataList.begin();
            MeshDataPtr meshData = modelData->mMeshes[0];
            updateVertexData(meshData, *itr);
        }
        else
        {
            // 不共享顶点模式，有多个mesh和多个submesh
            size_t meshCount = modelData->mMeshes.size();
            size_t i = 0;

            auto itr = mVertexDataList.begin();

            for (i = 0; i < meshCount; ++i)
            {
                // 根据网格数据来逐个创建渲染用网格对象
                MeshDataPtr meshData = modelData->mMeshes[i];
                updateVertexData(meshData, *itr);
                ++itr;
            }
        }
    }

    void SGModel::updateVertexData(ObjectPtr data, VertexDataPtr vertexData)
    {
        MeshDataPtr meshData = smart_pointer_cast<MeshData>(data);
        auto itr = meshData->mBuffers.begin();
        size_t stream = 0;
        while (itr != meshData->mBuffers.end())
        {
            auto buffer = *itr;

            size_t step = buffer->mVertexSize;
            size_t vertexCount = buffer->mVertices.size() / buffer->mVertexSize;
            size_t i = 0;
            VertexBuffer::Vertices vertices(buffer->mVertices);
            VertexElement posElement;
            getVertexElement(buffer, VertexElement::E_VES_POSITION, posElement);
            VertexElement weightElement;
            getVertexElement(buffer, VertexElement::E_VES_BLENDWEIGHT, weightElement);
            VertexElement indicesElement;
            getVertexElement(buffer, VertexElement::E_VES_BLENDINDICES, indicesElement);

            for (i = 0; i < buffer->mVertices.size(); i += step)
            {
                updateVertex(buffer, &vertices[i], posElement, weightElement, indicesElement);
            }

            HardwareVertexBufferPtr vb = vertexData->getVertexBuffer(stream);
            bool ret = vb->writeData(0, buffer->mVertices.size(), &buffer->mVertices[0]);
            stream++;
            ++itr;
        }
    }

    void SGModel::updateVertex(ObjectPtr buffer, void *vertex, const VertexElement &posElem, const VertexElement &weightElem, const VertexElement &indicesElem)
    {
        VertexBufferPtr vb = smart_pointer_cast<VertexBuffer>(buffer);
        uint8_t *data = (uint8_t *)vertex;

        Vector3 *pos = (Vector3 *)(data + posElem.getOffset());
        float *weights = (float *)(data + weightElem.getOffset());
        uint16_t *indices = (uint16_t *)(data + indicesElem.getOffset());

        ModelDataPtr modelData = smart_pointer_cast<ModelData>(mModel->getModelData());
        size_t i = 0;

        BonePtr bone = smart_pointer_cast<Bone>(mBones[indices[0]]);
        const Matrix4 matOffset0 = bone->getOffsetMatrix();
        const Matrix4 matCombine0 = bone->getCombineTransform().getAffineMatrix();

        bone = smart_pointer_cast<Bone>(mBones[indices[1]]);
        const Matrix4 matOffset1 = bone->getOffsetMatrix();
        const Matrix4 matCombine1 = bone->getCombineTransform().getAffineMatrix();

        bone = smart_pointer_cast<Bone>(mBones[indices[2]]);
        const Matrix4 matOffset2 = bone->getOffsetMatrix();
        const Matrix4 matCombine2 = bone->getCombineTransform().getAffineMatrix();

        bone = smart_pointer_cast<Bone>(mBones[indices[3]]);
        const Matrix4 matOffset3 = bone->getOffsetMatrix();
        const Matrix4 matCombine3= bone->getCombineTransform().getAffineMatrix();

        *pos = (weights[0] > 0 ? (/*matCombine0 * */matOffset0 * (*pos) * weights[0]) : Vector3::ZERO) 
            + (weights[1] > 0 ? (/*matCombine1 * */matOffset1 * (*pos) * weights[1]) : Vector3::ZERO)
            + (weights[2] > 0 ? (/*matCombine2 * */matOffset2 * (*pos) * weights[2]) : Vector3::ZERO)
            + (weights[3] > 0 ? (/*matCombine3 * */matOffset3 * (*pos) * weights[3]) : Vector3::ZERO);
    }

    bool SGModel::getVertexElement(ObjectPtr buffer, VertexElement::Semantic semantic, VertexElement &element)
    {
        VertexBufferPtr vb = smart_pointer_cast<VertexBuffer>(buffer);
        auto itr = vb->mAttributes.begin();

        while (itr != vb->mAttributes.end())
        {
            auto vertexElement = *itr;

            if (semantic == vertexElement.getSemantic())
            {
                element = vertexElement;
                return true;
                break;
            }

            ++itr;
        }

        return false;
    }
}
