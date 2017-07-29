/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "SceneGraph/T3DSGTransformNode.h"


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

            while (parent != nullptr && (parent->getNodeType() != E_NT_TRANSFORM && parent->getNodeType() != E_NT_BONE))
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

    void SGTransformNode::setLocalMatrix(const Matrix4 &m)
    {
        m.decomposition(mPosition, mScale, mOrientation);
        setDirty(true, true);
    }
}
