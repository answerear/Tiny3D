

#include "mconv_ogreconverter.h"
#include "mconv_command.h"
#include "mconv_settings.h"
#include "mconv_ogreserializer.h"
#include "mconv_t3dSerializer.h"
#include "mconv_node.h"
#include "mconv_scene.h"
#include "mconv_mesh.h"
#include "mconv_model.h"
#include "mconv_material.h"
#include "mconv_animation.h"
#include "mconv_skeleton.h"
#include "mconv_bone.h"
#include "mconv_skin.h"
#include "mconv_texture.h"
#include "mconv_bound.h"
#include "mconv_vertexbuffer.h"
#include "mconv_log.h"
#include "mconv_transform.h"


namespace mconv
{
    OgreConverter::OgreConverter(const Settings &settings)
        : ConverterImpl(settings)
    {

    }

    OgreConverter::~OgreConverter()
    {

    }

    bool OgreConverter::importScene()
    {
        bool result = true;

        delete mImporter;
        mImporter = nullptr;

        if (mSettings.mSrcType & E_FILETYPE_OGRE)
        {
            mImporter = new OgreSerializer();
            result = (mImporter != nullptr);
        }
        else
        {
            MCONV_LOG_ERROR("Create importer failed ! Because of invalid source file format !");
            result = false;
        }

        result = result && mImporter->load(mSettings.mSrcPath, mSrcData);

        return result;
    }

    bool OgreConverter::exportScene()
    {
        bool result = false;

        delete mExporter;
        mExporter = nullptr;

        if ((mSettings.mDstType & E_FILETYPE_T3D) == E_FILETYPE_T3D)
        {
            mExporter = new T3DSerializer();
            result = (mExporter != nullptr);
        }
        else if (mSettings.mDstType & E_FILETYPE_T3B)
        {
            mExporter = new T3DBinSerializer();
            result = (mExporter != nullptr);
        }
        else if (mSettings.mDstType & E_FILETYPE_T3T)
        {
            mExporter = new T3DXMLSerializer();
            result = (mExporter != nullptr);
        }
        else
        {
            MCONV_LOG_ERROR("Create exporter failed ! Because of invalid destination file format !");
            T3D_ASSERT(0);
            result = false;
        }

        result = result && mExporter->save(mSettings.mDstPath, mDstData);

        return result;
    }

    bool OgreConverter::convertToT3D()
    {
        if (mSrcData == nullptr)
        {
            MCONV_LOG_ERROR("Convert to T3D failed ! Because of invalid source data !");
            return false;
        }

        bool result = true;

        OgreMesh *pOgreMesh = (OgreMesh *)mSrcData;

        String name = "Scene";

        Scene *pScene = new Scene(name);
        
        result = processOgreMesh(*pOgreMesh, pScene);

        mDstData = pScene;

        return result;
    }

    void OgreConverter::cleanup()
    {
        OgreMesh *pOgreMesh = (OgreMesh *)mSrcData;

        delete pOgreMesh;
        mSrcData = nullptr;
    }

    bool OgreConverter::processOgreMesh(const OgreMesh &mesh, Node *pRoot)
    {
        bool result = false;

        String name = "Model";

        Model *pModel = new Model(name);

        pRoot->addChild(pModel);
        result = processOgreSubMeshes(mesh, pModel);

        if (mesh.hasSkeleton)
        {
            result = result && processOgreSkeleton(mesh.skeleton, pModel);
        }

        return result;
    }

    bool OgreConverter::processOgreGeometry(const OgreGeometry &geometry, Mesh *pMesh)
    {
        bool result = false;

        VertexBuffers *pVBS = new VertexBuffers(pMesh->getID());
        pMesh->addChild(pVBS);

        result = (geometry.buffers.size() > 0);
        size_t i = 0;
        auto itr = geometry.buffers.begin();

        while (itr != geometry.buffers.end())
        {
            std::stringstream ss;
            ss<<itr->bindIndex;
            VertexBuffer *pVB = new VertexBuffer(ss.str());
            pVBS->addChild(pVB);

            result = result && processOgreVertexAttributes(geometry, pVB, itr->bindIndex);
            result = result && processOgreVertexBuffer(*itr, pVB, itr->bindIndex);
            ++itr;
        }

        return result;
    }

