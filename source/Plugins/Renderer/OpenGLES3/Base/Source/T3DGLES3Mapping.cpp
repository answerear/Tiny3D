/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#include "T3DGLES3Mapping.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GLenum GLES3Mapping::get(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::E_PF_R8G8B8:      return GL_RGB;
        case PixelFormat::E_PF_B8G8R8:      return GL_RGB;
        case PixelFormat::E_PF_R8G8B8A8:    return GL_RGBA;
        case PixelFormat::E_PF_B8G8R8A8:    return GL_RGBA;
        case PixelFormat::E_PF_R8G8B8X8:    return GL_RGBA;
        case PixelFormat::E_PF_B8G8R8X8:    return GL_RGBA;
        case PixelFormat::E_PF_B5G6R5:      return GL_RGB;
        case PixelFormat::E_PF_B5G5R5A1:    return GL_RGBA;
        case PixelFormat::E_PF_B4R4G4A4:    return GL_RGBA;
        default: break;
        }
        return GL_RGBA;
    }

    //--------------------------------------------------------------------------

    GLenum GLES3Mapping::getInternalFormat(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::E_PF_R8G8B8:      return GL_RGB8;
        case PixelFormat::E_PF_B8G8R8:      return GL_RGB8;
        case PixelFormat::E_PF_R8G8B8A8:    return GL_RGBA8;
        case PixelFormat::E_PF_B8G8R8A8:    return GL_RGBA8;
        case PixelFormat::E_PF_R8G8B8X8:    return GL_RGBA8;
        case PixelFormat::E_PF_B8G8R8X8:    return GL_RGBA8;
        case PixelFormat::E_PF_B5G6R5:      return GL_RGB565;
        case PixelFormat::E_PF_B5G5R5A1:    return GL_RGB5_A1;
        case PixelFormat::E_PF_B4R4G4A4:    return GL_RGBA4;
        case PixelFormat::E_PF_D24_UNORM_S8_UINT:       return GL_DEPTH24_STENCIL8;
        case PixelFormat::E_PF_D32_FLOAT_S8X24_UINT:    return GL_DEPTH32F_STENCIL8;
        case PixelFormat::E_PF_D32_FLOAT:                return GL_DEPTH_COMPONENT32F;
        case PixelFormat::E_PF_D16_UNORM:                return GL_DEPTH_COMPONENT16;
        default: break;
        }
        return GL_RGBA8;
    }

    //--------------------------------------------------------------------------

    GLenum GLES3Mapping::getPixelType(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::E_PF_R8G8B8:
        case PixelFormat::E_PF_B8G8R8:
        case PixelFormat::E_PF_R8G8B8A8:
        case PixelFormat::E_PF_B8G8R8A8:
        case PixelFormat::E_PF_R8G8B8X8:
        case PixelFormat::E_PF_B8G8R8X8:
            return GL_UNSIGNED_BYTE;
        case PixelFormat::E_PF_B5G6R5:
            return GL_UNSIGNED_SHORT_5_6_5;
        case PixelFormat::E_PF_B5G5R5A1:
            return GL_UNSIGNED_SHORT_5_5_5_1;
        case PixelFormat::E_PF_B4R4G4A4:
            return GL_UNSIGNED_SHORT_4_4_4_4;
        case PixelFormat::E_PF_D24_UNORM_S8_UINT:
            return GL_UNSIGNED_INT_24_8;
        case PixelFormat::E_PF_D32_FLOAT:
        case PixelFormat::E_PF_D32_FLOAT_S8X24_UINT:
            return GL_FLOAT;
        case PixelFormat::E_PF_D16_UNORM:
            return GL_UNSIGNED_SHORT;
        default: break;
        }
        return GL_UNSIGNED_BYTE;
    }

    //--------------------------------------------------------------------------

    GLenum GLES3Mapping::get(BlendFactor factor)
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

    GLenum GLES3Mapping::get(BlendOperation op)
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

    GLenum GLES3Mapping::get(CompareFunction func)
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

    GLenum GLES3Mapping::get(StencilOp op)
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

    GLenum GLES3Mapping::get(CullingMode mode)
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

    GLenum GLES3Mapping::get(TextureAddressMode mode)
    {
        switch (mode)
        {
        case TextureAddressMode::kWrap:       return GL_REPEAT;
        case TextureAddressMode::kMirror:     return GL_MIRRORED_REPEAT;
        case TextureAddressMode::kClamp:      return GL_CLAMP_TO_EDGE;
        case TextureAddressMode::kBorder:     return GL_CLAMP_TO_EDGE;
        case TextureAddressMode::kMirrorOnce: return GL_MIRRORED_REPEAT;
        default: break;
        }
        return GL_REPEAT;
    }

    //--------------------------------------------------------------------------

    GLenum GLES3Mapping::getMinFilter(FilterOptions minFilter, FilterOptions mipFilter)
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

    GLenum GLES3Mapping::getMagFilter(FilterOptions magFilter)
    {
        return (magFilter == FilterOptions::kPoint) ? GL_NEAREST : GL_LINEAR;
    }

    //--------------------------------------------------------------------------

    GLenum GLES3Mapping::get(PrimitiveType primitive)
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

    GLenum GLES3Mapping::get(IndexType idxType)
    {
        switch (idxType)
        {
        case IndexType::E_IT_16BITS: return GL_UNSIGNED_SHORT;
        case IndexType::E_IT_32BITS: return GL_UNSIGNED_INT;
        }
        return GL_UNSIGNED_SHORT;
    }

    //--------------------------------------------------------------------------

    GLenum GLES3Mapping::getBufferUsage(Usage usage)
    {
        switch (usage)
        {
        case Usage::kStatic:    return GL_STATIC_DRAW;
        case Usage::kImmutable: return GL_STATIC_DRAW;
        case Usage::kDynamic:   return GL_DYNAMIC_DRAW;
        case Usage::kCopy:      return GL_STREAM_READ;
        }
        return GL_STATIC_DRAW;
    }

    //--------------------------------------------------------------------------

    GLint GLES3Mapping::getVertexAttribSize(VertexAttribute::Type type)
    {
        switch (type)
        {
        case VertexAttribute::Type::E_VAT_FLOAT1:  return 1;
        case VertexAttribute::Type::E_VAT_FLOAT2:  return 2;
        case VertexAttribute::Type::E_VAT_FLOAT3:  return 3;
        case VertexAttribute::Type::E_VAT_FLOAT4:  return 4;
        case VertexAttribute::Type::E_VAT_COLOR:   return 4;
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
        case VertexAttribute::Type::E_VAT_DOUBLE1: return 1;
        case VertexAttribute::Type::E_VAT_DOUBLE2: return 2;
        case VertexAttribute::Type::E_VAT_DOUBLE3: return 3;
        case VertexAttribute::Type::E_VAT_DOUBLE4: return 4;
        case VertexAttribute::Type::E_VAT_INT1:    return 1;
        case VertexAttribute::Type::E_VAT_INT2:    return 2;
        case VertexAttribute::Type::E_VAT_INT3:    return 3;
        default: break;
        }
        return 4;
    }

    //--------------------------------------------------------------------------

    GLenum GLES3Mapping::getVertexAttribType(VertexAttribute::Type type)
    {
        switch (type)
        {
        case VertexAttribute::Type::E_VAT_FLOAT1:
        case VertexAttribute::Type::E_VAT_FLOAT2:
        case VertexAttribute::Type::E_VAT_FLOAT3:
        case VertexAttribute::Type::E_VAT_FLOAT4:
            return GL_FLOAT;
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
            return GL_FLOAT;
        case VertexAttribute::Type::E_VAT_INT1:
        case VertexAttribute::Type::E_VAT_INT2:
        case VertexAttribute::Type::E_VAT_INT3:
            return GL_INT;
        default: break;
        }
        return GL_FLOAT;
    }

    //--------------------------------------------------------------------------

    GLboolean GLES3Mapping::getVertexAttribNormalized(VertexAttribute::Type type)
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

    bool GLES3Mapping::isIntegerAttrib(VertexAttribute::Type type)
    {
        switch (type)
        {
        case VertexAttribute::Type::E_VAT_BYTE4:
        case VertexAttribute::Type::E_VAT_UBYTE4:
        case VertexAttribute::Type::E_VAT_SHORT2:
        case VertexAttribute::Type::E_VAT_SHORT4:
        case VertexAttribute::Type::E_VAT_USHORT2:
        case VertexAttribute::Type::E_VAT_USHORT4:
            return true;
        default:
            break;
        }
        return false;
    }

    //--------------------------------------------------------------------------
}
