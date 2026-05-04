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

#ifndef __T3D_VK_MAPPING_H__
#define __T3D_VK_MAPPING_H__


#include "T3DVKPrerequisites.h"

namespace Tiny3D
{
    class VKMapping
    {
    public:
        static const char * const POSITION;
        static const char * const BLENDWEIGHT;
        static const char * const BLENDINDICES;
        static const char * const NORMAL;
        static const char * const COLOR;
        static const char * const TEXCOORD;
        static const char * const TANGENT;
        static const char * const BINORMAL;

        static VkFormat get(PixelFormat src);
        static VkImageUsageFlags get(Usage src);
        static VkBlendFactor get(BlendFactor factor);
        static VkBlendOp get(BlendOperation op);
        static VkCompareOp get(CompareFunction func);
        static VkStencilOp get(StencilOp op);
        static VkPolygonMode get(PolygonMode mode);
        static VkCullModeFlags get(CullingMode mode);
        static VkSamplerAddressMode get(TextureAddressMode mode);
        static VkFilter get(FilterOptions filter);
        static VkFormat get(VertexAttribute::Type type);
        static VkFormat get(IndexType idxType);
        static VkPrimitiveTopology get(PrimitiveType primitive);
    };
}


#endif    /*__T3D_VK_MAPPING_H__*/
