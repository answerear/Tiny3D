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


#include "Component/T3DTransform3D.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Transform3DPtr Transform3D::create()
    {
        return new Transform3D();
    }
    
    //--------------------------------------------------------------------------
    
    Transform3D::Transform3D()
    {
        mWorldTransform.setTranslation(mPosition);
        mWorldTransform.setOrientation(mOrientation);
        mWorldTransform.setScaling(mScaling);
    }
    
    //--------------------------------------------------------------------------

    Transform3D::~Transform3D()
    {

    }
    
    //--------------------------------------------------------------------------

    ComponentPtr Transform3D::clone() const
    {
        Transform3DPtr newObj = new Transform3D();
        if (T3D_FAILED(newObj->cloneProperties(this)))
        {
            newObj = nullptr;
        }
        return newObj;
    }

    //--------------------------------------------------------------------------

    TResult Transform3D::cloneProperties(const Component *const src)
    {
        TResult ret = T3D_OK;
        
        do
        {
            const Transform3D * const node = static_cast<const Transform3D* const>(src);
            if (node == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Invalid pointer !");
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            mPosition = node->mPosition;
            mOrientation = node->mOrientation;
            mScaling = node->mScaling;
            mWorldTransform = node->mWorldTransform;
            mIsDirty = node->mIsDirty;
        } while (false);

        if (ret == T3D_OK)
        {
            ret = TransformNode::cloneProperties(src);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void Transform3D::onDestroy()
    {
        TransformNode::onDestroy();
    }

    //--------------------------------------------------------------------------

    void Transform3D::setLocalMatrix(const Matrix4 &m)
    {
        m.decomposition(mPosition, mScaling, mOrientation);
        setDirty(true, true);
    }
    
    //--------------------------------------------------------------------------

    const Transform &Transform3D::getLocalToWorldTransform() const
    {
        if (isDirty())
        {
            TransformNode *node = getParent();

            while (node != nullptr)
            {
                node = node->getParent();
            }

            if (node != nullptr)
            {
                Transform3D *xform = static_cast<Transform3D *>(node);
                const Transform &transform = xform->getLocalToWorldTransform();
                mWorldTransform.applyTransform(transform, mPosition, mOrientation, mScaling);
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

    void Transform3D::setDirty(bool isDirty, bool recursive /* = false */)
    {
        if (mIsDirty != isDirty)
        {
            mIsDirty = isDirty;

            if (recursive)
            {
                TransformNode *child = getFirstChild();
                while (child != nullptr)
                {
                    Transform3D *node = static_cast<Transform3D*>(child);
                    node->setDirty(isDirty, recursive);
                    child = child->getNextSibling();
                }
            }
        }
    }
    
    //--------------------------------------------------------------------------

    void Transform3D::onAttachParent(TransformNodePtr parent)
    {
        TransformNode::onAttachParent(parent);

        setDirty(true, true);
    }
    
    //--------------------------------------------------------------------------

    void Transform3D::onDetachParent(TransformNodePtr parent)
    {
        TransformNode::onDetachParent(parent);
    }

    //--------------------------------------------------------------------------

    void Transform3D::update()
    {
        getLocalToWorldTransform();
    }

    //--------------------------------------------------------------------------
}
