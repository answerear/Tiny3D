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


#ifndef __T3D_CONSTANT_BUFFER_H__
#define __T3D_CONSTANT_BUFFER_H__


#include "Render/T3DRenderBuffer.h"


namespace Tiny3D
{
    /**
     * \brief 着色器常量缓冲区，绑定 shader 寄存器槽并持有 uniform 数据
     */
    class T3D_ENGINE_API ConstantBuffer : public RenderBuffer
    {
    public:
        /**
         * \brief 创建常量缓冲区对象（尚未创建底层 RHI 资源，需经 RenderResourceManager 加载）
         * \param [in] name : 常量缓冲区名称（对应 shader 绑定名）
         * \param [in] binding : shader 寄存器绑定槽
         * \param [in] buffer : 初始 CPU 数据；kBoth 时会拷贝一份，kVRAM 时直接接管 Data 指针
         * \param [in] memType : 内存驻留类型
         * \param [in] usage : 缓冲区用途
         * \param [in] accMode : CPU 访问模式（CPUAccessMode 组合）
         * \param [in] gpuAccess : GPU 侧附加访问权限（GPUAccessFlags 组合）
         * \return 新建的 ConstantBuffer 智能指针
         */
        static ConstantBufferPtr create(const String &name, uint32_t binding, const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode, uint32_t gpuAccess = kGPUNone);
        
        ~ConstantBuffer() override = default;

        /**
         * \brief 返回渲染资源类型标识
         * \return Type::kConstantBuffer
         */
        Type getType() const override;

        /// 常量缓冲区名称
        const String &getName() const { return mName; }

        /// shader 寄存器绑定槽
        uint32_t getBinding() const { return mBinding; }
        
    protected:
        /**
         * \brief 构造常量缓冲区并保存名称与绑定槽
         * \param [in] name : 常量缓冲区名称
         * \param [in] binding : shader 寄存器绑定槽
         * \param [in] buffer : 初始 CPU 数据
         * \param [in] memType : 内存驻留类型
         * \param [in] usage : 缓冲区用途
         * \param [in] accMode : CPU 访问模式
         * \param [in] gpuAccess : GPU 侧附加访问权限（GPUAccessFlags 组合）
         */
        ConstantBuffer(const String &name, uint32_t binding, const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode, uint32_t gpuAccess = kGPUNone);

        /**
         * \brief 通过活动 RHI 上下文创建 RHIConstantBuffer
         * \return 始终返回 true
         */
        bool onLoad() override;

        /**
         * \brief 释放 RHI 常量缓冲区引用
         * \return 始终返回 true
         */
        bool onUnload() override;

        String  mName {};
        uint32_t mBinding {0};
    };
}


#endif  /*__T3D_CONSTANT_BUFFER_H__*/
