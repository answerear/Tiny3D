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


#include "T3DD3D11Mapping.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    DXGI_FORMAT D3D11Mapping::get(PixelFormat src)
    {
        DXGI_FORMAT dst;
        switch (src)
        {
        case PixelFormat::E_PF_PALETTE8:
            
            break;
        case PixelFormat::E_PF_B5G6R5:
            dst = DXGI_FORMAT_B5G6R5_UNORM;
            break;
        case PixelFormat::E_PF_B5G5R5A1:
            dst = DXGI_FORMAT_B5G5R5A1_UNORM;
            break;
        case PixelFormat::E_PF_B4R4G4A4:
            dst = DXGI_FORMAT_B4G4R4A4_UNORM;
            break;
        case PixelFormat::E_PF_R8G8B8:
            dst = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case PixelFormat::E_PF_B8G8R8:
            dst = DXGI_FORMAT_B8G8R8X8_UNORM;
            break;
        case PixelFormat::E_PF_R8G8B8A8:
            dst = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case PixelFormat::E_PF_B8G8R8A8:
            dst = DXGI_FORMAT_B8G8R8A8_UNORM;
            break;
        case PixelFormat::E_PF_R8G8B8X8:
            dst = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case PixelFormat::E_PF_B8G8R8X8:
            dst = DXGI_FORMAT_B8G8R8X8_UNORM;
            break;
        }
        return dst;
    }

    //--------------------------------------------------------------------------

    D3D11_USAGE D3D11Mapping::get(Usage src)
    {
        D3D11_USAGE dst;

        switch (src)
        {
        case Usage::kStatic:
            dst = D3D11_USAGE_DEFAULT;
            break;
        case Usage::kImmutable:
            dst = D3D11_USAGE_IMMUTABLE;
            break;
        case Usage::kDynamic:
            dst = D3D11_USAGE_DYNAMIC;
            break;
        case Usage::kCopy:
            dst = D3D11_USAGE_STAGING;
            break;
        }
        
        return dst;
    }

    //--------------------------------------------------------------------------

    UINT D3D11Mapping::get(uint32_t accMode)
    {
        UINT dst = 0;

        switch (accMode)
        {
        case CPUAccessMode::kCPUNone:
            dst = 0;
            break;
        case CPUAccessMode::kCPUWrite:
            dst = D3D11_CPU_ACCESS_WRITE;
            break;
        case CPUAccessMode::kCPURead:
            dst = D3D11_CPU_ACCESS_READ;
            break;
        case CPUAccessMode::kCPUReadWrite:
            dst = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
            break;
        }
        
        return dst;
    }

    //--------------------------------------------------------------------------

    D3D11_TEXTURE1D_DESC D3D11Mapping::get(const PixelBuffer1DDesc &src)
    {
        D3D11_TEXTURE1D_DESC dst;
        memset(&dst, 0, sizeof(dst));
        
        dst.Width = src.width;          // 设置纹理宽度
        dst.MipLevels = src.mipmaps;    // 设置 Mipmap 层数
        dst.ArraySize = src.arraySize;  // 设置纹理数组大小
        dst.Format = get(src.format);   // 设置纹理格式
        
        return dst;
    }

    //--------------------------------------------------------------------------

    D3D11_TEXTURE2D_DESC D3D11Mapping::get(const PixelBuffer2DDesc &src)
    {
        D3D11_TEXTURE2D_DESC dst;
        memset(&dst, 0, sizeof(dst));
        
        dst.Width = src.width;          // 设置纹理宽度
        dst.Height = src.height;        // 设置纹理高度
        dst.MipLevels = src.mipmaps;    // 设置 Mipmap 层数
        dst.ArraySize = src.arraySize;  // 设置纹理数组大小
        dst.Format = get(src.format);   // 设置纹理格式
        dst.SampleDesc.Count = src.sampleDesc.Count;        // 设置多重采样数量
        dst.SampleDesc.Quality = src.sampleDesc.Quality;    // 设置多重采样质量
        
        return dst;
    }

    //--------------------------------------------------------------------------

    D3D11_TEXTURE3D_DESC D3D11Mapping::get(const PixelBuffer3DDesc &src)
    {
        D3D11_TEXTURE3D_DESC dst;
        memset(&dst, 0, sizeof(dst));
        return dst;
    }

    //--------------------------------------------------------------------------

    D3D_SRV_DIMENSION D3D11Mapping::get(TEXTURE_TYPE texType)
    {
        D3D_SRV_DIMENSION dim = D3D_SRV_DIMENSION_UNKNOWN;

        switch (texType)
        {
        case TEXTURE_TYPE::TT_1D:
            dim = D3D11_SRV_DIMENSION_TEXTURE1D;
            break;
        case TEXTURE_TYPE::TT_2D:
            dim = D3D11_SRV_DIMENSION_TEXTURE2D;
            break;
        case TEXTURE_TYPE::TT_2D_ARRAY:
            dim = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            break;
        case TEXTURE_TYPE::TT_3D:
            dim = D3D11_SRV_DIMENSION_TEXTURE3D;
            break;
        case TEXTURE_TYPE::TT_CUBE:
            dim = D3D11_SRV_DIMENSION_TEXTURECUBE;
            break;
        case TEXTURE_TYPE::TT_CUBE_ARRAY:
            dim = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
            break;
        }

        return dim;
    }

    //--------------------------------------------------------------------------

    TEXTURE_TYPE D3D11Mapping::get(D3D_SRV_DIMENSION srvDim)
    {
        TEXTURE_TYPE texType = TEXTURE_TYPE::TT_2D;

        switch (srvDim)
        {
        case D3D11_SRV_DIMENSION_TEXTURE1D:
            texType = TEXTURE_TYPE::TT_1D;
            break;
        case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
            break;
        case D3D11_SRV_DIMENSION_TEXTURE2D:
            texType = TEXTURE_TYPE::TT_2D;
            break;
        case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
            texType = TEXTURE_TYPE::TT_2D_ARRAY;
            break;
        case D3D11_SRV_DIMENSION_TEXTURE2DMS:
            break;
        case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
            break;
        case D3D11_SRV_DIMENSION_TEXTURE3D:
            texType = TEXTURE_TYPE::TT_3D;
            break;
        case D3D11_SRV_DIMENSION_TEXTURECUBE:
            texType = TEXTURE_TYPE::TT_CUBE;
            break;
        case D3D11_SRV_DIMENSION_TEXTURECUBEARRAY:
            texType = TEXTURE_TYPE::TT_CUBE_ARRAY;
            break;
        }

        return texType;
    }

    //--------------------------------------------------------------------------

    D3D11_BLEND D3D11Mapping::get(BlendFactor factor)
    {
        D3D11_BLEND dst = D3D11_BLEND_ONE;

        switch (factor)
        {
        case BlendFactor::kOne:  /// 1.0
            dst = D3D11_BLEND_ONE;
            break;
        case BlendFactor::kZero:    /// 0.0
            dst = D3D11_BLEND_ZERO;
            break;
        case BlendFactor::kDstColor:    /// C_dst
            dst = D3D11_BLEND_DEST_COLOR;
            break;
        case BlendFactor::kSrcColor:    /// C_src
            dst = D3D11_BLEND_SRC_COLOR;
            break;
        case BlendFactor::kOneMinusDstColor:    /// 1 - C_dst
            dst = D3D11_BLEND_INV_DEST_COLOR;
            break;
        case BlendFactor::kOneMinusSrcColor:    /// 1 - C_src
            dst = D3D11_BLEND_INV_SRC_COLOR;
            break;
        case BlendFactor::kDstAlpha:    /// A_dst
            dst = D3D11_BLEND_DEST_ALPHA;
            break;
        case BlendFactor::kSrcAlpha:    /// A_src
            dst = D3D11_BLEND_SRC_ALPHA;
            break;
        case BlendFactor::kOneMinusDstAlpha:    /// 1 - A_dst
            dst = D3D11_BLEND_INV_DEST_ALPHA;
            break;
        case BlendFactor::kOneMinusSrcAlpha:    /// 1 - A_src
            dst = D3D11_BLEND_INV_SRC_ALPHA;
            break;
        }
        
        return dst;
    }

    //--------------------------------------------------------------------------

    D3D11_BLEND_OP D3D11Mapping::get(BlendOperation op)
    {
        D3D11_BLEND_OP dst = D3D11_BLEND_OP_ADD;

        switch (op)
        {
        case BlendOperation::kAdd:  /// C_result = C_src * F_src + C_dst * F_dst
            dst = D3D11_BLEND_OP_ADD;
            break;
        case BlendOperation::kSubtract: /// C_result = C_src * F_src - C_dst * F_dst
            dst = D3D11_BLEND_OP_SUBTRACT;
            break;
        case BlendOperation::kReverseSubtract:  /// C_result = C_dst * F_dst - C_src * F_src
            dst = D3D11_BLEND_OP_REV_SUBTRACT;
            break;
        case BlendOperation::kMin:  ///
            dst = D3D11_BLEND_OP_MIN;
            break;
        case BlendOperation::kMax:
            dst = D3D11_BLEND_OP_MAX;
            break;
        }
        
        return dst;
    }
    
    //--------------------------------------------------------------------------

    uint8_t D3D11Mapping::get(BlendColorWriteMask mask)
    {
        uint8_t dst = 0;

        if (mask & kWriteMaskRed)
        {
            dst |= D3D11_COLOR_WRITE_ENABLE_RED;
        }
        if (mask & kWriteMaskGreen)
        {
            dst |= D3D11_COLOR_WRITE_ENABLE_GREEN;
        }
        if (mask & kWriteMaskBlue)
        {
            dst |= D3D11_COLOR_WRITE_ENABLE_BLUE;
        }
        if (mask & kWriteMaskAlpha)
        {
            dst |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
        }
        
        return dst;
    }

    //--------------------------------------------------------------------------

    D3D11_COMPARISON_FUNC D3D11Mapping::get(CompareFunction func)
    {
        D3D11_COMPARISON_FUNC dst = D3D11_COMPARISON_ALWAYS;

        switch (func)
        {
        case CompareFunction::kAlwaysFail:
            dst = D3D11_COMPARISON_NEVER;
            break;
        case CompareFunction::kAlwaysPass:
            dst = D3D11_COMPARISON_ALWAYS;
            break;
        case CompareFunction::kLess:
            dst = D3D11_COMPARISON_LESS;
            break;
        case CompareFunction::kLessEqual:
            dst = D3D11_COMPARISON_LESS_EQUAL;
            break;
        case CompareFunction::kEqual:
            dst = D3D11_COMPARISON_EQUAL;
            break;
        case CompareFunction::kNotEqual:
            dst = D3D11_COMPARISON_NOT_EQUAL;
            break;
        case CompareFunction::kGreaterEqual:
            dst = D3D11_COMPARISON_NOT_EQUAL;
            break;
        case CompareFunction::kGreater:
            dst = D3D11_COMPARISON_GREATER;
            break;
        }
        
        return dst;
    }

    //--------------------------------------------------------------------------

    D3D11_STENCIL_OP D3D11Mapping::get(StencilOp op)
    {
        D3D11_STENCIL_OP dst = D3D11_STENCIL_OP_KEEP;

        switch (op)
        {
        case StencilOp::kKeep:  /**< 保持现有的模板值 */
            dst = D3D11_STENCIL_OP_KEEP;
            break;
        case StencilOp::kZero:  /**< 将模板值置为0 */
            dst = D3D11_STENCIL_OP_ZERO;
            break;
        case StencilOp::kReplace:   /**< 将模板值设置为用 HWDepthStencilState::setStencilRef 函数设置的 ref 值*/
            dst = D3D11_STENCIL_OP_REPLACE;
            break;
        case StencilOp::kInc:   /**< 如果模板值不是最大值就将模板值+1 */
            dst = D3D11_STENCIL_OP_INVERT;
            break;
        case StencilOp::kIncWrap:   /**< 与 INCR 一样将模板值+1，如果模板值已经是最大值则设为0 */
            dst = D3D11_STENCIL_OP_INCR_SAT;
            break;
        case StencilOp::kDec:   /**< 如果模板值不是最小值就将模板值-1 */
            dst = D3D11_STENCIL_OP_DECR;
            break;
        case StencilOp::kDecWrap:   /**< 与 DECR 一样将模板值-1，如果模板值已经是最小值则设为最大值 */
            dst = D3D11_STENCIL_OP_DECR_SAT;
            break;
        case StencilOp::kInvert:    /**< 把模板值按位取反 */
            dst = D3D11_STENCIL_OP_INVERT;
            break;
        }
        return dst;
    }
    
    //--------------------------------------------------------------------------
}


