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

#ifndef __T3D_PREFAB_H__
#define __T3D_PREFAB_H__


#include "Resource/T3DResource.h"
#include "Kernel/T3DGameObject.h"


namespace Tiny3D
{
    /**
     * \brief 预制体资源，保存 GameObject 子树并在运行时实例化为链接实例
     */
    TCLASS()
    class T3D_ENGINE_API Prefab : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 创建 Prefab 资源
         * \param [in] name : 预制体名称
         * \return 新创建的 Prefab 智能指针
         */
        static PrefabPtr create(const String &name);

        /**
         * \brief 创建并绑定根 GameObject 的 Prefab 资源
         * \param [in] name : 预制体名称
         * \param [in] root : 预制体根 GameObject
         * \return 新创建的 Prefab 智能指针
         */
        static PrefabPtr create(const String &name, GameObjectPtr root);

        /// 析构 Prefab
        ~Prefab() override;

        /**
         * \brief 返回资源类型标识
         * \return Type::kPrefab
         */
        Type getType() const override;

        /**
         * \brief 获取运行时根 GameObject
         * \return 根节点；未设置时为 nullptr
         */
        GameObject *getRootGameObject() const { return mRootGameObject; }

        /**
         * \brief 设置运行时根 GameObject
         * \param [in] root : 预制体根节点
         */
        void setRootGameObject(GameObjectPtr root);

        /**
         * \brief 实例化预制体为带 PrefabInstance 链接的副本（不挂场景）
         * \return 克隆出的根 GameObject；mRootGameObject 为 nullptr 时返回 nullptr
         */
        GameObjectPtr instantiate() const;

        /**
         * \brief 实例化预制体并挂入场景
         * \param [in] scene : 目标场景，不可为 nullptr
         * \param [in] parent : 父节点；为空则挂到场景根 Transform
         * \return 实例根 GameObject；失败返回 nullptr
         */
        GameObjectPtr instantiate(Scene *scene, TransformNode *parent = nullptr) const;

        /**
         * \brief 深拷贝子树且不建立 PrefabInstance 链接
         * \return 无链接的根 GameObject
         */
        GameObjectPtr instantiateUnlinked() const;

        /**
         * \brief Prefab Variant 的基 Prefab UUID；非 Variant 时为 INVALID
         */
        TPROPERTY(RTTRFuncName="BasePrefabUUID", RTTRFuncType="getter")
        const UUID &getBasePrefabUUID() const { return mBasePrefabUUID; }

        TPROPERTY(RTTRFuncName="BasePrefabUUID", RTTRFuncType="setter")
        void setBasePrefabUUID(const UUID &uuid) { mBasePrefabUUID = uuid; }

        /// 是否为 Prefab Variant
        bool isVariant() const { return mBasePrefabUUID != UUID::INVALID; }
        
    protected:
        /**
         * \brief 构造预制体资源
         * \param [in] name : 预制体名称
         */
        Prefab(const String &name);

        /**
         * \brief 构造并绑定根 GameObject
         * \param [in] name : 预制体名称
         * \param [in] root : 根 GameObject
         */
        Prefab(const String &name, GameObjectPtr root);
        
        /**
         * \brief 克隆预制体资源（复制属性到新实例）
         * \return 新 Prefab 智能指针
         */
        ResourcePtr clone() const override;

        /**
         * \brief 从 src 复制 GameObjects、RootGameObjectUUID 及运行时根节点引用
         * \param [in] src : 源 Prefab 资源
         */
        void cloneProperties(const Resource * const src) override;

        /**
         * \brief 加载完成后，对 mNeedToLoadResourceComponents 中各组件调用 onLoadResource
         * \param [in] archive : 资源档案
         * \return 基类 onLoad 成功返回 T3D_OK
         */
        TResult onLoad(Archive *archive) override;

        /**
         * \brief 卸载时销毁预制体子树，断开 TransformNode 父子互持形成的引用环
         * \return 基类 onUnload 的返回值
         */
        TResult onUnload() override;

        /**
         * \brief 保存前将 mRootGameObject 子树收集到 mGameObjects 扁平表并记录根 UUID
         * \remarks mRootGameObject 为 nullptr 时跳过并输出警告
         */
        void onPreSave() override;

        /**
         * \brief 加载后为各 GameObject 调用 setupHierarchy，并按 RootGameObjectUUID 恢复 mRootGameObject
         * \remarks UUID 在 mGameObjects 中找不到时 mRootGameObject 置为 nullptr
         */
        void onPostLoad() override;

        /**
         * \brief 将需延迟加载资源的组件加入 mNeedToLoadResourceComponents
         * \param [in] component : 待加载资源的组件
         */
        void onAddComponentForLoadingResource(Component *component) override;

    private:
        /// 默认构造，委托 Prefab("")，反序列化时由 RTTR 调用
        Prefab() : Prefab("") {}

        /// RTTR 序列化：获取预制体子树扁平表
        TPROPERTY(RTTRFuncName="GameObjects", RTTRFuncType="getter")
        const GameObjects &getGameObjects() const { return mGameObjects; }

        /// RTTR 序列化：设置预制体子树扁平表
        TPROPERTY(RTTRFuncName="GameObjects", RTTRFuncType="setter")
        void setGameObjects(const GameObjects &gameObjects) { mGameObjects = gameObjects; }

        /// RTTR 序列化：获取根节点 UUID
        TPROPERTY(RTTRFuncName="RootGameObjectUUID", RTTRFuncType="getter")
        const UUID &getRootGameObjectUUID() const { return mRootGameObjectUUID; }

        /// RTTR 序列化：设置根节点 UUID
        TPROPERTY(RTTRFuncName="RootGameObjectUUID", RTTRFuncType="setter")
        void setRootGameObjectUUID(const UUID &uuid) { mRootGameObjectUUID = uuid; }

    protected:
        /// 预制体子树所有节点的扁平表（序列化用）
        GameObjects mGameObjects {};
        /// 预制体根节点 UUID（序列化用）
        UUID mRootGameObjectUUID {};
        /// 预制体根节点 GameObject（运行时使用，不序列化）
        GameObjectPtr mRootGameObject {nullptr};
        /// Variant 基 Prefab UUID（非 Variant 为 INVALID）
        UUID mBasePrefabUUID {};

        /// 加载过程中需调用 onLoadResource 的组件集合
        using NeedToLoadResourceComponents = TSet<Component*>;
        NeedToLoadResourceComponents mNeedToLoadResourceComponents {};

        GameObjectPtr instantiateInternal(bool linked) const;
        void setupPrefabInstanceLink(GameObject *instanceRoot,
            const GameObject::TemplateInstanceMap &map) const;
    };
}


#endif    /*__T3D_PREFAB_H__*/
