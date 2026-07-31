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


#include "Kernel/T3DGameObject.h"
#include "Serializer/T3DSerializerManager.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DSkeletalAnimation.h"
#include "Resource/T3DSkinnedMesh.h"
#include "Bound/T3DFrustumBound.h"
#include "Bound/T3DSphereBound.h"
#include "Bound/T3DAabbBound.h"
#include "Bound/T3DCapsuleBound.h"
#include "Component/T3DCamera.h"
#include "Component/T3DRenderable.h"
#include "Component/T3DTransform3D.h"
#include "Component/T3DComponent.h"
#include "Component/T3DBehaviour.h"
#include "Component/T3DGeometry.h"
#include "Component/T3DSkinnedGeometry.h"
#include "Light/T3DLight.h"
#include "Kernel/T3DAgent.h"
#include "Render/T3DRenderPipeline.h"
#include "Resource/T3DMesh.h"
#include "Resource/T3DScene.h"
#include "Resource/T3DSceneManager.h"
#include "bound/T3DBound.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    // 脚本组件是否应在本帧执行：受组件级 enabled 与 Play/Edit 模式约束。
    // 所属 GameObject 的 active 已在 visitActive 遍历层过滤，此处仅补充判定。
    static inline bool behaviourExecutable(Behaviour *b)
    {
        return b->isEnabled() && (T3D_AGENT.isPlaying() || b->executeInEditMode());
    }

    //--------------------------------------------------------------------------

