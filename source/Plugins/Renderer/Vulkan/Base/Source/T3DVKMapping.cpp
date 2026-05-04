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
        // TODO: Implement Vulkan pixel format mapping
        return VK_FORMAT_UNDEFINED;
    }

    //--------------------------------------------------------------------------

    VkImageUsageFlags VKMapping::get(Usage src)
    {
        // TODO: Implement Vulkan usage mapping
        return 0;
    }

    //--------------------------------------------------------------------------

    VkBlendFactor VKMapping::get(BlendFactor factor)
    {
        // TODO: Implement Vulkan blend factor mapping
        return VK_BLEND_FACTOR_ONE;
    }

    //--------------------------------------------------------------------------

    VkBlendOp VKMapping::get(BlendOperation op)
    {
        // TODO: Implement Vulkan blend operation mapping
        return VK_BLEND_OP_ADD;
    }

    //--------------------------------------------------------------------------

    VkCompareOp VKMapping::get(CompareFunction func)
    {
        // TODO: Implement Vulkan compare function mapping
        return VK_COMPARE_OP_ALWAYS;
    }

    //--------------------------------------------------------------------------

    VkStencilOp VKMapping::get(StencilOp op)
    {
        // TODO: Implement Vulkan stencil op mapping
        return VK_STENCIL_OP_KEEP;
    }

    //--------------------------------------------------------------------------

    VkPolygonMode VKMapping::get(PolygonMode mode)
    {
        // TODO: Implement Vulkan polygon mode mapping
        return VK_POLYGON_MODE_FILL;
    }

    //--------------------------------------------------------------------------

    VkCullModeFlags VKMapping::get(CullingMode mode)
    {
        // TODO: Implement Vulkan cull mode mapping
        return VK_CULL_MODE_BACK_BIT;
    }

    //--------------------------------------------------------------------------

    VkSamplerAddressMode VKMapping::get(TextureAddressMode mode)
    {
        // TODO: Implement Vulkan address mode mapping
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }

    //--------------------------------------------------------------------------

    VkFilter VKMapping::get(FilterOptions filter)
    {
        // TODO: Implement Vulkan filter mapping
        return VK_FILTER_LINEAR;
    }

    //--------------------------------------------------------------------------

    VkFormat VKMapping::get(VertexAttribute::Type type)
    {
        // TODO: Implement Vulkan vertex attribute type mapping
        return VK_FORMAT_UNDEFINED;
    }

    //--------------------------------------------------------------------------

    VkFormat VKMapping::get(IndexType idxType)
    {
        // TODO: Implement Vulkan index type mapping
        return VK_FORMAT_UNDEFINED;
    }

    //--------------------------------------------------------------------------

    VkPrimitiveTopology VKMapping::get(PrimitiveType primitive)
    {
        // TODO: Implement Vulkan primitive topology mapping
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }

    //--------------------------------------------------------------------------
}
