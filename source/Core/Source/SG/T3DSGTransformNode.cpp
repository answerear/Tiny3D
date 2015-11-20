

#include "SG/T3DSGTransformNode.h"


namespace Tiny3D
{
    SGTransformNode::SGTransformNode(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGNode(unID)
        , mPosition(Vector3::ZERO)
        , mOrientation(Quaternion::IDENTITY)
        , mScale(Real(1.0), Real(1.0), Real(1.0))
        , mWorldTransform(Matrix4::IDENTITY)
        , mIsVisible(true)
        , mIsDirty(true)
    {
        mType = E_NT_TRANSFORM;
    }

    SGTransformNode::~SGTransformNode()
    {

    }

    void SGTransformNode::onAttachParent(SGNode *parent)
    {
        SGNode::onAttachParent(parent);

        mIsDirty = true;
    }

    Bound *SGTransformNode::getBoundingBox()
    {
        return nullptr;
    }

    const Matrix4 &SGTransformNode::getLocalToWorldTransform() const
    {
        if (mIsDirty)
        {
            SGNode *parent = mParent;
            
        }

        return mWorldTransform;
    }
}
