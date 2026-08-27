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

#ifndef __T3D_D3D11_MAPPING_H__
#define __T3D_D3D11_MAPPING_H__


#include "T3DD3D11PrerequisitesBase.h"

namespace Tiny3D
{
    class D3D11Mapping
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
        
        static DXGI_FORMAT get(PixelFormat src);

        static D3D11_USAGE get(Usage src);

        static UINT get(uint32_t accMode);

        /**
         * \brief 把引擎 GPUAccessFlags 映射为 D3D11 BindFlags 附加位
         * \param [in] gpuAccess : GPUAccessFlags 组合
         * \return D3D11_BIND_* 组合；kGPUIndirectArgs 不产生 BindFlags（它是 MiscFlag）
         */
        static UINT getBindFlags(uint32_t gpuAccess);

        /**
         * \brief 计算结构化缓冲的 D3D11 MiscFlags
         * \param [in] kind : 缓冲形态
         * \param [in] gpuAccess : GPUAccessFlags 组合
         * \return D3D11_RESOURCE_MISC_* 组合
         */
        static UINT getBufferMiscFlags(StructuredBufferKind kind, uint32_t gpuAccess);

        /**
         * \brief 查询 DXGI 格式每像素字节数
         * \param [in] format : DXGI 格式
         * \return 每像素字节数；压缩格式、深度模板格式与未登记格式一律返回 0
         * \remarks 供 GPU 读回按行紧凑打包用。返回 0 即表示「本期不支持读回这种格式」，
         *          压缩格式的「块」概念和深度模板的位域解释都不能用一个 bpp 概括。
         */
        static uint32_t getBytesPerPixel(DXGI_FORMAT format);
        
        static D3D11_TEXTURE1D_DESC get(const PixelBuffer1DDesc &src);
        
        static D3D11_TEXTURE2D_DESC get(const PixelBuffer2DDesc &src);

        static D3D11_TEXTURE3D_DESC get(const PixelBuffer3DDesc &src);

        static D3D_SRV_DIMENSION get(TEXTURE_TYPE texType);

        static TEXTURE_TYPE get(D3D_SRV_DIMENSION srvDim);

        static D3D11_BLEND get(BlendFactor factor);

        static D3D11_BLEND_OP get(BlendOperation op);

        static uint8_t get(BlendColorWriteMask mask);

        static D3D11_COMPARISON_FUNC get(CompareFunction func);

        static D3D11_STENCIL_OP get(StencilOp op);

        static D3D11_FILL_MODE get(PolygonMode mode);

        static D3D11_CULL_MODE get(CullingMode mode);

        static D3D11_TEXTURE_ADDRESS_MODE get(TextureAddressMode mode);

        static D3D11_FILTER get(FilterOptions minFilter, FilterOptions magFilter, FilterOptions mipFilter, bool comparison = false);

        static const char *get(VertexAttribute::Semantic semantic);

        static DXGI_FORMAT get(VertexAttribute::Type type);

        static TResult get(Usage usage, uint32_t mode, D3D11_USAGE &d3dUsage, uint32_t &d3dAccessFlag);
        
        static DXGI_FORMAT get(IndexType idxType);

        static D3D11_PRIMITIVE_TOPOLOGY get(PrimitiveType primitive);

        static ShaderConstantParam::DATA_TYPE get(D3D_SHADER_VARIABLE_TYPE d3dType, uint32_t rows, uint32_t cols, uint32_t numOfElements);
    };
}


#endif    /*__T3D_D3D11_MAPPING_H__*/
