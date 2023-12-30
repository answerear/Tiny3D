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

#ifndef __T3D_SHADER_SAMPLER_PARAM_H__
#define __T3D_SHADER_SAMPLER_PARAM_H__


#include "T3DTypedef.h"
#include "Kernel/T3DConstant.h"


namespace Tiny3D
{
    /**
     * \brief 着色器里的纹理采样器
     */
    TCLASS()
    class T3D_ENGINE_API ShaderSamplerParam
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        static ShaderSamplerParamPtr create(const String &name, TEXTURE_TYPE texType);

        ~ShaderSamplerParam() override;

        ShaderSamplerParamPtr clone() const;

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const
        {
            return mName;
        }
        
        TPROPERTY(RTTRFuncName="TextureState", RTTRFuncType="getter")
        TEXTURE_TYPE getTextureType() const
        {
            return mTexType;
        }

        TexturePtr getTexture() const { return mTexture; }
        
    private:
        ShaderSamplerParam() = default;

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name)
        {
            mName = name;
        }

        TPROPERTY(RTTRFuncName="TextureState", RTTRFuncType="setter")
        void setTextureType(TEXTURE_TYPE texType)
        {
            mTexType = texType;
        }
        
    protected:
        ShaderSamplerParam(const String &name, TEXTURE_TYPE texType);

        bool onPostLoad() override;
        
        String          mName {};
        TEXTURE_TYPE    mTexType {TEXTURE_TYPE::TT_2D};
        TexturePtr      mTexture {nullptr};
    };
}


#endif    /*__T3D_SHADER_SAMPLER_PARAM_H__*/
