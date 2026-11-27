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


#ifndef __T3D_RHI_CONSTANT_H__
#define __T3D_RHI_CONSTANT_H__


#include "T3DTypedef.h"

namespace Tiny3D
{
    /// 最大 MRT 渲染目标数量
    const uint32_t T3D_MAX_RENDER_TARGET = 8;

    /**
     * \brief ShaderLab 内置标签名与渲染队列/光照模式枚举值
     */
    namespace ShaderLab
    {
        /// ShaderLab 内置标签：渲染队列（Queue）
        const char *const kBuiltinTagQueue = "Queue";
        /// ShaderLab 内置标签：光照模式（LightMode）
        const char *const kBuiltinTagLightMode = "LightMode";

        const char *const kBuiltinQueueBkgndStr = "Background";
        const char *const kBuiltinQueueGeometryStr = "Geometry";
        const char *const kBuiltinQueueAlphaTestStr = "AlphaTest";
        const char *const kBuiltinQueueTransparentStr = "Transparent";
        const char *const kBuiltinQueueOverlayStr = "Overlay";

        const char *const kBuiltinLightModeNoneStr = "None";
        const char *const kBuiltinLightModeShadowCasterStr = "ShadowCaster";
        const char *const kBuiltinLightModeForwardBaseStr = "ForwardBase";
        const char *const kBuiltinLightModeForwardAddStr = "ForwardAdd";

        /**
         * \brief ShaderLab 内置渲染队列优先级
         */
        enum BuiltinQueueValue : uint32_t
        {
            /// 背景物体，通常最先渲染
            kBuiltinQueueBkgnd = 0,
            /// 不透明几何体默认队列
            kBuiltinQueueGeometry = 2000,
            /// Alpha Test 裁剪透明物体
            kBuiltinQueueAlphaTest = 2450,
            /// 透明物体，通常在不透明物体之后渲染
            kBuiltinQueueTransparent = 3000,
            /// 场景内覆盖效果（镜头光晕、描边等），在本相机场景内最后渲染，仍会经过相机后处理；
            /// 屏幕空间 UI 需要在后处理之后绘制，不应使用此队列
            kBuiltinQueueOverlay = 4000,
        };

        /**
         * \brief ShaderLab 内置 Pass 光照模式标识
         */
        enum BuiltinLightModeValue : uint32_t
        {
            kBuiltinLightModeNone = static_cast<uint32_t>(-1),
            /// 阴影投射 Pass，用于生成阴影贴图
            kBuiltinLightModeShadowCaster = 0x10000,
            /// 前向渲染主光照 Pass
            kBuiltinLightModeForwardBase,
            /// 前向渲染附加光照 Pass
            kBuiltinLightModeForwardAdd,
        };
    }
    
    /**
     * \brief 颜色混合因子
     */
    TENUM()
    enum class BlendFactor : uint32_t
    {
        kOne = 0,               /// 1.0
        kZero,                  /// 0.0
        kDstColor,              /// 目标颜色 C_dst
        kSrcColor,              /// 源颜色 C_src
        kOneMinusDstColor,      /// 1 - C_dst
        kOneMinusSrcColor,      /// 1 - C_src
        kDstAlpha,              /// 目标 Alpha A_dst
        kSrcAlpha,              /// 源 Alpha A_src
        kOneMinusDstAlpha,      /// 1 - A_dst
        kOneMinusSrcAlpha,      /// 1 - A_src
    };

    /**
     * \brief 颜色混合运算
     */
    TENUM()
    enum class BlendOperation : uint32_t
    {
        kAdd,               /// C_result = C_src * F_src + C_dst * F_dst
        kSubtract,          /// C_result = C_src * F_src - C_dst * F_dst
        kReverseSubtract,   /// C_result = C_dst * F_dst - C_src * F_src
        kMin,               /// 取分量最小值
        kMax                /// 取分量最大值
    };

