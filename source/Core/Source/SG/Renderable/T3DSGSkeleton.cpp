

#include "SG/Renderable/T3DSGSkeleton.h"
#include "DataStruct/T3DBone.h"
#include "DataStruct/T3DSkinData.h"
#include "Render/T3DHardwareBufferManager.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Resource/T3DModel.h"


namespace Tiny3D
{
    SGSkeletonPtr SGSkeleton::create(ModelPtr model, uint32_t uID /* = E_NID_AUTOMATIC */)
    {
        SGSkeletonPtr skeleton = new SGSkeleton();

        if (skeleton != nullptr && skeleton->init(model))
        {
            skeleton->release();
        }
        else
        {
            T3D_SAFE_RELEASE(skeleton);
        }

        return skeleton;
    }

    SGSkeleton::SGSkeleton(uint32_t uID /* = E_NID_AUTOMATIC */)
        : SGGeometry(uID)
        , mModel(nullptr)
    {

    }

    SGSkeleton::~SGSkeleton()
    {

    }

    bool SGSkeleton::init(ModelPtr model)
    {
        mModel = model;

        BonePtr skeleton = smart_pointer_cast<Bone>(mModel->getSkeletonData());

        std::vector<BoneVertex> vertices;
        bool ret = buildSkeletonVertices(skeleton, vertices);

        if (ret)
        {
            HardwareVertexBufferPtr vb = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(sizeof(BoneVertex), vertices.size(), HardwareVertexBuffer::E_HBU_DYNAMIC, false);
            ret = vb->writeData(0, sizeof(BoneVertex) * vertices.size(), &vertices[0]);

            VertexDeclarationPtr decl = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
            decl->addElement(VertexElement(0, 0, VertexElement::E_VET_FLOAT3, VertexElement::E_VES_POSITION));
            decl->addElement(VertexElement(0, 12, VertexElement::E_VET_COLOR, VertexElement::E_VES_DIFFUSE));

            mVertexData = VertexData::create(decl);
            mVertexData->addVertexBuffer(vb);
        }

        return ret;
    }

    bool SGSkeleton::buildSkeletonVertices(const ObjectPtr &skeleton, std::vector<BoneVertex> &vertices)
    {
        BonePtr bone = smart_pointer_cast<Bone>(skeleton);

        BonePtr parentBone = smart_pointer_cast<Bone>(bone->getParent());

        static const Vector3 boneVertices[] =
        {
            Vector3(0.0f, 0.0f, 0.0f),
            Vector3(-0.2f, 0.2f,-0.2f),

            Vector3(0.2f, 0.2f,-0.2f),
            Vector3(0.0f, 3.0f, 0.0f),

            Vector3(-0.2f, 0.2f,-0.2f),
            Vector3(-0.2f, 0.2f, 0.2f),

            Vector3(0.0f, 0.0f, 0.0f),
            Vector3(0.2f, 0.2f,-0.2f),

            Vector3(0.2f, 0.2f, 0.2f),
            Vector3(0.0f, 0.0f, 0.0f),

            Vector3(-0.2f, 0.2f, 0.2f),
            Vector3(0.0f, 3.0f, 0.0f),

            Vector3(0.2f, 0.2f, 0.2f),
            Vector3(-0.2f, 0.2f, 0.2f)
        };

        if (parentBone == nullptr)
        {
//             // ¸ù¹Ç÷À
//             Transform t0;
//             const Transform &t1 = bone->getCombineTransform();
// 
//             Matrix4 bindpose1(false);
//             SkinDataPtr skin;
//             if (searchSkinData(bone->getName(), (ObjectPtr &)skin))
//             {
//                 bindpose1 = skin->getBindPose().inverse();
//             }
// 
// //             Vector3 p0 = t0.getAffineMatrix() * Vector3::ZERO;
// //             Vector3 p1 = bindpose1 * t1.getAffineMatrix() * Vector3::ZERO;
//             const Vector3 &p0 = t0.getTranslate();
//             const Vector3 &p1 = t1.getTranslate();
// 
//             BoneVertex vertex;
//             vertex.position = p0;
//             vertex.color = Color4::WHITE;
//             vertices.push_back(vertex);
//             vertex.position = p1;
//             vertex.color = Color4::WHITE;
//             vertices.push_back(vertex);
// 
//             T3D_LOG_DEBUG("Bone from joint root (%.6f, %.6f, %.6f) to joint %s (%.6f, %.6f, %.6f)", 
//                 p0[0], p0[1], p0[2], bone->getName().c_str(), p1[0], p1[1], p1[2]);
        }
        else
        {
            const Transform &t0 = parentBone->getCombineTransform();
            const Transform &t1 = bone->getCombineTransform();

            bool ret = false;
            Matrix4 bindpose0(false);
            SkinDataPtr s0;
            if (searchSkinData(parentBone->getName(), (ObjectPtr &)s0))
            {
                bindpose0 = s0->getBindPose().inverse();
                ret = true;
            }

            Matrix4 bindpose1(false);
            SkinDataPtr s1;
            if (ret && searchSkinData(bone->getName(), (ObjectPtr &)s1))
            {
                bindpose1 = s1->getBindPose().inverse();
                ret = true;
            }
            else
            {
                ret = true;
            }

            if (ret)
            {
                const Vector3 &p0 = t0.getTranslate();
                const Vector3 &p1 = t1.getTranslate();
//                 Vector3 p0 = bindpose0 * t0.getAffineMatrix() * Vector3::ZERO;
//                 Vector3 p1 = bindpose1 * t1.getAffineMatrix() * Vector3::ZERO;

                BoneVertex vertex;
                vertex.position = p0;
                vertex.color = Color4::WHITE;
                vertices.push_back(vertex);
                vertex.position = p1;
                vertex.color = Color4::WHITE;
                vertices.push_back(vertex);

                T3D_LOG_DEBUG("Bone from joint %s (%.6f, %.6f, %.6f) to joint %s (%.6f, %.6f, %.6f)", 
                    parentBone->getName().c_str(), p0[0], p0[1], p0[2], bone->getName().c_str(), p1[0], p1[1], p1[2]);
            }
        }

        bool ret = true;

        const Children &children = bone->getChildren();
        auto itr = children.begin();
        while (itr != children.end())
        {
            BonePtr childBone = smart_pointer_cast<Bone>(*itr);
            ret = ret && buildSkeletonVertices(childBone, vertices);
            ++itr;
        }

        return true;
    }

