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


#ifndef __T3D_VERTEX_DECLARATION_H__
#define __T3D_VERTEX_DECLARATION_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Render/T3DVertexAttribute.h"
#include "Render/T3DRenderResource.h"


namespace Tiny3D
{
    /**
     * \brief 顶点输入布局声明，onLoad 时创建 RHI 顶点声明
     */
    class T3D_ENGINE_API VertexDeclaration : public RenderResource
    {
    public:
        /**
         * \brief 创建顶点声明对象（尚未创建底层 RHI 资源，需经 RenderResourceManager 加载）
         * \param [in] attributes : 顶点属性列表
         * \param [in] vertexShader : 关联的顶点着色器变体
         * \return 新创建的 VertexDeclaration 指针
         */
        static VertexDeclarationPtr create(const VertexAttributes &attributes, ShaderVariant *vertexShader);

        ~VertexDeclaration() override;

        /**
         * \brief 返回渲染资源类型标识
         * \return Type::kVertexDeclaration
         */
        Type getType() const override;

        /// 返回顶点属性数量
        size_t getAttributeCount() const  {  return mVertexAttributes.size(); }

        /// 返回顶点属性列表
        const VertexAttributes &getAttributes() const { return mVertexAttributes; }

        /**
         * \brief 重新计算并返回属性列表的 CRC 哈希（写入 mHash）
         * \return 计算后的哈希值
         */
        uint32_t hash();
        
        /**
         * \brief 累加指定 slot 上全部属性的字节大小
         * \param [in] source : 顶点缓冲流索引
         * \return 该 slot 的顶点步长（字节）
         */
        uint32_t getVertexSize(uint32_t source) const;

        /// 返回关联的顶点着色器变体
        ShaderVariant *getVertexShader() const { return mVertexShader; }

    protected:
        /**
         * \brief 构造顶点声明，拷贝保存属性列表
         * \param [in] attributes : 顶点属性列表
         * \param [in] vertexShader : 关联的顶点着色器变体
         */
        VertexDeclaration(const VertexAttributes &attributes, ShaderVariant *vertexShader);

        /**
         * \brief 通过活动 RHI 上下文创建 RHIVertexDeclaration
         * \return 始终返回 true
         */
        bool onLoad() override;

        /**
         * \brief 释放 RHI 顶点声明引用
         * \return 始终返回 true
         */
        bool onUnload() override;

    protected:
        /// 顶点属性列表（值拷贝，避免外部引用析构后悬垂）
        const VertexAttributes mVertexAttributes;
        /// 属性列表 CRC 哈希
        uint32_t    mHash {0};
        /// 关联的顶点着色器变体
        ShaderVariantPtr    mVertexShader {nullptr};
    };
}


#endif  /*__T3D_VERTEX_DECLARATION_H__*/
