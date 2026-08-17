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

#ifndef __T3D_GAME_OBJECT_H__
#define __T3D_GAME_OBJECT_H__


#include "T3DTypedef.h"
#include "Component/T3DComponent.h"


namespace Tiny3D
{
    /**
     * \brief 场景实体：持有组件表、可选 TransformNode 层级，以及更新 / 剔除入口
     * \remarks 销毁走延迟队列（destroy → 入队 → Agent::endFrame 中 destroyComponents/destroyGameObjects）。
     *          managed=true 且当前有 Scene 时，构造会自动 Scene::addGameObject。
     */
    TCLASS()
    class T3D_ENGINE_API GameObject : public Object
    {
        friend class Scene;

        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建 GameObject
         * \param [in] name : 对象名称
         * \param [in] managed : 为 true 且当前 Scene 非空时登记到场景，默认 true
         * \return 新对象智能指针
         */
        static GameObjectPtr create(const String &name, bool managed = true);

        /**
         * \brief 创建并挂载 Transform3D 组件
         * \param [in] name : 对象名称
         * \param [in] managed : 是否登记到当前场景，默认 true
         * \return 带 Transform3D 的新对象；create 失败则为 nullptr
         */
        static GameObjectPtr createWithTransform(const String &name, bool managed = true);

        /**
         * \brief 创建带 Transform 与 Geometry/SkinnedGeometry 的对象，并用 mesh 播种材质与 Bound
         * \param [in] name : 对象名称
         * \param [in] mesh : 网格资源；类型须为 kMesh 或 kSkinnedMesh，否则返回 nullptr
         * \param [out] geometry : 输出创建的 Geometry（成功路径赋值）
         * \param [in] parent : 可选父 TransformNode，非空时在填充骨骼前挂到父节点
         * \param [in] managed : 是否登记到当前场景，默认 true
         * \return 成功返回新对象；mesh 类型非法返回 nullptr
         */
        static GameObjectPtr createWithMesh(const String &name, Mesh *mesh, Geometry *&geometry, TransformNode *parent = nullptr, bool managed = true);

        /**
         * \brief 用 mesh 的包围体种子播种 Bound 组件（仅当无 Bound 时），并设为渲染剔除包围体
         * \param [in] go : 需要挂载 Bound 的 GameObject
         * \param [in] mesh : 提供包围体种子的 mesh
         * \param [in] geometry : 需设置渲染剔除包围体的 geometry，可为 nullptr
         * \return 播种成功返回新建 Bound；go/mesh 为空、无种子或已有 Bound 时返回 nullptr
         */
        static BoundPtr seedBoundFromMesh(GameObject *go, Mesh *mesh, Geometry *geometry);

        /**
         * \brief 处理等待销毁的组件队列：逐个 onDestroy、断开 GameObject 引用并出队
         * \note 由 Agent::endFrame 调用
         */
        static void destroyComponents();

        /**
         * \brief 处理等待销毁的 GameObject 队列：逐个 onDestroy 并出队
         * \note 由 Agent::endFrame 调用
         */
        static void destroyGameObjects();

        /**
         * \brief 从根节点出发，收集整棵子树的所有 GameObject 到扁平表
         * \param [in] root : 子树根节点；为 nullptr 时直接返回
         * \param [out] out : 收集结果，key 为 UUID，value 为 GameObjectPtr（不 clear）
         * \note 若节点没有 TransformNode 则只收集自身，不继续遍历子节点
         */
        static void collectHierarchy(GameObject *root, GameObjects &out);

        /**
         * \brief 模板 UUID → 实例 Object 映射（Prefab 实例化用）
         */
        using TemplateInstanceMap = TUnorderedMap<UUID, Object*, UUIDHash, UUIDEqual>;

        /**
         * \brief 深拷贝当前节点及其整个子树（组件与 Transform 父子关系）
         * \param [out] outMap : 非空时记录模板 UUID → 实例 Object
         * \return 新子树根节点
         */
        GameObjectPtr clone(TemplateInstanceMap *outMap = nullptr) const;

        /**
         * \brief 反序列化后重建 TransformNode 场景树
         * \note 要求已有 TransformNode 组件，否则断言失败；供 Scene/Skeleton/Prefab 的 onPostLoad 调用
         */
        void setupHierarchy();

        ~GameObject() override = default;

        /**
         * \brief 对激活子树分发 onUpdate（经 TransformNode::visitActive）
         * \note 无 TransformNode 时为空操作
         */
        virtual void update();

        /**
         * \brief 对激活子树分发 onLateUpdate
         * \note 无 TransformNode 时为空操作
         */
        virtual void lateUpdate();

        /**
         * \brief 对激活子树分发 onFixedUpdate（由 Agent 固定步长循环驱动）
         * \note 无 TransformNode 时为空操作
         */
        virtual void fixedUpdate();

