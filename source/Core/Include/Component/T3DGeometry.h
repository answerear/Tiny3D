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

#ifndef __T3D_GEOMETRY_H__
#define __T3D_GEOMETRY_H__


#include "Component/T3DRenderable.h"


namespace Tiny3D
{
    /**
     * \brief 静态网格渲染组件，从 Mesh/SubMesh 加载几何数据并生成独立渲染材质
     */
    TCLASS()
    class T3D_ENGINE_API Geometry : public Renderable
    {
        TRTTI_ENABLE(Renderable)
        TRTTI_FRIEND
        
    public:
        /// 析构
        ~Geometry() override;

        /**
         * \brief 克隆当前 Geometry
         * \return 克隆成功返回新组件；cloneProperties 失败返回 nullptr
         */
        ComponentPtr clone() const override;

        /**
         * \brief 返回渲染材质
         * \return 由 generateRenderMaterial 克隆生成的材质；未生成时为 nullptr
         */
        Material *getMaterial() override;

        /**
         * \brief 返回图元类型
         * \return mesh 与 submesh 就绪时返回 submesh 类型，否则 kTriangleList
         */
        PrimitiveType getPrimitiveType() const override;

        /**
         * \brief 返回顶点声明
         * \return mesh 就绪时返回其顶点声明，否则 nullptr
         */
        VertexDeclaration *getVertexDeclaration() const override;
        
        /**
         * \brief 返回顶点缓冲集合
         * \return mesh 就绪时返回其顶点缓冲，否则空静态容器
         */
        const VertexBuffers &getVertexBuffers() const override;

        /**
         * \brief 返回索引缓冲
         * \return mesh 与 submesh 就绪时返回 submesh 索引缓冲，否则 nullptr
         */
        IndexBuffer *getIndexBuffer() const override;

        /**
         * \brief 返回各顶点流 stride
         * \return mesh 就绪时返回其 stride，否则空静态容器
         */
        const VertexStrides &getVertexStrides() const override;

        /**
         * \brief 返回各顶点流 offset
         * \return mesh 就绪时返回其 offset，否则空静态容器
         */
        const VertexOffsets &getVertexOffsets() const override;

        /// 返回当前绑定的 Mesh 对象
        Mesh *getMeshObject() const { return mMesh; }

        /**
         * \brief 绑定 Mesh 与 SubMesh 并生成渲染材质
         * \param [in] mesh : Mesh 资源，与 submesh 任一为 nullptr 时清空绑定并卸载材质
         * \param [in] submesh : 须属于 mesh 的 SubMesh；不在 mesh 内时不更新绑定
         */
        void setMeshObject(Mesh *mesh, SubMesh *submesh);
        
        /// 返回当前绑定的 SubMesh 对象
        SubMesh *getSubMeshObject() const { return mSubMesh; }
        
        // TPROPERTY(RTTRFuncName="Mesh", RTTRFuncType="getter")
        // const String &getMesh() const { return mMeshName; }
        //
        // TPROPERTY(RTTRFuncName="Mesh", RTTRFuncType="setter")
        // void setMesh(const String &name) { mMeshName = name; }

        /// 返回序列化的 Mesh UUID
        TPROPERTY(RTTRFuncName="MeshUUID", RTTRFuncType="getter")
        const UUID &getMeshUUID() const { return mMeshUUID; }

        /**
         * \brief 设置序列化的 Mesh UUID
         * \param [in] uuid : Mesh 资源 UUID
         */
        TPROPERTY(RTTRFuncName="MeshUUID", RTTRFuncType="setter")
        void setMeshUUID(const UUID &uuid) { mMeshUUID = uuid; }

        /// 返回序列化的 SubMesh 名称
        TPROPERTY(RTTRFuncName="SubMesh", RTTRFuncType="getter")
        const String &getSubMesh() const { return mSubMeshName; }

        /**
         * \brief 设置序列化的 SubMesh 名称
         * \param [in] name : SubMesh 名称
         */
        TPROPERTY(RTTRFuncName="SubMesh", RTTRFuncType="setter")
        void setSubMesh(const String &name) { mSubMeshName = name; }

    protected:
        /// 默认构造
        Geometry() = default;

        /**
         * \brief 以指定 UUID 构造
         * \param [in] uuid : 组件唯一标识
         */
        Geometry(const UUID &uuid);

        /**
         * \brief 从源 Geometry 拷贝 mesh/submesh 引用与 UUID 字段
         * \param [in] src : 源组件
         * \return 成功返回 T3D_OK
         */
        TResult cloneProperties(const Component * const src) override;

        /**
         * \brief 反序列化完成后回调（调用 Renderable::onPostLoad）
         */
        void onPostLoad() override;

        /**
         * \brief 从 Archive 加载 Mesh，解析 SubMesh 并生成渲染材质
         * \param [in] archive : 资源包；SubMesh 名称不匹配时回退到第一个 SubMesh
         */
        void onLoadResource(Archive *archive) override;

        /**
         * \brief 销毁时卸载非动态合批材质
         */
        void onDestroy() override;

        /**
         * \brief 从 SubMesh 关联材质克隆生成 mMaterial
         * \remarks mSubMesh 或材质未加载时记录日志并提前返回
         */
        virtual void generateRenderMaterial();
        
    protected:
        /// Mesh 资源的 UUID
        UUID mMeshUUID {};
        /// SubMesh 的名称
        String  mSubMeshName {};
        /// Mesh 对象
        MeshPtr mMesh {nullptr};
        /// SubMesh 对象
        SubMesh *mSubMesh {nullptr};
        /// 渲染使用的材质（非动态合批时为 clone 副本）
        MaterialPtr mMaterial {nullptr};
        /// 是否动态合批（当前实现恒为 false）
        bool mIsDynamicBatch {false};
    };
}


#endif  /*__T3D_GEOMETRY_H__*/