// #if defined (T3D_DEBUG)
//     Object *GameObject::acquire()
//     {
//         if (getName() == "Camera")
//         {
//             int a = 0;
//         }
//         return Object::acquire();
//     }
//
//     void GameObject::release()
//     {
//         if (getName() == "Camera")
//         {
//             int a = 0;
//         }
//         Object::release();
//     }
// #endif
    
    //--------------------------------------------------------------------------

    GameObject::WaitingDestroyComponents GameObject::msWaitingDestroyComponents;
    GameObject::WaitingDestroyGameObjects GameObject::msWaitingDestroyGameObjects;
    // GameObject::GameObjects GameObject::msGameObjects;
    
    //--------------------------------------------------------------------------

    void GameObject::destroyComponent(Component *component)
    {
        auto itr = std::find(msWaitingDestroyComponents.begin(), msWaitingDestroyComponents.end(), component);
        if (itr == msWaitingDestroyComponents.end())
        {
            msWaitingDestroyComponents.emplace_back(component);
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::destroyComponents()
    {
        while (!msWaitingDestroyComponents.empty())
        {
            Component *component = msWaitingDestroyComponents.front();
            component->onDestroy();
            component->setGameObject(nullptr);
            msWaitingDestroyComponents.pop_front();
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::destroyGameObjects()
    {
        while (!msWaitingDestroyGameObjects.empty())
        {
            GameObject *go = msWaitingDestroyGameObjects.front();
            go->onDestroy();
            msWaitingDestroyGameObjects.pop_front();
        }
    }

    //--------------------------------------------------------------------------

    GameObjectPtr GameObject::create(const String &name, bool managed)
    {
        return T3D_NEW GameObject(name, managed);
    }

    //--------------------------------------------------------------------------

    GameObjectPtr GameObject::createWithTransform(const String &name, bool managed)
    {
        GameObjectPtr go = create(name, managed);
        if (go != nullptr)
        {
            go->addComponent<Transform3D>();
        }
        return go;
    }

    //--------------------------------------------------------------------------

    void GameObject::collectHierarchy(GameObject *root, GameObjects &out)
    {
        if (root == nullptr)
        {
            return;
        }

        TransformNode *node = root->getTransformNode();
        if (node != nullptr)
        {
            // 有 TransformNode，通过 visitAll 递归收集整棵子树
            node->visitAll([&out](int32_t depth, TransformNode *n)
            {
                GameObject *go = n->getGameObject();
                out.emplace(go->getUUID(), go);
            });
        }
        else
        {
            // 没有 TransformNode，只收集自身
            out.emplace(root->getUUID(), root);
        }
    }
    
    //--------------------------------------------------------------------------

    GameObjectPtr GameObject::cloneSelf() const
    {
        // 创建新的 GameObject 作为克隆根，拥有独立的 UUID
        GameObjectPtr newGO = create(mName);

        // 遍历源 GameObject 上挂载的所有 Component，逐一克隆并挂载到新 GameObject
        for (const auto &item : mComponents)
        {
            const RTTRType &type = item.first;
            ComponentPtr comp = item.second;

            // 对所有 Component（包括 TransformNode 派生类）统一调用 clone()
            ComponentPtr newComp = comp->clone();
            if (newComp != nullptr)
            {
                newComp->setGameObject(newGO);
                newGO->mComponents.emplace(type, newComp);
                newGO->mComponentObjects.emplace(type.get_name(), newComp);
                newGO->putUpdatingQueue(type, newComp);

                // 若是 TransformNode 派生类，赋值 mTransformNode，与 addComponent 路径保持一致
                if (type.is_derived_from<TransformNode>())
                {
                    newGO->mTransformNode = static_cast<TransformNode*>(newComp.get());
                }

                // 内置组件保持原「克隆即 onStart」；Behaviour 待所有组件就位后统一 Awake/Start
                if (newComp->asBehaviour() == nullptr)
                {
                    newComp->onStart();
                }
            }
        }

        // 所有组件就位后，对本对象上的 Behaviour 统一 Awake + OnEnable + 投递 Start，
        // 使脚本 onAwake 时可可靠访问兄弟组件（对齐实例化语义，见设计文档 §10-5）
        newGO->awakeBehaviours(T3D_SCENE_MGR.getCurrentScene());

        return newGO;
    }

    //--------------------------------------------------------------------------

    GameObjectPtr GameObject::clone() const
    {
        // 克隆当前节点自身（不含子节点）
        GameObjectPtr newGO = cloneSelf();

        // 使用 child_begin()/child_end() 迭代器递归克隆子节点，建立新树的父子关系
        TransformNode *myNode = getTransformNode();
        if (myNode != nullptr)
        {
            TransformNode *newNode = newGO->getTransformNode();
            for (auto it = myNode->child_begin(); it != myNode->child_end(); ++it)
            {
                TransformNode *srcChildNode = it->get();
                GameObjectPtr childClone = srcChildNode->getGameObject()->clone();
                if (childClone != nullptr && newNode != nullptr)
                {
                    TransformNode *childNode = childClone->getTransformNode();
                    if (childNode != nullptr)
                    {
                        newNode->addChild(childNode);
                    }
                }
            }
        }

        return newGO;
    }

    //--------------------------------------------------------------------------

    GameObjectPtr GameObject::createWithMesh(const String &name, Mesh *mesh, Geometry *&geometry, TransformNode *parent, bool managed)
    {
        if (mesh->getType() != Resource::Type::kMesh && mesh->getType() != Resource::Type::kSkinnedMesh)
        {
            return nullptr;
        }
        
        GameObjectPtr go = createWithTransform(name, managed);

        // 必须在骨骼填充之前先挂到父节点：SkinnedGeometry::populateAllChildren 会把克隆
        // 出的骨骼根作为兄弟节点挂到本节点的父节点下，仅当本节点已有父节点时才会挂接。
        // 若此时游离（无父节点），骨骼子树将不在场景树中，销毁时遍历不到而泄漏。
        if (parent != nullptr)
        {
            TransformNode *node = go->getTransformNode();
            if (node != nullptr)
            {
                parent->addChild(node);
            }
        }
        
        GeometryPtr geo = nullptr;
        
        if (mesh->getType() == Resource::Type::kMesh)
        {
            geo = go->addComponent<Geometry>();
        }
        else if (mesh->getType() == Resource::Type::kSkinnedMesh)
        {
            geo = go->addComponent<SkinnedGeometry>();
        }
        
        StringArray enableKeywrods;
        enableKeywrods.push_back("");
        StringArray disableKeywords;
        int32_t i = 0;
        for (auto submesh : mesh->getSubMeshes())
        {
            Material *material = static_cast<Material *>(T3D_MATERIAL_MGR.getResource(submesh.second->getMaterialUUID()));
            T3D_ASSERT(material != nullptr);
            TResult ret = material->switchKeywords(enableKeywrods, disableKeywords);
            T3D_ASSERT(T3D_SUCCEEDED(ret));
            if (i == 0)
            {
                geo->setMeshObject(mesh, submesh.second);
            }
            i++;
        }
        
        if (mesh->getType() == Resource::Type::kSkinnedMesh)
        {
            SkinnedMesh *skinnedMesh = static_cast<SkinnedMesh*>(mesh);
            T3D_ASSERT(skinnedMesh != nullptr);
            SkeletalAnimation *skeletalAni = skinnedMesh->getSkeletalAnimation();
            T3D_ASSERT(skeletalAni != nullptr);
            const AnimationClips &clips = skeletalAni->getAnimationClips();
            T3D_ASSERT(!clips.empty());
            const String &clipName = clips.begin()->first;
            SkinnedGeometry *skinnedGeometry = smart_pointer_cast<SkinnedGeometry>(geo);
            skinnedGeometry->populateAllChildren();
            skinnedGeometry->setDefaultClipName(clipName);
            T3D_ASSERT(skinnedGeometry != nullptr);
        }

        // 若尚无任何 Bound 组件，则用 mesh 的包围体种子播种一个，并作为渲染剔除包围体。
        // 一旦已有 Bound（例如预制体带出），则以组件为准，此处跳过。
        seedBoundFromMesh(go, mesh, geo);
        
        geometry = geo;
        
        return go;
    }

    //--------------------------------------------------------------------------

    BoundPtr GameObject::seedBoundFromMesh(GameObject *go, Mesh *mesh, Geometry *geometry)
    {
        if (go == nullptr || mesh == nullptr)
        {
            return nullptr;
        }

        Bound::Type type = mesh->getBoundSeedType();
        if (type == Bound::Type::NONE)
        {
            return nullptr;
        }

        if (!go->getComponents<Bound>().empty())
        {
            // 已有 Bound 组件，以组件为准
            return nullptr;
        }

        const Vector3 &a = mesh->getBoundSeedA();
        const Vector3 &b = mesh->getBoundSeedB();
        Real radius = mesh->getBoundSeedRadius();

        BoundPtr bound = nullptr;

        switch (type)
        {
        case Bound::Type::SPHERE:
            {
                SphereBoundPtr sphere = go->addComponent<SphereBound>();
                if (sphere != nullptr)
                {
                    sphere->setParams(a, radius);
                    bound = sphere;
                }
            }
            break;
        case Bound::Type::AABB:
            {
                AabbBoundPtr aabb = go->addComponent<AabbBound>();
                if (aabb != nullptr)
                {
                    aabb->setParams(a.x(), b.x(), a.y(), b.y(), a.z(), b.z());
                    bound = aabb;
                }
            }
            break;
        case Bound::Type::CAPSULE:
            {
                CapsuleBoundPtr capsule = go->addComponent<CapsuleBound>();
                if (capsule != nullptr)
                {
                    capsule->setParams(a, b, radius);
                    bound = capsule;
                }
            }
            break;
        default:
            break;
        }

        if (bound != nullptr && geometry != nullptr)
        {
            geometry->setRenderBound(bound);
        }

        return bound;
    }

    //--------------------------------------------------------------------------

    GameObject::GameObject(const String &name, bool managed)
        : mName(name)
    {
        mUUID = UUID::generate();
        const ComponentSettings &settings = T3D_AGENT.getSettings().componentSettins;
        int32_t i = 0;
        for (auto it = settings.updateOrders.begin(); it != settings.updateOrders.end(); ++it, ++i)
        {
            mUpdateComponents.emplace(i, ComponentList());
        }

        if (managed && T3D_SCENE_MGR.getCurrentScene() != nullptr)
        {
            // msGameObjects.emplace(mUUID, this);
            T3D_SCENE_MGR.getCurrentScene()->addGameObject(this);
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::update()
    {
        TransformNodePtr node = getComponent<TransformNode>();
        if (node != nullptr)
        {
            node->visitActive([](int32_t depth, TransformNode *node)
            {
                GameObject *go = node->getGameObject();
                go->onUpdate();
                // node->onUpdate();
                //
                // GameObject *go = node->getGameObject();
                // for (auto component : go->getComponents<Component>())
                // {
                //     if (component != node)
                //     {
                //         component->onUpdate();
                //     }
                // }
            });
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::onUpdate()
    {
        for (auto item : mUpdateComponents)
        {
            for (auto component : item.second)
            {
                Behaviour *b = component->asBehaviour();
                if (b == nullptr || behaviourExecutable(b))
                {
                    component->onUpdate();
                }
            }
        }

        for (auto item : mUpdateComponents2)
        {
            Behaviour *b = item.second->asBehaviour();
            if (b == nullptr || behaviourExecutable(b))
            {
                item.second->onUpdate();
            }
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::onLateUpdate()
    {
        for (auto item : mUpdateComponents)
        {
            for (auto component : item.second)
            {
                Behaviour *b = component->asBehaviour();
                if (b != nullptr && behaviourExecutable(b))
                {
                    b->onLateUpdate();
                }
            }
        }

        for (auto item : mUpdateComponents2)
        {
            Behaviour *b = item.second->asBehaviour();
            if (b != nullptr && behaviourExecutable(b))
            {
                b->onLateUpdate();
            }
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::onFixedUpdate()
    {
        for (auto item : mUpdateComponents)
        {
            for (auto component : item.second)
            {
                Behaviour *b = component->asBehaviour();
                if (b != nullptr && behaviourExecutable(b))
                {
                    b->onFixedUpdate();
                }
            }
        }

        for (auto item : mUpdateComponents2)
        {
            Behaviour *b = item.second->asBehaviour();
            if (b != nullptr && behaviourExecutable(b))
            {
                b->onFixedUpdate();
            }
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::lateUpdate()
    {
        TransformNodePtr node = getComponent<TransformNode>();
        if (node != nullptr)
        {
            node->visitActive([](int32_t depth, TransformNode *node)
            {
                node->getGameObject()->onLateUpdate();
            });
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::fixedUpdate()
    {
        TransformNodePtr node = getComponent<TransformNode>();
        if (node != nullptr)
        {
            node->visitActive([](int32_t depth, TransformNode *node)
            {
                node->getGameObject()->onFixedUpdate();
            });
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::awakeBehaviours(Scene *scene)
    {
        // 第一趟：本对象所有 Behaviour 同步 Awake（此刻兄弟组件已就位）
        for (const auto &item : mComponents)
        {
            Behaviour *b = item.second->asBehaviour();
            if (b != nullptr)
            {
                b->invokeAwake();
            }
        }

        // 第二趟：Awake 完成后补发 OnEnable，并投递 pending-start（Start 延迟）
        for (const auto &item : mComponents)
        {
            Behaviour *b = item.second->asBehaviour();
            if (b != nullptr)
            {
                b->refreshActiveState();
                if (scene != nullptr)
                {
                    scene->enqueuePendingStart(b);
                }
                else
                {
                    b->invokeStart();
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::frustumCulling(Camera *camera, RenderPipeline *pipeline) const
    {
        TransformNodePtr node = getComponent<TransformNode>();
        if (node != nullptr && camera != nullptr)
        {
            FrustumBound *frustum = camera->getGameObject()->getComponent<FrustumBound>();
            node->visitActive([](int32_t depth, TransformNode *node, Camera *camera, FrustumBound *frustum, RenderPipeline *pipeline)
            {
                GameObject *go = node->getGameObject();
                Renderable *renderable = go->getComponent<Renderable>();
                // 剔除以“种子播种出来的渲染包围体”为准；未设置则回退到任一 Bound 组件
                Bound *bound = (renderable != nullptr) ? renderable->getRenderBound() : nullptr;
                if (bound == nullptr)
                {
                    bound = go->getComponent<Bound>();
                }
                if (renderable != nullptr && renderable->isEnabled())
                {
                    if (bound == nullptr || frustum == nullptr)
                    {
                        // 没有包围盒，不剔除，直接渲染
                        pipeline->addRenderable(camera, renderable);
                    }
                    else
                    {
                        // 有包围盒，根据包围盒来判断
                        if (frustum->test(bound))
                        {
                            pipeline->addRenderable(camera, renderable);
                        }
                    }
                }
            },
            camera, frustum, pipeline);
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::setupLights(RenderPipeline *pipeline) const
    {
        TransformNodePtr node = getComponent<TransformNode>();
        if (node != nullptr)
        {
            node->visitActive([](int32_t depth, TransformNode *node, RenderPipeline *pipeline)
            {
                GameObject *go = node->getGameObject();
                Light *light = go->getComponent<Light>();
                if (light != nullptr && light->isEnabled())
                {
                    pipeline->addLight(light);
                }
            }, pipeline);
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::destroy(GameObject *gameObject)
    {
        TransformNodePtr node = gameObject->getComponent<TransformNode>();
        if (node != nullptr)
        {
            node->reverseVisitAll([](int32_t depth, TransformNode *node)
            {
                node->getGameObject()->removeAllComponents();
                destroyGameObject(node->getGameObject());
            });
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::destroyGameObject(GameObject *gameObject)
    {
        auto itr = std::find(msWaitingDestroyGameObjects.begin(), msWaitingDestroyGameObjects.end(), gameObject);
        if (itr == msWaitingDestroyGameObjects.end())
        {
            msWaitingDestroyGameObjects.emplace_back(gameObject);
            // msGameObjects.erase(gameObject->getUUID());
            T3D_SCENE_MGR.getCurrentScene()->removeGameObject(gameObject);
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::onDestroy()
    {
        
    }

    //--------------------------------------------------------------------------

    void GameObject::setupHierarchy()
    {
        TransformNode *node = getComponent<TransformNode>();
        T3D_ASSERT(node != nullptr);
        node->setupHierarchy();
    }
    
    //--------------------------------------------------------------------------

    void GameObject::putUpdatingQueue(const RTTRType &type, Component *component)
    {
        const ComponentSettings &settings = T3D_AGENT.getSettings().componentSettins;

        // 1) 先按精确类名匹配预设更新顺序；同时记录 "Behaviour" 段位下标
        int32_t i = 0;
        int32_t behaviourSlot = -1;
        for (auto it = settings.updateOrders.begin(); it != settings.updateOrders.end(); ++it, ++i)
        {
            if (*it == "Behaviour")
            {
                behaviourSlot = i;
            }

            if (*it == type.get_name())
            {
                auto itUpdate = mUpdateComponents.find(i);
                T3D_ASSERT(itUpdate != mUpdateComponents.end());
                itUpdate->second.emplace_back(component);
                return;
            }
        }

        // 2) 脚本组件（Behaviour 派生）统一落入 "Behaviour" 段位，获得确定更新次序，
        //    而不是落入无序队列（见设计文档 §4.4）
        if (behaviourSlot >= 0 && type.is_derived_from<Behaviour>())
        {
            auto itUpdate = mUpdateComponents.find(behaviourSlot);
            T3D_ASSERT(itUpdate != mUpdateComponents.end());
            itUpdate->second.emplace_back(component);
            return;
        }

        // 3) 其它不在预设序列里的组件，放入无序更新队列
        mUpdateComponents2.emplace(type.get_name(), component);
    }

    //--------------------------------------------------------------------------

    void GameObject::eraseUpdatingQueue(Component *component)
    {
        // 这里按指针在两个队列里查找，而不是照 putUpdatingQueue 的规则反推当初的
        // 落位。组件的落位依赖 settings.updateOrders，两处各推一遍一旦不一致就会
        // 残留野指针，按指针找则不受落位规则影响
        for (auto &item : mUpdateComponents)
        {
            ComponentList &components = item.second;
            auto itr = std::find(components.begin(), components.end(), component);

            if (itr != components.end())
            {
                components.erase(itr);
                return;
            }
        }

        for (auto itr = mUpdateComponents2.begin();
            itr != mUpdateComponents2.end(); ++itr)
        {
            if (itr->second == component)
            {
                mUpdateComponents2.erase(itr);
                return;
            }
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::unlinkComponentReferences(Component *component)
    {
        // Renderable 用裸指针记着视锥剔除包围体（由 seedBoundFromMesh 播种），
        // 移除 Bound 组件时必须同步清空，否则剔除时会踩到野指针。
        // 清空后 frustumCulling 会自动回退到「取任一 Bound 组件」的分支
        Renderable *renderable = getComponent<Renderable>();

        if (renderable != nullptr && renderable->getRenderBound() == component)
        {
            renderable->setRenderBound(nullptr);
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::setupComponents()
    {
        // 工具态（关闭生命周期回调）：只做纯数据反序列化，不建立组件->宿主的反向
        // 强引用(setGameObject)，避免 GameObject 与 Component 之间的智能指针循环引用
        // ——离线场景（如 BundleBuilder）不经显式销毁流程，建了环就无人断链而泄漏；
        // 同时不跑 onStart（无渲染设备的离线环境不需要，且部分组件 onStart 依赖运行
        // 时资源）。序列化输出只依赖 mComponentObjects，与这些运行时接线无关。
        const bool wire = T3D_SERIALIZER_MGR.isInvokeLifecycleCallbacks();

        for (const auto &item : mComponentObjects)
        {
            RTTRType type = RTTRType::get_by_name(item.first);
            mComponents.emplace(type, item.second);
            if (wire)
            {
                item.second->setGameObject(this);
            }
            putUpdatingQueue(type, item.second);

            // 内置组件保持原「就位即 onStart」行为；Behaviour 的 Awake/Start
            // 改由 Scene::onPostLoad 在整树 setupHierarchy 之后统一触发，
            // 保证 Awake 时兄弟组件 + 父子层级都已就位（见设计文档 §4.2 / §10-3）。
            if (wire && item.second->asBehaviour() == nullptr)
            {
                item.second->onStart();
            }
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::setupTransformNode()
    {
        for (const auto &item : mComponentObjects)
        {
            if (mTransformNode == nullptr)
            {
                RTTRType type = RTTRType::get_by_name(item.first);
                if (type.is_derived_from<TransformNode>())
                {
                    mTransformNode = smart_pointer_cast<TransformNode>(item.second);
                    break;
                }
            }
        }
    }

    //--------------------------------------------------------------------------
    
    ComponentPtr GameObject::addComponent(const RTTRType &type)
    {
        ComponentPtr component;
        
        do
        {
            if (!type.is_derived_from<Component>())
            {
                // 不是 Component 子类，无法创建 component
                break;
            }

            bool isDerivedFromTransformNode = type.is_derived_from<TransformNode>();
            if (isDerivedFromTransformNode && mTransformNode != nullptr)
            {
                // transform node 子类，不能重复添加
                break;
            }

            TArray<rttr::argument> args;
            UUID uuid = UUID::generate();
            args.push_back(uuid);
            rttr::variant var = type.create(args);
            bool ok = false;
            Component *comp = var.convert<Component*>(&ok);
            if (!ok)
            {
                break;
            }

            component = comp;
            component->setGameObject(this);

            // 放入组件对象表里
            mComponents.emplace(type, component);
            mComponentObjects.emplace(type.get_name(), component);

            if (isDerivedFromTransformNode)
            {
                mTransformNode = static_cast<TransformNode*>(comp);
            }

            // 放入组件更新队列
            putUpdatingQueue(type, component);

            Behaviour *b = component->asBehaviour();
            if (b != nullptr)
            {
                // 脚本组件：单加语义与 Unity 一致——此刻尚未添加的兄弟组件本就拿不到。
                // 同步 Awake + OnEnable，Start 延迟到首帧 update 前统一 flush。
                b->invokeAwake();
                b->refreshActiveState();

                Scene *scene = T3D_SCENE_MGR.getCurrentScene();
                if (scene != nullptr)
                {
                    scene->enqueuePendingStart(b);
                }
                else
                {
                    b->invokeStart();
                }
            }
            else
            {
                // 内置组件保持原路径：addComponent 即 onStart
                component->onStart();
            }
        } while (false);

        return component;
    }

    //--------------------------------------------------------------------------

    void GameObject::removeAllComponents()
    {
        for (auto itr = mComponents.begin(); itr != mComponents.end(); ++itr)
        {
            // 销毁前对已 Awake 且处于运行态的 Behaviour 补发 onDisable（配对 onEnable）
            Behaviour *b = itr->second->asBehaviour();
            if (b != nullptr && b->wasAwaked())
            {
                b->invokeDisable();
            }
            destroyComponent(itr->second);
        }

        mComponents.clear();
        mComponentObjects.clear();
        mUpdateComponents.clear();
        mUpdateComponents2.clear();

        mTransformNode = nullptr;
    }

    //--------------------------------------------------------------------------

    TResult GameObject::removeComponent(const RTTRType &type)
    {
        TResult ret = T3D_OK;

        do
        {
            auto itr = mComponents.find(type);
            if (itr == mComponents.end())
            {
                // 没找到
                ret = T3D_ERR_NOT_FOUND;
                break;
            }

            auto it = mComponentObjects.find(type.get_name().data());
            T3D_ASSERT(it != mComponentObjects.end());

            if (itr->second == mTransformNode)
            {
                mTransformNode = nullptr;
            }

            Behaviour *b = itr->second->asBehaviour();
            if (b != nullptr && b->wasAwaked())
            {
                b->invokeDisable();
            }

            // 更新队列与兄弟组件都用裸指针引用它，销毁前必须先断开，
            // 否则组件被释放后下一帧就会踩到野指针
            eraseUpdatingQueue(itr->second);
            unlinkComponentReferences(itr->second);

            destroyComponent(itr->second);
            mComponents.erase(itr);
            mComponentObjects.erase(it);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GameObject::removeComponents(const RTTRType &type)
    {
        TResult ret = T3D_OK;

        do
        {
            auto range = mComponents.equal_range(type);
            if (range.first == range.second)
            {
                ret = T3D_ERR_NOT_FOUND;
                break;
            }
            
            for (auto itr = range.first; itr != range.second; )
            {
                if (itr->second == mTransformNode)
                {
                    mTransformNode = nullptr;
                }

                Behaviour *b = itr->second->asBehaviour();
                if (b != nullptr && b->wasAwaked())
                {
                    b->invokeDisable();
                }

                eraseUpdatingQueue(itr->second);
                unlinkComponentReferences(itr->second);
                destroyComponent(itr->second);

                // erase 会让当前迭代器失效，必须用它的返回值继续遍历
                itr = mComponents.erase(itr);
            }

            mComponentObjects.erase(type.get_name().data());
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    ComponentPtr GameObject::getComponent(const RTTRType &type) const
    {
        ComponentPtr component;

        do
        {
            bool found = false;
            auto itr = mComponents.find(type);
            if (itr == mComponents.end())
            {
                // 没找到，找所有子类
                for (auto t : type.get_derived_classes())
                {
                    auto it = mComponents.find(t);
                    if (it != mComponents.end())
                    {
                        itr = it;
                        found = true;
                        break;
                    }
                }
            }
            else
            {
                found = true;
            }

            if (!found)
            {
                break;
            }
            
            component = itr->second;
        } while (false);

        return component;
    }

    //--------------------------------------------------------------------------

    // TArray<ComponentPtr> GameObject::getComponents(const RTTRType &type) const
    // {
    //     TArray<ComponentPtr> components;
    //     
    //     auto range = mComponents.equal_range(type);
    //     for (auto itr = range.first; itr != range.second; ++itr)
    //     {
    //         components.emplace_back(itr->second);
    //     }
    //
    //     return components;
    // }

    //--------------------------------------------------------------------------
}