    bool OgreConverter::processOgreVertexAttributes(const OgreGeometry &geometry, VertexBuffer *pVertexBuffer, size_t source)
    {
        bool result = (geometry.elements.size() > 0);

        auto itr = geometry.elements.begin();

        while (itr != geometry.elements.end())
        {
            const OgreVertexElement &element = *itr;
            if (element.source == source)
            {
                result = result && putVertexAttribute(element, pVertexBuffer);
            }

            ++itr;
        }

        return result;
    }

    bool OgreConverter::putVertexAttribute(const OgreVertexElement &element, VertexBuffer *pVertexBuffer)
    {
        bool result;

        VertexAttribute attribute;
        result = processVertexSemantic(element.semantic, attribute);
        result = result && processVertexType(element.type, attribute);

        if (result)
        {
            pVertexBuffer->mAttributes.push_back(attribute);
        }

        return result;
    }

    bool OgreConverter::processVertexSemantic(uint16_t semantic, VertexAttribute &attribute)
    {
        bool result = true;

        switch (semantic)
        {
        case VES_POSITION:
            {
                attribute.mVertexType = VertexAttribute::E_VT_POSITION;
            }
            break;
        case VES_BLEND_WEIGHTS:
            {
                attribute.mVertexType = VertexAttribute::E_VT_BLEND_WEIGHT;
            }
            break;
        case VES_BLEND_INDICES:
            {
                attribute.mVertexType = VertexAttribute::E_VT_BLEND_INDEX;
            }
            break;
        case VES_NORMAL:
            {
                attribute.mVertexType = VertexAttribute::E_VT_NORMAL;
            }
            break;
        case VES_DIFFUSE:
            {
                attribute.mVertexType = VertexAttribute::E_VT_COLOR;
            }
            break;
        case VES_SPECULAR:
            {
                attribute.mVertexType = VertexAttribute::E_VT_COLOR;
            }
            break;
        case VES_TEXTURE_COORDINATES:
            {
                attribute.mVertexType = VertexAttribute::E_VT_TEXCOORD;
            }
            break;
        case VES_BINORMAL:
            {
                attribute.mVertexType = VertexAttribute::E_VT_BINORMAL;
            }
            break;
        case VES_TANGENT:
            {
                attribute.mVertexType = VertexAttribute::E_VT_TANGENT;
            }
            break;
        default:
            {
                result = false;
            }
            break;
        }

        return result;
    }

    bool OgreConverter::processVertexType(uint16_t type, VertexAttribute &attribute)
    {
        bool result = true;

        switch (type)
        {
        case VET_FLOAT1:
            {
                attribute.mDataType = VertexAttribute::E_VT_FLOAT;
                attribute.mSize = 1;
            }
            break;
        case VET_FLOAT2:
            {
                attribute.mDataType = VertexAttribute::E_VT_FLOAT;
                attribute.mSize = 2;
            }
            break;
        case VET_FLOAT3:
            {
                attribute.mDataType = VertexAttribute::E_VT_FLOAT;
                attribute.mSize = 3;
            }
            break;
        case VET_FLOAT4:
            {
                attribute.mDataType = VertexAttribute::E_VT_FLOAT;
                attribute.mSize = 4;
            }
            break;
        case VET_COLOR:
            {
                attribute.mDataType = VertexAttribute::E_VT_FLOAT;
                attribute.mSize = 4;
            }
            break;
        case VET_SHORT1:
            {
                attribute.mDataType = VertexAttribute::E_VT_INT16;
                attribute.mSize = 1;
            }
            break;
        case VET_SHORT2:
            {
                attribute.mDataType = VertexAttribute::E_VT_INT16;
                attribute.mSize = 2;
            }
            break;
        case VET_SHORT3:
            {
                attribute.mDataType = VertexAttribute::E_VT_INT16;
                attribute.mSize = 3;
            }
            break;
        case VET_SHORT4:
            {
                attribute.mDataType = VertexAttribute::E_VT_INT16;
                attribute.mSize = 4;
            }
            break;
        case VET_UBYTE4:
            {
                attribute.mDataType = VertexAttribute::E_VT_INT8;
                attribute.mSize = 4;
            }
            break;
        case VET_COLOR_ARGB:
            {
                attribute.mDataType = VertexAttribute::E_VT_FLOAT;
                attribute.mSize = 4;
            }
            break;
        case VET_COLOR_ABGR:
            {
                attribute.mDataType = VertexAttribute::E_VT_FLOAT;
                attribute.mSize = 4;
            }
            break;
        default:
            {
                result = false;
            }
            break;
        }

        return result;
    }