    /**
     * \brief 混合颜色通道写掩码
     */
    TENUM()
    enum BlendColorWriteMask : uint8_t
    {
        kWriteMaskNone = 0,
        kWriteMaskAlpha = 1 << 0,
        kWriteMaskRed = 1 << 1,
        kWriteMaskGreen = 1 << 2,
        kWriteMaskBlue = 1 << 3,
        kWriteMaskAll = 0x0F
    };

    /**
     * \brief 比较函数，用于深度测试与模板测试
     */
    TENUM()
    enum class CompareFunction : uint32_t
    {
        kAlwaysFail = 0,
        kAlwaysPass,
        kLess,
        kLessEqual,
        kEqual,
        kNotEqual,
        kGreaterEqual,
        kGreater
    };

    /**
     * \brief 模板缓冲操作
     */
    TENUM()
    enum class StencilOp : uint32_t
    {
        kKeep = 0,  /// 保持现有模板值
        kZero,      /// 将模板值置为 0
        kReplace,   /// 将模板值设为 StencilRef
        kInc,       /// 模板值 +1（未到最大值时）
        kIncWrap,   /// 模板值 +1，溢出时回绕为 0
        kDec,       /// 模板值 -1（未到最小值时）
        kDecWrap,   /// 模板值 -1，下溢时回绕为最大值
        kInvert     /// 按位取反模板值
    };

    /**
     * \brief 多边形光栅化填充模式
     */
    TENUM()
    enum class PolygonMode : uint32_t
    {
        kNone = 0,
        kPoint,         /// 点模式
        kWireframe,     /// 线框模式
        kSolid,         /// 实体填充模式
        kMax
    };

    /**
     * \brief 面剔除模式
     */
    TENUM()
    enum class CullingMode : uint32_t
    {
        kNone = 0,  /// 不剔除
        kFront,     /// 剔除正面
        kBack,      /// 剔除背面
    };

    /**
     * \brief 纹理过滤方式
     */
    TENUM()
    enum class FilterOptions : uint32_t
    {
        kNone = 0,
        kPoint,
        kLinear,
        kAnisotropic,
        kMax
    };

    /**
     * \brief 纹理寻址模式
     */
    TENUM()
    enum class TextureAddressMode : uint32_t
    {
        kNone = 0,
        kWrap,
        kMirror,
        kClamp,
        kBorder,
        kMirrorOnce
    };

    /**
     * \brief 资源映射/锁定的访问方式
     */
    TENUM()
    enum class LockOptions : uint32_t
    {
        kRead = 0,          /// 只读映射
        kWrite,             /// 只写映射
        kReadWrite,         /// 读写映射
        kWriteDiscard,      /// 写映射并丢弃原有 GPU 数据
        kWriteNoOverwrite   /// 写映射但不覆盖已有 GPU 数据
    };

    /**
     * \brief 渲染资源内存驻留位置
     */
    TENUM()
    enum class MemoryType : uint32_t
    {
        kBoth = 0,  /// CPU 与 GPU 各持有一份
        kRAM,       /// 仅 CPU 内存
        kVRAM       /// 仅 GPU 内存（CPU 镜像由 RenderBuffer 接管或拷贝）
    };

    /**
     * \brief 渲染资源用途与 CPU/GPU 访问模式
     */
    TENUM()
    enum class Usage : uint32_t
    {
        /// 初始化后 GPU 可读写，CPU 不可访问
        kStatic = 0,
        /// 创建后 GPU 只读，不可被 CPU 或 GPU 写入
        kImmutable,
        /// CPU 写、GPU 读（动态更新）
        kDynamic,
        /// 可从 GPU 复制到 CPU 读取
        kCopy,
    };

    /**
     * \brief CPU 对渲染资源的访问权限（可组合）
     */
    TENUM()
    enum CPUAccessMode : uint32_t
    {
        kCPUNone = 0,                       /// CPU 不访问
        kCPUWrite = (1 << 0),               /// CPU 可写
        kCPURead = (1 << 1),                /// CPU 可读
        kCPUReadWrite = kCPURead | kCPUWrite, /// CPU 读写
    };