        /**
         * \brief 视锥剔除：将启用的 Renderable 按 Bound 测试结果加入管线
         * \param [in] camera : 相机；为 nullptr 时为空操作
         * \param [in] pipeline : 渲染管线（实现未判空）
         * \note 无 TransformNode 或 camera 为空时不遍历；无 Bound / 无 FrustumBound 时不做剔除直接加入
         */
        virtual void frustumCulling(Camera *camera, RenderPipeline *pipeline) const;

        /**
         * \brief 将激活子树上启用的 Light 加入管线
         * \param [in] pipeline : 渲染管线
         * \note 无 TransformNode 时为空操作
         */
        virtual void setupLights(RenderPipeline *pipeline) const;

        /// 返回对象 UUID
        TPROPERTY(RTTRFuncName="UUID", RTTRFuncType="getter")
        const UUID &getUUID() const { return mUUID; }

        /// 返回对象名称
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        /**
         * \brief 返回自身激活标志（对标 Unity 的 activeSelf）
         * \remarks 同时影响逻辑更新与渲染遍历；「只藏不停」由 Renderable.enabled 表达
         */
        TPROPERTY(RTTRFuncName="Active", RTTRFuncType="getter")
        bool isActive() const { return mIsActive; }

        /**
         * \brief 设置自身激活标志
         * \param [in] active : 是否激活
         */
        TPROPERTY(RTTRFuncName="Active", RTTRFuncType="setter")
        void setActive(bool active) { mIsActive = active; }

        /**
         * \brief 自身与所有祖先是否都处于激活状态（对标 Unity 的 activeInHierarchy）
         * \remarks 场景树 visitActive 会在祖先失活时裁掉子树；
         *          从独立登记表取到的组件（如相机）才需要主动查询本接口。
         *          无 TransformNode 时仅看自身 mIsActive。
         */
        bool isActiveInHierarchy() const;

        /**
         * \brief 设置相机可见掩码
         * \param [in] mask : 掩码，默认 0x1
         */
        TPROPERTY(RTTRFuncName="CameraMask", RTTRFuncType="setter")
        void setCameraMask(uint32_t mask) { mCameraMask = mask; }

        /// 返回相机可见掩码
        TPROPERTY(RTTRFuncName="CameraMask", RTTRFuncType="getter")
        uint32_t getCameraMask() const { return mCameraMask; }

        /// 返回缓存的 TransformNode 指针，可能为 nullptr
        TransformNode *getTransformNode() const { return mTransformNode; }

        /**
         * \brief 销毁对象及其 Transform 子树：逆序 visit，每节点 removeAllComponents 再入待销毁队列
         * \param [in] gameObject : 根对象；无 TransformNode 时为空操作
         */
        static void destroy(GameObject *gameObject);

        /**
         * \brief 按类名添加组件
         * \param [in] name : 组件类名（RTTR）
         * \return 新增组件；类型非法 / 创建失败 / 重复 TransformNode 时返回 nullptr
         */
        ComponentPtr addComponent(const String &name)
        {
            RTTRType type = RTTRType::get_by_name(name);
            return addComponent(type);
        }

        /**
         * \brief 按类型添加组件
         * \tparam [in] T : Component 子类
         * \return 新增组件智能指针；失败时为 nullptr
         */
        template <typename T>
        SmartPtr<T> addComponent()
        {
            RTTRType type = RTTRType::get<T>();
            return smart_pointer_cast<T>(addComponent(type));
        }

        /**
         * \brief 移除第一个匹配类名的组件
         * \param [in] name : 组件类名
         * \return 成功返回 T3D_OK；未找到返回 T3D_ERR_NOT_FOUND
         */
        TResult removeComponent(const String &name)
        {
            RTTRType type = RTTRType::get_by_name(name);
            return removeComponent(type);
        }

        /**
         * \brief 移除第一个匹配类型的组件
         * \tparam [in] T : 组件类型
         * \return 成功返回 T3D_OK；未找到返回 T3D_ERR_NOT_FOUND
         */
        template <typename T>
        TResult removeComponent()
        {
            RTTRType type = RTTRType::get<T>();
            return removeComponent(type);
        }

        /**
         * \brief 移除所有匹配类名的组件
         * \param [in] name : 组件类名
         * \return 成功返回 T3D_OK；未找到返回 T3D_ERR_NOT_FOUND
         */
        TResult removeComponents(const String &name)
        {
            RTTRType type = RTTRType::get_by_name(name);
            return removeComponents(type);
        }

        /**
         * \brief 移除所有匹配类型的组件
         * \tparam [in] T : 组件类型
         * \return 成功返回 T3D_OK；未找到返回 T3D_ERR_NOT_FOUND
         */
        template <typename T>
        TResult removeComponents()
        {
            RTTRType type = RTTRType::get<T>();
            return removeComponents(type);
        }

