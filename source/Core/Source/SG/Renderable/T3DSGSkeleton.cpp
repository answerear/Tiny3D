

#include "SG/Renderable/T3DSGSkeleton.h"
#include "DataStruct/T3DBone.h"


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
        return nullptr;
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