    bool OgreConverter::processOgreVertexBuffer(const OgreVertexBuffer &buffer, VertexBuffer *pVertexBuffer, size_t source)
    {
        bool result = (buffer.vertices.size() > 0);

        size_t i = 0;

        while (i < buffer.vertices.size())
        {
            pVertexBuffer->mVertices.push_back(Vertex());
            Vertex &vertex = pVertexBuffer->mVertices.back();
            result = result && putVertexData(buffer.vertices, i, pVertexBuffer->mAttributes, vertex);
        }

        return result;
    }

    bool OgreConverter::putVertexData(const std::vector<float> &vertices, size_t &index, const VertexAttributes &attributes, Vertex &vertex)
    {
        bool result = (attributes.size() > 0);

//         while (index < vertices.size())
        {
            auto itr = attributes.begin();

            while (itr != attributes.end())
            {
                const VertexAttribute &attr = *itr;

                switch (attr.mVertexType)
                {
                case VertexAttribute::E_VT_POSITION:
                    {
                        vertex.mPosition[0] = vertices[index++];
                        vertex.mPosition[1] = vertices[index++];
                        vertex.mPosition[2] = vertices[index++];
                    }
                    break;
                case VertexAttribute::E_VT_TEXCOORD:
                    {
                        vertex.mTexElements.push_back(Vector2());
                        Vector2 &texcoord = vertex.mTexElements.back();
                        texcoord[0] = vertices[index++];
                        texcoord[1] = vertices[index++];
                    }
                    break;
                case VertexAttribute::E_VT_NORMAL:
                    {
                        vertex.mNormalElements.push_back(Vector3());
                        Vector3 &normal = vertex.mNormalElements.back();
                        normal[0] = vertices[index++];
                        normal[1] = vertices[index++];
                        normal[2] = vertices[index++];
                    }
                    break;
                case VertexAttribute::E_VT_TANGENT:
                    {
                        vertex.mTangentElements.push_back(Vector3());
                        Vector3 &tangent = vertex.mTangentElements.back();
                        tangent[0] = vertices[index++];
                        tangent[1] = vertices[index++];
                        tangent[2] = vertices[index++];
                    }
                    break;
                case VertexAttribute::E_VT_BINORMAL:
                    {
                        vertex.mBinormalElements.push_back(Vector3());
                        Vector3 &binormal = vertex.mBinormalElements.back();
                        binormal[0] = vertices[index++];
                        binormal[1] = vertices[index++];
                        binormal[2] = vertices[index++];
                    }
                    break;
                case VertexAttribute::E_VT_COLOR:
                    {
                        uint32_t value = *(uint32_t *)&vertices[index++];
                        uint8_t b = value & 0xFF;
                        uint8_t g = (value >> 8) & 0xFF;
                        uint8_t r = (value >> 16) & 0xFF;
                        uint8_t a = (value >> 24) & 0xFF;
                        vertex.mColorElements.push_back(Vector4());
                        Vector4 &color = vertex.mColorElements.back();
                        color[0] = (float)b / 255.0f;
                        color[1] = (float)g / 255.0f;
                        color[2] = (float)r / 255.0f;
                        color[3] = (float)a / 255.0f;
                    }
                    break;
                case VertexAttribute::E_VT_BLEND_WEIGHT:
                    {
                        
                    }
                    break;
                case VertexAttribute::E_VT_BLEND_INDEX:
                    {
                    }
                    break;
                default:
                    {
                        result = false;
                    }
                    break;
                }

                ++itr;
            }
        }

        return result;
    }

    bool OgreConverter::createMesh(Model *pModel, Mesh *&pMesh, SubMeshes *&pSubMeshes, size_t index)
    {
        char szName[64];
        snprintf(szName, sizeof(szName)-1, "Mesh#% 3d", index);
        String strName = szName;
        pMesh = new Mesh(strName);
        bool result = (pMesh != nullptr);
        if (result)
        {
            pModel->addChild(pMesh);
        }

        pSubMeshes = new SubMeshes("SubMeshes");
        result = (pSubMeshes != nullptr);
        if (result)
        {
            pMesh->addChild(pSubMeshes);
        }

        return result;
    }

