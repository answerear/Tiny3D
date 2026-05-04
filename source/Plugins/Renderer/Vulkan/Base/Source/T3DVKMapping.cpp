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


#include "T3DVKMapping.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char * const VKMapping::POSITION = "POSITION";
    const char * const VKMapping::BLENDWEIGHT = "BLENDWEIGHT";
    const char * const VKMapping::BLENDINDICES = "BLENDINDICES";
    const char * const VKMapping::NORMAL = "NORMAL";
    const char * const VKMapping::COLOR = "COLOR";
    const char * const VKMapping::TEXCOORD = "TEXCOORD";
    const char * const VKMapping::TANGENT = "TANGENT";
    const char * const VKMapping::BINORMAL = "BINORMAL";

    //--------------------------------------------------------------------------

    VkFormat VKMapping::get(PixelFormat src)
    {
        VkFormat dst = VK_FORMAT_UNDEFINED;

        switch (src)
        {
        case PixelFormat::E_PF_PALETTE8:
            break;
        case PixelFormat::E_PF_B5G6R5:
            dst = VK_FORMAT_B5G6R5_UNORM_PACK16;
            break;
        case PixelFormat::E_PF_B5G5R5A1:
            dst = VK_FORMAT_B5G5R5A1_UNORM_PACK16;
            break;
        case PixelFormat::E_PF_B4R4G4A4:
            dst = VK_FORMAT_B4G4R4A4_UNORM_PACK16;
            break;
        case PixelFormat::E_PF_R8G8B8:
            dst = VK_FORMAT_R8G8B8A8_UNORM;
            break;
        case PixelFormat::E_PF_B8G8R8:
            dst = VK_FORMAT_B8G8R8A8_UNORM;
            break;
        case PixelFormat::E_PF_R8G8B8A8:
            dst = VK_FORMAT_R8G8B8A8_UNORM;
            break;
        case PixelFormat::E_PF_B8G8R8A8:
            dst = VK_FORMAT_B8G8R8A8_UNORM;
            break;
        case PixelFormat::E_PF_R8G8B8X8:
            dst = VK_FORMAT_R8G8B8A8_UNORM;
            break;
        case PixelFormat::E_PF_B8G8R8X8:
            dst = VK_FORMAT_B8G8R8A8_UNORM;
            break;
        case PixelFormat::E_PF_D24_UNORM_S8_UINT:
            dst = VK_FORMAT_D24_UNORM_S8_UINT;
            break;
        case PixelFormat::E_PF_D32_FLOAT_S8X24_UINT:
            dst = VK_FORMAT_D32_SFLOAT_S8_UINT;
            break;
        case PixelFormat::E_PF_D32_FLOAT:
            dst = VK_FORMAT_D32_SFLOAT;
            break;
        case PixelFormat::E_PF_D16_UNORM:
            dst = VK_FORMAT_D16_UNORM;
            break;
        }

        return dst;
    }

    //--------------------------------------------------------------------------

    VkMemoryPropertyFlags VKMapping::get(Usage src)
    {
        VkMemoryPropertyFlags dst = 0;

        switch (src)
        {
        case Usage::kStatic:
            dst = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        case Usage::kImmutable:
            dst = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        case Usage::kDynamic:
            dst = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            break;
        case Usage::kCopy:
            dst = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            break;
        }

        return dst;
    }

    //--------------------------------------------------------------------------

    VkMemoryPropertyFlags VKMapping::get(uint32_t accMode)
    {
        VkMemoryPropertyFlags dst = 0;

        switch (accMode)
        {
        case CPUAccessMode::kCPUNone:
            dst = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        case CPUAccessMode::kCPUWrite:
            dst = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            break;
        case CPUAccessMode::kCPURead:
            dst = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
            break;
        case CPUAccessMode::kCPUReadWrite:
            dst = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
            break;
        }

        return dst;
    }

    //--------------------------------------------------------------------------

    VkImageViewType VKMapping::get(TEXTURE_TYPE texType)
    {
        VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;

        switch (texType)
        {
        case TEXTURE_TYPE::TT_1D:
            viewType = VK_IMAGE_VIEW_TYPE_1D;
            break;
        case TEXTURE_TYPE::TT_2D:
            viewType = VK_IMAGE_VIEW_TYPE_2D;
            break;
        case TEXTURE_TYPE::TT_2D_ARRAY:
            viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            break;
        case TEXTURE_TYPE::TT_3D:
            viewType = VK_IMAGE_VIEW_TYPE_3D;
            break;
        case TEXTURE_TYPE::TT_CUBE:
            viewType = VK_IMAGE_VIEW_TYPE_CUBE;
            break;
        case TEXTURE_TYPE::TT_CUBE_ARRAY:
            viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
            break;
        }

        return viewType;
    }

    //--------------------------------------------------------------------------

    VkBlendFactor VKMapping::get(BlendFactor factor)
    {
        VkBlendFactor vkFactor = VK_BLEND_FACTOR_ONE;

        switch (factor)
        {
        case BlendFactor::kOne:
            vkFactor = VK_BLEND_FACTOR_ONE;
            break;
        case BlendFactor::kZero:
            vkFactor = VK_BLEND_FACTOR_ZERO;
            break;
        case BlendFactor::kDstColor:
            vkFactor = VK_BLEND_FACTOR_DST_COLOR;
            break;
        case BlendFactor::kSrcColor:
            vkFactor = VK_BLEND_FACTOR_SRC_COLOR;
            break;
        case BlendFactor::kOneMinusDstColor:
            vkFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
            break;
        case BlendFactor::kOneMinusSrcColor:
            vkFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
            break;
        case BlendFactor::kDstAlpha:
            vkFactor = VK_BLEND_FACTOR_DST_ALPHA;
            break;
        case BlendFactor::kSrcAlpha:
            vkFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            break;
        case BlendFactor::kOneMinusDstAlpha:
            vkFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
            break;
        case BlendFactor::kOneMinusSrcAlpha:
            vkFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            break;
        }

        return vkFactor;
    }

    //--------------------------------------------------------------------------

    VkBlendOp VKMapping::get(BlendOperation op)
    {
        VkBlendOp vkOp = VK_BLEND_OP_ADD;

        switch (op)
        {
        case BlendOperation::kAdd:
            vkOp = VK_BLEND_OP_ADD;
            break;
        case BlendOperation::kSubtract:
            vkOp = VK_BLEND_OP_SUBTRACT;
            break;
        case BlendOperation::kReverseSubtract:
            vkOp = VK_BLEND_OP_REVERSE_SUBTRACT;
            break;
        case BlendOperation::kMin:
            vkOp = VK_BLEND_OP_MIN;
            break;
        case BlendOperation::kMax:
            vkOp = VK_BLEND_OP_MAX;
            break;
        }

        return vkOp;
    }

    //--------------------------------------------------------------------------

    VkColorComponentFlags VKMapping::get(BlendColorWriteMask mask)
    {
        VkColorComponentFlags vkMask = 0;

        if (mask & kWriteMaskRed)
        {
            vkMask |= VK_COLOR_COMPONENT_R_BIT;
        }
        if (mask & kWriteMaskGreen)
        {
            vkMask |= VK_COLOR_COMPONENT_G_BIT;
        }
        if (mask & kWriteMaskBlue)
        {
            vkMask |= VK_COLOR_COMPONENT_B_BIT;
        }
        if (mask & kWriteMaskAlpha)
        {
            vkMask |= VK_COLOR_COMPONENT_A_BIT;
        }

        return vkMask;
    }

    //--------------------------------------------------------------------------

    VkCompareOp VKMapping::get(CompareFunction func)
    {
        VkCompareOp vkOp = VK_COMPARE_OP_ALWAYS;

        switch (func)
        {
        case CompareFunction::kAlwaysFail:
            vkOp = VK_COMPARE_OP_NEVER;
            break;
        case CompareFunction::kAlwaysPass:
            vkOp = VK_COMPARE_OP_ALWAYS;
            break;
        case CompareFunction::kLess:
            vkOp = VK_COMPARE_OP_LESS;
            break;
        case CompareFunction::kLessEqual:
            vkOp = VK_COMPARE_OP_LESS_OR_EQUAL;
            break;
        case CompareFunction::kEqual:
            vkOp = VK_COMPARE_OP_EQUAL;
            break;
        case CompareFunction::kNotEqual:
            vkOp = VK_COMPARE_OP_NOT_EQUAL;
            break;
        case CompareFunction::kGreaterEqual:
            vkOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
            break;
        case CompareFunction::kGreater:
            vkOp = VK_COMPARE_OP_GREATER;
            break;
        }

        return vkOp;
    }

    //--------------------------------------------------------------------------

    VkStencilOp VKMapping::get(StencilOp op)
    {
        VkStencilOp vkOp = VK_STENCIL_OP_KEEP;

        switch (op)
        {
        case StencilOp::kKeep:
            vkOp = VK_STENCIL_OP_KEEP;
            break;
        case StencilOp::kZero:
            vkOp = VK_STENCIL_OP_ZERO;
            break;
        case StencilOp::kReplace:
            vkOp = VK_STENCIL_OP_REPLACE;
            break;
        case StencilOp::kInc:
            vkOp = VK_STENCIL_OP_INCREMENT_AND_CLAMP;
            break;
        case StencilOp::kIncWrap:
            vkOp = VK_STENCIL_OP_INCREMENT_AND_WRAP;
            break;
        case StencilOp::kDec:
            vkOp = VK_STENCIL_OP_DECREMENT_AND_CLAMP;
            break;
        case StencilOp::kDecWrap:
            vkOp = VK_STENCIL_OP_DECREMENT_AND_WRAP;
            break;
        case StencilOp::kInvert:
            vkOp = VK_STENCIL_OP_INVERT;
            break;
        }

        return vkOp;
    }

    //--------------------------------------------------------------------------

    VkPolygonMode VKMapping::get(PolygonMode mode)
    {
        VkPolygonMode vkMode = VK_POLYGON_MODE_FILL;

        switch (mode)
        {
        case PolygonMode::kNone:
            break;
        case PolygonMode::kPoint:
            vkMode = VK_POLYGON_MODE_POINT;
            break;
        case PolygonMode::kWireframe:
            vkMode = VK_POLYGON_MODE_LINE;
            break;
        case PolygonMode::kSolid:
            vkMode = VK_POLYGON_MODE_FILL;
            break;
        }

        return vkMode;
    }

    //--------------------------------------------------------------------------

    VkCullModeFlags VKMapping::get(CullingMode mode)
    {
        VkCullModeFlags vkMode = VK_CULL_MODE_BACK_BIT;

        switch (mode)
        {
        case CullingMode::kNone:
            vkMode = VK_CULL_MODE_NONE;
            break;
        case CullingMode::kFront:
            vkMode = VK_CULL_MODE_FRONT_BIT;
            break;
        case CullingMode::kBack:
            vkMode = VK_CULL_MODE_BACK_BIT;
            break;
        }

        return vkMode;
    }

    //--------------------------------------------------------------------------

    VkSamplerAddressMode VKMapping::get(TextureAddressMode mode)
    {
        VkSamplerAddressMode vkMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        switch (mode)
        {
        case TextureAddressMode::kWrap:
            vkMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            break;
        case TextureAddressMode::kMirror:
            vkMode = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            break;
        case TextureAddressMode::kClamp:
            vkMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            break;
        case TextureAddressMode::kBorder:
            vkMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            break;
        }

        return vkMode;
    }

    //--------------------------------------------------------------------------

    VkFilter VKMapping::get(FilterOptions filter)
    {
        VkFilter vkFilter = VK_FILTER_LINEAR;

        switch (filter)
        {
        case FilterOptions::kPoint:
            vkFilter = VK_FILTER_NEAREST;
            break;
        case FilterOptions::kLinear:
        case FilterOptions::kAnisotropic:
            vkFilter = VK_FILTER_LINEAR;
            break;
        }

        return vkFilter;
    }

    //--------------------------------------------------------------------------

    VkSamplerMipmapMode VKMapping::getMipmapMode(FilterOptions mipFilter)
    {
        VkSamplerMipmapMode vkMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        switch (mipFilter)
        {
        case FilterOptions::kPoint:
            vkMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
            break;
        case FilterOptions::kLinear:
        case FilterOptions::kAnisotropic:
            vkMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            break;
        }

        return vkMode;
    }

    //--------------------------------------------------------------------------

    const char *VKMapping::get(VertexAttribute::Semantic semantic)
    {
        switch (semantic)
        {
        case VertexAttribute::Semantic::E_VAS_POSITION:
            return POSITION;
        case VertexAttribute::Semantic::E_VAS_BLENDWEIGHT:
            return BLENDWEIGHT;
        case VertexAttribute::Semantic::E_VAS_BLENDINDICES:
            return BLENDINDICES;
        case VertexAttribute::Semantic::E_VAS_NORMAL:
            return NORMAL;
        case VertexAttribute::Semantic::E_VAS_DIFFUSE:
        case VertexAttribute::Semantic::E_VAS_SPECULAR:
            return COLOR;
        case VertexAttribute::Semantic::E_VAS_TEXCOORD:
            return TEXCOORD;
        case VertexAttribute::Semantic::E_VAS_TANGENT:
            return TANGENT;
        case VertexAttribute::Semantic::E_VAS_BINORMAL:
            return BINORMAL;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    VkFormat VKMapping::get(VertexAttribute::Type type)
    {
        VkFormat vkFormat = VK_FORMAT_R32G32B32_SFLOAT;

        switch (type)
        {
        case VertexAttribute::Type::E_VAT_COLOR:
            vkFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
            break;
        case VertexAttribute::Type::E_VAT_FLOAT1:
            vkFormat = VK_FORMAT_R32_SFLOAT;
            break;
        case VertexAttribute::Type::E_VAT_FLOAT2:
            vkFormat = VK_FORMAT_R32G32_SFLOAT;
            break;
        case VertexAttribute::Type::E_VAT_FLOAT3:
            vkFormat = VK_FORMAT_R32G32B32_SFLOAT;
            break;
        case VertexAttribute::Type::E_VAT_FLOAT4:
            vkFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
            break;
        case VertexAttribute::Type::E_VAT_SHORT2:
            vkFormat = VK_FORMAT_R16G16_SINT;
            break;
        case VertexAttribute::Type::E_VAT_SHORT4:
            vkFormat = VK_FORMAT_R16G16B16A16_SINT;
            break;
        case VertexAttribute::Type::E_VAT_UBYTE4:
            vkFormat = VK_FORMAT_R8G8B8A8_UINT;
            break;
        case VertexAttribute::Type::E_VAT_UBYTE4_NORM:
            vkFormat = VK_FORMAT_R8G8B8A8_UNORM;
            break;
        case VertexAttribute::Type::E_VAT_BYTE4:
            vkFormat = VK_FORMAT_R8G8B8A8_SINT;
            break;
        case VertexAttribute::Type::E_VAT_BYTE4_NORM:
            vkFormat = VK_FORMAT_R8G8B8A8_SNORM;
            break;
        }

        return vkFormat;
    }

    //--------------------------------------------------------------------------

    VkIndexType VKMapping::get(IndexType idxType)
    {
        VkIndexType vkType = VK_INDEX_TYPE_UINT16;

        switch (idxType)
        {
        case IndexType::E_IT_16BITS:
            vkType = VK_INDEX_TYPE_UINT16;
            break;
        case IndexType::E_IT_32BITS:
            vkType = VK_INDEX_TYPE_UINT32;
            break;
        default:
            vkType = VK_INDEX_TYPE_UINT16;
            break;
        }

        return vkType;
    }

    //--------------------------------------------------------------------------

    VkPrimitiveTopology VKMapping::get(PrimitiveType primitive)
    {
        VkPrimitiveTopology vkPrimitive = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        switch (primitive)
        {
        case PrimitiveType::kPointList:
            vkPrimitive = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            break;
        case PrimitiveType::kLineList:
            vkPrimitive = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            break;
        case PrimitiveType::kLineStrip:
            vkPrimitive = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            break;
        case PrimitiveType::kTriangleList:
            vkPrimitive = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        case PrimitiveType::kTriangleStrip:
            vkPrimitive = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            break;
        default:
            vkPrimitive = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        }

        return vkPrimitive;
    }

    //--------------------------------------------------------------------------
}
