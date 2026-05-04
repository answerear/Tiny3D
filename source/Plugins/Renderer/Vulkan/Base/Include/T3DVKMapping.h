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


#include "T3DVKPrerequisitesBase.h"


namespace Tiny3D
{
    class VKMapping
    {
    public:
        static const char * const POSITION;     /**< The position */
        static const char * const BLENDWEIGHT;  /**< The blendweight */
        static const char * const BLENDINDICES; /**< The blendindices */
        static const char * const NORMAL;       /**< The normal */
        static const char * const COLOR;        /**< The color */
        static const char * const TEXCOORD;     /**< The texcoord */
        static const char * const TANGENT;      /**< The tangent */
        static const char * const BINORMAL;     /**< The binormal */

        static VkFormat get(PixelFormat src);

        static VkMemoryPropertyFlags get(Usage src);

        static VkMemoryPropertyFlags get(uint32_t accMode);

        static VkImageViewType get(TEXTURE_TYPE texType);

        static VkBlendFactor get(BlendFactor factor);

        static VkBlendOp get(BlendOperation op);

        static VkColorComponentFlags get(BlendColorWriteMask mask);

        static VkCompareOp get(CompareFunction func);

        static VkStencilOp get(StencilOp op);

        static VkPolygonMode get(PolygonMode mode);

        static VkCullModeFlags get(CullingMode mode);

        static VkSamplerAddressMode get(TextureAddressMode mode);

        static VkFilter get(FilterOptions filter);

        static VkSamplerMipmapMode getMipmapMode(FilterOptions mipFilter);

        static const char *get(VertexAttribute::Semantic semantic);

        static VkFormat get(VertexAttribute::Type type);

        static VkIndexType get(IndexType idxType);

        static VkPrimitiveTopology get(PrimitiveType primitive);
    };
}


#endif    /*__T3D_VK_MAPPING_H__*/
