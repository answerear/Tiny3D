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

#ifndef __T3D_SKELETON_H__
#define __T3D_SKELETON_H__


#include "Resource/T3DResource.h"
#include "Kernel/T3DGameObject.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief 骨架资源，保存骨骼 GameObject 子树及根节点引用
     */
    TCLASS()
    class T3D_ENGINE_API Skeleton : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 创建空白骨架资源
         * \param [in] name : 骨架名称
         * \return 新 Skeleton 智能指针
         */
        static SkeletonPtr create(const String &name);

        /**
         * \brief 创建并绑定根骨骼 GameObject 的骨架资源
         * \param [in] name : 骨架名称
         * \param [in] rootBoneGameObject : 根骨骼 GameObject
         * \return 新 Skeleton 智能指针
         */
        static SkeletonPtr create(const String &name, GameObjectPtr rootBoneGameObject);

        /// 析构 Skeleton
        ~Skeleton() override;

        /**
         * \brief 返回资源类型标识
         * \return Type::kSkeleton
         */
        Type getType() const override;

        /**
         * \brief 获取骨骼根 GameObject（运行时使用，不序列化）
         * \return 根骨骼 GameObject 裸指针
         */
        GameObject *getRootBoneGameObject() const
        {
            return mRootBoneGameObject;
        }

    protected:
        /// 默认构造
        Skeleton() = default;
        
        /**
         * \brief 构造骨架资源
         * \param [in] name : 骨架名称
         */
        Skeleton(const String &name);

        /**
         * \brief 构造并绑定根骨骼 GameObject
         * \param [in] name : 骨架名称
         * \param [in] rootBoneGameObject : 根骨骼 GameObject
         */
        Skeleton(const String &name, GameObjectPtr rootBoneGameObject);
        
        /**
         * \brief 克隆骨架并复制属性
         * \return 新 Skeleton 智能指针
         */
        ResourcePtr clone() const override;

        /**
         * \brief 从 src 复制根骨骼引用、BoneGameObjects 及 RootBoneUUID
         * \param [in] src : 源 Skeleton 资源
         */
        void cloneProperties(const Resource * const src) override;

        /**
         * \brief 资源创建回调，委托基类 onCreate
         * \return 基类 onCreate 返回值
         */
        TResult onCreate() override;
        
        /**
         * \brief 从 Archive 加载骨架资源
         * \param [in] archive : 资源档案
         * \return 基类 onLoad 返回值
         */
        TResult onLoad(Archive *archive) override;
        
        /**
         * \brief 卸载骨架：清空 BoneGameObjects 并销毁根骨骼 GameObject
         * \return 基类 onUnload 返回值
         */
        TResult onUnload() override;

        /**
         * \brief 保存前将 mRootBoneGameObject 子树收集到 mBoneGameObjects 并记录根 UUID
         * \remarks mRootBoneGameObject 为 nullptr 时跳过并输出警告
         */
        void onPreSave() override;

        /**
         * \brief 加载后为各骨骼 GameObject 调用 setupHierarchy，并按 RootBoneUUID 恢复 mRootBoneGameObject
         * \remarks UUID 在 mBoneGameObjects 中找不到时 mRootBoneGameObject 置为 nullptr
         */
        void onPostLoad() override;
        
    private:
        /**
         * \brief 设置运行时根骨骼 GameObject
         * \param [in] rootBoneGameObject : 根骨骼 GameObject
         */
        void setRootBoneGameObject(GameObjectPtr rootBoneGameObject)
        {
            mRootBoneGameObject = rootBoneGameObject;
        }

        /// RTTR 序列化：获取骨骼子树扁平表
        TPROPERTY(RTTRFuncName="BoneGameObjects", RTTRFuncType="getter")
        const GameObjects &getBoneGameObjects() const { return mBoneGameObjects; }

        /// RTTR 序列化：设置骨骼子树扁平表
        TPROPERTY(RTTRFuncName="BoneGameObjects", RTTRFuncType="setter")
        void setBoneGameObjects(const GameObjects &boneGameObjects) { mBoneGameObjects = boneGameObjects; }

        /// RTTR 序列化：获取根骨骼 UUID
        TPROPERTY(RTTRFuncName="RootBoneUUID", RTTRFuncType="getter")
        const UUID &getRootBoneUUID() const { return mRootBoneUUID; }

        /// RTTR 序列化：设置根骨骼 UUID
        TPROPERTY(RTTRFuncName="RootBoneUUID", RTTRFuncType="setter")
        void setRootBoneUUID(const UUID &uuid) { mRootBoneUUID = uuid; }
        
    protected:
        /// 骨骼根 GameObject（运行时使用，不序列化）
        GameObjectPtr mRootBoneGameObject;
        /// 骨骼子树所有节点的扁平表（序列化用）
        GameObjects mBoneGameObjects {};
        /// 骨骼根节点 UUID（序列化用）
        UUID mRootBoneUUID {};
    };
}


#endif    /*__T3D_SKELETON_H__*/
