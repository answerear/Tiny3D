

#include "SG/T3DSGTransformNode.h"


namespace Tiny3D
{
    SGTransformNode::SGTransformNode(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGNode(unID)
        , mPosition(Vector3::ZERO)
        , mOrientation(Quaternion::IDENTITY)
        , mScale(Real(1.0), Real(1.0), Real(1.0))
        , mWorldTransform(false)
        , mIsTransformDirty(true)
        , mIsVisible(true)
    {
        
    }

    SGTransformNode::~SGTransformNode()
    {

    }

    void SGTransformNode::onAttachParent(SGNode *parent)
    {
        mIsTransformDirty = true;
    }

    void SGTransformNode::onDetachParent(SGNode *parent)
    {

    }

    const Matrix4 &SGTransformNode::getLocalToWorldTransform() const
    {
        if (mIsTransformDirty)
        {
            SGNode *parent = mParent;

            while (parent != nullptr && parent->getNodeType() != E_NT_TRANSFORM)
                parent = parent->getParent();

            if (parent != nullptr)
            {
                SGTransformNode *node = (SGTransformNode *)parent;
                const Matrix4 &mat = node->getLocalToWorldTransform();
                Matrix4 xform;
                Matrix3 R;
                mOrientation.toRotationMatrix(R);
                Matrix3 S;
                S[0][0] = mScale.x();
                S[1][1] = mScale.y();
                S[2][2] = mScale.z();
                xform = R * S;
                xform.setTranslate(mPosition);
                mWorldTransform = mat * xform;

                mIsTransformDirty = false;
            }
        }

        return mWorldTransform;
    }

    Matrix4 SGTransformNode::getWorldToLocalTransform() const
    {
        return getLocalToWorldTransform().inverseAffine();
    }

    void SGTransformNode::lookAt(const Vector3 &pos, const Vector3 &obj, const Vector3 &up)
    {
        mPosition = pos;
    }
}
