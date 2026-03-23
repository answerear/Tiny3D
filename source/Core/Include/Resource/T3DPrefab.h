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
    TCLASS()
    class T3D_ENGINE_API Prefab : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND
        
    public:
        static PrefabPtr create(const String &name);

        ~Prefab() override;

        Type getType() const override;

        /**
         * \brief 实例化 Prefab，深拷贝整棵子树并返回新的根节点
         * \return 返回克隆出的根节点 GameObjectPtr，若 mRootGameObject 为 nullptr 则返回 nullptr
         */
        GameObjectPtr instantiate() const;
        
    protected:
        Prefab(const String &name);
        
        ResourcePtr clone() const override;

        void cloneProperties(const Resource * const src) override;

        TResult onLoad(Archive *archive) override;

        void onPreSave() override;

        void onPostLoad() override;

        void onAddComponentForLoadingResource(Component *component) override;

    private:
        TPROPERTY(RTTRFuncName="GameObjects", RTTRFuncType="getter")
        const GameObjects &getGameObjects() const { return mGameObjects; }

        TPROPERTY(RTTRFuncName="GameObjects", RTTRFuncType="setter")
        void setGameObjects(const GameObjects &gameObjects) { mGameObjects = gameObjects; }

        TPROPERTY(RTTRFuncName="RootGameObjectUUID", RTTRFuncType="getter")
        const UUID &getRootGameObjectUUID() const { return mRootGameObjectUUID; }

        TPROPERTY(RTTRFuncName="RootGameObjectUUID", RTTRFuncType="setter")
        void setRootGameObjectUUID(const UUID &uuid) { mRootGameObjectUUID = uuid; }

    protected:
        /// 预制体子树所有节点的扁平表（序列化用）
        GameObjects mGameObjects {};
        /// 预制体根节点 UUID（序列化用）
        UUID mRootGameObjectUUID {};
        /// 预制体根节点 GameObject（运行时使用，不序列化）
        GameObjectPtr mRootGameObject {nullptr};

        // Component* : 组件对象
        using NeedToLoadResourceComponents = TSet<Component*>;
        NeedToLoadResourceComponents mNeedToLoadResourceComponents {};
    };
}


#endif    /*__T3D_PREFAB_H__*/
