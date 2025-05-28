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

    const char * const D3D11Mapping::POSITION = "POSITION";
    const char * const D3D11Mapping::BLENDWEIGHT = "BLENDWEIGHT";
    const char * const D3D11Mapping::BLENDINDICES = "BLENDINDICES";
    const char * const D3D11Mapping::NORMAL = "NORMAL";
    const char * const D3D11Mapping::COLOR = "COLOR";
    const char * const D3D11Mapping::TEXCOORD = "TEXCOORD";
    const char * const D3D11Mapping::TANGENT = "TANGENT";
    const char * const D3D11Mapping::BINORMAL = "BINORMAL";
    
    //--------------------------------------------------------------------------

    DXGI_FORMAT D3D11Mapping::get(PixelFormat src)
    {
        DXGI_FORMAT dst = DXGI_FORMAT_UNKNOWN;
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
        case PixelFormat::E_PF_D24_UNORM_S8_UINT:
            dst = DXGI_FORMAT_D24_UNORM_S8_UINT;
            break;
        case PixelFormat::E_PF_D32_FLOAT_S8X24_UINT:
            dst = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
            break;
        case PixelFormat::E_PF_D32_FLOAT:
            dst = DXGI_FORMAT_D32_FLOAT;
            break;
        case PixelFormat::E_PF_D16_UNORM:
            dst = DXGI_FORMAT_D16_UNORM;
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
        D3D11_BLEND d3dFactor = D3D11_BLEND_ONE;

        switch (factor)
        {
        case BlendFactor::kOne:  /// 1.0
            d3dFactor = D3D11_BLEND_ONE;
            break;
        case BlendFactor::kZero:    /// 0.0
            d3dFactor = D3D11_BLEND_ZERO;
            break;
        case BlendFactor::kDstColor:    /// C_dst
            d3dFactor = D3D11_BLEND_DEST_COLOR;
            break;
        case BlendFactor::kSrcColor:    /// C_src
            d3dFactor = D3D11_BLEND_SRC_COLOR;
            break;
        case BlendFactor::kOneMinusDstColor:    /// 1 - C_dst
            d3dFactor = D3D11_BLEND_INV_DEST_COLOR;
            break;
        case BlendFactor::kOneMinusSrcColor:    /// 1 - C_src
            d3dFactor = D3D11_BLEND_INV_SRC_COLOR;
            break;
        case BlendFactor::kDstAlpha:    /// A_dst
            d3dFactor = D3D11_BLEND_DEST_ALPHA;
            break;
        case BlendFactor::kSrcAlpha:    /// A_src
            d3dFactor = D3D11_BLEND_SRC_ALPHA;
            break;
        case BlendFactor::kOneMinusDstAlpha:    /// 1 - A_dst
            d3dFactor = D3D11_BLEND_INV_DEST_ALPHA;
            break;
        case BlendFactor::kOneMinusSrcAlpha:    /// 1 - A_src
            d3dFactor = D3D11_BLEND_INV_SRC_ALPHA;
            break;
        }
        
        return d3dFactor;
    }

    //--------------------------------------------------------------------------

    D3D11_BLEND_OP D3D11Mapping::get(BlendOperation op)
    {
        D3D11_BLEND_OP d3dOp = D3D11_BLEND_OP_ADD;

        switch (op)
        {
        case BlendOperation::kAdd:  /// C_result = C_src * F_src + C_dst * F_dst
            d3dOp = D3D11_BLEND_OP_ADD;
            break;
        case BlendOperation::kSubtract: /// C_result = C_src * F_src - C_dst * F_dst
            d3dOp = D3D11_BLEND_OP_SUBTRACT;
            break;
        case BlendOperation::kReverseSubtract:  /// C_result = C_dst * F_dst - C_src * F_src
            d3dOp = D3D11_BLEND_OP_REV_SUBTRACT;
            break;
        case BlendOperation::kMin:  ///
            d3dOp = D3D11_BLEND_OP_MIN;
            break;
        case BlendOperation::kMax:
            d3dOp = D3D11_BLEND_OP_MAX;
            break;
        }
        
        return d3dOp;
    }
    
    //--------------------------------------------------------------------------

    uint8_t D3D11Mapping::get(BlendColorWriteMask mask)
    {
        uint8_t d3dMask = 0;

        if (mask & kWriteMaskRed)
        {
            d3dMask |= D3D11_COLOR_WRITE_ENABLE_RED;
        }
        if (mask & kWriteMaskGreen)
        {
            d3dMask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
        }
        if (mask & kWriteMaskBlue)
        {
            d3dMask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
        }
        if (mask & kWriteMaskAlpha)
        {
            d3dMask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
        }
        
        return d3dMask;
    }

    //--------------------------------------------------------------------------

    D3D11_COMPARISON_FUNC D3D11Mapping::get(CompareFunction func)
    {
        D3D11_COMPARISON_FUNC d3dFunc = D3D11_COMPARISON_ALWAYS;

        switch (func)
        {
        case CompareFunction::kAlwaysFail:
            d3dFunc = D3D11_COMPARISON_NEVER;
            break;
        case CompareFunction::kAlwaysPass:
            d3dFunc = D3D11_COMPARISON_ALWAYS;
            break;
        case CompareFunction::kLess:
            d3dFunc = D3D11_COMPARISON_LESS;
            break;
        case CompareFunction::kLessEqual:
            d3dFunc = D3D11_COMPARISON_LESS_EQUAL;
            break;
        case CompareFunction::kEqual:
            d3dFunc = D3D11_COMPARISON_EQUAL;
            break;
        case CompareFunction::kNotEqual:
            d3dFunc = D3D11_COMPARISON_NOT_EQUAL;
            break;
        case CompareFunction::kGreaterEqual:
            d3dFunc = D3D11_COMPARISON_NOT_EQUAL;
            break;
        case CompareFunction::kGreater:
            d3dFunc = D3D11_COMPARISON_GREATER;
            break;
        }
        
        return d3dFunc;
    }

    //--------------------------------------------------------------------------

    D3D11_STENCIL_OP D3D11Mapping::get(StencilOp op)
    {
        D3D11_STENCIL_OP d3dOp = D3D11_STENCIL_OP_KEEP;

        switch (op)
        {
        case StencilOp::kKeep:  /**< 保持现有的模板值 */
            d3dOp = D3D11_STENCIL_OP_KEEP;
            break;
        case StencilOp::kZero:  /**< 将模板值置为0 */
            d3dOp = D3D11_STENCIL_OP_ZERO;
            break;
        case StencilOp::kReplace:   /**< 将模板值设置为用 HWDepthStencilState::setStencilRef 函数设置的 ref 值*/
            d3dOp = D3D11_STENCIL_OP_REPLACE;
            break;
        case StencilOp::kInc:   /**< 如果模板值不是最大值就将模板值+1 */
            d3dOp = D3D11_STENCIL_OP_INVERT;
            break;
        case StencilOp::kIncWrap:   /**< 与 INCR 一样将模板值+1，如果模板值已经是最大值则设为0 */
            d3dOp = D3D11_STENCIL_OP_INCR_SAT;
            break;
        case StencilOp::kDec:   /**< 如果模板值不是最小值就将模板值-1 */
            d3dOp = D3D11_STENCIL_OP_DECR;
            break;
        case StencilOp::kDecWrap:   /**< 与 DECR 一样将模板值-1，如果模板值已经是最小值则设为最大值 */
            d3dOp = D3D11_STENCIL_OP_DECR_SAT;
            break;
        case StencilOp::kInvert:    /**< 把模板值按位取反 */
            d3dOp = D3D11_STENCIL_OP_INVERT;
            break;
        }
        return d3dOp;
    }
    
    //--------------------------------------------------------------------------

    D3D11_FILL_MODE D3D11Mapping::get(PolygonMode mode)
    {
        D3D11_FILL_MODE d3dMode = D3D11_FILL_SOLID;
        
        switch (mode)
        {
        case PolygonMode::kNone:
            break;
        case PolygonMode::kPoint:
            break;
        case PolygonMode::kWireframe:
            d3dMode = D3D11_FILL_WIREFRAME;
            break;
        case PolygonMode::kSolid:
            d3dMode = D3D11_FILL_SOLID;
            break;
        }
        
        return d3dMode;
    }

    //--------------------------------------------------------------------------

    D3D11_CULL_MODE D3D11Mapping::get(CullingMode mode)
    {
        D3D11_CULL_MODE d3dMode = D3D11_CULL_BACK;

        switch (mode)
        {
        case CullingMode::kNone:
            d3dMode = D3D11_CULL_NONE;
            break;
        case CullingMode::kFront:
            d3dMode = D3D11_CULL_FRONT;
            break;
        case CullingMode::kBack:
            d3dMode = D3D11_CULL_BACK;
            break;
        }
        
        return d3dMode;
    }
    
    //--------------------------------------------------------------------------

    D3D11_TEXTURE_ADDRESS_MODE D3D11Mapping::get(TextureAddressMode mode)
    {
        D3D11_TEXTURE_ADDRESS_MODE d3dMode = D3D11_TEXTURE_ADDRESS_WRAP;

        switch (mode)
        {
        case TextureAddressMode::kWrap:
            d3dMode = D3D11_TEXTURE_ADDRESS_WRAP;
            break;
        case TextureAddressMode::kMirror:
            d3dMode = D3D11_TEXTURE_ADDRESS_MIRROR;
            break;
        case TextureAddressMode::kClamp:
            d3dMode = D3D11_TEXTURE_ADDRESS_CLAMP;
            break;
        case TextureAddressMode::kBorder:
            d3dMode = D3D11_TEXTURE_ADDRESS_BORDER;
            break;
        }

        return d3dMode;
    }
    
    //--------------------------------------------------------------------------

    D3D11_FILTER D3D11Mapping::get(FilterOptions min, FilterOptions mag, FilterOptions mip, bool comparison)
    {
        if (min == FilterOptions::kAnisotropic
            || mag == FilterOptions::kAnisotropic
            || mip == FilterOptions::kAnisotropic)
            return comparison ? D3D11_FILTER_COMPARISON_ANISOTROPIC : D3D11_FILTER_ANISOTROPIC;

        // FilterOptions::FilterOptions::NONE is not supported
#define MERGE_FOR_SWITCH(_comparison_, _min_ , _mag_, _mip_ ) \
    ((_comparison_ ? 8 : 0) | (_min_ == FilterOptions::kLinear ? 4 : 0) | (_mag_ == FilterOptions::kLinear ? 2 : 0) | (_mip_ == FilterOptions::kLinear ? 1 : 0))

        switch ((MERGE_FOR_SWITCH(comparison, min, mag, mip)))
        {
        case MERGE_FOR_SWITCH(true, FilterOptions::kPoint, FilterOptions::kPoint, FilterOptions::kPoint):
            return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
        case MERGE_FOR_SWITCH(true, FilterOptions::kPoint, FilterOptions::kPoint, FilterOptions::kLinear):
            return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
        case MERGE_FOR_SWITCH(true, FilterOptions::kPoint, FilterOptions::kLinear, FilterOptions::kPoint):
            return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
        case MERGE_FOR_SWITCH(true, FilterOptions::kPoint, FilterOptions::kLinear, FilterOptions::kLinear):
            return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
        case MERGE_FOR_SWITCH(true, FilterOptions::kLinear, FilterOptions::kPoint, FilterOptions::kPoint):
            return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
        case MERGE_FOR_SWITCH(true, FilterOptions::kLinear, FilterOptions::kPoint, FilterOptions::kLinear):
            return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        case MERGE_FOR_SWITCH(true, FilterOptions::kLinear, FilterOptions::kLinear, FilterOptions::kPoint):
            return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
        case MERGE_FOR_SWITCH(true, FilterOptions::kLinear, FilterOptions::kLinear, FilterOptions::kLinear):
            return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        case MERGE_FOR_SWITCH(false, FilterOptions::kPoint, FilterOptions::kPoint, FilterOptions::kPoint):
            return D3D11_FILTER_MIN_MAG_MIP_POINT;
        case MERGE_FOR_SWITCH(false, FilterOptions::kPoint, FilterOptions::kPoint, FilterOptions::kLinear):
            return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
        case MERGE_FOR_SWITCH(false, FilterOptions::kPoint, FilterOptions::kLinear, FilterOptions::kPoint):
            return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
        case MERGE_FOR_SWITCH(false, FilterOptions::kPoint, FilterOptions::kLinear, FilterOptions::kLinear):
            return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
        case MERGE_FOR_SWITCH(false, FilterOptions::kLinear, FilterOptions::kPoint, FilterOptions::kPoint):
            return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        case MERGE_FOR_SWITCH(false, FilterOptions::kLinear, FilterOptions::kPoint, FilterOptions::kLinear):
            return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        case MERGE_FOR_SWITCH(false, FilterOptions::kLinear, FilterOptions::kLinear, FilterOptions::kPoint):
            return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        case MERGE_FOR_SWITCH(false, FilterOptions::kLinear, FilterOptions::kLinear, FilterOptions::kLinear):
            return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        }

#undef MERGE_FOR_SWITCH

        return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    }
    
    //--------------------------------------------------------------------------

    const char *D3D11Mapping::get(VertexAttribute::Semantic semantic)
    {
        switch (semantic)
        {
        case VertexAttribute::Semantic::E_VAS_POSITION:
            return POSITION;
            break;
        case VertexAttribute::Semantic::E_VAS_BLENDWEIGHT:
            return BLENDWEIGHT;
            break;
        case VertexAttribute::Semantic::E_VAS_BLENDINDICES:
            return BLENDINDICES;
            break;
        case VertexAttribute::Semantic::E_VAS_NORMAL:
            return NORMAL;
            break;
        case VertexAttribute::Semantic::E_VAS_DIFFUSE:
        case VertexAttribute::Semantic::E_VAS_SPECULAR:
            return COLOR;
            break;
        case VertexAttribute::Semantic::E_VAS_TEXCOORD:
            return TEXCOORD;
            break;
        case VertexAttribute::Semantic::E_VAS_TANGENT:
            return TANGENT;
            break;
        case VertexAttribute::Semantic::E_VAS_BINORMAL:
            return BINORMAL;
            break;
        }

        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    DXGI_FORMAT D3D11Mapping::get(VertexAttribute::Type type)
    {
        DXGI_FORMAT d3dformat = DXGI_FORMAT_R32G32B32_FLOAT;

        switch (type)
        {
        case VertexAttribute::Type::E_VAT_COLOR:
            d3dformat = DXGI_FORMAT_R32G32B32A32_FLOAT;
            break;
        case VertexAttribute::Type::E_VAT_FLOAT1:
            d3dformat = DXGI_FORMAT_R32_FLOAT;
            break;
        case VertexAttribute::Type::E_VAT_FLOAT2:
            d3dformat = DXGI_FORMAT_R32G32_FLOAT;
            break;
        case VertexAttribute::Type::E_VAT_FLOAT3:
            d3dformat = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case VertexAttribute::Type::E_VAT_FLOAT4:
            d3dformat = DXGI_FORMAT_R32G32B32A32_FLOAT;
            break;
        case VertexAttribute::Type::E_VAT_SHORT2:
            d3dformat = DXGI_FORMAT_R16G16_SINT;
            break;
        case VertexAttribute::Type::E_VAT_SHORT4:
            d3dformat = DXGI_FORMAT_R16G16B16A16_SINT;
            break;
        case VertexAttribute::Type::E_VAT_UBYTE4:
            d3dformat = DXGI_FORMAT_R8G8B8A8_UINT;
            break;
        }

        return d3dformat;
    }
    
    //--------------------------------------------------------------------------

    TResult D3D11Mapping::get(Usage usage, uint32_t mode, D3D11_USAGE &d3dUsage, uint32_t &d3dAccessFlag)
    {
        TResult ret = T3D_OK;

        do 
        {
            switch (usage)
            {
            case Usage::kImmutable:
                {
                    if (mode != kCPUNone)
                    {
                        ret = T3D_ERR_INVALID_PARAM;
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Usage is kImmutable, so access mode must be kCPUNone !");
                    }
                    else
                    {
                        d3dUsage = D3D11_USAGE_IMMUTABLE;
                        d3dAccessFlag = 0;
                    }
                }
                break;
            case Usage::kStatic:
                {
                    if (mode == CPUAccessMode::kCPUNone)
                    {
                        // 静态缓冲，CPU不可读写，只能初始化时候设置数据
                        d3dUsage = D3D11_USAGE_IMMUTABLE;
                        d3dAccessFlag = 0;
                    }
                    else if (mode == CPUAccessMode::kCPUWrite)
                    {
                        d3dUsage = D3D11_USAGE_DEFAULT;
                        d3dAccessFlag = D3D11_CPU_ACCESS_WRITE;
                    }
                    else
                    {
                        // 其他 CPU 访问标签在这里都是非法
                        ret = T3D_ERR_INVALID_PARAM;
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Usage is kStatic, so access mode must be kCPUNone or kCPUWrite !");
                    }
                }
                break;
            case Usage::kDynamic:
                {
                    if (mode == CPUAccessMode::kCPUNone)
                    {
                        // CPU不读也不写，这里建议使用STATIC性能更好
                        d3dUsage = D3D11_USAGE_DEFAULT;
                        d3dAccessFlag = 0;
                        T3D_LOG_WARNING(LOG_TAG_D3D11RENDERER, "Usage is kDynamic, but CPU access mode is kCPUNone. Here suggests kStatic instead of kDynamic !");
                    }
                    else if ((mode == (CPUAccessMode::kCPURead | CPUAccessMode::kCPUWrite)))
                    {
                        // CPU读写，GPU读写
                        d3dUsage = D3D11_USAGE_STAGING;
                        d3dAccessFlag = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
                    }
                    else if (mode == CPUAccessMode::kCPURead)
                    {
                        // CPU读，GPU读写
                        d3dUsage = D3D11_USAGE_STAGING;
                        d3dAccessFlag = D3D11_CPU_ACCESS_READ;
                    }
                    else if (mode == CPUAccessMode::kCPUWrite)
                    {
                        // CPU写，GPU读
                        d3dUsage = D3D11_USAGE_DYNAMIC;
                        d3dAccessFlag = D3D11_CPU_ACCESS_WRITE;
                    }
                    // else if (mode == CPUAccessMode::GPU_COPY)
                    // {
                    //     // CPU读写，GPU读写
                    //     d3dUsage = D3D11_USAGE_STAGING;
                    //     d3dAccessFlag = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
                    // }
                    else
                    {
                        // 无效 CPU 访问方式参数
                        ret = T3D_ERR_INVALID_PARAM;
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Invalid CPU access mode parameter !");
                    }
                }
                break;
            default:
                {
                    ret = T3D_ERR_INVALID_PARAM;
                }
                break;
            }
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------
    
    DXGI_FORMAT D3D11Mapping::get(IndexType idxType)
    {
        DXGI_FORMAT d3dFormat;

        switch (idxType)
        {
        case IndexType::E_IT_16BITS:
            d3dFormat = DXGI_FORMAT_R16_UINT;
            break;
        case IndexType::E_IT_32BITS:
            d3dFormat = DXGI_FORMAT_R32_UINT;
            break;
        default:
            d3dFormat = DXGI_FORMAT_R16_UINT;
            break;
        }

        return d3dFormat;
    }
        
    //--------------------------------------------------------------------------

    D3D11_PRIMITIVE_TOPOLOGY D3D11Mapping::get(PrimitiveType primitive)
    {
        D3D11_PRIMITIVE_TOPOLOGY d3dPrimitive;

        switch (primitive)
        {
        case PrimitiveType::kPointList:
            d3dPrimitive = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
            break;
        case PrimitiveType::kLineList:
            d3dPrimitive = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
            break;
        case PrimitiveType::kLineStrip:
            d3dPrimitive = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
            break;
        case PrimitiveType::kTriangleList:
            d3dPrimitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            break;
        case PrimitiveType::kTriangleStrip:
            d3dPrimitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            break;
        default:
            d3dPrimitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            break;
        }

        return d3dPrimitive;
    }
    
    //--------------------------------------------------------------------------

    ShaderConstantParam::DATA_TYPE D3D11Mapping::get(D3D_SHADER_VARIABLE_TYPE d3dType, uint32_t rows, uint32_t cols, uint32_t numOfElements)
    {
        ShaderConstantParam::DATA_TYPE type = ShaderConstantParam::DATA_TYPE::DT_FLOAT;

        switch (d3dType)
        {
        case D3D_SVT_BOOL:
            {
                type = ShaderConstantParam::DATA_TYPE::DT_BOOL;
                if (numOfElements > 1)
                {
                    type = ShaderConstantParam::DATA_TYPE::DT_BOOL_ARRAY;
                }
            }
            break;
        case D3D_SVT_INT:
            {
                type = ShaderConstantParam::DATA_TYPE::DT_INTEGER;
                if (numOfElements > 1)
                {
                    type = ShaderConstantParam::DATA_TYPE::DT_INTEGER_ARRAY;
                }
            }
            break;
        case D3D_SVT_FLOAT:
            {
                type = ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                if (rows == 4 && cols == 4)
                {
                    // Matrix4
                    type = ShaderConstantParam::DATA_TYPE::DT_MATRIX4;
                    if (numOfElements > 1)
                    {
                        type = ShaderConstantParam::DATA_TYPE::DT_MATRIX4_ARRAY;
                    }
                }
                else if (rows == 4 && cols == 1 || rows == 1 && cols == 4)
                {
                    // Vector4
                    type = ShaderConstantParam::DATA_TYPE::DT_VECTOR4;
                    if (numOfElements > 1)
                    {
                        type = ShaderConstantParam::DATA_TYPE::DT_VECTOR4_ARRAY;
                    }
                }
                else if (numOfElements > 1)
                {
                    type = ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY;
                }
            }
            break;
        case D3D_SVT_UINT:
            break;
        case D3D_SVT_UINT8:
            break;
        case D3D_SVT_DOUBLE:
            break;
        case D3D_SVT_INT16:
            break;
        case D3D_SVT_UINT16:
            break;
        case D3D_SVT_FLOAT16:
            break;
        case D3D_SVT_INT64:
            break;
        case D3D_SVT_UINT64:
            break;
        case D3D11_SVT_STRUCTURED_BUFFER:
        case D3D11_SVT_RWSTRUCTURED_BUFFER:
            {
                type = ShaderConstantParam::DATA_TYPE::DT_STRUCT;
            }
            break;
        }

        return type;
    }
    
    //--------------------------------------------------------------------------
}


