

#include "SG/Renderable/T3DSGSkeleton.h"
#include "DataStruct/T3DBone.h"
#include "Render/T3DHardwareBufferManager.h"
#include "Render/T3DHardwareVertexBuffer.h"


namespace Tiny3D
{
    SGSkeletonPtr SGSkeleton::create(ObjectPtr skeletonData, uint32_t uID /* = E_NID_AUTOMATIC */)
    {
        SGSkeletonPtr skeleton = new SGSkeleton();
        if (skeleton != nullptr && skeleton->init(skeletonData))
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
        , mSkeleton(nullptr)
    {

    }

    SGSkeleton::~SGSkeleton()
    {

    }

    bool SGSkeleton::init(ObjectPtr skeletonData)
    {
        mSkeleton = skeletonData;

        std::vector<BoneVertex> vertices;
        bool ret = buildSkeletonVertices(mSkeleton, vertices);

        if (ret)
        {
            HardwareVertexBufferPtr vb = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(sizeof(BoneVertex), vertices.size(), HardwareVertexBuffer::E_HBU_DYNAMIC, false);
            ret = vb->writeData(0, sizeof(BoneVertex) * vertices.size(), &vertices[0]);

            VertexDeclarationPtr decl = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
            decl->addElement(VertexElement(0, VertexElement::E_VET_FLOAT3, VertexElement::E_VES_POSITION));
            decl->addElement(VertexElement(12, VertexElement::E_VET_COLOR, VertexElement::E_VES_DIFFUSE));

            mVertexData = VertexData::create(decl, vb);
        }

        return ret;
    }

    bool SGSkeleton::buildSkeletonVertices(const ObjectPtr &skeleton, std::vector<BoneVertex> &vertices)
    {
        BonePtr bone = smart_pointer_cast<Bone>(skeleton);

        BonePtr parentBone = smart_pointer_cast<Bone>(bone->getParent());

        if (parentBone == nullptr)
        {
            // ¸ù¹Ç÷À
            Transform t0;
            const Transform &t1 = bone->getCombineTransform();
            Vector3 p0 = t0.getAffineMatrix() * Vector3::ZERO;
            Vector3 p1 = t1.getAffineMatrix() * Vector3::ZERO;
//             const Vector3 &p0 = t0.getTranslate();
//             const Vector3 &p1 = t1.getTranslate();
// 
            BoneVertex vertex;
            vertex.position = p0;
            vertex.color = Color4::WHITE;
            vertices.push_back(vertex);
            vertex.position = p1;
            vertex.color = Color4::WHITE;
            vertices.push_back(vertex);
        }
        else
        {
            const Transform &t0 = parentBone->getCombineTransform();
            const Transform &t1 = bone->getCombineTransform();
//             const Vector3 &p0 = t0.getTranslate();
//             const Vector3 &p1 = t1.getTranslate();
            Vector3 p0 = t0.getAffineMatrix() * Vector3::ZERO;
            Vector3 p1 = t1.getAffineMatrix() * Vector3::ZERO;

            BoneVertex vertex;
            vertex.position = p0;
            vertex.color = Color4::WHITE;
            vertices.push_back(vertex);
            vertex.position = p1;
            vertex.color = Color4::WHITE;
            vertices.push_back(vertex);
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

    SGNode::Type SGSkeleton::getNodeType() const
    {
        return E_NT_SKELETON;
    }

    NodePtr SGSkeleton::clone() const
    {
        SGSkeletonPtr skeleton = SGSkeleton::create(mSkeleton);
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