    /**
     * \brief GPU 侧附加访问权限位标志
     * \remarks 与 CPUAccessMode 对称。缓冲/纹理的「本职」绑定（顶点缓冲之于
     *          VertexBuffer、着色器资源之于普通纹理）由资源类自身隐含，
     *          本枚举只描述**额外**开放的能力，映射到 D3D11 的 BindFlags
     *          附加位、Vulkan 的 VkBufferUsageFlags、GL 的 buffer target。
     */
    TENUM()
    enum GPUAccessFlags : uint32_t
    {
        /// 无附加权限
        kGPUNone            = 0,
        /// 可作为着色器只读资源（SRV / sampled image / SSBO readonly）
        kGPUShaderResource  = (1 << 0),
        /// 可作为着色器读写资源（UAV / storage image / SSBO）
        kGPUUnorderedAccess = (1 << 1),
        /// 可作为 indirect draw / dispatch 的参数缓冲
        kGPUIndirectArgs    = (1 << 2),
        /// 允许 RHIContext::generateMips 在 GPU 上生成 mip 链。只对 mipmaps > 1 的颜色渲染纹理生效，
        /// 格式须同时可作渲染目标并支持线性过滤（D3D11 要据此加 MISC_GENERATE_MIPS）
        kGPUGenerateMips    = (1 << 3),
    };

    /**
     * \brief 渲染目标深度 / 模板附件的只读位，见 RenderTarget::setDepthStencilReadOnly
     * \remarks 只读的部分仍参与深度 / 模板测试，但不会被写入，所以同一张深度纹理
     *          可以同时作为 SRV 采样（软粒子、贴花、体积雾）。没有模板的格式忽略 kReadOnlyStencil。
     */
    enum DepthStencilReadOnlyFlags : uint32_t
    {
        /// 深度与模板都可写，即普通的深度模板附件
        kDepthStencilWritable   = 0,
        /// 深度只读
        kReadOnlyDepth          = (1 << 0),
        /// 模板只读
        kReadOnlyStencil        = (1 << 1),
        /// 深度与模板都只读
        kReadOnlyDepthStencil   = kReadOnlyDepth | kReadOnlyStencil,
    };

    /**
     * \brief 纹理的子资源范围，用于渲染目标附件与 SRV / UAV 绑定
     * \remarks mipCount / sliceCount 为 0 表示「从起点到末尾」，所以默认值选中整张纹理，
     *          与引入子资源之前的绑定行为一致。Cubemap 的层号是 cubeIndex * 6 + face，
     *          face 顺序为 +X、-X、+Y、-Y、+Z、-Z。
     *          - 渲染目标（RTV / DSV）与 UAV 只用 mipLevel 这一级，忽略 mipCount；
     *          - SRV 用 [mipLevel, mipLevel + mipCount) 的 mip 范围。
     *
     *          视图维度在各后端一致：单层资源是 2D 视图；数组 / Cube 资源选中全部层时
     *          保持原维度（Cube 的 SRV 仍是 Cube），只选部分层时一律是 2D 数组视图，
     *          即使只选一层，shader 也要声明成 Texture2DArray / RWTexture2DArray。
     *          渲染目标选中多层时是分层渲染，由 GS 输出 SV_RenderTargetArrayIndex 选层。
     */
    struct TextureSubresource
    {
        /// 起始 mip
        uint32_t mipLevel {0};
        /// mip 个数，0 表示到最后一级；只对 SRV 有效
        uint32_t mipCount {0};
        /// 起始数组层
        uint32_t firstSlice {0};
        /// 层数，0 表示到最后一层
        uint32_t sliceCount {0};

        /// 选中单个 mip 的全部层
        static TextureSubresource mip(uint32_t level)
        {
            return TextureSubresource{level, 1, 0, 0};
        }