    bool OgreConverter::processOgreSubMeshes(const OgreMesh &mesh, Model *pModel)
    {
        bool result = (mesh.submeshes.size() > 0);

        Mesh *pGlobalMesh = nullptr;
        SubMeshes *pGlobalSubMeshes = nullptr;

        bool bHasSharedVertices = false;
        pModel->mMeshCount = 0;

        size_t i = 0;
        auto itr = mesh.submeshes.begin();

        while (itr != mesh.submeshes.end() && result)
        {
            auto submesh = *itr;

            if (submesh.hasSharedVertices)
            {
                pModel->mMeshCount++;

                if (0 == i)
                {
                    bHasSharedVertices = submesh.hasSharedVertices;

                    // 第一个submesh，用于存放共享的顶点数据
                    result = createMesh(pModel, pGlobalMesh, pGlobalSubMeshes, i);
                    result = result && processOgreGeometry(mesh.geometry, pGlobalMesh);
                }
                else
                {
                    T3D_ASSERT(bHasSharedVertices == submesh.hasSharedVertices);
                }

                result = result && processOgreSubMesh(submesh, pGlobalMesh, pGlobalSubMeshes);

                if (mesh.boneAssignments.size() > 0)
                {
                    result = result && processOgreBoneAssignment(mesh.boneAssignments, pGlobalMesh);
                }
            }
            else
            {
                // submesh自己独享顶点数据，需要创建一个mesh出来
                pModel->mMeshCount++;

                if (i == 0)
                {
                    bHasSharedVertices = submesh.hasSharedVertices;
                }
                else
                {
                    T3D_ASSERT(bHasSharedVertices == submesh.hasSharedVertices);
                }
                
                Mesh *pMesh = nullptr;
                SubMeshes *pSubMeshes = nullptr;

                result = createMesh(pModel, pMesh, pSubMeshes, i);
                result = result && processOgreGeometry(submesh.geometry, pMesh);
                result = result && processOgreSubMesh(submesh, pMesh, pSubMeshes);

                if (mesh.boneAssignments.size() > 0)
                {
                    result = result && processOgreBoneAssignment(mesh.boneAssignments, pMesh);
                }
            }

            ++i;
            ++itr;
        }

        pModel->mSharedVertex = (pModel->mMeshCount == 1);

        return result;
    }

    bool OgreConverter::processOgreSubMesh(const OgreSubMesh &submesh, Mesh *pMesh, SubMeshes *pSubMeshes)
    {
        bool result = (submesh.indices.size() > 0);

        SubMesh *pSubMesh = new SubMesh("SubMesh");
        pSubMeshes->addChild(pSubMesh);

        pSubMesh->mMaterialName = submesh.materialName;

        pSubMesh->mIndices.resize(submesh.indices.size());

        auto itr = pSubMesh->mIndices.begin();
        size_t i = 0;

        for (i = 0; i < submesh.indices.size() && itr != pSubMesh->mIndices.end(); ++i)
        {
            *itr = submesh.indices[i];
            ++itr;
        }

        if (submesh.boneAssignments.size() > 0)
        {
            result = result && processOgreBoneAssignment(submesh.boneAssignments, pMesh);
        }

        return result;
    }

    bool OgreConverter::searchVertexBuffer(Mesh *pMesh, VertexBuffer *&pVertexBuffer)
    {
        bool found = false;
        size_t i = 0;

        for (i = 0; i < pMesh->getChildrenCount(); ++i)
        {
            Node *pChild = pMesh->getChild(i);

            if (pChild->getNodeType() == Node::E_TYPE_VERTEX_BUFFERS)
            {
                size_t j = 0;

                for (j = 0; j < pChild->getChildrenCount(); ++j)
                {
                    Node *pNode = pChild->getChild(j);

                    if (pNode->getNodeType() == Node::E_TYPE_VERTEX_BUFFER)
                    {
                        // 查找跟position顶点数据放置的vertex buffer
                        VertexBuffer *pVB = (VertexBuffer *)pNode;
                        auto itr = pVB->mAttributes.begin();

                        while (itr != pVB->mAttributes.end())
                        {
                            auto attribute = *itr;

                            if (attribute.mVertexType == VertexAttribute::E_VT_POSITION)
                            {
                                pVertexBuffer = pVB;
                                found = true;
                                break;
                            }

                            ++itr;
                        }

                        if (pVertexBuffer != nullptr)
                            break;
                    }
                }
            }
        }

        return found;
    }

