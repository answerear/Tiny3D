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

#ifndef __T3D_SHADER_SAMPLER_PARAM_H__
#define __T3D_SHADER_SAMPLER_PARAM_H__


#include "T3DTypedef.h"
#include "Kernel/T3DConstant.h"
#include "Resource/T3DTexture.h"


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
        static ShaderSamplerParamPtr create(const String &name, const String &texName, TEXTURE_TYPE texType);

        static ShaderSamplerParamPtr create(const String &name, TEXTURE_TYPE texType, TexturePtr texture);

        ~ShaderSamplerParam() override;

        ShaderSamplerParamPtr clone() const;

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const
        {
            return mName;
        }

        TPROPERTY(RTTRFuncName="TextureName", RTTRFuncType="getter")
        const String &getTexName() const
        {
            return mTexName;
        }
        
        TPROPERTY(RTTRFuncName="TextureState", RTTRFuncType="getter")
        TEXTURE_TYPE getTextureType() const
        {
            return mTexType;
        }

        TexturePtr getTexture() const { return mTexture; }

        void setTexture(Texture *tex) { mTexture = tex; }
        
    private:
        ShaderSamplerParam() = default;

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name)
        {
            mName = name;
        }

        TPROPERTY(RTTRFuncName="TextureName", RTTRFuncType="setter")
        void setTexName(const String &texName)
        {
            mTexName = texName;
        }

        TPROPERTY(RTTRFuncName="TextureState", RTTRFuncType="setter")
        void setTextureType(TEXTURE_TYPE texType)
        {
            mTexType = texType;
        }
        
    protected:
        ShaderSamplerParam(const String &name, const String &texName, TEXTURE_TYPE texType);

        ShaderSamplerParam(const String &name, TEXTURE_TYPE texType, TexturePtr texture);

        void onPostLoad() override;
        
        String          mName {};
        String          mTexName {};
        TEXTURE_TYPE    mTexType {TEXTURE_TYPE::TT_2D};
        TexturePtr      mTexture {nullptr};
    };
}


#endif    /*__T3D_SHADER_SAMPLER_PARAM_H__*/
