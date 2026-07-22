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

#ifndef __T3D_MESH_H__
#define __T3D_MESH_H__


#include "Resource/T3DResource.h"
#include "Kernel/T3DConstant.h"
#include "Resource/T3DSubMesh.h"
#include "Render/T3DVertexAttribute.h"
#include "Bound/T3DBound.h"


namespace Tiny3D
{
    /**
     * \brief 网格资源
     */
    TCLASS()
    class T3D_ENGINE_API Mesh : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 给加载 mesh 资源时用来创建 mesh 对象
         * \param [in] name : mesh 名称
         * \return 调用成功返回一个 mesh 对象
         */
        static MeshPtr create(const String &name);

        /**
         * \brief 给创建 mesh 资源时用来创建 mesh 对象
         * \param [in] name : mesh 名称 
         * \param [in] attributes : 顶点属性，也就是顶点定义
         * \param [in] vertices : 顶点数据
         * \param [in] strides : 每个顶点的跨度，也就是顶点大小
         * \param [in] offsets : 顶点在顶点缓冲区的偏移
         * \param [in] submeshes : 子 mesh 对象
         * \param [in] position : mesh 位置
         * \param [in] orientation : mesh 旋转
         * \param [in] scaling : mesh 缩放
         * \return 调用成功返回一个 mesh 对象
         */
        static MeshPtr create(const String &name, VertexAttributes &&attributes, Vertices &&vertices, VertexStrides &&strides, VertexOffsets &&offsets, SubMeshes &&submeshes, 
            const Vector3 &position = Vector3::ZERO, const Quaternion &orientation = Quaternion::IDENTITY, const Vector3 &scaling = Vector3::UNIT_SCALE,
            const String &meshNodeName = "");

        /**
         * \brief 析构函数
         */
        ~Mesh() override;

        Type getType() const override;

        TPROPERTY(RTTRFuncName="Vertices", RTTRFuncType="getter")
        const Vertices &getVertices() const { return mVertices; }

        TPROPERTY(RTTRFuncName="Vertices", RTTRFuncType="setter")
        void setVertices(const Vertices &vertices);

        TPROPERTY(RTTRFuncName="VertexAttributes", RTTRFuncType="getter")
        const VertexAttributes &getVertexAttributes() const { return mVertexAttributes; }

        TPROPERTY(RTTRFuncName="SubMeshes", RTTRFuncType="getter")
        const SubMeshes &getSubMeshes() const { return mSubMeshes; }

        TPROPERTY(RTTRFuncName="VertexStrides", RTTRFuncType="getter")
        const VertexStrides &getVertexStrides() const { return mVertexStrides; }

        TPROPERTY(RTTRFuncName="VertexOffsets", RTTRFuncType="getter")
        const VertexOffsets &getVertexOffsets() const { return mVertexOffsets; }
        
        TPROPERTY(RTTRFuncName="MeshPosition", RTTRFuncType="getter")
        const Vector3 &getMeshPosition() const { return mMeshT; }

        TPROPERTY(RTTRFuncName="MeshOrientation", RTTRFuncType="getter")
        const Quaternion &getMeshOrientation() const { return mMeshQ; }

        TPROPERTY(RTTRFuncName="MeshScaling", RTTRFuncType="getter")
        const Vector3 &getMeshScaling() const { return mMeshS; }

        TPROPERTY(RTTRFuncName="MeshNodeName", RTTRFuncType="getter")
        const String &getMeshNodeName() const { return mMeshNodeName; }

        /// 获取包围体种子类型（局部空间，实例化时用于播种 Bound 组件的默认值）
        TPROPERTY(RTTRFuncName="BoundSeedType", RTTRFuncType="getter")
        Bound::Type getBoundSeedType() const { return mBoundSeedType; }

        /// 获取包围体种子标量 A：Sphere=center，Aabb=min，Capsule=端点0
        TPROPERTY(RTTRFuncName="BoundSeedA", RTTRFuncType="getter")
        const Vector3 &getBoundSeedA() const { return mBoundSeedA; }

        /// 获取包围体种子标量 B：Aabb=max，Capsule=端点1（Sphere 不使用）
        TPROPERTY(RTTRFuncName="BoundSeedB", RTTRFuncType="getter")
        const Vector3 &getBoundSeedB() const { return mBoundSeedB; }

        /// 获取包围体种子半径：Sphere/Capsule 有效（Aabb 不使用）
        TPROPERTY(RTTRFuncName="BoundSeedRadius", RTTRFuncType="getter")
        Real getBoundSeedRadius() const { return mBoundSeedRadius; }

        /**
         * \brief 设置包围体种子（局部空间）
         * \param [in] type : 包围体类型
         * \param [in] a : Sphere=center / Aabb=min / Capsule=端点0
         * \param [in] b : Aabb=max / Capsule=端点1（Sphere 不使用）
         * \param [in] radius : Sphere/Capsule 半径（Aabb 不使用）
         */
        void setBoundSeed(Bound::Type type, const Vector3 &a, const Vector3 &b, Real radius);

        VertexDeclarationPtr getVertexDeclaration() const { return mVertexDecl; }

        const VertexBuffers &getVertexBuffers() const { return mVBuffers; }

        SubMesh *getSubMesh(const String &name) const;

        const VertexAttribute &getVertexAttribute(size_t index) const;

        const VertexAttribute &addVertexAttribute(uint32_t slot, uint32_t offset, VertexAttribute::Type type, VertexAttribute::Semantic semantic, uint32_t semanticIndex);

        const VertexAttribute &insertVertexAttribute(uint32_t pos, uint32_t slot, uint32_t offset, VertexAttribute::Type type, VertexAttribute::Semantic semantic, uint32_t semanticIndex);