        /**
         * \brief 移除全部组件：已 Awake 的 Behaviour 先 invokeDisable，再入销毁队列并清空表与更新队列
         */
        void removeAllComponents();

        /**
         * \brief 获取第一个匹配类名的组件
         * \param [in] name : 组件类名
         * \return 命中返回组件，否则返回 nullptr
         */
        ComponentPtr getComponent(const String &name) const
        {
            RTTRType type = RTTRType::get_by_name(name);
            return getComponent(type);
        }

        /**
         * \brief 获取第一个匹配类型的组件
         * \tparam [in] T : 组件类型
         * \return 命中返回组件，否则返回 nullptr
         */
        template <typename T>
        SmartPtr<T> getComponent() const
        {
            RTTRType type = RTTRType::get<T>();
            return smart_pointer_cast<T>(getComponent(type));
        }

        /**
         * \brief 获取所有匹配类名的组件
         * \param [in] name : 组件类名
         * \return 匹配列表（可能为空）
         */
        TArray<ComponentPtr> getComponents(const String &name) const
        {
            RTTRType type = RTTRType::get_by_name(name);
            return getComponents<Component>(type);
        }

        /**
         * \brief 获取所有匹配类型（含派生）的组件
         * \tparam [in] T : 组件类型
         * \return 匹配列表（可能为空）
         */
        template <typename T>
        TArray<SmartPtr<T>> getComponents() const
        {
            RTTRType type = RTTRType::get<T>();
            return getComponents<T>(type);
        }

        using ComponentsSet = TUnorderedMultimap<String, ComponentPtr>;

        /// 返回序列化用组件表（类名 → 组件）
        TPROPERTY(RTTRFuncName = "Components", RTTRFuncType = "getter")
        const ComponentsSet &getAllComponents() const { return mComponentObjects; }

    protected:
        /// 委托 GameObject("")，managed 默认 true
        GameObject() : GameObject("") {}

        /**
         * \brief 构造：设名称、生成 UUID、按 Settings 初始化更新槽；managed 时登记到当前 Scene
         * \param [in] name : 对象名称
         * \param [in] managed : 是否登记到当前场景
         */
        GameObject(const String &name, bool managed = true);

        /**
         * \brief 设置对象名称
         * \param [in] name : 新名称
         */
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }

        /**
         * \brief 按 RTTR 类型创建并挂载组件，写入更新队列并触发生命周期
         * \param [in] type : 组件类型，须派生自 Component；TransformNode 不可重复
         * \return 成功返回组件；类型非法 / 重复 Transform / 创建失败返回 nullptr
         * \remarks Behaviour：仅在播放态或 executeInEditMode 时 Awake + refreshActiveState，
         *          有 Scene 则 enqueuePendingStart，否则立即 Start；非 Behaviour：立即 onStart。
         */
        ComponentPtr addComponent(const RTTRType &type);

        /**
         * \brief 移除第一个精确匹配 type 的组件
         * \param [in] type : 组件类型
         * \return 成功返回 T3D_OK；未找到返回 T3D_ERR_NOT_FOUND
         * \remarks 已 Awake 的 Behaviour 先 invokeDisable；随后 erase 更新队列、unlink、入销毁队列。
         */
        TResult removeComponent(const RTTRType &type);

        /**
         * \brief 移除所有精确匹配 type 的组件
         * \param [in] type : 组件类型
         * \return 成功返回 T3D_OK；未找到返回 T3D_ERR_NOT_FOUND
         */
        TResult removeComponents(const RTTRType &type);

        /**
         * \brief 获取第一个精确匹配，或首个已挂载的派生类组件
         * \param [in] type : 组件类型
         * \return 命中返回组件，否则返回 nullptr
         */
        ComponentPtr getComponent(const RTTRType &type) const;

        /**
         * \brief 收集 type 及其派生类的全部组件实例
         * \tparam [in] T : 返回元素类型
         * \param [in] type : 匹配基类型
         * \return 匹配列表
         */
        template<typename T>
        TArray<SmartPtr<T>> getComponents(const RTTRType &type) const
        {
            TArray<SmartPtr<T>> components;

            for (const auto &item : mComponents)
            {
                if (item.first == type || item.first.is_derived_from(type))
                {
                    components.emplace_back(item.second);
                }
            }

            return components;
        }

        /**
         * \brief 将组件放入更新队列：优先精确匹配 Settings.updateOrders，Behaviour 派生落入 Behaviour 段，否则入无序队列
         * \param [in] type : 组件类型
         * \param [in] component : 组件裸指针
         */
        void putUpdatingQueue(const RTTRType &type, Component *component);

        /**
         * \brief 从有序 / 无序更新队列中摘除组件
         * \param [in] component : 待摘除组件
         * \note 队列存裸指针，销毁前必须先摘掉，否则后续 Update 会访问已释放对象
         */
        void eraseUpdatingQueue(Component *component);

