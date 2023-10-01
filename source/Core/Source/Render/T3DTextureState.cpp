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


#include "Render/T3DTextureState.h"
#include "Render/T3DRenderResourceManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    TextureStatePtr TextureState::create()
    {
        return new TextureState();
    }
    
    //--------------------------------------------------------------------------

    void TextureState::setSamplerDesc(const SamplerDesc &desc)
    {
        uint32_t hashSrc = CRC::crc32((uint8_t*)&mSamplerDesc, sizeof(mSamplerDesc));
        uint32_t hashDst = CRC::crc32((uint8_t*)&desc, sizeof(desc));

        if (hashSrc != hashDst || mSamplerState == nullptr)
        {
            mSamplerDesc = desc;

            // 新生成一个 sampler state 对象
            mSamplerState = T3D_RENDER_STATE_MGR.loadSamplerState(mSamplerDesc, hashDst);
        }
        
    }
    
    //--------------------------------------------------------------------------
}
