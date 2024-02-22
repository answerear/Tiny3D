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
    /**
     * @enum    BlendFactor
     * @brief   混合因子
     */
    TENUM()
    enum class BlendFactor : uint32_t
    {
        kOne = 0,               /// 1.0
        kZero,                  /// 0.0
        kDstColor,              /// C_dst
        kSrcColor,              /// C_src
        kOneMinusDstColor,      /// 1 - C_dst
        kOneMinusSrcColor,      /// 1 - C_src
        kDstAlpha,              /// A_dst
        kSrcAlpha,              /// A_src
        kOneMinusDstAlpha,      /// 1 - A_dst
        kOneMinusSrcAlpha,      /// 1 - A_src
    };

    /**
     * @enum    BlendOperation
     * @brief   混合操作
     */
    TENUM()
    enum class BlendOperation : uint32_t
    {
        kAdd,               /// C_result = C_src * F_src + C_dst * F_dst
        kSubtract,          /// C_result = C_src * F_src - C_dst * F_dst
        kReverseSubtract,   /// C_result = C_dst * F_dst - C_src * F_src
        kMin,               /// 
        kMax
    };

    /**
     * \brief 混合颜色写掩码
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
     * @enum    CompareFunction
     * @brief   比较函数，用于深度缓冲测试和模板缓冲测试
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
     * \brief 模板操作
     */
    TENUM()
    enum class StencilOp : uint32_t
    {
        kKeep = 0,  /**< 保持现有的模板值 */
        kZero,      /**< 将模板值置为0 */
        kReplace,   /**< 将模板值设置为用 HWDepthStencilState::setStencilRef 函数设置的 ref 值*/
        kInc,       /**< 如果模板值不是最大值就将模板值+1 */
        kIncWrap,   /**< 与 INCR 一样将模板值+1，如果模板值已经是最大值则设为0 */
        kDec,       /**< 如果模板值不是最小值就将模板值-1 */
        kDecWrap,   /**< 与 DECR 一样将模板值-1，如果模板值已经是最小值则设为最大值 */
        kInvert     /**< 把模板值按位取反 */
    };

    /**
     * @brief 多边形渲染模式
     */
    TENUM()
    enum class PolygonMode : uint32_t
    {
        kNone = 0,
        /**< 顶点模式 */
        kPoint,
        /**< 线框模式 */
        kWireframe,
        /**< 着色模式 */
        kSolid,
        kMax
    };

    /**
     * @enum    CullingMode
     * @brief   背面剔除模式
     */
    TENUM()
    enum class CullingMode : uint32_t
    {
        /**< 不做消隐面剔除 */
        kNone = 0,
        /**< 按照顶点顺时针顺序的消隐面剔除 */
        kClockwise,
        /**< 按照顶点逆时针顺序的消隐面剔除 */
        kAnticlockwise,
        kMax
    };

    /**
     * @enum    FilterOptions
     * @brief   Values that represent filter options
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
     * @enum    TextureAddressMode
     * @brief   Values that represent texture address modes
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

    TENUM()
    enum class LockOptions : uint32_t
    {
        /**< 映射到内存的资源用于读取 */
        kRead = 0,
        /**< 映射到内存的资源用于写入 */
        kWrite,
        /**< 映射到内存的资源用于读写 */
        kReadWrite,
        /**< 映射到内存的资源用于写入，之前的资源数据将会被抛弃 */
        kWriteDiscard,
        /**< 映射到内存的资源用于写入，但不能复写已经存在的资源 */
        kWriteNoOverwrite
    };

    TENUM()
    enum class MemoryType : uint32_t
    {
        /**< 数据分别存储在 RAM 和 VRAM */
        kBoth = 0,
        /**< 数据仅存储在 RAM */
        kRAM,
        /**< 数据仅存储在 VRAM */
        kVRAM
    };

    TENUM()
    enum class Usage : uint32_t
    {
        /// 数据在初始化后，GPU 能读写，CPU 不能访问
        kStatic = 0,
        /// 资源创建后不会被 GPU 写，GPU 只进行读
        kImmutable,
        /// 资源 CPU 写，GPU 读
        kDynamic,
        /// 资源可以从 GPU 复制到 CPU 读
        kCopy,
    };

    TENUM()
    enum CPUAccessMode : uint32_t
    {
        /// CPU 不访问
        kCPUNone = 0,
        /// CPU 可写
        kCPUWrite = (1 << 0),
        /// CPU 可读
        kCPURead = (1 << 1),
        /// CPU 读写
        kCPUReadWrite = kCPURead | kCPUWrite,
    };

    TENUM()
    enum class IndexType : uint32_t
    {
        E_IT_16BITS = 0,    /**< 16位索引 */
        E_IT_32BITS,        /**< 32位索引 */
    };
}


#endif  /*__T3D_RHI_CONSTANT_H__*/
