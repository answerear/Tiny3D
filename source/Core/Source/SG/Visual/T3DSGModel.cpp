

#include "SG/Visual/T3DSGModel.h"
#include "SG/Renderable/T3DSGMesh.h"
#include "SG/Renderable/T3DSGSkeleton.h"
#include "Resource/T3DModel.h"
#include "Resource/T3DModelManager.h"
#include "DataStruct/T3DModelData.h"
#include "SG/Node/T3DSGBone.h"
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
        : SGVisual(unID)
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
                mMeshes.resize(submeshCount);

                auto itr = meshData->mSubMeshes.begin();
                for (i = 0; i < submeshCount; ++i)
                {
                    SubMeshDataPtr submeshData = *itr;
                    SGMeshPtr mesh = SGMesh::create(vertexData, submeshData);
                    mesh->setName(meshData->mName);
                    mMeshes[i] = mesh;
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

                    auto itr = meshData->mSubMeshes.begin();
                    for (j = 0; j < submeshCount; ++j)
                    {
                        SubMeshDataPtr submeshData = *itr;
                        SGMeshPtr mesh = SGMesh::create(vertexData, meshData);
                        mesh->setName(meshData->mName);
                        mMeshes.push_back(mesh);
                        ++itr;
                    }
                }
            }

            if (modelData->mNodes.size() > 0)
            {
                // 有变换结点，则创建所有结点
                ret = ret && createNodes();
            }
            else
            {
                // 没有变换结点，直接把所有mesh挂到model下
                auto itr = mMeshes.begin();
                while (itr != mMeshes.end())
                {
                    auto mesh = *itr;
                    addChild(mesh);
                    ++itr;
                }
            }

            if (modelData->mBones.size() > 0)
            {
                // 创建骨骼层次和骨骼偏移矩阵数组
                ret = createSkeletons();

                // 更新蒙皮
                updateSkins();
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
            SGVisual::cloneProperties(model);
        }
        
        return model;
    }

    void SGModel::updateTransform()
    {
        if (isActionRunning())
        {
            // 根据动作数据，更新骨骼姿势
            updatePoses();

            // 根据更新后的姿势数据，更新所有骨骼变换生成骨骼最终变换
            updateSkeletons();

            // 根据骨骼最终变换更新蒙皮数据
            updateSkins();
        }

        // 更新所有子结点变换
        SGVisual::updateTransform();
    }

    void SGModel::enableRenderingEntities()
    {
        removeAllChildren(false);

        if (mNodes.size() > 0)
        {
            auto itr = mNodes.begin();
            while (itr != mNodes.end())
            {
                auto node = *itr;

                if (node->getParent() == nullptr)
                {
                    addChild(node);
                    break;
                }

                ++itr;
            }
        }
        else
        {
            auto itr = mMeshes.begin();
            while (itr != mMeshes.end())
            {
                auto mesh = *itr;
                addChild(mesh);
                ++itr;
            }
        }
    }

    void SGModel::enableRenderingSkeletons()
    {
        removeAllChildren(false);

        if (mSkeleton == nullptr)
        {
            // 没有生成过骨骼渲染对象，先创建
            mSkeleton = SGSkeleton::create(mRootBone);
        }

        addChild(mSkeleton);
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
                    enableRenderingEntities();
                }
                break;
            case E_RENDER_SKELETON:
                {
                    enableRenderingSkeletons();
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
            SGBonePtr bone = SGBone::create(boneData);
            mBones[i] = bone;
        }

        for (i = 0; i < mBones.size(); ++i)
        {
            auto bone = smart_pointer_cast<SGBone>(mBones[i]);
            auto boneData = modelData->mBones[i];

            Vector3 pos;
            Quaternion orientation;
            Vector3 scale;
            boneData->mLocalMatrix.decomposition(pos, scale, orientation);
            bone->setPosition(pos);
            bone->setScale(scale);
            bone->setOrientation(orientation);

            if (boneData->mParent == 0xFFFF)
            {
                mRootBone = bone;
            }
            else
            {
                T3D_ASSERT(boneData->mParent < mBones.size());
                auto parentBone = smart_pointer_cast<SGBone>(mBones[boneData->mParent]);
                parentBone->addChild(bone);
            }
        }

        return true;
    }

    bool SGModel::createNodes()
    {
        ModelDataPtr modelData = smart_pointer_cast<ModelData>(mModel->getModelData());
        
        mNodes.resize(modelData->mNodes.size());

        // 创建所有结点
        size_t i = 0;
        for (i = 0; i < modelData->mNodes.size(); ++i)
        {
            SGTransformNodePtr node = SGTransformNode::create();
            mNodes[i] = node;

            NodeDataPtr nodeData = modelData->mNodes[i];
            Vector3 pos;
            Vector3 scale;
            Quaternion orientation;
            nodeData->mLocalMatrix.decomposition(pos, scale, orientation);
            node->setPosition(pos);
            node->setScale(scale);
            node->setOrientation(orientation);
        }

        // 构建结点的树形结构
        for (i = 0; i < modelData->mNodes.size(); ++i)
        {
            auto nodeData = modelData->mNodes[i];
            auto node = mNodes[i];
            node->setName(nodeData->mName);

            if (nodeData->mParent == 0xFFFF)
            {
                addChild(node);
            }
            else
            {
                T3D_ASSERT(nodeData->mParent < modelData->mNodes.size());
                mNodes[nodeData->mParent]->addChild(node);
            }

            SGMeshPtr mesh;
            if (nodeData->mHasLink && searchMesh(nodeData->mLinkMesh, nodeData->mLinkSubMesh, mesh))
            {
                node->addChild(mesh);
            }
        }

        return true;
    }

    bool SGModel::searchMesh(const String &meshName, const String &submeshName, SGMeshPtr &mesh)
    {
        bool found = false;
        size_t i = 0;

        for (i = 0; i < mMeshes.size(); ++i)
        {
            auto target = mMeshes[i];
            if (target->getName() == meshName && target->getSubMeshName() == submeshName)
            {
                found = true;
                mesh = target;
                break;
            }
        }

        return found;
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

    void SGModel::updatePoses()
    {
        int64_t current = DateTime::currentMSecsSinceEpoch();
        int64_t time = current - mStartTime;
        ActionDataPtr actionData = smart_pointer_cast<ActionData>(mCurActionData);
        int64_t dt = time % actionData->mDuration;
        updatePose(dt, mRootBone);
    }

    void SGModel::updatePose(int64_t time, ObjectPtr skeleton)
    {
        SGBonePtr bone = smart_pointer_cast<SGBone>(skeleton);

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
//                 T3D_LOG_INFO("Keyframe #1 T(%f, %f, %f)", keyframe1->mTranslation[0], keyframe1->mTranslation[1], keyframe1->mTranslation[2]);
//                 T3D_LOG_INFO("Keyframe #2 T(%f, %f, %f)", keyframe2->mTranslation[0], keyframe2->mTranslation[1], keyframe2->mTranslation[2]);
//                 T3D_LOG_INFO("Bone : %s [%f], T(%f, %f, %f)", bone->getName().c_str(), t, translation[0], translation[1], translation[2]);

                bone->setPosition(translation);
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
//                 orientation.slerp(keyframe1->mOrientation, keyframe2->mOrientation, t, true);
//                 T3D_LOG_INFO("Keyframe #1 R(%f, %f, %f, %f)", keyframe1->mOrientation[0], keyframe1->mOrientation[1], keyframe1->mOrientation[2], keyframe1->mOrientation[3]);
//                 T3D_LOG_INFO("Keyframe #2 R(%f, %f, %f, %f)", keyframe2->mOrientation[0], keyframe2->mOrientation[1], keyframe2->mOrientation[2], keyframe2->mOrientation[3]);
                Degree deg;
                Vector3 axis;
                orientation.toAngleAxis(deg, axis);
//                 T3D_LOG_INFO("Bone : %s [%lld], keyframe=%d, kf1t=%lld, kf2t=%lld, R(%f, %f, %f, %f), deg=%f, Axis(%f, %f, %f)", 
//                     bone->getName().c_str(), time, frame, keyframe1->mTimestamp, keyframe2->mTimestamp,
//                     orientation[0], orientation[1], orientation[2], orientation[3], deg.valueDegrees(), axis[0], axis[1], axis[2]);
                bone->setOrientation(orientation);
            }
            else
            {
                KeyFrameDataRPtr keyframe = smart_pointer_cast<KeyFrameDataR>(keyframesR.back());
                Degree deg;
                Vector3 axis;
                orientation = keyframe->mOrientation;
                orientation.toAngleAxis(deg, axis);
//                 T3D_LOG_INFO("Bone : %s [%lld], keyframe=%d, R(%f, %f, %f, %f), deg=%f, Axis(%f, %f, %f)", bone->getName().c_str(), time, mCurKeyFrameR,
//                     orientation[0], orientation[1], orientation[2], orientation[3], deg.valueDegrees(), axis[0], axis[1], axis[2]);
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

                bone->setScale(scaling);
            }
        }

        auto itr = bone->getChildren().begin();
        while (itr != bone->getChildren().end())
        {
            updatePose(time, *itr);
            ++itr;
        }
    }

    void SGModel::updateSkeletons()
    {
        mRootBone->updateTransform();
    }

    void SGModel::updateSkins()
    {
        ModelDataPtr modelData = smart_pointer_cast<ModelData>(mModel->getModelData());

        if (modelData->mIsVertexShared)
        {
            // 共享顶点模式，只有一个mesh，多个submesh
            T3D_ASSERT(modelData->mMeshes.size() == 1);

            auto itr = mVertexDataList.begin();
            MeshDataPtr meshData = modelData->mMeshes[0];
            updateSkinData(meshData, *itr);
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
                updateSkinData(meshData, *itr);
                ++itr;
            }
        }
    }

    void SGModel::updateSkinData(ObjectPtr data, VertexDataPtr vertexData)
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
            bool result = getVertexElement(buffer, VertexElement::E_VES_POSITION, posElement);
            VertexElement weightElement;
            result = result && getVertexElement(buffer, VertexElement::E_VES_BLENDWEIGHT, weightElement);
            VertexElement indicesElement;
            result = result && getVertexElement(buffer, VertexElement::E_VES_BLENDINDICES, indicesElement);

            if (result)
            {
                for (i = 0; i < buffer->mVertices.size(); i += step)
                {
                    updateSkinVertex(buffer, &vertices[i], posElement, weightElement, indicesElement);
                }

                HardwareVertexBufferPtr vb = vertexData->getVertexBuffer(stream);
                result = result && vb->writeData(0, buffer->mVertices.size(), &vertices[0]);
            }

            stream++;
            ++itr;
        }
    }

    void SGModel::updateSkinVertex(ObjectPtr buffer, void *vertex, const VertexElement &posElem, const VertexElement &weightElem, const VertexElement &indicesElem)
    {
        VertexBufferPtr vb = smart_pointer_cast<VertexBuffer>(buffer);
        uint8_t *data = (uint8_t *)vertex;

        Vector3 *pos = (Vector3 *)(data + posElem.getOffset());
        float *weights = (float *)(data + weightElem.getOffset());
        uint16_t *indices = (uint16_t *)(data + indicesElem.getOffset());

        ModelDataPtr modelData = smart_pointer_cast<ModelData>(mModel->getModelData());
        size_t i = 0;

        SGBonePtr bone;
        BoneDataPtr boneData;

        bone = smart_pointer_cast<SGBone>(mBones[indices[0]]);
        boneData = bone->getBoneData();
        const Matrix4 &matVertex0 = bone->getFinalMatrix();

        bone = smart_pointer_cast<SGBone>(mBones[indices[1]]);
        boneData = bone->getBoneData();
        const Matrix4 &matVertex1 = bone->getFinalMatrix();

        bone = smart_pointer_cast<SGBone>(mBones[indices[2]]);
        boneData = bone->getBoneData();
        const Matrix4 &matVertex2 = bone->getFinalMatrix();

        bone = smart_pointer_cast<SGBone>(mBones[indices[3]]);
        boneData = bone->getBoneData();
        const Matrix4 &matVertex3 = bone->getFinalMatrix();

        *pos = (weights[0] > 0 ? (matVertex0 * (*pos) * weights[0]) : Vector3::ZERO)
            + (weights[1] > 0 ? (matVertex1 * (*pos) * weights[1]) : Vector3::ZERO)
            + (weights[2] > 0 ? (matVertex2 * (*pos) * weights[2]) : Vector3::ZERO)
            + (weights[3] > 0 ? (matVertex3 * (*pos) * weights[3]) : Vector3::ZERO);
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
