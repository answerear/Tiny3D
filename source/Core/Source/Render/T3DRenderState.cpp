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


#include "Render/T3DRenderState.h"
#include "Render/T3DRenderResourceManager.h"
#include "RHI/T3DRHIResource.h"


namespace  Tiny3D
{
    //--------------------------------------------------------------------------

    RenderStatePtr RenderState::create()
    {
        return T3D_NEW RenderState();
    }

    //--------------------------------------------------------------------------

    void RenderState::setBlendDesc(const BlendDesc &desc)
    {
        uint32_t hashSrc = CRC::crc32((uint8_t*)&mBlendDesc, sizeof(mBlendDesc));
        uint32_t hashDst = CRC::crc32((uint8_t*)&desc, sizeof(desc));

        if (hashSrc != hashDst || mBlendState == nullptr)
        {
            mBlendDesc = desc;

            // 新生成一个 blend state 对象
            mBlendState = T3D_RENDER_STATE_MGR.loadBlendState(mBlendDesc, hashDst);
        }
    }
    
    //--------------------------------------------------------------------------

    void RenderState::setDepthStencilDesc(const DepthStencilDesc &desc)
    {
        uint32_t hashSrc = CRC::crc32((uint8_t*)&mDepthStencilDesc, sizeof(mDepthStencilDesc));
        uint32_t hashDst = CRC::crc32((uint8_t*)&desc, sizeof(desc));

        if (hashSrc != hashDst || mDepthStencilState == nullptr)
        {
            mDepthStencilDesc = desc;

            // 新生成一个 depth & stencil state 对象
            mDepthStencilState = T3D_RENDER_STATE_MGR.loadDepthStencilState(mDepthStencilDesc, hashDst);
        }
    }
    
    //--------------------------------------------------------------------------

    void RenderState::setRasterizerDesc(const RasterizerDesc &desc)
    {
        uint32_t hashSrc = CRC::crc32((uint8_t*)&mRasterizerDesc, sizeof(mRasterizerDesc));
        uint32_t hashDst = CRC::crc32((uint8_t*)&desc, sizeof(desc));

        if (hashSrc != hashDst || mRasterizerState == nullptr)
        {
            mRasterizerDesc = desc;

            // 新生成一个 rasterizer state 对象
            mRasterizerState = T3D_RENDER_STATE_MGR.loadRasterizerState(mRasterizerDesc, hashDst);
        }
    }
    
    //--------------------------------------------------------------------------
}