    bool OgreConverter::searchVertexAttribute(VertexBuffer *pVertexBuffer, VertexAttribute::VertexType type)
    {
        bool found = false;

        auto itr = pVertexBuffer->mAttributes.begin();

        while (itr != pVertexBuffer->mAttributes.end())
        {
            if (type == itr->mVertexType)
            {
                found = true;
                break;
            }

            ++itr;
        }

        return found;
    }

    void OgreConverter::putVertexBlendAndWeightAttributes(VertexBuffer *pVB)
    {
        VertexAttribute attribute;
        attribute.mVertexType = VertexAttribute::E_VT_BLEND_WEIGHT;
        attribute.mSize = 4;
        attribute.mDataType = VertexAttribute::E_VT_FLOAT;
        pVB->mAttributes.push_back(attribute);

        attribute.mVertexType = VertexAttribute::E_VT_BLEND_INDEX;
        attribute.mSize = 4;
        attribute.mDataType = VertexAttribute::E_VT_INT16;
        pVB->mAttributes.push_back(attribute);
    }

    bool OgreConverter::processOgreBoneAssignment(const std::vector<OgreBoneAssignment> &assignments, Mesh *pMesh)
    {
        bool result = false;

        VertexBuffer *pVB = nullptr;

        if (searchVertexBuffer(pMesh, pVB))
        {
            if (!searchVertexAttribute(pVB, VertexAttribute::E_VT_BLEND_INDEX))
            {
                // 没有这个顶点属性，先添加顶点属性
                putVertexBlendAndWeightAttributes(pVB);
            }

            size_t i = 0;

            for (i = 0; i < assignments.size(); ++i)
            {
                const OgreBoneAssignment &bone = assignments[i];
                Vertex &vertex = pVB->mVertices[bone.vertexID];
                BlendInfo blend;
                blend.mBlendIndex = bone.boneID;
                blend.mBlendWeight = bone.weight;
//                 BlendInfoValue value(bone.weight, blend);
//                 vertex.mBlendInfo.insert(value);
                vertex.mBlendInfo.push_back(blend);
                vertex.mBlendInfo.sort();
            }

            result = true;
        }

        return result;
    }

    bool OgreConverter::processOgreSkeleton(const OgreSkeleton &skeleton, Model *pModel)
    {
        bool result = processOgreBones(skeleton, pModel);
        result = result && processOgreSkin(skeleton, pModel);
        result = result && processOgreAnimations(skeleton, pModel);
        return result;
    }

    bool OgreConverter::processOgreSkin(const OgreSkeleton &skeleton, Model *pModel)
    {
        bool result = (skeleton.bones.size() > 0);

        if (result)
        {
            // 先查找skeleton
            Skeleton *pSkel = nullptr;
            size_t i = 0;
            result = false;

            for (i = 0; i < pModel->getChildrenCount(); ++i)
            {
                auto pChild = pModel->getChild(i);
                if (pChild->getNodeType() == Node::E_TYPE_SKELETON)
                {
                    pSkel = (Skeleton *)pChild;
                    result = true;
                    break;
                }
            }

            if (pSkel != nullptr)
            {
                for (i = 0; i < pSkel->getChildrenCount(); ++i)
                {
                    Bone *pBone = (Bone *)pSkel->getChild(i);
                    Matrix4 m;
                    processBone(pBone, m);
                }
            }
        }

        return result;
    }

    bool OgreConverter::processBone(Bone *pBone, const Matrix4 &m)
    {
        bool result = true;

        Matrix4 matWorld = m * pBone->mLocalTransform;
        pBone->mOffsetMatrix = matWorld.inverse();

        size_t i = 0;

        for (i = 0; i < pBone->getChildrenCount(); ++i)
        {
            Bone *pChildBone = (Bone *)pBone->getChild(i);
            processBone(pChildBone, matWorld);
        }

        return result;
    }