    bool SGSkeleton::searchSkinData(const String &name, ObjectPtr &skin)
    {
        bool found = false;

        const Model::SkinMap &meshSkin = mModel->getSkinData();
        auto itrSkin = meshSkin.begin();

        while (itrSkin != meshSkin.end())
        {
            const Model::SkinDataList &skins = itrSkin->second;
            auto itr = skins.begin();

            while (itr != skins.end())
            {
                SkinDataPtr skinData = smart_pointer_cast<SkinData>(*itr);
                if (name == skinData->getName())
                {
                    skin = skinData;
                    found = true;
                    break;
                }

                ++itr;
            }

            ++itrSkin;
        }

        return found;
    }

    void SGSkeleton::updateVertices()
    {
        BonePtr skeleton = smart_pointer_cast<Bone>(mModel->getSkeletonData());
        std::vector<BoneVertex> vertices;
        bool ret = buildSkeletonVertices(skeleton, vertices);

        if (ret)
        {
            HardwareVertexBufferPtr vb = mVertexData->getVertexBuffer(0);
            ret = vb->writeData(0, sizeof(BoneVertex) * vertices.size(), &vertices[0]);
        }
    }

    SGNode::Type SGSkeleton::getNodeType() const
    {
        return E_NT_SKELETON;
    }

    NodePtr SGSkeleton::clone() const
    {
        SGSkeletonPtr skeleton = SGSkeleton::create(mModel);
        cloneProperties(skeleton);
        return skeleton;
    }

    void SGSkeleton::cloneProperties(const NodePtr &node) const
    {
        SGGeometry::cloneProperties(node);
    }

    MaterialPtr SGSkeleton::getMaterial() const
    {
        return nullptr;
    }

    Renderer::PrimitiveType SGSkeleton::getPrimitiveType() const
    {
        return Renderer::E_PT_LINE_LIST;
    }

    VertexDataPtr SGSkeleton::getVertexData() const
    {
        return mVertexData;
    }

    IndexDataPtr SGSkeleton::getIndexData() const
    {
        return nullptr;
    }

    bool SGSkeleton::isIndicesUsed() const
    {
        return false;
    }
}
