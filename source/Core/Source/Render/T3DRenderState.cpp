/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#include "Render/T3DRenderState.h"
#include "Render/T3DRenderResourceManager.h"
#include "RHI/T3DRHIResource.h"


namespace  Tiny3D
{
    //--------------------------------------------------------------------------

    RenderStatePtr RenderState::create()
    {
        return new RenderState();
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
