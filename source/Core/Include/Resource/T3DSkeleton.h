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
     * \brief 骨架资源
     */
    TCLASS()
    class T3D_ENGINE_API Skeleton : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND
        
    public:
        static SkeletonPtr create(const String &name);

        static SkeletonPtr create(const String &name, GameObjectPtr rootBoneGameObject);

        /**
         * \brief 析构函数
         */
        ~Skeleton() override;

        Type getType() const override;

        /**
         * \brief 获取骨骼根节点 GameObject（运行时使用，不序列化）
         */
        GameObject *getRootBoneGameObject() const
        {
            return mRootBoneGameObject;
        }

    protected:
        Skeleton() = default;
        
        Skeleton(const String &name);

        Skeleton(const String &name, GameObjectPtr rootBoneGameObject);
        
        ResourcePtr clone() const override;

        void cloneProperties(const Resource * const src) override;

        TResult onCreate() override;
        
        TResult onLoad(Archive *archive) override;
        
        TResult onUnload() override;

        void onPreSave() override;

        void onPostLoad() override;
        
    private:
        void setRootBoneGameObject(GameObjectPtr rootBoneGameObject)
        {
            mRootBoneGameObject = rootBoneGameObject;
        }

        TPROPERTY(RTTRFuncName="BoneGameObjects", RTTRFuncType="getter")
        const GameObjects &getBoneGameObjects() const { return mBoneGameObjects; }

        TPROPERTY(RTTRFuncName="BoneGameObjects", RTTRFuncType="setter")
        void setBoneGameObjects(const GameObjects &boneGameObjects) { mBoneGameObjects = boneGameObjects; }

        TPROPERTY(RTTRFuncName="RootBoneUUID", RTTRFuncType="getter")
        const UUID &getRootBoneUUID() const { return mRootBoneUUID; }

        TPROPERTY(RTTRFuncName="RootBoneUUID", RTTRFuncType="setter")
        void setRootBoneUUID(const UUID &uuid) { mRootBoneUUID = uuid; }
        
    protected:
        /// 骨骼根节点 GameObject（运行时使用，不序列化）
        GameObjectPtr mRootBoneGameObject;
        /// 骨骼子树所有节点的扁平表（序列化用）
        GameObjects mBoneGameObjects {};
        /// 骨骼根节点 UUID（序列化用）
        UUID mRootBoneUUID {};
    };
}


#endif    /*__T3D_SKELETON_H__*/
