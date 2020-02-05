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


#include "Scene/T3DSceneNode.h"
#include "Component/T3DCamera.h"
#include "Component/T3DCube.h"
#include "Component/T3DGlobe.h"
#include "Kernel/T3DAgent.h"
#include "Component/T3DComponent.h"
#include "Component/T3DComponentCreator.h"
#include "Component/T3DRenderable.h"
#include "Scene/T3DSceneManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SceneNodePtr SceneNode::create(ID uID /* = E_NID_AUTOMATIC */)
    {
        SceneNodePtr node = new SceneNode(uID);
        node->release();
        return node;
    }

    //--------------------------------------------------------------------------

    SceneNode::SceneNode(ID uID /* = E_NID_AUTOMATIC */)
        : Node(uID)
        , mIsVisible(true)
        , mIsEnabled(true)
        , mCameraMask(0)
        , mTransform3D(nullptr)
        , mRenderable(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    SceneNode::~SceneNode()
    {

    }

    //--------------------------------------------------------------------------

    Node::Type SceneNode::getNodeType() const
    {
        return Type::SCENE_NODE;
    }

    //--------------------------------------------------------------------------

    NodePtr SceneNode::clone() const
    {
        SceneNodePtr node = SceneNode::create();
        TResult ret = cloneProperties(node);

        if (ret != T3D_OK)
        {
            node = nullptr;
        }

        return node;
    }

    //--------------------------------------------------------------------------

    void SceneNode::update()
    {
        if (!isEnabled())
            return;

        bool isDirty = false;

        if (mTransform3D != nullptr)
        {
            // 先更新 RTS 
            isDirty = mTransform3D->isDirty();
            mTransform3D->updateTransform();
        }

        // 更新所有组件
        auto itr = mComponents.begin();

        while (itr != mComponents.end())
        {
            auto component = itr->second;
            component->update();
            ++itr;
        }

        // 以防组件又去更新了 RTS，所以这里再获取一遍 RTS 是否有更新
        if (!isDirty && mTransform3D != nullptr)
        {
            isDirty = mTransform3D->isDirty();
        }

        // RTS 有更新，需要更新可渲染对象的包围体
        if (mRenderable != nullptr && isDirty)
        {
            mRenderable->updateBound();
        }
    }

    //--------------------------------------------------------------------------

    void SceneNode::frustumCulling(BoundPtr bound, RenderQueuePtr queue)
    {
        if (mRenderable != nullptr)
        {
            mRenderable->frustumCulling(bound, queue);
        }
    }

    //--------------------------------------------------------------------------

    TResult SceneNode::cloneProperties(NodePtr node) const
    {
        TResult ret = Node::cloneProperties(node);

        if (ret == T3D_OK)
        {
            SceneNodePtr newNode = smart_pointer_cast<SceneNode>(node);
            newNode->mIsEnabled = mIsEnabled;
            newNode->mIsVisible = mIsVisible;

            ComponentPtr component;
            if (mTransform3D != nullptr)
            {
                component = newNode->addTransform();
            }

            if (mRenderable != nullptr)
            {
                component = newNode->addRenderable(mRenderable->getType());
            }

            auto itr = mComponents.begin();

            while (itr != mComponents.end())
            {
                if (itr->second != mTransform3D && itr->second != mRenderable)
                    component = itr->second->clone();
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void SceneNode::setVisible(bool visible)
    {
        if (mIsVisible != visible)
        {
            mIsVisible = visible;

            NodePtr node = getFirstChild();

            while (node != nullptr)
            {
                SceneNodePtr child = smart_pointer_cast<SceneNode>(node);
                child->setVisible(visible);
                node = node->getNextSibling();
            }
        }
    }

    //--------------------------------------------------------------------------

    void SceneNode::setEnabled(bool enabled)
    {
        if (mIsEnabled != enabled)
        {
            mIsEnabled = enabled;

            NodePtr node = getFirstChild();

            while (node != nullptr)
            {
                SceneNodePtr child = smart_pointer_cast<SceneNode>(node);
                child->setEnabled(enabled);
                node = node->getNextSibling();
            }
        }
    }

    //--------------------------------------------------------------------------

    void SceneNode::setCameraMask(uint32_t mask)
    {
        if (mCameraMask == 0)
        {
            // 之前没有设置过，直接添加到默认场景里面
            mCameraMask = mask;

            if (getParent() != nullptr && mRenderable != nullptr)
            {
                T3D_SCENE_MGR.addRenderable(mRenderable);
            }
        }
        else if (mCameraMask != mask)
        {
            if (getParent() != nullptr && mRenderable != nullptr)
            {
                T3D_SCENE_MGR.removeRenderable(mRenderable);
                mCameraMask = mask;
                T3D_SCENE_MGR.addRenderable(mRenderable);
            }
            else
            {
                mCameraMask = mask;
            }
        }

        NodePtr node = getFirstChild();

        while (node != nullptr)
        {
            SceneNodePtr child = smart_pointer_cast<SceneNode>(node);
            child->setCameraMask(mask);
            node = node->getNextSibling();
        }
    }

    //--------------------------------------------------------------------------

    void SceneNode::visit()
    {
        // 先调用更新
        if (isEnabled())
        {
            update();

            // 再遍历子结点
            NodePtr node = getFirstChild();

            while (node != nullptr)
            {
                SceneNodePtr child = smart_pointer_cast<SceneNode>(node);
                child->visit();
                node = node->getNextSibling();
            }
        }
    }

    //--------------------------------------------------------------------------

    Transform3DPtr SceneNode::addTransform()
    {
        ComponentPtr component = createComponent(1, "Transform3D");

        if (component != nullptr)
        {
            mTransform3D = smart_pointer_cast<Transform3D>(component);
            component->onAttachSceneNode(this);
        }

        return mTransform3D;
    }

    //--------------------------------------------------------------------------

    RenderablePtr SceneNode::addRenderable(const String &type)
    {
        ComponentPtr component = createComponent(1, type.c_str());

        if (component != nullptr)
        {
            mRenderable = smart_pointer_cast<Renderable>(component);
            component->onAttachSceneNode(this);
        }

        return mRenderable;
    }

    //--------------------------------------------------------------------------

    CameraPtr SceneNode::addCamera()
    {
        if (mTransform3D != nullptr)
        {
            removeComponent(mTransform3D);
            mTransform3D = nullptr;
        }

        ComponentPtr component = createComponent(1, "Camera");

        if (component != nullptr)
        {
            mTransform3D = smart_pointer_cast<Transform3D>(component);
            component->onAttachSceneNode(this);
        }

        return smart_pointer_cast<Camera>(component);
    }

    //--------------------------------------------------------------------------

    CubePtr SceneNode::addCube(const Vector3 &center, const Vector3 &extent)
    {
        if (mRenderable != nullptr)
        {
            removeComponent(mRenderable);
            mRenderable = nullptr;
        }

        ComponentPtr component = createComponent(3, "Cube", &center, &extent);

        if (component != nullptr)
        {
            mRenderable = smart_pointer_cast<Renderable>(component);
            component->onAttachSceneNode(this);
        }

        return smart_pointer_cast<Cube>(component);
    }

    //--------------------------------------------------------------------------

    GlobePtr SceneNode::addSphere(const Vector3 &center, Real radius)
    {
        if (mRenderable != nullptr)
        {
            removeComponent(mRenderable);
            mRenderable = nullptr;
        }

        ComponentPtr component = createComponent(3, "Globe", &center, &radius);

        if (component != nullptr)
        {
            mRenderable = smart_pointer_cast<Renderable>(component);
            component->onAttachSceneNode(this);
        }

        return smart_pointer_cast<Globe>(component);
    }

    //--------------------------------------------------------------------------

    ComponentPtr SceneNode::addComponent(const String &type)
    {
        ComponentPtr component = createComponent(1, type.c_str());

        if (component != nullptr)
        {
            component->onAttachSceneNode(this);

        }

        return component;
    }

    //--------------------------------------------------------------------------

    ComponentPtr SceneNode::createComponent(int32_t argc, ...)
    {
        ComponentPtr component;

        do
        {
            ComponentCreatorPtr creator = T3D_AGENT.getComponentCreator();
            va_list args;
            va_start(args, argc);
            component = creator->createObject(argc, args);
            va_end(args);

            if (component == nullptr)
            {
                break;
            }

            auto ret = mComponents.insert(ComponentsValue(component->getType(), component));

            if (!ret.second)
            {
                component = nullptr;
                break;
            }

        } while (0);

        return component;
    }

    //--------------------------------------------------------------------------

    ComponentPtr SceneNode::getComponent(const String &type) const
    {
        ComponentPtr component;
        auto itr = mComponents.find(type);

        if (itr != mComponents.end())
        {
            component = itr->second;
        }

        return component;
    }

    //--------------------------------------------------------------------------

    void SceneNode::removeComponent(const String &type)
    {
        auto itr = mComponents.find(type);

        if (itr != mComponents.end())
        {
            ComponentPtr component = itr->second;
            component->onDetachSceneNode(this);
            mComponents.erase(itr);
        }
    }

    //--------------------------------------------------------------------------

    void SceneNode::removeAllComponents()
    {
        auto itr = mComponents.begin();

        while (itr != mComponents.end())
        {
            itr->second->onDetachSceneNode(this);
            ++itr;
        }

        mComponents.clear();
    }

    //--------------------------------------------------------------------------

    void SceneNode::removeComponent(Component *component)
    {
        auto itr = mComponents.begin();
        while (itr != mComponents.end())
        {
            if (itr->second == component)
            {
                component->onDetachSceneNode(this);
                mComponents.erase(itr);
                break;
            }

            ++itr;
        }
    }
}
