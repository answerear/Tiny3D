/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#include "SceneGraph/T3DSGTransform3D.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SGTransform3DPtr SGTransform3D::create(ID uID /* = E_NID_AUTOMATIC */)
    {
        SGTransform3DPtr node = new SGTransform3D(uID);
        node->release();
        return node;
    }

    //--------------------------------------------------------------------------

    SGTransform3D::SGTransform3D(ID uID /* = E_NID_AUTOMATIC */)
        : SGNode(uID)
        , mPosition(Vector3::ZERO)
        , mOrientation(Quaternion::IDENTITY)
        , mScaling(Vector3::UNIT_SCALE)
        , mIsDirty(false)
    {
        mWorldTransform.setTranslation(mPosition);
        mWorldTransform.setOrientation(mOrientation);
        mWorldTransform.setScaling(mScaling);
    }

    //--------------------------------------------------------------------------

    SGTransform3D::~SGTransform3D()
    {

    }

    //--------------------------------------------------------------------------

    Node::Type SGTransform3D::getNodeType() const
    {
        return E_NT_TRANSFORM3D;
    }

    //--------------------------------------------------------------------------

    void SGTransform3D::setLocalMatrix(const Matrix4 &m)
    {
        m.decomposition(mPosition, mScaling, mOrientation);
        setDirty(true, true);
    }

    //--------------------------------------------------------------------------

    const Transform &SGTransform3D::getLocalToWorldTransform() const
    {
        if (isDirty())
        {
            NodePtr parent = getParent();

            while (parent != nullptr)
            {
                parent = parent->getParent();
            }

            if (parent != nullptr)
            {
                SGTransform3DPtr node 
                    = smart_pointer_cast<SGTransform3D>(parent);
                const Transform &transform = node->getLocalToWorldTransform();
                mWorldTransform.applyTransform(transform, mPosition, 
                    mOrientation, mScaling);
            }
            else
            {
                mWorldTransform.setTranslation(mPosition);
                mWorldTransform.setOrientation(mOrientation);
                mWorldTransform.setScaling(mScaling);
                mWorldTransform.update();
            }

            mIsDirty = false;
        }

        return mWorldTransform;
    }

    //--------------------------------------------------------------------------

    void SGTransform3D::setDirty(bool isDirty, bool recursive /* = false */)
    {
        mIsDirty = isDirty;

        if (recursive)
        {
            NodePtr child = getFirstChild();
            while (child != nullptr)
            {
                SGTransform3DPtr node 
                    = smart_pointer_cast<SGTransform3D>(child);
                node->setDirty(isDirty, recursive);
                child = child->getNextSibling();
            }
        }
    }

    //--------------------------------------------------------------------------

    NodePtr SGTransform3D::clone() const
    {
        SGTransform3DPtr node = create();
        if (cloneProperties(node) != T3D_OK)
        {
            node = nullptr;
        }
        return node;
    }
    //--------------------------------------------------------------------------

    void SGTransform3D::onAttachParent(NodePtr parent)
    {
        SGNode::onAttachParent(parent);

        setDirty(true, true);
    }

    //--------------------------------------------------------------------------

    void SGTransform3D::onDetachParent(NodePtr parent)
    {
        SGNode::onDetachParent(parent);
    }

    //--------------------------------------------------------------------------

    void SGTransform3D::updateTransform()
    {
        getLocalToWorldTransform();
        SGNode::updateTransform();
    }

    //--------------------------------------------------------------------------

    TResult SGTransform3D::cloneProperties(NodePtr node) const
    {
        TResult ret = SGNode::cloneProperties(node);

        if (ret == T3D_OK)
        {
            SGTransform3DPtr newNode = smart_pointer_cast<SGTransform3D>(node);
            newNode->mPosition = mPosition;
            newNode->mOrientation = mOrientation;
            newNode->mScaling = mScaling;
            newNode->mWorldTransform = mWorldTransform;
            newNode->mIsDirty = mIsDirty;
        }

        return ret;
    }
}
