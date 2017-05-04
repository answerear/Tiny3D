

#include "SG/Node/T3DSGTransformNode.h"


namespace Tiny3D
{
    SGTransformNodePtr SGTransformNode::create(uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGTransformNode *node = new SGTransformNode(unID);
        SGTransformNodePtr ptr(node);
        node->release();
        return ptr;
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

    Node::Type SGTransformNode::getNodeType() const
    {
        return E_NT_TRANSFORM;
    }

    void SGTransformNode::onAttachParent(const NodePtr &parent)
    {
        SGNode::onAttachParent(parent);
        setDirty(true, true);
    }

    void SGTransformNode::onDetachParent(const NodePtr &parent)
    {
        SGNode::onDetachParent(parent);
    }

    void SGTransformNode::updateTransform()
    {
        getLocalToWorldTransform();
        SGNode::updateTransform();
    }

    const Transform &SGTransformNode::getLocalToWorldTransform()
    {
        if (isDirty())
        {
            Node *parent = getParent();

            while (parent != nullptr && parent->getNodeType() != E_NT_TRANSFORM)
                parent = parent->getParent();

            if (parent != nullptr)
            {
                SGTransformNode *node = (SGTransformNode *)parent;
                const Transform &transform = node->getLocalToWorldTransform();
                mWorldTransform.applyTransform(transform, mPosition, mOrientation, mScale);
            }
            else
            {
                mWorldTransform.setTranslate(mPosition);
                mWorldTransform.setOrientation(mOrientation);
                mWorldTransform.setScale(mScale);
                mWorldTransform.update();
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
        V.normalize();
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

    void SGTransformNode::addChild(const NodePtr &node)
    {
        SGNode::addChild(node);
    }

    NodePtr SGTransformNode::clone() const
    {
        SGTransformNodePtr node = create();
        cloneProperties(node);
        return node;
    }

    void SGTransformNode::cloneProperties(const NodePtr &node) const
    {
        SGNode::cloneProperties(node);

        const SGTransformNodePtr &newNode = smart_pointer_cast<SGTransformNode>(node);
        newNode->mPosition = mPosition;
        newNode->mOrientation = mOrientation;
        newNode->mScale = mScale;
        newNode->mWorldTransform = mWorldTransform;
    }
}
