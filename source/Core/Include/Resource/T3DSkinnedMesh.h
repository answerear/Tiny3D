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

#ifndef __T3D_SKINNED_MESH_H__
#define __T3D_SKINNED_MESH_H__


#include "Resource/T3DMesh.h"
#include "Component/T3DBone.h"


namespace Tiny3D
{
    /**
     * \brief 蒙皮网格资源，关联 Skeleton 与 SkeletalAnimation 并支持动态顶点
     */
    TCLASS()
    class T3D_ENGINE_API SkinnedMesh : public Mesh
    {
        TRTTI_ENABLE(Mesh)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 创建空白蒙皮网格资源
         * \param [in] name : 网格名称
         * \return 新 SkinnedMesh 智能指针
         */
        static SkinnedMeshPtr create(const String &name);

        /**
         * \brief 创建完整蒙皮网格并绑定骨架与动画资源
         * \param [in] name : 网格名称
         * \param [in] attributes : 顶点属性描述
         * \param [in] vertices : 顶点缓冲数据
         * \param [in] strides : 各顶点流 stride
         * \param [in] offsets : 各顶点流 offset
         * \param [in] submeshes : 子网格列表
         * \param [in] skeleton : 关联 Skeleton，不可为 nullptr
         * \param [in] skeletalAni : 关联 SkeletalAnimation，不可为 nullptr
         * \param [in] position : 网格节点局部位置
         * \param [in] orientation : 网格节点局部旋转
         * \param [in] scaling : 网格节点局部缩放
         * \param [in] meshNodeName : 网格节点名称
         * \return 新 SkinnedMesh 智能指针
         */
        static SkinnedMeshPtr create(const String &name,
            VertexAttributes &&attributes, Vertices &&vertices,
            VertexStrides &&strides, VertexOffsets &&offsets,
            SubMeshes &&submeshes, Skeleton *skeleton,
            SkeletalAnimation *skeletalAni, 
            const Vector3 &position, const Quaternion &orientation, const Vector3 &scaling,
            const String &meshNodeName);

        /// 析构 SkinnedMesh
        ~SkinnedMesh() override;

        /**
         * \brief 返回资源类型标识
         * \return Type::kSkinnedMesh
         */
        Type getType() const override;

        /// 获取关联 Skeleton 的 UUID（序列化用）
        TPROPERTY(RTTRFuncName="SkeletonUUID", RTTRFuncType="getter")
        const UUID &getSkeletonUUID() const
        {
            return mSkeletonUUID;
        }
        
        /// 获取关联 SkeletalAnimation 的 UUID（序列化用）
        TPROPERTY(RTTRFuncName="SkeletalAnimationUUID", RTTRFuncType="getter")
        const UUID &getSkeletalAnimationUUID() const
        {
            return mSkeletalAniUUID;
        }

        /// 获取运行时 Skeleton 对象
        Skeleton *getSkeleton() const { return mSkeleton; }
        
        /// 获取运行时 SkeletalAnimation 对象
        SkeletalAnimation *getSkeletalAnimation() const { return mSkeletalAni; }

#if defined (T3D_DEBUG)
        /**
         * \brief 输出骨骼层级与动画轨道调试信息
         * \return 调试文本字符串
         */
        String getDebugInfo() const override;
#endif

    protected:
        /// 默认构造
        SkinnedMesh() = default;
        
        /**
         * \brief 构造蒙皮网格资源
         * \param [in] name : 网格名称
         */
        SkinnedMesh(const String &name);

        /**
         * \brief 构造完整蒙皮网格
         * \param [in] name : 网格名称
         * \param [in] attributes : 顶点属性描述
         * \param [in] vertices : 顶点缓冲数据
         * \param [in] strides : 各顶点流 stride
         * \param [in] offsets : 各顶点流 offset
         * \param [in] submeshes : 子网格列表
         * \param [in] skeleton : 关联 Skeleton
         * \param [in] skeletalAni : 关联 SkeletalAnimation
         * \param [in] position : 网格节点局部位置
         * \param [in] orientation : 网格节点局部旋转
         * \param [in] scaling : 网格节点局部缩放
         * \param [in] meshNodeName : 网格节点名称
         */
        SkinnedMesh(const String &name, VertexAttributes &&attributes,
            Vertices &&vertices, VertexStrides &&strides,
            VertexOffsets &&offsets, SubMeshes &&submeshes, Skeleton *skeleton,
            SkeletalAnimation *skeletalAni,
            const Vector3 &position, const Quaternion &orientation, const Vector3 &scaling,
            const String &meshNodeName);
        