        /// 选中某一层的单个 mip，常用于渲染 Cube 的一个面或数组的一层
        static TextureSubresource slice(uint32_t slice, uint32_t level = 0)
        {
            return TextureSubresource{level, 1, slice, 1};
        }

        /// 完整 mip 链的级数（直到 1x1）
        static uint32_t getFullMipCount(uint32_t width, uint32_t height)
        {
            uint32_t size = width > height ? width : height;
            uint32_t count = 1;
            while (size > 1)
            {
                size >>= 1;
                ++count;
            }
            return count;
        }

        /// 第 level 级 mip 的边长，最小为 1
        static uint32_t getMipSize(uint32_t size, uint32_t level)
        {
            const uint32_t s = (level < 32) ? (size >> level) : 0;
            return s == 0 ? 1 : s;
        }

        /// 是否为默认值（整张纹理）
        bool isDefault() const
        {
            return mipLevel == 0 && mipCount == 0 && firstSlice == 0 && sliceCount == 0;
        }

        /**
         * \brief 按纹理的实际 mip 数与层数展开 0 值并校验范围
         * \param [in] totalMips : 纹理的 mip 总数
         * \param [in] totalSlices : 纹理的层数（Cube 为 6 * 立方体个数，非数组为 1）
         * \param [out] out : 展开后的范围，mipCount / sliceCount 都不为 0
         * \return 范围合法返回 true
         */
        bool resolve(uint32_t totalMips, uint32_t totalSlices, TextureSubresource &out) const
        {
            totalMips = totalMips == 0 ? 1 : totalMips;
            totalSlices = totalSlices == 0 ? 1 : totalSlices;
            if (mipLevel >= totalMips || firstSlice >= totalSlices)
            {
                return false;
            }
            out.mipLevel = mipLevel;
            out.mipCount = (mipCount == 0) ? (totalMips - mipLevel) : mipCount;
            out.firstSlice = firstSlice;
            out.sliceCount = (sliceCount == 0) ? (totalSlices - firstSlice) : sliceCount;
            return out.mipLevel + out.mipCount <= totalMips && out.firstSlice + out.sliceCount <= totalSlices;
        }

        /// 展开后的范围打包成视图缓存的键
        uint64_t key() const
        {
            return (uint64_t)(mipLevel & 0xFFu) | ((uint64_t)(mipCount & 0xFFu) << 8)
                | ((uint64_t)(firstSlice & 0xFFFFu) << 16) | ((uint64_t)(sliceCount & 0xFFFFu) << 32);
        }

        bool operator ==(const TextureSubresource &other) const
        {
            return mipLevel == other.mipLevel && mipCount == other.mipCount
                && firstSlice == other.firstSlice && sliceCount == other.sliceCount;
        }

        bool operator !=(const TextureSubresource &other) const
        {
            return !(*this == other);
        }
    };

    /// 与 PixelBuffers / UnorderedAccessBuffers 按下标一一对应的子资源数组，为空表示全部取默认值
    using TextureSubresources = TArray<TextureSubresource>;

    /// UAVInitialCounts 中表示「不修改计数器」的哨兵值，对应 D3D11 的 (UINT)-1
    const uint32_t kKeepUAVCounter = 0xFFFFFFFFu;

    /**
     * \brief GPU 读回时选择的纹理子区域
     * \remarks size 全 0 表示该 mip / slice 的整层。offset / size 的单位是纹素，
     *          按源资源的维度解释（1D 只用 x，2D 用 x/y，3D 用 x/y/z）。
     */
    struct ReadbackRegion
    {
        /// mip 层级
        uint32_t mipLevel {0};
        /// 数组层；Cubemap 传面号（0-5）或 cubeIndex * 6 + face
        uint32_t arraySlice {0};
        /// 区域起点（纹素）
        Vector3  offset {Vector3::ZERO};
        /// 区域尺寸（纹素），全 0 表示整层
        Vector3  size {Vector3::ZERO};
    };

