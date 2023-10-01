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


#ifndef __T3D_TEXTURE_STATE_H__
#define __T3D_TEXTURE_STATE_H__


#include "RHI/T3DRHISamplerState.h"
#include "Resource/T3DTexture.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API TextureState : public Object
    {
    public:
        static TextureStatePtr create();

        SamplerStatePtr getSamplerSTate() const { return mSamplerState; }

        TexturePtr getTexture() const { return mTexture; }
        
        TPROPERTY(RTTRFuncName="SamplerDesc", RTTRFuncType="getter")
        const SamplerDesc &getSamplerDesc() const { return mSamplerDesc; }

        TPROPERTY(RTTRFuncName="SamplerDesc", RTTRFuncType="setter")
        void setSamplerDesc(const SamplerDesc &desc);

        TPROPERTY(RTTRFuncName="TextureName", RTTRFuncType="getter")
        const String &getTextureName() const { return TexName; }

        TPROPERTY(RTTRFuncName="TextureName", RTTRFuncType="setter")
        void setTextureName(const String &name) { TexName = name; }

    protected:
        String              TexName {};
        SamplerDesc         mSamplerDesc {};
        SamplerStatePtr     mSamplerState {nullptr};
        TexturePtr          mTexture {nullptr};
    };
}


#endif  /*__T3D_TEXTURE_STATE_H__*/