        /**
         * \brief 克隆蒙皮网格（当前 cloneProperties 为空，不复制骨架/动画引用）
         * \return 新 SkinnedMesh 智能指针
         */
        ResourcePtr clone() const override;

        /**
         * \brief 克隆属性（当前实现仅委托 Mesh::cloneProperties，不复制骨架/动画字段）
         * \param [in] src : 源 SkinnedMesh 资源
         */
        void cloneProperties(const Resource * const src) override;

        /**
         * \brief 资源创建回调，委托 Mesh::onCreate
         * \return Mesh::onCreate 返回值
         */
        TResult onCreate() override;
        
        /**
         * \brief 从 Archive 加载蒙皮网格
         * \param [in] archive : 资源档案
         * \return Mesh::onLoad 返回值
         */
        TResult onLoad(Archive *archive) override;

        /**
         * \brief 生成渲染资源并按 UUID 加载 Skeleton 与 SkeletalAnimation
         * \param [in] archive : 资源档案
         * \return Mesh::generateRenderResource 返回值
         */
        TResult generateRenderResource(Archive *archive) override;

    private:
        /// RTTR：设置 SkeletalAnimation UUID
        TPROPERTY(RTTRFuncName="SkeletalAnimationUUID", RTTRFuncType="setter")
        void setSkeletalAnimationUUID(const UUID &skeletalAniUUID)
        {
            mSkeletalAniUUID = skeletalAniUUID;
        }
        
        /// RTTR：设置 Skeleton UUID
        TPROPERTY(RTTRFuncName="SkeletonUUID", RTTRFuncType="setter")
        void setSkeletonUUID(const UUID &skeletonUUID)
        {
            mSkeletonUUID = skeletonUUID;
        }

        /// 蒙皮网格顶点数据随骨骼动画动态更新
        bool isDynamicVertices() const override { return true; }

#if defined (T3D_DEBUG)
        /// 构建骨骼关节层级树（调试用）
        void populateBoneHierarchy() const;
        /// 释放 mJoints 调试节点
        void clearJoints() const;
#endif
        
    protected:
        /// 关联 Skeleton 的 UUID
        UUID mSkeletonUUID {};
        /// 关联 SkeletalAnimation 的 UUID
        UUID mSkeletalAniUUID {};

        /// 运行时 Skeleton 对象
        SkeletonPtr mSkeleton {nullptr};
        /// 运行时 SkeletalAnimation 对象
        SkeletalAnimationPtr mSkeletalAni {nullptr};

    private:
#if defined (T3D_DEBUG)
        /// 骨骼层级是否需要重建
        mutable bool mIsBonesDirty {true};

        using JointChildren = TArray<uint16_t>;
        
        /// 调试用关节树节点
        struct JointNode
        {
            JointNode() = default;
            
            uint16_t joint {Bone::kInvalidIndex};
            JointChildren children {};
        };

        using Joints = TArray<JointNode*>;
        /// 调试用关节节点数组
        mutable Joints mJoints {};
        /// 调试树根关节索引
        mutable uint16_t mJointRootIdx {Bone::kInvalidIndex};
        /// 调试用所有骨骼 GameObject 列表
        mutable TArray<GameObject*> mAllBoneGOs {};

        /**
         * \brief 递归输出关节树调试字符串
         * \param [in] node : 当前关节节点
         * \param [in] tabCount : 缩进层级
         * \return 格式化调试文本
         */
        String getBoneDebugString(JointNode *node, int32_t tabCount) const;
#endif
    };
}


#endif    /*__T3D_SKINNED_MESH_H__*/