    /**
     * \brief map 返回的读回票据，用于在 unmap 认领对应的 staging 数据
     * \remarks 只是一个不透明句柄，调用方不要解释字段含义。
     */
    struct ReadbackHandle
    {
        /// 世代号，用于识别已消费 / 过期的句柄
        uint32_t generation {0};
        /// pending 表下标，0xFFFFFFFF 表示无效
        uint32_t index {0xFFFFFFFFu};

        /// 是否为有效句柄
        bool isValid() const { return index != 0xFFFFFFFFu; }

        /// 返回一个无效句柄
        static ReadbackHandle invalid() { return ReadbackHandle(); }
    };

    /**
     * \brief 索引缓冲区位宽
     */
    TENUM()
    enum class IndexType : uint32_t
    {
        E_IT_16BITS = 0,    /// 16 位索引
        E_IT_32BITS,        /// 32 位索引
    };

    /**
     * \brief 非索引间接绘制参数
     * \remarks 二进制布局与 D3D11 DrawInstancedIndirect /
     *          VkDrawIndirectCommand / GL DrawArraysIndirectCommand 一致
     */
    TSTRUCT()
    struct T3D_ENGINE_API DrawIndirectArgs
    {
        /// 单个实例的顶点数量
        TPROPERTY()
        uint32_t    vertexCountPerInstance {0};
        /// 实例数量
        TPROPERTY()
        uint32_t    instanceCount {0};
        /// 顶点缓冲区起始位置
        TPROPERTY()
        uint32_t    startVertexLocation {0};
        /// 起始实例编号
        TPROPERTY()
        uint32_t    startInstanceLocation {0};
    };

    /**
     * \brief 索引间接绘制参数
     * \remarks 二进制布局与 D3D11 DrawIndexedInstancedIndirect /
     *          VkDrawIndexedIndirectCommand / GL DrawElementsIndirectCommand 一致
     */
    TSTRUCT()
    struct T3D_ENGINE_API DrawIndexedIndirectArgs
    {
        /// 单个实例的索引数量
        TPROPERTY()
        uint32_t    indexCountPerInstance {0};
        /// 实例数量
        TPROPERTY()
        uint32_t    instanceCount {0};
        /// 索引缓冲区起始位置
        TPROPERTY()
        uint32_t    startIndexLocation {0};
        /// 有符号：允许负的顶点基址偏移
        TPROPERTY()
        int32_t     baseVertexLocation {0};
        /// 起始实例编号
        TPROPERTY()
        uint32_t    startInstanceLocation {0};
    };

    /**
     * \brief 间接派发参数
     * \remarks 二进制布局与 D3D11 DispatchIndirect /
     *          VkDispatchIndirectCommand / GL DispatchIndirectCommand 一致
     */
    TSTRUCT()
    struct T3D_ENGINE_API DispatchIndirectArgs
    {
        /// X 维线程组数量
        TPROPERTY()
        uint32_t    threadGroupCountX {0};
        /// Y 维线程组数量
        TPROPERTY()
        uint32_t    threadGroupCountY {0};
        /// Z 维线程组数量
        TPROPERTY()
        uint32_t    threadGroupCountZ {0};
    };

    // 这三个结构直接以字节形式喂给各后端的 indirect API，布局被 D3D11 / Vulkan /
    // OpenGL 三套 API 同时约束，任何字段增删或对齐变化都会静默画错东西。
    static_assert(sizeof(DrawIndirectArgs) == 16, "DrawIndirectArgs layout must match native indirect args");
    static_assert(sizeof(DrawIndexedIndirectArgs) == 20, "DrawIndexedIndirectArgs layout must match native indirect args");
    static_assert(sizeof(DispatchIndirectArgs) == 12, "DispatchIndirectArgs layout must match native indirect args");
}


#endif  /*__T3D_RHI_CONSTANT_H__*/
