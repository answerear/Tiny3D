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


#ifndef __T3D_RENDER_BUFFER_H__
#define __T3D_RENDER_BUFFER_H__


#include "Render/T3DRenderResource.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    using RenderBuffeReadCallback = TFunction<void(size_t size, void *dst)>;
    
    /**
     * \brief 渲染缓冲基类，管理 CPU 侧数据与 RHI 缓冲的读写
     */
    class T3D_ENGINE_API RenderBuffer : public RenderResource
    {
    public:
        /**
         * \brief 同步读取缓冲数据到 dst（仅 kStatic 且非 kVRAM 时有效）
         * \param [in] offset : 起始字节偏移
         * \param [in] size : 请求读取的字节数
         * \param [out] dst : 输出缓冲区，调用方需保证足够大
         * \return 实际读取的字节数；不满足读取条件时返回 0
         */
        virtual size_t readData(size_t offset, size_t size, void *dst);

        /**
         * \brief 异步读取缓冲数据（当前实现未接通 RHI，仅做前置校验）
         * \param [in] offset : 起始字节偏移
         * \param [in] size : 请求读取的字节数
         * \param [in] callback : 读取完成回调
         * \return 调用成功返回 T3D_OK；静态/RAM/无 CPU 读权限时返回对应错误码
         * \note 当前 RHI 异步读取路径为 TODO，校验通过后仍返回 T3D_OK
         */
        virtual TResult readData(size_t offset, size_t size, RenderBuffeReadCallback callback);

        /**
         * \brief 向缓冲区写入数据，同步更新 CPU 镜像并通过 RHI 提交到 GPU
         * \param [in] offset : 起始字节偏移
         * \param [in] buffer : 待写入的数据
         * \param [in] discardWholeBuffer : 是否丢弃整个缓冲区后再写入（传给 RHI writeBuffer）
         * \return 成功写入的字节数；失败时返回 0
         */
        virtual size_t writeData(size_t offset, const Buffer &buffer, bool discardWholeBuffer = false);

        /**
         * \brief 从另一个 RenderBuffer 拷贝指定区间（当前未实现）
         * \param [in] srcBuffer : 源缓冲区
         * \param [in] srcOffset : 源起始偏移
         * \param [in] dstOffset : 目标起始偏移
         * \param [in] size : 拷贝字节数
         * \param [in] discardWholeBuffer : 是否丢弃整个目标缓冲区
         * \return 当前始终返回 0
         */
        virtual size_t copyData(RenderBufferPtr srcBuffer, size_t srcOffset, size_t dstOffset, size_t size, bool discardWholeBuffer = false);

        /**
         * \brief 从另一个 RenderBuffer 拷贝全部数据（当前未实现）
         * \param [in] srcBuffer : 源缓冲区
         * \return 当前始终返回 0
         */
        virtual size_t copyData(RenderBufferPtr srcBuffer);

        /// CPU 侧镜像数据
        const Buffer &getBuffer() const { return mBuffer; }

        /// CPU 镜像数据字节数
        size_t getBufferSize() const { return mBuffer.DataSize; }

        /**
         * \brief GPU 侧资源的字节大小
         * \return 默认与 CPU 镜像大小一致
         * \remarks 允许无初始 CPU 数据的资源（典型是 compute 输出用的结构化缓冲）
         *          仍然上报真实的 GPU 尺寸，供 indirect 参数偏移等越界校验使用；
         *          否则那些校验会把「没有 CPU 镜像」误判成「零长度缓冲」。
         */
        virtual size_t getGPUSizeInBytes() const { return mBuffer.DataSize; }

        /// 内存驻留类型
        MemoryType getMemoryType() const { return mMemoryType; }
        
        /// 缓冲区用途
        Usage getUsage() const { return mUsage; }
        
        /// CPU 访问模式（CPUAccessMode 组合）
        uint32_t getCPUAccessMode() const { return mAccessMode; }

        /**
         * \brief GPU 侧附加访问权限（GPUAccessFlags 组合）
         * \return 构造时经一致性校验后的权限位
         * \remarks 各后端在创建原生资源时统一从这里读取，据此决定是否额外附加
         *          SRV / UAV / IndirectArgs 的绑定标记与视图。
         */
        uint32_t getGPUAccess() const { return mGPUAccess; }
        
    protected:
        /**
         * \brief 构造渲染缓冲；kBoth 时拷贝 buffer 数据，否则直接接管 buffer.Data
         * \param [in] buffer : 初始 CPU 数据
         * \param [in] memType : 内存驻留类型
         * \param [in] usage : 缓冲区用途
         * \param [in] accMode : CPU 访问模式
         * \param [in] gpuAccess : GPU 侧附加访问权限（GPUAccessFlags 组合）
         * \note 与 usage 冲突的权限位会被打日志后清掉，见 validateGPUAccess
         */
        RenderBuffer(const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode,
            uint32_t gpuAccess = kGPUNone);

        ~RenderBuffer() override;

        /**
         * \brief 校验 GPU 权限位与 usage 的组合合法性，返回修正后的权限位
         * \param [in] gpuAccess : 待校验的 GPUAccessFlags 组合
         * \param [in] usage : 缓冲区用途
         * \return 剔除非法位之后的权限位
         * \remarks 放在基类构造里做，一次性拦住所有后端的同类踩坑：
         *          D3D11 明确禁止 IMMUTABLE / DYNAMIC 资源创建 UAV，
         *          Vulkan / GL 侧虽然限制不同，但这类组合在语义上本身就是矛盾的。
         */
        static uint32_t validateGPUAccess(uint32_t gpuAccess, Usage usage);
        
        Buffer      mBuffer {};
        MemoryType  mMemoryType {MemoryType::kVRAM};
        Usage       mUsage {Usage::kStatic};
        uint32_t    mAccessMode {kCPUNone};
        /// GPU 侧附加访问权限（GPUAccessFlags 组合）
        uint32_t    mGPUAccess {kGPUNone};
    };
}


#endif  /*__T3D_RENDER_BUFFER_H__*/
