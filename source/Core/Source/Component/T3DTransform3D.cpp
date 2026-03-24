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
        return T3D_NEW Transform3D(UUID::generate());
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
        Transform3DPtr newObj = T3D_NEW Transform3D(UUID::generate());
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

            // 先调用父类复制父类属性
            ret = TransformNode::cloneProperties(src);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 再复制自身扩展属性（位置、旋转、缩放、世界变换、脏标记）
            mPosition = node->mPosition;
            mOrientation = node->mOrientation;
            mScaling = node->mScaling;
            mWorldTransform = node->mWorldTransform;
            mIsDirty = node->mIsDirty;
        } while (false);

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

            // while (node != nullptr)
            // {
            //     node = node->getParent();
            // }

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

    // void Transform3D::debugInfo(int32_t tab)
    // {
    //     std::stringstream ss;
    //     for (int32_t i = 0; i < tab; i++)
    //     {
    //         ss << "\t";
    //     }
    //
    //     const Transform &worldXform = getLocalToWorldTransform();
    //     
    //     T3D_LOG_INFO(LOG_TAG_COMPONENT, "%sTransform 3D - %s, "
    //         "Local : T %s, Q %s, S %s"
    //         "    World : T (%f, %f, %f), R (%f, %f, %f, %f), S (%f, %f, %f)",
    //         ss.str().c_str(), getGameObject()->getName().c_str(),
    //         mPosition.getDebugString().c_str(),
    //         mOrientation.getDebugString().c_str(),
    //         mScaling.getDebugString().c_str(),
    //         worldXform.getTranslation().getDebugString().c_str(),
    //         worldXform.getOrientation().getDebugString().c_str(),
    //         worldXform.getScaling().getDebugString().c_str());
    //
    //     tab++;
    //     
    //     for (auto itr = child_begin(); itr != child_end(); ++itr)
    //     {
    //         Transform3D *node = static_cast<Transform3D*>(itr->get());
    //         node->debugInfo(tab);
    //     }
    // }

    String Transform3D::printHierarchy(bool outputLog, bool verbose)
    {
        String output;
        visitAll([&output, verbose](int32_t depth, const TransformNode *node)
        {
            String indent(depth * 2, ' ');
            GameObject *go = node->getGameObject();
            if (go == nullptr)
            {
                output += indent + "[NULL_GAMEOBJECT]\n";
                return;
            }

            const Transform3D *xformNode = static_cast<const Transform3D*>(node);
            std::stringstream ss;
            ss << indent << go->getName();

            if (verbose)
            {
                // 输出所有 Component 类型名称，并检测是否含 Bone
                bool hasBone = false;
                String compList = " [";
                bool first = true;
                auto comps = go->getComponents<Component>();
                for (const auto &comp : comps)
                {
                    if (!first) compList += ", ";
                    String typeName = comp->get_type().get_name().to_string();
                    compList += typeName;
                    if (typeName == "Bone") hasBone = true;
                    first = false;
                }
                compList += "]";
                ss << compList;
                if (hasBone) ss << " [BONE]";

                // 输出本地 position / orientation（欧拉角）/ scaling
                ss << "\n" << indent << "  T:" << xformNode->getPosition().getDebugString()
                   << ", E:" << xformNode->getRotation().getDebugString()
                   << ", S:" << xformNode->getScaling().getDebugString();
            }

            ss << "\n";
            output += ss.str();
        });
        if (outputLog)
        {
            T3D_LOG_DEBUG(LOG_TAG_COMPONENT, "Hierarchy : \n%s", output.c_str());
        }
        return output;
    }

    //--------------------------------------------------------------------------
}
