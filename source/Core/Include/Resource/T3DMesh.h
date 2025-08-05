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
         * \return 调用成功返回一个 mesh 对象
         */
        static MeshPtr create(const String &name, VertexAttributes &&attributes, Vertices &&vertices, VertexStrides &&strides, VertexOffsets &&offsets, SubMeshes &&submeshes);

        /**
         * \brief 析构函数
         */
        ~Mesh() override;

        Type getType() const override;

        TPROPERTY(RTTRFuncName="Vertices", RTTRFuncType="getter")
        const Vertices &getVertices() const { return mVertices; }

        TPROPERTY(RTTRFuncName="Vertices", RTTRFuncType="setter")
        void setVertices(const Vertices &vertices) { mVertices = vertices; }

        TPROPERTY(RTTRFuncName="VertexAttributes", RTTRFuncType="getter")
        const VertexAttributes &getVertexAttributes() const { return mVertexAttributes; }

        TPROPERTY(RTTRFuncName="SubMeshes", RTTRFuncType="getter")
        const SubMeshes &getSubMeshes() const { return mSubMeshes; }

        TPROPERTY(RTTRFuncName="VertexStrides", RTTRFuncType="getter")
        const VertexStrides &getVertexStrides() const { return mVertexStrides; }

        TPROPERTY(RTTRFuncName="VertexOffsets", RTTRFuncType="getter")
        const VertexOffsets &getVertexOffsets() const { return mVertexOffsets; }

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

        Mesh(const String &name, VertexAttributes &&attributes, Vertices &&vertices, VertexStrides &&strides, VertexOffsets &&offsets, SubMeshes &&submeshes);
        
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

        virtual TResult generateRenderResource(Archive *archive);
        
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
        
        /// 顶点属性是否需要更新
        bool    mIsAttrDirty {false};
        /// 创建的时候是否传递了数据进来，用于是否创建渲染对象
        bool    mCreatedFromData {false};
    };
}


#endif    /*__T3D_MESH_H__*/
