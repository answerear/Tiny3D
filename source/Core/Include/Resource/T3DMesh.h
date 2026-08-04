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
     * \brief 网格资源，持有顶点/索引数据、子网格及渲染用 VertexDeclaration/VertexBuffer
     */
    TCLASS()
    class T3D_ENGINE_API Mesh : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 创建空 Mesh（供反序列化或后续填充数据）
         * \param [in] name : 网格名称
         * \return 新建的 Mesh 智能指针
         */
        static MeshPtr create(const String &name);

        /**
         * \brief 用完整网格数据创建 Mesh
         * \param [in] name : 网格名称
         * \param [in] attributes : 顶点属性声明
         * \param [in] vertices : 顶点缓冲数据
         * \param [in] strides : 各 stream 的顶点跨度（字节）
         * \param [in] offsets : 各 stream 在交错顶点中的偏移
         * \param [in] submeshes : 子网格集合
         * \param [in] position : 网格节点局部位置
         * \param [in] orientation : 网格节点局部旋转
         * \param [in] scaling : 网格节点局部缩放
         * \param [in] meshNodeName : 对应场景节点名称
         * \return 新建的 Mesh 智能指针；onCreate 时会自动生成渲染资源
         */
        static MeshPtr create(const String &name, VertexAttributes &&attributes, Vertices &&vertices, VertexStrides &&strides, VertexOffsets &&offsets, SubMeshes &&submeshes, 
            const Vector3 &position = Vector3::ZERO, const Quaternion &orientation = Quaternion::IDENTITY, const Vector3 &scaling = Vector3::UNIT_SCALE,
            const String &meshNodeName = "");

        /// 析构；顶点数据若未交给 VertexBuffer 则由本对象释放
        ~Mesh() override;

        /**
         * \brief 获取资源类型
         * \return 固定返回 Type::kMesh
         */
        Type getType() const override;

        /// 获取 CPU 端顶点数据
        TPROPERTY(RTTRFuncName="Vertices", RTTRFuncType="getter")
        const Vertices &getVertices() const { return mVertices; }

        /**
         * \brief 设置顶点数据（浅拷贝）
         * \param [in] vertices : 新顶点数据；替换前先 release 未被 VBO 接管的旧块
         */
        TPROPERTY(RTTRFuncName="Vertices", RTTRFuncType="setter")
        void setVertices(const Vertices &vertices);

        /// 获取顶点属性声明列表
        TPROPERTY(RTTRFuncName="VertexAttributes", RTTRFuncType="getter")
        const VertexAttributes &getVertexAttributes() const { return mVertexAttributes; }

        /// 获取子网格集合
        TPROPERTY(RTTRFuncName="SubMeshes", RTTRFuncType="getter")
        const SubMeshes &getSubMeshes() const { return mSubMeshes; }

        /// 获取各 stream 顶点跨度
        TPROPERTY(RTTRFuncName="VertexStrides", RTTRFuncType="getter")
        const VertexStrides &getVertexStrides() const { return mVertexStrides; }

        /// 获取各 stream 顶点偏移
        TPROPERTY(RTTRFuncName="VertexOffsets", RTTRFuncType="getter")
        const VertexOffsets &getVertexOffsets() const { return mVertexOffsets; }
        
        /// 获取网格节点局部平移
        TPROPERTY(RTTRFuncName="MeshPosition", RTTRFuncType="getter")
        const Vector3 &getMeshPosition() const { return mMeshT; }

        /// 获取网格节点局部旋转
        TPROPERTY(RTTRFuncName="MeshOrientation", RTTRFuncType="getter")
        const Quaternion &getMeshOrientation() const { return mMeshQ; }

        /// 获取网格节点局部缩放
        TPROPERTY(RTTRFuncName="MeshScaling", RTTRFuncType="getter")
        const Vector3 &getMeshScaling() const { return mMeshS; }

        /// 获取对应场景节点名称
        TPROPERTY(RTTRFuncName="MeshNodeName", RTTRFuncType="getter")
        const String &getMeshNodeName() const { return mMeshNodeName; }

        /// 获取包围体种子类型（局部空间，实例化时用于播种 Bound 组件默认值）
        TPROPERTY(RTTRFuncName="BoundSeedType", RTTRFuncType="getter")
        Bound::Type getBoundSeedType() const { return mBoundSeedType; }

        /// 获取包围体种子标量 A：Sphere=圆心，Aabb=最小点，Capsule=端点 0
        TPROPERTY(RTTRFuncName="BoundSeedA", RTTRFuncType="getter")
        const Vector3 &getBoundSeedA() const { return mBoundSeedA; }

        /// 获取包围体种子标量 B：Aabb=最大点，Capsule=端点 1（Sphere 不使用）
        TPROPERTY(RTTRFuncName="BoundSeedB", RTTRFuncType="getter")
        const Vector3 &getBoundSeedB() const { return mBoundSeedB; }

        /// 获取包围体种子半径（Sphere/Capsule 有效，Aabb 不使用）
        TPROPERTY(RTTRFuncName="BoundSeedRadius", RTTRFuncType="getter")
        Real getBoundSeedRadius() const { return mBoundSeedRadius; }

        /**
         * \brief 设置包围体种子（局部空间）
         * \param [in] type : 包围体类型
         * \param [in] a : Sphere=圆心 / Aabb=最小点 / Capsule=端点 0
         * \param [in] b : Aabb=最大点 / Capsule=端点 1
         * \param [in] radius : Sphere/Capsule 半径
         */
        void setBoundSeed(Bound::Type type, const Vector3 &a, const Vector3 &b, Real radius);

        /// 获取渲染用顶点声明；generateRenderResource 后有效
        VertexDeclarationPtr getVertexDeclaration() const { return mVertexDecl; }

        /// 获取渲染用顶点缓冲数组
        const VertexBuffers &getVertexBuffers() const { return mVBuffers; }

        /**
         * \brief 按名称查找子网格
         * \param [in] name : 子网格名称
         * \return 找到返回 SubMesh 裸指针；否则返回 nullptr
         */
        SubMesh *getSubMesh(const String &name) const;

        /**
         * \brief 按索引获取顶点属性
         * \param [in] index : 属性下标
         * \return 对应 VertexAttribute 的常量引用
         * \remarks 越界触发断言
         */
        const VertexAttribute &getVertexAttribute(size_t index) const;

        /**
         * \brief 在末尾追加顶点属性
         * \return 新追加属性的常量引用
         */
        const VertexAttribute &addVertexAttribute(uint32_t slot, uint32_t offset, VertexAttribute::Type type, VertexAttribute::Semantic semantic, uint32_t semanticIndex);

        /**
         * \brief 在指定位置插入顶点属性
         * \param [in] pos : 插入位置；超出范围则等价于 append
         * \return 新插入属性的常量引用
         */
        const VertexAttribute &insertVertexAttribute(uint32_t pos, uint32_t slot, uint32_t offset, VertexAttribute::Type type, VertexAttribute::Semantic semantic, uint32_t semanticIndex);

        /**
         * \brief 追加已有 VertexAttribute 对象
         * \return 调用成功返回 T3D_OK
         */
        TResult addVertexAttribute(const VertexAttribute &vertexAttribute);

        /**
         * \brief 在指定位置插入已有 VertexAttribute 对象
         * \param [in] pos : 插入位置
         * \return 调用成功返回 T3D_OK
         */
        TResult insertVertexAttribute(uint32_t pos, const VertexAttribute &vertexAttribute);

        /**
         * \brief 按位置删除顶点属性
         * \param [in] pos : 属性下标
         * \return 越界返回 T3D_ERR_OUT_OF_BOUND；否则返回 T3D_OK
         */
        TResult removeVertexAttribute(uint32_t pos);

        /**
         * \brief 按语义删除顶点属性
         * \param [in] semantic : 顶点语义
         * \param [in] semanticIndex : 语义索引
         * \return 未找到返回 T3D_ERR_NOT_FOUND；否则返回 T3D_OK
         */
        TResult removeVertexAttribute(VertexAttribute::Semantic semantic, uint32_t semanticIndex);

        /**
         * \brief 清空所有顶点属性
         * \return 调用成功返回 T3D_OK
         */
        TResult removeAllVertexAttributes();

        /**
         * \brief 更新指定位置的顶点属性
         * \param [in] pos : 属性下标
         * \return 越界返回 T3D_ERR_OUT_OF_BOUND；否则返回 T3D_OK
         */
        TResult updateVertexAttribute(uint32_t pos, uint32_t slot, uint32_t offset, VertexAttribute::Type type, VertexAttribute::Semantic semantic, uint32_t semanticIndex);

        /**
         * \brief 按语义查找顶点属性
         * \return 找到返回属性指针；否则返回 nullptr
         */
        const VertexAttribute *findVertexAttributeBySemantic(VertexAttribute::Semantic semantic, uint32_t semanticIndex) const;

        /**
         * \brief 若顶点属性有变更则刷新顶点声明
         * \remarks 当前实现仅清除 mIsAttrDirty 标志，未重建 mVertexDecl
         */
        void flushVertexAttributes();

        /// 获取指定 stream 的顶点跨度（字节）
        uint32_t getVertexStride(uint32_t slot) const { return mVertexStrides[slot]; }

        /// 获取指定 stream 的顶点偏移（字节）
        uint32_t getVertexOffset(uint32_t slot) const { return mVertexOffsets[slot]; }