        /**
         * \brief 断开同对象上其它组件对该组件的裸指针引用（当前清理 Renderable::renderBound）
         * \param [in] component : 即将被移除的组件
         */
        void unlinkComponentReferences(Component *component);

        /**
         * \brief 对象真正销毁时的回调；当前实现为空，可供派生类覆盖
         */
        virtual void onDestroy();

        /**
         * \brief 对本对象有序/无序更新队列中的组件调用 onUpdate
         * \remarks Behaviour 仅在 enabled 且（播放态或 executeInEditMode）时执行
         */
        void onUpdate();

        /**
         * \brief 对本对象更新队列中的 Behaviour 调用 onLateUpdate（受 behaviourExecutable 约束）
         */
        void onLateUpdate();

        /**
         * \brief 对本对象更新队列中的 Behaviour 调用 onFixedUpdate（受 behaviourExecutable 约束）
         */
        void onFixedUpdate();

        /**
         * \brief 对本对象全部 Behaviour 同步 Awake + OnEnable，并向 scene 投递 pending-start（scene 为空则立即 Start）
         * \param [in] scene : 目标场景，可为 nullptr
         * \remarks 未处于播放态且非 executeInEditMode 的 Behaviour 跳过，等进入 Play 再补发
         */
        void awakeBehaviours(Scene *scene);

        /**
         * \brief 将组件加入待销毁队列（已在队列则跳过）
         * \param [in] component : 待销毁组件
         */
        static void destroyComponent(Component *component);

        /**
         * \brief 将对象加入待销毁队列，并从当前 Scene 移除（已在队列则跳过）
         * \param [in] gameObject : 待销毁对象
         */
        static void destroyGameObject(GameObject *gameObject);

    private:
        /**
         * \brief 设置 UUID（序列化用）
         * \param [in] uuid : 新 UUID
         */
        TPROPERTY(RTTRFuncName="UUID", RTTRFuncType="setter")
        void setUUID(const UUID &uuid) { mUUID = uuid; }

        /**
         * \brief 克隆当前节点自身（不含子节点）：create + 克隆各组件并挂载，再 awakeBehaviours
         * \param [out] outMap : 非空时记录模板 UUID → 实例 Object
         * \return 不含子树的新 GameObject
         */
        GameObjectPtr cloneSelf(TemplateInstanceMap *outMap = nullptr) const;

        /**
         * \brief 反序列化设置组件表：赋值后 setupTransformNode + setupComponents
         * \param [in] components : 类名映射的组件集合
         */
        TPROPERTY(RTTRFuncName="Components", RTTRFuncType="setter")
        void setAllComponents(const ComponentsSet &components)
        {
            mComponentObjects = components;
            setupTransformNode();
            setupComponents();
        }

        /**
         * \brief 根据 mComponentObjects 重建 mComponents 与更新队列；可选触发非 Behaviour 的 onStart
         * \note Behaviour 的 Awake/Start 由 Scene::onPostLoad / awakeBehaviours 统一处理；
         *       编辑态（非 executeInEditMode）推迟到进入 Play
         */
        void setupComponents();

        /**
         * \brief 从 mComponentObjects 中缓存第一个 TransformNode 派生组件到 mTransformNode
         */
        void setupTransformNode();

        ComponentsSet mComponentObjects {};

    protected:
        UUID mUUID {};                              ///< 对象 UUID
        String mName {};                            ///< 对象名称
        bool mIsActive {true};                      ///< 自身激活标志，影响子树更新与渲染遍历
        uint32_t mCameraMask {0x1};                 ///< 相机可见掩码

        TransformNode *mTransformNode {nullptr};    ///< 缓存的 TransformNode，便于快速访问

        using Components = TUnorderedMultimap<RTTRType, ComponentPtr, RTTRTypeHash, RTTRTypeEqual>;

        Components  mComponents {};                 ///< 类型 → 组件表

        using ComponentList = TList<Component*>;
        using ComponentQueue = TMap<int32_t, ComponentList>;

        ComponentQueue mUpdateComponents {};        ///< 按 Settings.updateOrders 分槽的有序更新队列

        using ComponentQueue2 = TMultimap<String, Component*>;

        ComponentQueue2 mUpdateComponents2 {};      ///< 未列入 updateOrders 的无序更新队列

        using WaitingDestroyComponents = TList<ComponentPtr>;

        static WaitingDestroyComponents msWaitingDestroyComponents; ///< 待销毁组件队列

        using WaitingDestroyGameObjects = TList<GameObjectPtr>;

        static WaitingDestroyGameObjects msWaitingDestroyGameObjects; ///< 待销毁对象队列
    };
}


#endif    /*__T3D_GAME_OBJECT_H__*/
