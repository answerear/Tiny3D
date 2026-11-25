/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#include "T3DGL4Mapping.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GLenum GL4Mapping::get(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::E_PF_PALETTE8:    return GL_RED;
        case PixelFormat::E_PF_R8G8B8:      return GL_RGB;
        case PixelFormat::E_PF_B8G8R8:      return GL_BGR;
        case PixelFormat::E_PF_R8G8B8A8:    return GL_RGBA;
        case PixelFormat::E_PF_B8G8R8A8:    return GL_BGRA;
        case PixelFormat::E_PF_R8G8B8X8:    return GL_RGBA;
        case PixelFormat::E_PF_B8G8R8X8:    return GL_BGRA;
        case PixelFormat::E_PF_R8G8B8A8_SRGB:   return GL_RGBA;
        case PixelFormat::E_PF_B8G8R8A8_SRGB:   return GL_BGRA;
        case PixelFormat::E_PF_B5G6R5:      return GL_RGB;
        case PixelFormat::E_PF_B5G5R5A1:    return GL_RGBA;
        case PixelFormat::E_PF_B4R4G4A4:    return GL_RGBA;
        case PixelFormat::E_PF_R8_UNORM:            return GL_RED;
        case PixelFormat::E_PF_R8G8_UNORM:          return GL_RG;
        case PixelFormat::E_PF_R16_UNORM:           return GL_RED;
        case PixelFormat::E_PF_R16G16_UNORM:        return GL_RG;
        case PixelFormat::E_PF_R10G10B10A2_UNORM:   return GL_RGBA;
        case PixelFormat::E_PF_R16_FLOAT:           return GL_RED;
        case PixelFormat::E_PF_R16G16_FLOAT:        return GL_RG;
        case PixelFormat::E_PF_R16G16B16A16_FLOAT:  return GL_RGBA;
        case PixelFormat::E_PF_R32_FLOAT:           return GL_RED;
        case PixelFormat::E_PF_R32G32_FLOAT:        return GL_RG;
        case PixelFormat::E_PF_R32G32B32A32_FLOAT:  return GL_RGBA;
        case PixelFormat::E_PF_R11G11B10_FLOAT:     return GL_RGB;
        // 整数纹理的上传 / 读回必须用 *_INTEGER，否则 GL_INVALID_OPERATION
        case PixelFormat::E_PF_R8G8B8A8_UINT:       return GL_RGBA_INTEGER;
        case PixelFormat::E_PF_R16G16_UINT:         return GL_RG_INTEGER;
        case PixelFormat::E_PF_R32_UINT:            return GL_RED_INTEGER;
        case PixelFormat::E_PF_D24_UNORM_S8_UINT:
        case PixelFormat::E_PF_D32_FLOAT_S8X24_UINT:
            return GL_DEPTH_STENCIL;
        case PixelFormat::E_PF_D32_FLOAT:
        case PixelFormat::E_PF_D16_UNORM:
            return GL_DEPTH_COMPONENT;
        default: break;
        }
        return GL_RGBA;
    }

    //--------------------------------------------------------------------------

    GLenum GL4Mapping::getInternalFormat(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::E_PF_PALETTE8:    return GL_R8;
        case PixelFormat::E_PF_R8G8B8:      return GL_RGB8;
        case PixelFormat::E_PF_B8G8R8:      return GL_RGB8;
        case PixelFormat::E_PF_R8G8B8A8:    return GL_RGBA8;
        case PixelFormat::E_PF_B8G8R8A8:    return GL_RGBA8;
        case PixelFormat::E_PF_R8G8B8X8:    return GL_RGBA8;
        case PixelFormat::E_PF_B8G8R8X8:    return GL_RGBA8;
        case PixelFormat::E_PF_R8G8B8A8_SRGB:   return GL_SRGB8_ALPHA8;
        case PixelFormat::E_PF_B8G8R8A8_SRGB:   return GL_SRGB8_ALPHA8;
        // S3TC 不是核心功能，驱动没有 GL_EXT_texture_compression_s3tc 时能力表里不报告 kFormatSupportTexture
        case PixelFormat::E_PF_BC1_UNORM:   return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        case PixelFormat::E_PF_BC1_SRGB:    return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
        case PixelFormat::E_PF_BC2_UNORM:   return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        case PixelFormat::E_PF_BC2_SRGB:    return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
        case PixelFormat::E_PF_BC3_UNORM:   return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        case PixelFormat::E_PF_BC3_SRGB:    return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
        case PixelFormat::E_PF_BC4_UNORM:   return GL_COMPRESSED_RED_RGTC1;
        case PixelFormat::E_PF_BC4_SNORM:   return GL_COMPRESSED_SIGNED_RED_RGTC1;
        case PixelFormat::E_PF_BC5_UNORM:   return GL_COMPRESSED_RG_RGTC2;
        case PixelFormat::E_PF_BC5_SNORM:   return GL_COMPRESSED_SIGNED_RG_RGTC2;
        case PixelFormat::E_PF_BC6H_UF16:   return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB;
        case PixelFormat::E_PF_BC6H_SF16:   return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB;
        case PixelFormat::E_PF_BC7_UNORM:   return GL_COMPRESSED_RGBA_BPTC_UNORM_ARB;
        case PixelFormat::E_PF_BC7_SRGB:    return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB;
        case PixelFormat::E_PF_B5G6R5:      return GL_RGB565;
        case PixelFormat::E_PF_B5G5R5A1:    return GL_RGB5_A1;
        case PixelFormat::E_PF_B4R4G4A4:    return GL_RGBA4;
        case PixelFormat::E_PF_R8_UNORM:            return GL_R8;
        case PixelFormat::E_PF_R8G8_UNORM:          return GL_RG8;
        case PixelFormat::E_PF_R16_UNORM:           return GL_R16;
        case PixelFormat::E_PF_R16G16_UNORM:        return GL_RG16;
        case PixelFormat::E_PF_R10G10B10A2_UNORM:   return GL_RGB10_A2;
        case PixelFormat::E_PF_R16_FLOAT:           return GL_R16F;
        case PixelFormat::E_PF_R16G16_FLOAT:        return GL_RG16F;
        case PixelFormat::E_PF_R16G16B16A16_FLOAT:  return GL_RGBA16F;
        case PixelFormat::E_PF_R32_FLOAT:           return GL_R32F;
        case PixelFormat::E_PF_R32G32_FLOAT:        return GL_RG32F;
        case PixelFormat::E_PF_R32G32B32A32_FLOAT:  return GL_RGBA32F;
        case PixelFormat::E_PF_R11G11B10_FLOAT:     return GL_R11F_G11F_B10F;
        case PixelFormat::E_PF_R8G8B8A8_UINT:       return GL_RGBA8UI;
        case PixelFormat::E_PF_R16G16_UINT:         return GL_RG16UI;
        case PixelFormat::E_PF_R32_UINT:            return GL_R32UI;
        case PixelFormat::E_PF_D24_UNORM_S8_UINT:       return GL_DEPTH24_STENCIL8;
        case PixelFormat::E_PF_D32_FLOAT_S8X24_UINT:    return GL_DEPTH32F_STENCIL8;
        case PixelFormat::E_PF_D32_FLOAT:                return GL_DEPTH_COMPONENT32F;
        case PixelFormat::E_PF_D16_UNORM:                return GL_DEPTH_COMPONENT16;
        default: break;
        }
        return GL_NONE;
    }

    //--------------------------------------------------------------------------

    GLenum GL4Mapping::getPixelType(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::E_PF_PALETTE8:
        case PixelFormat::E_PF_R8G8B8:
        case PixelFormat::E_PF_B8G8R8:
        case PixelFormat::E_PF_R8G8B8A8:
        case PixelFormat::E_PF_B8G8R8A8:
        case PixelFormat::E_PF_R8G8B8X8:
        case PixelFormat::E_PF_B8G8R8X8:
        case PixelFormat::E_PF_R8G8B8A8_SRGB:
        case PixelFormat::E_PF_B8G8R8A8_SRGB:
            return GL_UNSIGNED_BYTE;
        case PixelFormat::E_PF_B5G6R5:
            return GL_UNSIGNED_SHORT_5_6_5;
        case PixelFormat::E_PF_B5G5R5A1:
            return GL_UNSIGNED_SHORT_5_5_5_1;
        case PixelFormat::E_PF_B4R4G4A4:
            return GL_UNSIGNED_SHORT_4_4_4_4;
        case PixelFormat::E_PF_R8_UNORM:
        case PixelFormat::E_PF_R8G8_UNORM:
        case PixelFormat::E_PF_R8G8B8A8_UINT:
            return GL_UNSIGNED_BYTE;
        case PixelFormat::E_PF_R16_UNORM:
        case PixelFormat::E_PF_R16G16_UNORM:
        case PixelFormat::E_PF_R16G16_UINT:
            return GL_UNSIGNED_SHORT;
        case PixelFormat::E_PF_R10G10B10A2_UNORM:
            return GL_UNSIGNED_INT_2_10_10_10_REV;
        case PixelFormat::E_PF_R16_FLOAT:
        case PixelFormat::E_PF_R16G16_FLOAT:
        case PixelFormat::E_PF_R16G16B16A16_FLOAT:
            return GL_HALF_FLOAT;
        case PixelFormat::E_PF_R32_FLOAT:
        case PixelFormat::E_PF_R32G32_FLOAT:
        case PixelFormat::E_PF_R32G32B32A32_FLOAT:
            return GL_FLOAT;
        case PixelFormat::E_PF_R11G11B10_FLOAT:
            return GL_UNSIGNED_INT_10F_11F_11F_REV;
        case PixelFormat::E_PF_R32_UINT:
            return GL_UNSIGNED_INT;
        case PixelFormat::E_PF_D24_UNORM_S8_UINT:
            return GL_UNSIGNED_INT_24_8;
        case PixelFormat::E_PF_D32_FLOAT:
            return GL_FLOAT;
        case PixelFormat::E_PF_D32_FLOAT_S8X24_UINT:
            return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
        case PixelFormat::E_PF_D16_UNORM:
            return GL_UNSIGNED_SHORT;
        default: break;
        }
        return GL_UNSIGNED_BYTE;
    }

    //--------------------------------------------------------------------------

    uint32_t GL4Mapping::getBytesPerPixel(PixelFormat format)
    {
        return PixelFormatUtil::getBytesPerPixel(format);
    }

    //--------------------------------------------------------------------------

    bool GL4Mapping::isDepthFormat(PixelFormat format)
    {
        return PixelFormatUtil::isDepthFormat(format);
    }

    //--------------------------------------------------------------------------

    bool GL4Mapping::hasStencil(PixelFormat format)
    {
        return PixelFormatUtil::hasStencil(format);
    }

    //--------------------------------------------------------------------------

    GLenum GL4Mapping::get(BlendFactor factor)
    {
        switch (factor)
        {
        case BlendFactor::kOne:                return GL_ONE;
        case BlendFactor::kZero:               return GL_ZERO;
        case BlendFactor::kDstColor:           return GL_DST_COLOR;
        case BlendFactor::kSrcColor:           return GL_SRC_COLOR;
        case BlendFactor::kOneMinusDstColor:   return GL_ONE_MINUS_DST_COLOR;
        case BlendFactor::kOneMinusSrcColor:   return GL_ONE_MINUS_SRC_COLOR;
        case BlendFactor::kDstAlpha:           return GL_DST_ALPHA;
        case BlendFactor::kSrcAlpha:           return GL_SRC_ALPHA;
        case BlendFactor::kOneMinusDstAlpha:   return GL_ONE_MINUS_DST_ALPHA;
        case BlendFactor::kOneMinusSrcAlpha:   return GL_ONE_MINUS_SRC_ALPHA;
        }
        return GL_ONE;
    }

    //--------------------------------------------------------------------------

    GLenum GL4Mapping::get(BlendOperation op)
    {
        switch (op)
        {
        case BlendOperation::kAdd:             return GL_FUNC_ADD;
        case BlendOperation::kSubtract:        return GL_FUNC_SUBTRACT;
        case BlendOperation::kReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
        case BlendOperation::kMin:             return GL_MIN;
        case BlendOperation::kMax:             return GL_MAX;
        }
        return GL_FUNC_ADD;
    }

    //--------------------------------------------------------------------------

    GLenum GL4Mapping::get(CompareFunction func)
    {
        switch (func)
        {
        case CompareFunction::kAlwaysFail:   return GL_NEVER;
        case CompareFunction::kAlwaysPass:   return GL_ALWAYS;
        case CompareFunction::kLess:         return GL_LESS;
        case CompareFunction::kLessEqual:    return GL_LEQUAL;
        case CompareFunction::kEqual:        return GL_EQUAL;
        case CompareFunction::kNotEqual:     return GL_NOTEQUAL;
        case CompareFunction::kGreaterEqual: return GL_GEQUAL;
        case CompareFunction::kGreater:      return GL_GREATER;
        }
        return GL_LESS;
    }

    //--------------------------------------------------------------------------

    GLenum GL4Mapping::get(StencilOp op)
    {
        switch (op)
        {
        case StencilOp::kKeep:    return GL_KEEP;
        case StencilOp::kZero:    return GL_ZERO;
        case StencilOp::kReplace: return GL_REPLACE;
        case StencilOp::kInc:     return GL_INCR;
        case StencilOp::kIncWrap: return GL_INCR_WRAP;
        case StencilOp::kDec:     return GL_DECR;
        case StencilOp::kDecWrap: return GL_DECR_WRAP;
        case StencilOp::kInvert:  return GL_INVERT;
        }
        return GL_KEEP;
    }

    //--------------------------------------------------------------------------

    GLenum GL4Mapping::get(PolygonMode mode)
    {
        switch (mode)
        {
        case PolygonMode::kPoint:     return GL_POINT;
        case PolygonMode::kWireframe: return GL_LINE;
        case PolygonMode::kSolid:     return GL_FILL;
        default: break;
        }
        return GL_FILL;
    }

    //--------------------------------------------------------------------------

    GLenum GL4Mapping::get(CullingMode mode)
    {
        switch (mode)
        {
        case CullingMode::kNone:  return GL_NONE;
        case CullingMode::kFront: return GL_FRONT;
        case CullingMode::kBack:  return GL_BACK;
        }
        return GL_BACK;
    }

    //--------------------------------------------------------------------------

    GLenum GL4Mapping::get(TextureAddressMode mode)
    {
        switch (mode)
        {
        case TextureAddressMode::kWrap:       return GL_REPEAT;
        case TextureAddressMode::kMirror:     return GL_MIRRORED_REPEAT;
        case TextureAddressMode::kClamp:      return GL_CLAMP_TO_EDGE;
        case TextureAddressMode::kBorder:     return GL_CLAMP_TO_BORDER;
        case TextureAddressMode::kMirrorOnce: return GL_MIRROR_CLAMP_TO_EDGE;
        default: break;
        }
        return GL_REPEAT;
    }

    //--------------------------------------------------------------------------

    GLenum GL4Mapping::getMinFilter(FilterOptions minFilter, FilterOptions mipFilter)
    {
        if (mipFilter == FilterOptions::kNone || mipFilter == FilterOptions::kPoint)
        {
            if (minFilter == FilterOptions::kPoint)
                return (mipFilter == FilterOptions::kNone) ? GL_NEAREST : GL_NEAREST_MIPMAP_NEAREST;
            else
                return (mipFilter == FilterOptions::kNone) ? GL_LINEAR : GL_LINEAR_MIPMAP_NEAREST;
        }
        else
        {
            if (minFilter == FilterOptions::kPoint)
                return GL_NEAREST_MIPMAP_LINEAR;
            else
                return GL_LINEAR_MIPMAP_LINEAR;
        }
    }

    //--------------------------------------------------------------------------

    GLenum GL4Mapping::getMagFilter(FilterOptions magFilter)
    {
        return (magFilter == FilterOptions::kPoint) ? GL_NEAREST : GL_LINEAR;
    }

    //--------------------------------------------------------------------------

    GLenum GL4Mapping::get(PrimitiveType primitive)
    {
        switch (primitive)
        {
        case PrimitiveType::kPointList:      return GL_POINTS;
        case PrimitiveType::kLineList:       return GL_LINES;
        case PrimitiveType::kLineStrip:      return GL_LINE_STRIP;
        case PrimitiveType::kTriangleList:   return GL_TRIANGLES;
        case PrimitiveType::kTriangleStrip:  return GL_TRIANGLE_STRIP;
        }
        return GL_TRIANGLES;
    }

    //--------------------------------------------------------------------------

    GLenum GL4Mapping::get(IndexType idxType)
    {
        switch (idxType)
        {
        case IndexType::E_IT_16BITS: return GL_UNSIGNED_SHORT;
        case IndexType::E_IT_32BITS: return GL_UNSIGNED_INT;
        }
        return GL_UNSIGNED_SHORT;
    }

    //--------------------------------------------------------------------------

    TResult GL4Mapping::checkUsage(Usage usage, uint32_t accessMode)
    {
        TResult ret = T3D_OK;

        switch (usage)
        {
        case Usage::kImmutable:
            // kCPURead 只是读回许可，读回走内部 staging 中转
            if ((accessMode & ~(uint32_t)kCPURead) != 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Usage is kImmutable, so access mode must be kCPUNone or kCPURead !");
                ret = T3D_ERR_INVALID_PARAM;
            }
            break;
        case Usage::kStatic:
            break;
        case Usage::kDynamic:
            if (accessMode == kCPUNone)
            {
                T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "Usage is kDynamic, but CPU access mode is kCPUNone. Here suggests kStatic instead of kDynamic !");
            }
            else if (accessMode != kCPUWrite)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Usage is kDynamic, so access mode must be kCPUNone or kCPUWrite ! "
                    "Use Usage::kStatic with kCPURead for readback instead !");
                ret = T3D_ERR_INVALID_PARAM;
            }
            break;
        case Usage::kCopy:
            if ((accessMode & kCPUReadWrite) == 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Usage is kCopy, so access mode must contain kCPURead or kCPUWrite !");
                ret = T3D_ERR_INVALID_PARAM;
            }
            break;
        default:
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Invalid usage [%u] !", (uint32_t)usage);
            ret = T3D_ERR_INVALID_PARAM;
            break;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    bool GL4Mapping::isDefaultUsage(Usage usage, uint32_t accessMode)
    {
        return usage == Usage::kStatic
            || (usage == Usage::kDynamic && accessMode == kCPUNone);
    }

    //--------------------------------------------------------------------------

    GLenum GL4Mapping::getBufferUsage(Usage usage, uint32_t accessMode, uint32_t gpuAccess)
    {
        // UAV 由 compute 写入；间接参数缓冲不带 UAV 时也由 GPU 写入（copyStructureCount）。
        // kCPURead 的读回经 staging 拷贝，源缓冲本身不会被 CPU 映射，不需要 *_READ
        if ((gpuAccess & (kGPUUnorderedAccess | kGPUIndirectArgs)) != 0)
        {
            return GL_DYNAMIC_COPY;
        }

        switch (usage)
        {
        case Usage::kStatic:    return GL_STATIC_DRAW;
        case Usage::kImmutable: return GL_STATIC_DRAW;
        case Usage::kDynamic:   return (accessMode == kCPUNone) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
        case Usage::kCopy:      return (accessMode & kCPURead) != 0 ? GL_STREAM_READ : GL_STREAM_DRAW;
        }
        return GL_STATIC_DRAW;
    }

    //--------------------------------------------------------------------------

    GLint GL4Mapping::getVertexAttribSize(VertexAttribute::Type type)
    {
        switch (type)
        {
        case VertexAttribute::Type::E_VAT_FLOAT1:    return 1;
        case VertexAttribute::Type::E_VAT_FLOAT2:    return 2;
        case VertexAttribute::Type::E_VAT_FLOAT3:    return 3;
        case VertexAttribute::Type::E_VAT_FLOAT4:    return 4;
        case VertexAttribute::Type::E_VAT_COLOR:     return 4;
        case VertexAttribute::Type::E_VAT_BYTE4:
        case VertexAttribute::Type::E_VAT_BYTE4_NORM:
        case VertexAttribute::Type::E_VAT_UBYTE4:
        case VertexAttribute::Type::E_VAT_UBYTE4_NORM:
            return 4;
        case VertexAttribute::Type::E_VAT_SHORT2:
        case VertexAttribute::Type::E_VAT_SHORT2_NORM:
        case VertexAttribute::Type::E_VAT_USHORT2:
        case VertexAttribute::Type::E_VAT_USHORT2_NORM:
            return 2;
        case VertexAttribute::Type::E_VAT_SHORT4:
        case VertexAttribute::Type::E_VAT_SHORT4_NORM:
        case VertexAttribute::Type::E_VAT_USHORT4:
        case VertexAttribute::Type::E_VAT_USHORT4_NORM:
            return 4;
        case VertexAttribute::Type::E_VAT_DOUBLE1:   return 1;
        case VertexAttribute::Type::E_VAT_DOUBLE2:   return 2;
        case VertexAttribute::Type::E_VAT_DOUBLE3:   return 3;
        case VertexAttribute::Type::E_VAT_DOUBLE4:   return 4;
        case VertexAttribute::Type::E_VAT_INT1:      return 1;
        case VertexAttribute::Type::E_VAT_INT2:      return 2;
        case VertexAttribute::Type::E_VAT_INT3:      return 3;
        case VertexAttribute::Type::E_VAT_INT4:      return 4;
        case VertexAttribute::Type::E_VAT_UINT1:     return 1;
        case VertexAttribute::Type::E_VAT_UINT2:     return 2;
        case VertexAttribute::Type::E_VAT_UINT3:     return 3;
        case VertexAttribute::Type::E_VAT_UINT4:     return 4;
        case VertexAttribute::Type::E_VAT_FLOAT16_2: return 2;
        case VertexAttribute::Type::E_VAT_FLOAT16_4: return 4;
        default: break;
        }
        return 4;
    }

    //--------------------------------------------------------------------------

    GLenum GL4Mapping::getVertexAttribType(VertexAttribute::Type type)
    {
        switch (type)
        {
        case VertexAttribute::Type::E_VAT_FLOAT1:
        case VertexAttribute::Type::E_VAT_FLOAT2:
        case VertexAttribute::Type::E_VAT_FLOAT3:
        case VertexAttribute::Type::E_VAT_FLOAT4:
            return GL_FLOAT;
        case VertexAttribute::Type::E_VAT_FLOAT16_2:
        case VertexAttribute::Type::E_VAT_FLOAT16_4:
            return GL_HALF_FLOAT;
        case VertexAttribute::Type::E_VAT_COLOR:
        case VertexAttribute::Type::E_VAT_UBYTE4:
        case VertexAttribute::Type::E_VAT_UBYTE4_NORM:
            return GL_UNSIGNED_BYTE;
        case VertexAttribute::Type::E_VAT_BYTE4:
        case VertexAttribute::Type::E_VAT_BYTE4_NORM:
            return GL_BYTE;
        case VertexAttribute::Type::E_VAT_SHORT2:
        case VertexAttribute::Type::E_VAT_SHORT4:
        case VertexAttribute::Type::E_VAT_SHORT2_NORM:
        case VertexAttribute::Type::E_VAT_SHORT4_NORM:
            return GL_SHORT;
        case VertexAttribute::Type::E_VAT_USHORT2:
        case VertexAttribute::Type::E_VAT_USHORT4:
        case VertexAttribute::Type::E_VAT_USHORT2_NORM:
        case VertexAttribute::Type::E_VAT_USHORT4_NORM:
            return GL_UNSIGNED_SHORT;
        case VertexAttribute::Type::E_VAT_DOUBLE1:
        case VertexAttribute::Type::E_VAT_DOUBLE2:
        case VertexAttribute::Type::E_VAT_DOUBLE3:
        case VertexAttribute::Type::E_VAT_DOUBLE4:
            return GL_DOUBLE;
        case VertexAttribute::Type::E_VAT_INT1:
        case VertexAttribute::Type::E_VAT_INT2:
        case VertexAttribute::Type::E_VAT_INT3:
        case VertexAttribute::Type::E_VAT_INT4:
            return GL_INT;
        case VertexAttribute::Type::E_VAT_UINT1:
        case VertexAttribute::Type::E_VAT_UINT2:
        case VertexAttribute::Type::E_VAT_UINT3:
        case VertexAttribute::Type::E_VAT_UINT4:
            return GL_UNSIGNED_INT;
        default: break;
        }
        return GL_FLOAT;
    }

    //--------------------------------------------------------------------------

    GLboolean GL4Mapping::getVertexAttribNormalized(VertexAttribute::Type type)
    {
        switch (type)
        {
        case VertexAttribute::Type::E_VAT_COLOR:
        case VertexAttribute::Type::E_VAT_BYTE4_NORM:
        case VertexAttribute::Type::E_VAT_UBYTE4_NORM:
        case VertexAttribute::Type::E_VAT_SHORT2_NORM:
        case VertexAttribute::Type::E_VAT_SHORT4_NORM:
        case VertexAttribute::Type::E_VAT_USHORT2_NORM:
        case VertexAttribute::Type::E_VAT_USHORT4_NORM:
            return GL_TRUE;
        default: break;
        }
        return GL_FALSE;
    }

    //--------------------------------------------------------------------------

    bool GL4Mapping::isIntegerAttrib(VertexAttribute::Type type)
    {
        switch (type)
        {
        case VertexAttribute::Type::E_VAT_BYTE4:
        case VertexAttribute::Type::E_VAT_UBYTE4:
        case VertexAttribute::Type::E_VAT_SHORT2:
        case VertexAttribute::Type::E_VAT_SHORT4:
        case VertexAttribute::Type::E_VAT_USHORT2:
        case VertexAttribute::Type::E_VAT_USHORT4:
        case VertexAttribute::Type::E_VAT_INT1:
        case VertexAttribute::Type::E_VAT_INT2:
        case VertexAttribute::Type::E_VAT_INT3:
        case VertexAttribute::Type::E_VAT_INT4:
        case VertexAttribute::Type::E_VAT_UINT1:
        case VertexAttribute::Type::E_VAT_UINT2:
        case VertexAttribute::Type::E_VAT_UINT3:
        case VertexAttribute::Type::E_VAT_UINT4:
            return true;
        default:
            break;
        }
        return false;
    }

    //--------------------------------------------------------------------------

    const char *GL4Mapping::getVertexSemanticName(VertexAttribute::Semantic semantic)
    {
        switch (semantic)
        {
        case VertexAttribute::Semantic::E_VAS_POSITION:     return "POSITION";
        case VertexAttribute::Semantic::E_VAS_BLENDWEIGHT:  return "BLENDWEIGHT";
        case VertexAttribute::Semantic::E_VAS_BLENDINDICES: return "BLENDINDICES";
        case VertexAttribute::Semantic::E_VAS_NORMAL:       return "NORMAL";
        case VertexAttribute::Semantic::E_VAS_DIFFUSE:
        case VertexAttribute::Semantic::E_VAS_SPECULAR:     return "COLOR";
        case VertexAttribute::Semantic::E_VAS_TEXCOORD:     return "TEXCOORD";
        case VertexAttribute::Semantic::E_VAS_TANGENT:      return "TANGENT";
        case VertexAttribute::Semantic::E_VAS_BINORMAL:     return "BINORMAL";
        default: break;
        }
        return "";
    }

    //--------------------------------------------------------------------------
}
