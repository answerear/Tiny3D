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


#include "T3DMetalMapping.h"


namespace Tiny3D
{
    uint32_t MetalBytesPerPixel(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::E_PF_PALETTE8:
            return 1;
        case PixelFormat::E_PF_B5G6R5:
        case PixelFormat::E_PF_B5G5R5A1:
        case PixelFormat::E_PF_B4R4G4A4:
        case PixelFormat::E_PF_D16_UNORM:
            return 2;
        case PixelFormat::E_PF_R8G8B8:
        case PixelFormat::E_PF_B8G8R8:
            return 3;
        case PixelFormat::E_PF_R8G8B8A8:
        case PixelFormat::E_PF_B8G8R8A8:
        case PixelFormat::E_PF_R8G8B8X8:
        case PixelFormat::E_PF_B8G8R8X8:
        case PixelFormat::E_PF_D24_UNORM_S8_UINT:
        case PixelFormat::E_PF_D32_FLOAT:
            return 4;
        case PixelFormat::E_PF_D32_FLOAT_S8X24_UINT:
            return 8;
        default:
            return 4;
        }
    }

    MTLPixelFormat MetalMapPixelFormat(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::E_PF_R8G8B8A8:
        case PixelFormat::E_PF_R8G8B8X8:
            return MTLPixelFormatRGBA8Unorm;
        case PixelFormat::E_PF_B8G8R8A8:
        case PixelFormat::E_PF_B8G8R8X8:
            return MTLPixelFormatBGRA8Unorm;
        case PixelFormat::E_PF_D16_UNORM:
            return MTLPixelFormatDepth16Unorm;
        case PixelFormat::E_PF_D32_FLOAT:
            return MTLPixelFormatDepth32Float;
        case PixelFormat::E_PF_D24_UNORM_S8_UINT:
            return MTLPixelFormatDepth32Float_Stencil8;
        case PixelFormat::E_PF_D32_FLOAT_S8X24_UINT:
            return MTLPixelFormatDepth32Float_Stencil8;
        default:
            return MTLPixelFormatBGRA8Unorm;
        }
    }

    MTLVertexFormat MetalMapVertexFormat(VertexAttribute::Type type)
    {
        switch (type)
        {
        case VertexAttribute::Type::E_VAT_FLOAT1: return MTLVertexFormatFloat;
        case VertexAttribute::Type::E_VAT_FLOAT2: return MTLVertexFormatFloat2;
        case VertexAttribute::Type::E_VAT_FLOAT3: return MTLVertexFormatFloat3;
        case VertexAttribute::Type::E_VAT_FLOAT4: return MTLVertexFormatFloat4;
        case VertexAttribute::Type::E_VAT_COLOR:
        case VertexAttribute::Type::E_VAT_UBYTE4_NORM: return MTLVertexFormatUChar4Normalized;
        case VertexAttribute::Type::E_VAT_BYTE4: return MTLVertexFormatChar4;
        case VertexAttribute::Type::E_VAT_BYTE4_NORM: return MTLVertexFormatChar4Normalized;
        case VertexAttribute::Type::E_VAT_UBYTE4: return MTLVertexFormatUChar4;
        case VertexAttribute::Type::E_VAT_SHORT2: return MTLVertexFormatShort2;
        case VertexAttribute::Type::E_VAT_SHORT4: return MTLVertexFormatShort4;
        case VertexAttribute::Type::E_VAT_SHORT2_NORM: return MTLVertexFormatShort2Normalized;
        case VertexAttribute::Type::E_VAT_SHORT4_NORM: return MTLVertexFormatShort4Normalized;
        case VertexAttribute::Type::E_VAT_USHORT2: return MTLVertexFormatUShort2;
        case VertexAttribute::Type::E_VAT_USHORT4: return MTLVertexFormatUShort4;
        case VertexAttribute::Type::E_VAT_USHORT2_NORM: return MTLVertexFormatUShort2Normalized;
        case VertexAttribute::Type::E_VAT_USHORT4_NORM: return MTLVertexFormatUShort4Normalized;
        case VertexAttribute::Type::E_VAT_INT1: return MTLVertexFormatInt;
        case VertexAttribute::Type::E_VAT_INT2: return MTLVertexFormatInt2;
        case VertexAttribute::Type::E_VAT_INT3: return MTLVertexFormatInt3;
        case VertexAttribute::Type::E_VAT_INT4: return MTLVertexFormatInt4;
        case VertexAttribute::Type::E_VAT_UINT1: return MTLVertexFormatUInt;
        case VertexAttribute::Type::E_VAT_UINT2: return MTLVertexFormatUInt2;
        case VertexAttribute::Type::E_VAT_UINT3: return MTLVertexFormatUInt3;
        case VertexAttribute::Type::E_VAT_UINT4: return MTLVertexFormatUInt4;
        case VertexAttribute::Type::E_VAT_FLOAT16_2: return MTLVertexFormatHalf2;
        case VertexAttribute::Type::E_VAT_FLOAT16_4: return MTLVertexFormatHalf4;
        default: return MTLVertexFormatFloat3;
        }
    }

    MTLPrimitiveType MetalMapPrimitive(PrimitiveType primitive)
    {
        switch (primitive)
        {
        case PrimitiveType::kPointList: return MTLPrimitiveTypePoint;
        case PrimitiveType::kLineList: return MTLPrimitiveTypeLine;
        case PrimitiveType::kLineStrip: return MTLPrimitiveTypeLineStrip;
        case PrimitiveType::kTriangleStrip: return MTLPrimitiveTypeTriangleStrip;
        case PrimitiveType::kTriangleList:
        default: return MTLPrimitiveTypeTriangle;
        }
    }

    MTLCompareFunction MetalMapCompare(CompareFunction func)
    {
        switch (func)
        {
        case CompareFunction::kAlwaysFail: return MTLCompareFunctionNever;
        case CompareFunction::kAlwaysPass: return MTLCompareFunctionAlways;
        case CompareFunction::kLess: return MTLCompareFunctionLess;
        case CompareFunction::kLessEqual: return MTLCompareFunctionLessEqual;
        case CompareFunction::kEqual: return MTLCompareFunctionEqual;
        case CompareFunction::kNotEqual: return MTLCompareFunctionNotEqual;
        case CompareFunction::kGreaterEqual: return MTLCompareFunctionGreaterEqual;
        case CompareFunction::kGreater: return MTLCompareFunctionGreater;
        default: return MTLCompareFunctionLess;
        }
    }

    MTLSamplerMinMagFilter MetalMapFilter(FilterOptions filter)
    {
        return (filter == FilterOptions::kPoint) ? MTLSamplerMinMagFilterNearest : MTLSamplerMinMagFilterLinear;
    }

    MTLSamplerMipFilter MetalMapMipFilter(FilterOptions filter)
    {
        if (filter == FilterOptions::kNone)
        {
            return MTLSamplerMipFilterNotMipmapped;
        }
        return (filter == FilterOptions::kPoint) ? MTLSamplerMipFilterNearest : MTLSamplerMipFilterLinear;
    }

    MTLSamplerAddressMode MetalMapAddress(TextureAddressMode mode)
    {
        switch (mode)
        {
        case TextureAddressMode::kMirror: return MTLSamplerAddressModeMirrorRepeat;
        case TextureAddressMode::kClamp: return MTLSamplerAddressModeClampToEdge;
        case TextureAddressMode::kBorder: return MTLSamplerAddressModeClampToBorderColor;
        case TextureAddressMode::kMirrorOnce: return MTLSamplerAddressModeMirrorClampToEdge;
        case TextureAddressMode::kWrap:
        default: return MTLSamplerAddressModeRepeat;
        }
    }

    MTLBlendFactor MetalMapBlendFactor(BlendFactor factor)
    {
        switch (factor)
        {
        case BlendFactor::kZero: return MTLBlendFactorZero;
        case BlendFactor::kDstColor: return MTLBlendFactorDestinationColor;
        case BlendFactor::kSrcColor: return MTLBlendFactorSourceColor;
        case BlendFactor::kOneMinusDstColor: return MTLBlendFactorOneMinusDestinationColor;
        case BlendFactor::kOneMinusSrcColor: return MTLBlendFactorOneMinusSourceColor;
        case BlendFactor::kDstAlpha: return MTLBlendFactorDestinationAlpha;
        case BlendFactor::kSrcAlpha: return MTLBlendFactorSourceAlpha;
        case BlendFactor::kOneMinusDstAlpha: return MTLBlendFactorOneMinusDestinationAlpha;
        case BlendFactor::kOneMinusSrcAlpha: return MTLBlendFactorOneMinusSourceAlpha;
        case BlendFactor::kOne:
        default: return MTLBlendFactorOne;
        }
    }

    MTLBlendOperation MetalMapBlendOp(BlendOperation op)
    {
        switch (op)
        {
        case BlendOperation::kSubtract: return MTLBlendOperationSubtract;
        case BlendOperation::kReverseSubtract: return MTLBlendOperationReverseSubtract;
        case BlendOperation::kMin: return MTLBlendOperationMin;
        case BlendOperation::kMax: return MTLBlendOperationMax;
        case BlendOperation::kAdd:
        default: return MTLBlendOperationAdd;
        }
    }

    MTLStencilOperation MetalMapStencilOp(StencilOp op)
    {
        switch (op)
        {
        case StencilOp::kZero: return MTLStencilOperationZero;
        case StencilOp::kReplace: return MTLStencilOperationReplace;
        case StencilOp::kInc: return MTLStencilOperationIncrementClamp;
        case StencilOp::kIncWrap: return MTLStencilOperationIncrementWrap;
        case StencilOp::kDec: return MTLStencilOperationDecrementClamp;
        case StencilOp::kDecWrap: return MTLStencilOperationDecrementWrap;
        case StencilOp::kInvert: return MTLStencilOperationInvert;
        case StencilOp::kKeep:
        default: return MTLStencilOperationKeep;
        }
    }

    MTLCullMode MetalMapCull(CullingMode mode)
    {
        switch (mode)
        {
        case CullingMode::kFront: return MTLCullModeFront;
        case CullingMode::kBack: return MTLCullModeBack;
        case CullingMode::kNone:
        default: return MTLCullModeNone;
        }
    }

    MTLTriangleFillMode MetalMapFill(PolygonMode mode)
    {
        return (mode == PolygonMode::kWireframe) ? MTLTriangleFillModeLines : MTLTriangleFillModeFill;
    }

    MTLWinding MetalMapWinding(bool frontAnticlockwise)
    {
        return frontAnticlockwise ? MTLWindingCounterClockwise : MTLWindingClockwise;
    }
}
