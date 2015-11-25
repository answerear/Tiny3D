

#include "SG/T3DSGTransformNode.h"


namespace Tiny3D
{
    SGTransformNode *SGTransformNode::create(uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGTransformNode *node = new SGTransformNode(unID);
        if (node != nullptr)
        {
            node->autorelease();
        }
        else
        {
            T3D_SAFE_DELETE(node);
        }

        return node;
    }

    SGTransformNode::SGTransformNode(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGNode(unID)
        , mPosition(Vector3::ZERO)
        , mOrientation(Quaternion::IDENTITY)
        , mScale(Real(1.0), Real(1.0), Real(1.0))
    {
        mWorldTransform.setTranslate(mPosition);
        mWorldTransform.setOrientation(mOrientation);
        mWorldTransform.setScale(mScale);
    }

    SGTransformNode::~SGTransformNode()
    {

    }

    void SGTransformNode::onAttachParent(SGNode *parent)
    {
        setDirty(true, true);
    }

    void SGTransformNode::onDetachParent(SGNode *parent)
    {

    }

    const Transform &SGTransformNode::getLocalToWorldTransform()
    {
        if (isDirty())
        {
            SGNode *parent = getParent();

            while (parent != nullptr && parent->getNodeType() != E_NT_TRANSFORM)
                parent = parent->getParent();

            if (parent != nullptr)
            {
                SGTransformNode *node = (SGTransformNode *)parent;
                const Transform &transform = node->getLocalToWorldTransform();
                Quaternion R = transform.getOrientation() * mOrientation;
                Vector3 S = transform.getScale() * mScale;
                Vector3 T = transform.getOrientation() * mPosition;
                T = T * transform.getScale();
                T = transform.getTranslate() + mPosition;
                mWorldTransform.setTranslate(T);
                mWorldTransform.setOrientation(R);
                mWorldTransform.setScale(S);
                mWorldTransform.update();
//                 Matrix4 xform;
//                 Matrix3 R;
//                 mOrientation.toRotationMatrix(R);
//                 Matrix3 S;
//                 S[0][0] = mScale.x();
//                 S[1][1] = mScale.y();
//                 S[2][2] = mScale.z();
//                 xform = R * S;
//                 xform.setTranslate(mPosition);
//                 mWorldTransform = mat * xform;
            }
            else
            {
                mWorldTransform.setTranslate(mPosition);
                mWorldTransform.setOrientation(mOrientation);
                mWorldTransform.setScale(mScale);
            }

            setDirty(false);
        }

        return mWorldTransform;
    }

//     Transform SGTransformNode::getWorldToLocalTransform()
//     {
//         return getLocalToWorldTransform().inverseAffine();
//     }

    void SGTransformNode::lookAt(const Vector3 &pos, const Vector3 &obj, const Vector3 &up)
    {
        Vector3 N = obj - pos;
        N.normalize();
        Vector3 V = up;
        Vector3 U = N.cross(V);
        U.normalize();
        V = U.cross(N);

        mPosition = pos;
        Matrix3 mat;
        mat.setColumn(0, U);
        mat.setColumn(1, V);
        mat.setColumn(2, -N);
        mOrientation.fromRotationMatrix(mat);

        mScale[0] = Real(1.0);
        mScale[1] = Real(1.0);
        mScale[2] = Real(1.0);

        setDirty(true, true);
    }

    void SGTransformNode::addChild(SGNode *node)
    {
        T3D_ASSERT(node->getNodeType() != E_NT_SUBMESH);
        SGNode::addChild(node);
    }

    SGNode *SGTransformNode::clone()
    {
        SGTransformNode *node = create();
        cloneProperties(node);
        return node;
    }

    void SGTransformNode::cloneProperties(SGNode *node)
    {
        SGNode::cloneProperties(node);

        SGTransformNode *src = (SGTransformNode *)node;
        mPosition = src->mPosition;
        mOrientation = src->mOrientation;
        mScale = src->mScale;
        mWorldTransform = src->mWorldTransform;
    }
}
