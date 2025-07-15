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


namespace Tiny3D
{
    /**
     * \brief 蒙皮网格资源
     */
    TCLASS()
    class T3D_ENGINE_API SkinnedMesh : public Mesh
    {
        TRTTI_ENABLE(Mesh)
        TRTTI_FRIEND
        
    public:
        static SkinnedMeshPtr create(const String &name);

        static SkinnedMeshPtr create(const String &name,
            VertexAttributes &&attributes, Vertices &&vertices,
            VertexStrides &&strides, VertexOffsets &&offsets,
            SubMeshes &&submeshes, SkeletalAnimation *skeletalAni,
            Bones &&bones);

        /**
         * \brief 析构函数
         */
        ~SkinnedMesh() override;

        Type getType() const override;

        TPROPERTY(RTTRFuncName="SkeletalAnimationUUID", RTTRFuncType="getter")
        const UUID &getSkeletalAnimationUUID() const
        {
            return mSkeletalAniUUID;
        }

        TPROPERTY(RTTRFuncName="Bones", RTTRFuncType="getter")
        const Bones &getBones() const
        {
            return mBones;
        }

        SkeletalAnimation *getSkeletalAnimation() const
        {
            return mSkeletalAni;
        }

    protected:
        SkinnedMesh() = default;
        
        SkinnedMesh(const String &name);

        SkinnedMesh(const String &name, VertexAttributes &&attributes,
            Vertices &&vertices, VertexStrides &&strides,
            VertexOffsets &&offsets, SubMeshes &&submeshes,
            SkeletalAnimation *skeletalAni, Bones &&bones);
        
        ResourcePtr clone() const override;

        void cloneProperties(const Resource * const src) override;

        TResult onCreate() override;
        
        TResult onLoad(Archive *archive) override;

        TResult generateRenderResource(Archive *archive) override;

    private:
        TPROPERTY(RTTRFuncName="SkeletalAnimationUUID", RTTRFuncType="setter")
        void setSkeletalAnimationUUID(const UUID &skeletalAniUUID)
        {
            mSkeletalAniUUID = skeletalAniUUID;
        }
        
        TPROPERTY(RTTRFuncName="Bones", RTTRFuncType="setter")
        void setBones(const Bones &bones)
        {
            mBones = bones;
        }
        
    protected:
        /// 骨骼动画数据资源 UUID
        UUID mSkeletalAniUUID {};

        /// 骨骼 hierarchy
        Bones mBones {};

        /// 骨骼动画数据对象
        SkeletalAnimationPtr mSkeletalAni {nullptr};
    };
}


#endif    /*__T3D_SKINNED_MESH_H__*/
