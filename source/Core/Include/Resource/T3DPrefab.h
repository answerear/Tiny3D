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
     * \brief 预制体资源，保存 GameObject 子树并在运行时实例化为独立副本
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

        /// 析构 Prefab
        ~Prefab() override;

        /**
         * \brief 返回资源类型标识
         * \return Type::kPrefab
         */
        Type getType() const override;

        /**
         * \brief 实例化预制体，深拷贝根 GameObject 整棵子树
         * \return 克隆出的根 GameObject 智能指针；mRootGameObject 为 nullptr 时返回 nullptr
         */
        GameObjectPtr instantiate() const;
        
    protected:
        /**
         * \brief 构造预制体资源
         * \param [in] name : 预制体名称
         */
        Prefab(const String &name);
        
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

        /// 加载过程中需调用 onLoadResource 的组件集合
        using NeedToLoadResourceComponents = TSet<Component*>;
        NeedToLoadResourceComponents mNeedToLoadResourceComponents {};
    };
}


#endif    /*__T3D_PREFAB_H__*/
