/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, publish, distribute, sublicense, and/or sell
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

#ifndef __T3D_SKINNED_GEOMETRY_H__
#define __T3D_SKINNED_GEOMETRY_H__


#include "Component/T3DGeometry.h"
#include "Animation/T3DAnimationPlayer.h"


namespace Tiny3D
{
    /**
     * \brief 蒙皮网格渲染组件，克隆骨骼子树并驱动 AnimationPlayer 播放动画
     */
    TCLASS()
    class T3D_ENGINE_API SkinnedGeometry : public Geometry
    {
        TRTTI_ENABLE(Geometry)
        TRTTI_FRIEND
        
    public:
        /// 析构并释放 AnimationPlayer
        ~SkinnedGeometry() override;

        /**
         * \brief 克隆当前 SkinnedGeometry
         * \return 克隆成功返回新组件；cloneProperties 失败返回 nullptr
         */
        ComponentPtr clone() const override;
    
        /**
         * \brief 从 SkinnedMesh 骨架模板克隆运行时骨骼子树并建立索引映射
         * \return 成功返回 T3D_OK；SkinnedMesh、Skeleton 或克隆失败返回 T3D_ERR_RES_INVALID_OBJECT
         * \remarks 若已有旧骨骼子树会先清理；克隆根挂接到宿主 TransformNode 的父节点下
         */
        TResult populateAllChildren();

        /// 返回骨骼名称到 GameObject 的映射表
        const BoneGameObjectsMap &getAllBones() const { return mAllBones; }

        /// 返回按模板 DFS 顺序排列的骨骼 GameObject 数组
        const BoneGameObjects &getBoneGameObjects() const { return mBoneGameObjects; }

        /// 返回默认动画剪辑名称
        TPROPERTY(RTTRFuncName="DefaultClip", RTTRFuncType="getter")
        const String &getDefaultClipName() const { return mDefaultClipName; }

        /**
         * \brief 设置默认动画剪辑名称
         * \param [in] name : 剪辑名称
         */
        TPROPERTY(RTTRFuncName="DefaultClip", RTTRFuncType="setter")
        void setDefaultClipName(const String &name) { mDefaultClipName = name; }

        /// 是否使用 GPU 蒙皮
        TPROPERTY(RTTRFuncName="GPUSkinning", RTTRFuncType="getter")
        bool isGPUSkinning() const { return mIsGPUSkinning; }

        /**
         * \brief 设置是否使用 GPU 蒙皮
         * \param [in] isGPUSkinning : GPU 蒙皮开关
         */
        TPROPERTY(RTTRFuncName="GPUSkinning", RTTRFuncType="setter")
        void setGPUSkinning(bool isGPUSkinning) { mIsGPUSkinning = isGPUSkinning; }

        /**
         * \brief 播放指定动画剪辑
         * \param [in] clipName : 剪辑名称
         * \param [in] isLoop : 是否循环
         * \return 当前实现恒返回 true
         */
        bool play(const String &clipName, bool isLoop);
        
        /**
         * \brief 停止当前播放
         * \return 恒返回 true
         */
        bool stop();
        
    protected:
        /// 默认构造并创建 AnimationPlayer
        SkinnedGeometry() = default;

        /**
         * \brief 以指定 UUID 构造并创建 AnimationPlayer
         * \param [in] uuid : 组件唯一标识
         */
        SkinnedGeometry(const UUID &uuid);

        /**
         * \brief 从源 SkinnedGeometry 拷贝默认剪辑名与 GPU 蒙皮开关
         * \param [in] src : 源组件
         * \return 成功返回 T3D_OK
         */
        TResult cloneProperties(const Component * const src) override;

        /**
         * \brief 反序列化完成后回调（调用 Geometry::onPostLoad）
         */
        void onPostLoad() override;

        /**
         * \brief 加载 Mesh 资源后调用 populateAllChildren 构建骨骼子树
         * \param [in] archive : 资源包
         */
        void onLoadResource(Archive *archive) override;

        /**
         * \brief 启动时调用 Geometry::onStart
         */
        void onStart() override;

        /**
         * \brief 每帧驱动 AnimationPlayer 更新
         */
        void onUpdate() override;

        /**
         * \brief 销毁骨骼子树并清理映射表
         */
        void onDestroy() override;
        
        /**
         * \brief 调用 Geometry::generateRenderMaterial
         */
        void generateRenderMaterial() override;
        
    protected:
        /// 动画播放器
        AnimationPlayerPtr mAnimationPlayer {nullptr};

        /// 克隆出的骨骼根节点 GameObject（该实例独占的运行时骨骼副本）
        GameObjectPtr mRootBoneGameObject {nullptr};

        /// 骨骼名称到 GameObject 的映射
        BoneGameObjectsMap mAllBones {};
        /// 按模板 DFS 顺序排列的骨骼 GameObject 数组
        BoneGameObjects mBoneGameObjects {};

        /// 默认动画剪辑名称
        String mDefaultClipName {};
        
        /// 当前播放会话 ID
        ID mPlaybackID {AnimationPlayer::INVALID_ID};
        
        /// 是否 GPU 蒙皮
        bool mIsGPUSkinning {false};
    };
}


#endif  /*__T3D_SKINNED_GEOMETRY_H__*/