    bool OgreConverter::processOgreBones(const OgreSkeleton &skeleton, Model *pModel)
    {
        bool result = (skeleton.bones.size() > 0);

        if (result)
        {
            Skeleton *pSkel = new Skeleton("skeleton");
            pModel->addChild(pSkel);
            pSkel->mBoneCount = skeleton.bones.size();

            // 先分配所有骨骼结点对象
            mBones.resize(skeleton.bones.size(), nullptr);
            size_t i = 0;
            for (i = 0; i < mBones.size(); ++i)
            {
                mBones[i] = new Bone("unknown");
                mBones[i]->mBoneIndex = i;
            }

            // 构建树形骨骼关系和计算变换矩阵
            auto itr = skeleton.bones.begin();

            while (itr != skeleton.bones.end())
            {
                const OgreBone &bone = *itr;
                Bone *pBone = mBones[bone.handle];

                if (bone.parent == 0xFFFF)
                {
                    pBone->setID(bone.name);
                    pSkel->addChild(pBone);
                }
                else
                {
                    Bone *pParent = mBones[bone.parent];
                    pBone->setID(bone.name);
                    T3D_ASSERT(pBone->getParent() == nullptr);
                    pParent->addChild(pBone);
                }

                // 计算变换矩阵
                Matrix4 R(bone.orientation);
                Matrix4 T(bone.position);
                Matrix4 S;
                S.setScale(bone.scale);
                pBone->mLocalTransform = T * R * S;
                ++itr;
            }
        }

        return result;
    }

    bool OgreConverter::processOgreAnimations(const OgreSkeleton &skeleton, Model *pModel)
    {
        bool result = true;

        Animation *pAnimation = new Animation(pModel->getID());
        pModel->addChild(pAnimation);

        auto itr = skeleton.animations.begin();
        while (itr != skeleton.animations.end())
        {
            auto action = *itr;
            Action *pAction = new Action(action.name);
            pAnimation->addChild(pAction);
            pAction->mDuration = action.length;

            // 先把所有骨骼关键帧变换矩阵重置为本地绑定骨骼矩阵，稍后按照帧来逐帧计算当前帧的绝对变换
            size_t idx = 0;
            for (idx = 0; idx < mBones.size(); ++idx)
            {
                Bone *pBone = mBones[idx];
                pBone->mKeyframeMatrix = pBone->mLocalTransform;
            }

            int k = 0;
            auto i = action.keyframes.begin();
            while (i != action.keyframes.end())
            {
                auto keyframe = *i;
                Bone *pBone = mBones[keyframe.boneID];

                k = pAction->getKeyFramesSize(pBone->getID(), pAction->mTKeyframes);
                KeyframeT *pTFrame = new KeyframeT(k);

                const Vector3 &translation = keyframe.position;
                const Quaternion &rotation = keyframe.orientation;
                const Vector3 &scaling = keyframe.scale;

                Matrix4 T(translation);
                Matrix4 R(rotation);
                Matrix4 S(scaling[0], scaling[1], scaling[2], 1.0f);
                Matrix4 M = T * R * S;
                Matrix4 M1 = M * pBone->mLocalTransform;

                Vector3 pos, scale;
                Quaternion orientation;
                M1.decomposition(pos, scale, orientation);
//                 const Vector3 &pos = translation;
//                 const Vector3 &scale = scaling;
//                 const Quaternion &orientation = rotation;

                pTFrame->x = pos[0];
                pTFrame->y = pos[1];
                pTFrame->z = pos[2];
                pTFrame->mTimestamp = keyframe.time;
                pAction->addKeyframe(pTFrame, pBone->getID(), pAction->mTKeyframes);

                k = pAction->getKeyFramesSize(pBone->getID(), pAction->mRKeyframes);
                KeyframeR *pRFrame = new KeyframeR(k);
                pRFrame->x = orientation.x();
                pRFrame->y = orientation.y();
                pRFrame->z = orientation.z();
                pRFrame->w = orientation.w();
                pRFrame->mTimestamp = keyframe.time;
                pAction->addKeyframe(pRFrame, pBone->getID(), pAction->mRKeyframes);

                k = pAction->getKeyFramesSize(pBone->getID(), pAction->mSKeyframes);
                KeyframeS *pSFrame = new KeyframeS(k);
                pSFrame->x = scale[0];
                pSFrame->y = scale[1];
                pSFrame->z = scale[2];
                pSFrame->mTimestamp = keyframe.time;
                pAction->addKeyframe(pSFrame, pBone->getID(), pAction->mSKeyframes);

                ++i;
            }

            ++itr;
        }

        return result;
    }
}