        TResult addVertexAttribute(const VertexAttribute &vertexAttribute);

        TResult insertVertexAttribute(uint32_t pos, const VertexAttribute &vertexAttribute);

        TResult removeVertexAttribute(uint32_t pos);

        TResult removeVertexAttribute(VertexAttribute::Semantic semantic, uint32_t semanticIndex);

        TResult removeAllVertexAttributes();

        TResult updateVertexAttribute(uint32_t pos, uint32_t slot, uint32_t offset, VertexAttribute::Type type, VertexAttribute::Semantic semantic, uint32_t semanticIndex);

        const VertexAttribute *findVertexAttributeBySemantic(VertexAttribute::Semantic semantic, uint32_t semanticIndex) const;

        void flushVertexAttributes();

        uint32_t getVertexStride(uint32_t slot) const { return mVertexStrides[slot]; }

        uint32_t getVertexOffset(uint32_t slot) const { return mVertexOffsets[slot]; }

#if defined(T3D_DEBUG)
        virtual String getDebugInfo() const;
#endif
        
    protected:
        Mesh() = default;
        
        Mesh(const String &name);

        Mesh(const String &name, VertexAttributes &&attributes, Vertices &&vertices, VertexStrides &&strides, VertexOffsets &&offsets, SubMeshes &&submeshes,
            const Vector3 &position = Vector3::ZERO, const Quaternion &orientation = Quaternion::IDENTITY, const Vector3 &scaling = Vector3::UNIT_SCALE,
            const String &meshNodeName = "");
        
        ResourcePtr clone() const override;

        void cloneProperties(const Resource * const src) override;

        TResult onCreate() override;
        
        TResult onLoad(Archive *archive) override;

        TPROPERTY(RTTRFuncName="VertexAttributes", RTTRFuncType="setter")
        void setVertexAttributes(const VertexAttributes &attributes) { mVertexAttributes = attributes; }

        TPROPERTY(RTTRFuncName="SubMeshes", RTTRFuncType="setter")
        void setSubMeshes(const SubMeshes &submeshes) { mSubMeshes = submeshes; }

        TPROPERTY(RTTRFuncName="VertexStrides", RTTRFuncType="setter")
        void setVertexStrides(const VertexStrides &strides) { mVertexStrides = strides; }

        TPROPERTY(RTTRFuncName="VertexOffsets", RTTRFuncType="setter")
        void setVertexOffsets(const VertexOffsets &offsets) { mVertexOffsets = offsets; }
        
        TPROPERTY(RTTRFuncName="MeshPosition", RTTRFuncType="setter")
        void setMeshPosition(const Vector3 &position) { mMeshT = position; }
        
        TPROPERTY(RTTRFuncName="MeshOrientation", RTTRFuncType="setter")
        void setMeshOrientation(const Quaternion &orientation) { mMeshQ = orientation; }
        
        TPROPERTY(RTTRFuncName="MeshScaling", RTTRFuncType="setter")
        void setMeshScaling(const Vector3 &scaling) { mMeshS = scaling; }

        TPROPERTY(RTTRFuncName="MeshNodeName", RTTRFuncType="setter")
        void setMeshNodeName(const String &name) { mMeshNodeName = name; }

        TPROPERTY(RTTRFuncName="BoundSeedType", RTTRFuncType="setter")
        void setBoundSeedType(Bound::Type type) { mBoundSeedType = type; }

        TPROPERTY(RTTRFuncName="BoundSeedA", RTTRFuncType="setter")
        void setBoundSeedA(const Vector3 &a) { mBoundSeedA = a; }

        TPROPERTY(RTTRFuncName="BoundSeedB", RTTRFuncType="setter")
        void setBoundSeedB(const Vector3 &b) { mBoundSeedB = b; }

        TPROPERTY(RTTRFuncName="BoundSeedRadius", RTTRFuncType="setter")
        void setBoundSeedRadius(Real radius) { mBoundSeedRadius = radius; }

        virtual TResult generateRenderResource(Archive *archive);

        virtual bool isDynamicVertices() const { return false; }
        
    protected:
        /// 顶点定义
        VertexAttributes mVertexAttributes {};
        /// 顶点数据
        Vertices  mVertices {};
        /// 顶点数据大小
        VertexStrides   mVertexStrides {};
        /// 顶点数据偏移
        VertexOffsets   mVertexOffsets {};

        /// 子网格
        SubMeshes mSubMeshes {};

        /// 渲染用的顶点声明
        VertexDeclarationPtr    mVertexDecl {nullptr};
        /// 渲染用的顶点缓冲区
        VertexBuffers mVBuffers {nullptr};
        
        String mMeshNodeName {};
        Vector3 mMeshT {Vector3::ZERO};
        Quaternion mMeshQ {Quaternion::IDENTITY};
        Vector3 mMeshS {Vector3::UNIT_SCALE};

        /// 包围体种子（局部空间）：转换期算好、实例化时用于播种 Bound 组件的默认值。
        /// 一旦 GameObject 已挂 Bound 组件，则以组件为准，此种子不再使用。
        /// 语义随 mBoundSeedType 而定（见 setBoundSeed 注释）。
        Bound::Type mBoundSeedType {Bound::Type::NONE};
        Vector3     mBoundSeedA {Vector3::ZERO};
        Vector3     mBoundSeedB {Vector3::ZERO};
        Real        mBoundSeedRadius {Real(0.0)};
        
        /// 顶点属性是否需要更新
        bool    mIsAttrDirty {false};
        /// 创建的时候是否传递了数据进来，用于是否创建渲染对象
        bool    mCreatedFromData {false};
    };
}


#endif    /*__T3D_MESH_H__*/