#if defined(T3D_DEBUG)
        /**
         * \brief 获取调试信息字符串
         * \return 当前实现返回空字符串
         */
        virtual String getDebugInfo() const;
#endif
        
    protected:
        /// 默认构造
        Mesh() = default;
        
        /**
         * \brief 以名称构造空 Mesh
         * \param [in] name : 网格名称
         */
        Mesh(const String &name);

        /**
         * \brief 用完整数据构造 Mesh
         * \remarks 设置 mCreatedFromData = true，onCreate 时会 generateRenderResource
         */
        Mesh(const String &name, VertexAttributes &&attributes, Vertices &&vertices, VertexStrides &&strides, VertexOffsets &&offsets, SubMeshes &&submeshes,
            const Vector3 &position = Vector3::ZERO, const Quaternion &orientation = Quaternion::IDENTITY, const Vector3 &scaling = Vector3::UNIT_SCALE,
            const String &meshNodeName = "");
        
        /**
         * \brief 克隆 Mesh 资源
         * \return 新 Mesh 智能指针
         */
        ResourcePtr clone() const override;

        /**
         * \brief 从源 Mesh 拷贝包围体种子
         * \param [in] src : 源 Mesh 对象
         */
        void cloneProperties(const Resource * const src) override;

        /**
         * \brief 资源创建回调；若 mCreatedFromData 则生成渲染资源
         * \return generateRenderResource 失败时返回对应错误码
         */
        TResult onCreate() override;
        
        /**
         * \brief 资源加载后生成渲染资源
         * \param [in] archive : 来源档案，用于加载子网格材质
         * \return generateRenderResource 的返回值
         */
        TResult onLoad(Archive *archive) override;

        /// 设置顶点属性声明
        TPROPERTY(RTTRFuncName="VertexAttributes", RTTRFuncType="setter")
        void setVertexAttributes(const VertexAttributes &attributes) { mVertexAttributes = attributes; }

        /// 设置子网格集合
        TPROPERTY(RTTRFuncName="SubMeshes", RTTRFuncType="setter")
        void setSubMeshes(const SubMeshes &submeshes) { mSubMeshes = submeshes; }

        /// 设置顶点跨度
        TPROPERTY(RTTRFuncName="VertexStrides", RTTRFuncType="setter")
        void setVertexStrides(const VertexStrides &strides) { mVertexStrides = strides; }

        /// 设置顶点偏移
        TPROPERTY(RTTRFuncName="VertexOffsets", RTTRFuncType="setter")
        void setVertexOffsets(const VertexOffsets &offsets) { mVertexOffsets = offsets; }
        
        /// 设置网格节点局部平移
        TPROPERTY(RTTRFuncName="MeshPosition", RTTRFuncType="setter")
        void setMeshPosition(const Vector3 &position) { mMeshT = position; }
        
        /// 设置网格节点局部旋转
        TPROPERTY(RTTRFuncName="MeshOrientation", RTTRFuncType="setter")
        void setMeshOrientation(const Quaternion &orientation) { mMeshQ = orientation; }
        
        /// 设置网格节点局部缩放
        TPROPERTY(RTTRFuncName="MeshScaling", RTTRFuncType="setter")
        void setMeshScaling(const Vector3 &scaling) { mMeshS = scaling; }

        /// 设置场景节点名称
        TPROPERTY(RTTRFuncName="MeshNodeName", RTTRFuncType="setter")
        void setMeshNodeName(const String &name) { mMeshNodeName = name; }

        /// 设置包围体种子类型
        TPROPERTY(RTTRFuncName="BoundSeedType", RTTRFuncType="setter")
        void setBoundSeedType(Bound::Type type) { mBoundSeedType = type; }

        /// 设置包围体种子 A
        TPROPERTY(RTTRFuncName="BoundSeedA", RTTRFuncType="setter")
        void setBoundSeedA(const Vector3 &a) { mBoundSeedA = a; }

        /// 设置包围体种子 B
        TPROPERTY(RTTRFuncName="BoundSeedB", RTTRFuncType="setter")
        void setBoundSeedB(const Vector3 &b) { mBoundSeedB = b; }

        /// 设置包围体种子半径
        TPROPERTY(RTTRFuncName="BoundSeedRadius", RTTRFuncType="setter")
        void setBoundSeedRadius(Real radius) { mBoundSeedRadius = radius; }

        /**
         * \brief 为各 SubMesh 与子网格材质创建 GPU 资源，并构建顶点声明与顶点缓冲
         * \param [in] archive : 非 nullptr 时从档案加载材质；nullptr 时从 MaterialManager 缓存查找
         * \return 材质/Shader 无效或缓冲创建失败时返回对应错误码
         */
        virtual TResult generateRenderResource(Archive *archive);

        /**
         * \brief 子类可覆写以标记动态顶点 stream
         * \return 默认 false；SkinnedMesh 等可返回 true 以使用 Dynamic 顶点缓冲
         */
        virtual bool isDynamicVertices() const { return false; }
        
    protected:
        /// 顶点属性声明
        VertexAttributes mVertexAttributes {};
        /// CPU 顶点数据
        Vertices  mVertices {};
        /// 各 stream 顶点跨度
        VertexStrides   mVertexStrides {};
        /// 各 stream 顶点偏移
        VertexOffsets   mVertexOffsets {};

        /// 子网格集合
        SubMeshes mSubMeshes {};

        /// 渲染用顶点声明
        VertexDeclarationPtr    mVertexDecl {nullptr};
        /// 渲染用顶点缓冲
        VertexBuffers mVBuffers {nullptr};
        
        String mMeshNodeName {};
        Vector3 mMeshT {Vector3::ZERO};
        Quaternion mMeshQ {Quaternion::IDENTITY};
        Vector3 mMeshS {Vector3::UNIT_SCALE};

        /// 包围体种子（局部空间）；GameObject 已挂 Bound 组件后以组件为准
        Bound::Type mBoundSeedType {Bound::Type::NONE};
        Vector3     mBoundSeedA {Vector3::ZERO};
        Vector3     mBoundSeedB {Vector3::ZERO};
        Real        mBoundSeedRadius {Real(0.0)};
        
        /// 顶点属性是否需要刷新
        bool    mIsAttrDirty {false};
        /// 创建时是否携带了完整数据（决定是否 onCreate 时生成渲染资源）
        bool    mCreatedFromData {false};
    };
}


#endif    /*__T3D_MESH_H__*/
