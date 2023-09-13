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

#ifndef __T3D_TEXTURE_H__
#define __T3D_TEXTURE_H__


#include "Resource/T3DResource.h"
#include "RHI/T3DRHISamplerState.h"


namespace Tiny3D
{
    /**
     * \brief 纹理
     */
    TCLASS()
    class T3D_ENGINE_API Texture : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND
        
    public:
        TENUM()
        enum TEXTURE_TYPE : uint32_t
        {
            TT_1D = 0,
            TT_2D,
            TT_3D,
            TT_CUBE,
            TT_MAX
        };
        
        static TexturePtr create(const String &name);

        ~Texture() override = default;

        Type getType() const override;
        
    private:
        Texture() : Texture("") {}
        
    protected:
        Texture(const String &name);
        
        ResourcePtr clone() const override;

        void cloneProperties(const Resource * const src) override;
    };

    TSTRUCT()
    struct T3D_ENGINE_API TextureState
    {
        TPROPERTY(RTTRFuncName="SamplerState", RTTRFuncType="getter")
        const SamplerState &getSamplerState() const;

        TPROPERTY(RTTRFuncName="SamplerState", RTTRFuncType="setter")
        void setSamplerState(const SamplerState &state);

        TPROPERTY(RTTRFuncName="Texture", RTTRFuncType="getter")
        const String &getTextureName() const;

        TPROPERTY(RTTRFuncName="Texture", RTTRFuncType="setter")
        void setTextureName(const String &name);

        String              mTextureName {};
        RHISamplerStatePtr  mRHISamplerState {nullptr};
        TexturePtr          mTexture {nullptr};
    };
}


#endif    /*__T3D_TEXTURE_H__*/
