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


#include "T3DD3D11Prerequisites.h"
#include "T3DD3D11Context.h"

namespace Tiny3D
{
    class D3D11Mapping
    {
    public:
        static DXGI_FORMAT get(PixelFormat src);

        static D3D11_USAGE get(Usage src);

        static UINT get(uint32_t accMode);
        
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
    };
}


#endif    /*__T3D_D3D11_MAPPING_H__*/
