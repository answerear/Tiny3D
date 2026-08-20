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


#ifndef __T3D_METAL_MAPPING_H__
#define __T3D_METAL_MAPPING_H__


#include "T3DMetalPrerequisites.h"
#include "Render/T3DVertexAttribute.h"
#include "Render/T3DRenderConstant.h"

#ifdef __OBJC__
#import <Metal/Metal.h>
#endif


namespace Tiny3D
{
    uint32_t MetalBytesPerPixel(PixelFormat format);

#ifdef __OBJC__
    MTLPixelFormat MetalMapPixelFormat(PixelFormat format);
    MTLVertexFormat MetalMapVertexFormat(VertexAttribute::Type type);
    MTLPrimitiveType MetalMapPrimitive(PrimitiveType primitive);
    MTLCompareFunction MetalMapCompare(CompareFunction func);
    MTLSamplerMinMagFilter MetalMapFilter(FilterOptions filter);
    MTLSamplerMipFilter MetalMapMipFilter(FilterOptions filter);
    MTLSamplerAddressMode MetalMapAddress(TextureAddressMode mode);
    MTLBlendFactor MetalMapBlendFactor(BlendFactor factor);
    MTLBlendOperation MetalMapBlendOp(BlendOperation op);
    MTLStencilOperation MetalMapStencilOp(StencilOp op);
    MTLCullMode MetalMapCull(CullingMode mode);
    MTLTriangleFillMode MetalMapFill(PolygonMode mode);
    MTLWinding MetalMapWinding(bool frontAnticlockwise);
#endif
}


#endif  /*__T3D_METAL_MAPPING_H__*/
