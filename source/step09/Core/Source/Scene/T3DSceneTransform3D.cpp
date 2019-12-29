/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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


#include "Scene/T3DSceneTransform3D.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SceneTransform3DPtr SceneTransform3D::create(ID uID /* = E_NID_AUTOMATIC */)
    {
        SceneTransform3DPtr node = new SceneTransform3D(uID);
        node->release();
        return node;
    }

    //--------------------------------------------------------------------------

    SceneTransform3D::SceneTransform3D(ID uID /* = E_NID_AUTOMATIC */)
        : SceneNode(uID)
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

    SceneTransform3D::~SceneTransform3D()
    {

    }

    //--------------------------------------------------------------------------

    Node::Type SceneTransform3D::getNodeType() const
    {
        return Type::TRANSFORM3D;
    }

    //--------------------------------------------------------------------------

    void SceneTransform3D::setLocalMatrix(const Matrix4 &m)
    {
        m.decomposition(mPosition, mScaling, mOrientation);
        setDirty(true, true);
    }

    //--------------------------------------------------------------------------

    const Transform &SceneTransform3D::getLocalToWorldTransform() const
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
                SceneTransform3DPtr node 
                    = smart_pointer_cast<SceneTransform3D>(parent);
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

    void SceneTransform3D::setDirty(bool isDirty, bool recursive /* = false */)
    {
        mIsDirty = isDirty;

        if (recursive)
        {
            NodePtr child = getFirstChild();
            while (child != nullptr)
            {
                SceneTransform3DPtr node 
                    = smart_pointer_cast<SceneTransform3D>(child);
                node->setDirty(isDirty, recursive);
                child = child->getNextSibling();
            }
        }
    }

    //--------------------------------------------------------------------------

    NodePtr SceneTransform3D::clone() const
    {
        SceneTransform3DPtr node = create();
        if (cloneProperties(node) != T3D_OK)
        {
            node = nullptr;
        }
        return node;
    }
    //--------------------------------------------------------------------------

    void SceneTransform3D::onAttachParent(NodePtr parent)
    {
        SceneNode::onAttachParent(parent);

        setDirty(true, true);
    }

    //--------------------------------------------------------------------------

    void SceneTransform3D::onDetachParent(NodePtr parent)
    {
        SceneNode::onDetachParent(parent);
    }

    //--------------------------------------------------------------------------

    void SceneTransform3D::updateTransform()
    {
        getLocalToWorldTransform();
        SceneNode::updateTransform();
    }

    //--------------------------------------------------------------------------

    TResult SceneTransform3D::cloneProperties(NodePtr node) const
    {
        TResult ret = SceneNode::cloneProperties(node);

        if (ret == T3D_OK)
        {
            SceneTransform3DPtr newNode = smart_pointer_cast<SceneTransform3D>(node);
            newNode->mPosition = mPosition;
            newNode->mOrientation = mOrientation;
            newNode->mScaling = mScaling;
            newNode->mWorldTransform = mWorldTransform;
            newNode->mIsDirty = mIsDirty;
        }

        return ret;
    }
}
