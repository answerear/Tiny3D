/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "Component/T3DTransform3D.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Transform3DPtr Transform3D::create()
    {
        return new Transform3D(UUID::generate());
    }
    
    //--------------------------------------------------------------------------
    
    Transform3D::Transform3D(const UUID &uuid)
        : TransformNode(uuid)
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
                for (auto itr = child_begin(); itr != child_end(); ++itr)
                {
                    Transform3D *node = static_cast<Transform3D*>(itr->get());
                    node->setDirty(isDirty, recursive);
                }
            }
        }
    }
    
    //--------------------------------------------------------------------------

    void Transform3D::onAttachParent(TransformNode *parent)
    {
        TransformNode::onAttachParent(parent);

        setDirty(true, true);
    }
    
    //--------------------------------------------------------------------------

    void Transform3D::onDetachParent(TransformNode *parent)
    {
        TransformNode::onDetachParent(parent);
    }

    //--------------------------------------------------------------------------

    void Transform3D::onUpdate()
    {
        getLocalToWorldTransform();
    }

    //--------------------------------------------------------------------------

    void Transform3D::addPositionChangedCallback(Component *component, const PositionChangedCallback &callback)
    {
        mPositionCallbacks.emplace(component, callback);
    }

    //--------------------------------------------------------------------------

    void Transform3D::removePositionChangedCallback(Component *component)
    {
        mPositionCallbacks.erase(component);
    }

    //--------------------------------------------------------------------------

    void Transform3D::addOrientationChangedCallback(Component *component, const OrientationChangedCallback &callback)
    {
        mOrientationCallbacks.emplace(component, callback);
    }

    //--------------------------------------------------------------------------

    void Transform3D::removeOrientationChangedCallback(Component *component)
    {
        mOrientationCallbacks.erase(component);
    }

    //--------------------------------------------------------------------------

    void Transform3D::addScalingChangedCallback(Component *component, const ScalingChangedCallback &callback)
    {
        mScalingCallbacks.emplace(component, callback);
    }

    //--------------------------------------------------------------------------

    void Transform3D::removeScalingChangedCallback(Component *component)
    {
        mScalingCallbacks.erase(component);
    }

    //--------------------------------------------------------------------------

    void Transform3D::onPositionChanged(const Vector3 &oldPos, const Vector3 &newPos)
    {
        for (const auto &val : mPositionCallbacks)
        {
            val.second(oldPos, newPos);
        }
    }

    //--------------------------------------------------------------------------

    void Transform3D::onOrientationChanged(const Quaternion &oldOrient, const Quaternion &newOrient)
    {
        for (const auto &val : mOrientationCallbacks)
        {
            val.second(oldOrient, newOrient);
        }
    }

    //--------------------------------------------------------------------------

    void Transform3D::onScalingChanged(const Vector3 &oldScaling, const Vector3 &newScaling)
    {
        for (const auto &val : mScalingCallbacks)
        {
            val.second(oldScaling, newScaling);
        }
    }

    //--------------------------------------------------------------------------

    void Transform3D::debugInfo(int32_t tab)
    {
        std::stringstream ss;
        for (int32_t i = 0; i < tab; i++)
        {
            ss << "\t";
        }

        const Transform &worldXform = getLocalToWorldTransform();
        
        T3D_LOG_INFO(LOG_TAG_COMPONENT, "%sTransform 3D - %s, "
            "Local : T (%f, %f, %f), R (%f, %f, %f, %f), S (%f, %f, %f)"
            "    World : T (%f, %f, %f), R (%f, %f, %f, %f), S (%f, %f, %f)",
            ss.str().c_str(), getGameObject()->getName().c_str(),
            mPosition.x(), mPosition.y(), mPosition.z(),
            mOrientation.x(), mOrientation.y(), mOrientation.z(), mOrientation.w(),
            mScaling.x(), mScaling.y(), mScaling.z(),
            worldXform.getTranslation().x(), worldXform.getTranslation().y(), worldXform.getTranslation().z(),
            worldXform.getOrientation().x(), worldXform.getOrientation().y(), worldXform.getOrientation().z(), worldXform.getOrientation().w(),
            worldXform.getScaling().x(), worldXform.getScaling().y(), worldXform.getScaling().z());

        tab++;
        
        for (auto itr = child_begin(); itr != child_end(); ++itr)
        {
            Transform3D *node = static_cast<Transform3D*>(itr->get());
            node->debugInfo(tab);
        }
    }

    //--------------------------------------------------------------------------
}
