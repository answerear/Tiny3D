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
        static ShaderSamplerParamPtr create(const String &name);

        ~ShaderSamplerParam() override;

        ShaderSamplerParamPtr clone() const;

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }
        
        TPROPERTY(RTTRFuncName="TextureState", RTTRFuncType="getter")
        TEXTURE_TYPE getTextureType() const { return mTexType; }

        TPROPERTY(RTTRFuncName="TextureState", RTTRFuncType="setter")
        void setTextureType(TEXTURE_TYPE texType) { mTexType = texType; }
        
        TPROPERTY(RTTRFuncName="SamplerBinding", RTTRFuncType="getter")
        uint32_t getSamplerBinding() const { return mSamplerBinding; }

        TPROPERTY(RTTRFuncName="SamplerBinding", RTTRFuncType="setter")
        void setSamplerBinding(uint32_t binding) { mSamplerBinding = binding; }

        TPROPERTY(RTTRFuncName="TexBinding", RTTRFuncType="getter")
        uint32_t getTexBinding() const { return mTexBinding; }

        TPROPERTY(RTTRFuncName="TexBinding", RTTRFuncType="setter")
        void setTexBinding(uint32_t binding) { mTexBinding = binding; }
        
    private:
        ShaderSamplerParam() = default;

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }
        
    protected:
        ShaderSamplerParam(const String &name);

        void onPostLoad() override;

        /// 采样参数名称
        String          mName {};
        /// 纹理类型
        TEXTURE_TYPE    mTexType {TEXTURE_TYPE::TT_2D};
        /// 纹理绑定点
        uint32_t        mTexBinding {0};
        /// 采样器绑定点
        uint32_t        mSamplerBinding {0};
    };

    TCLASS()
    class T3D_ENGINE_API ShaderSamplerValue
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        static ShaderSamplerValuePtr create(const String &name);

        ~ShaderSamplerValue() override;

        ShaderSamplerValuePtr clone() const;

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        TPROPERTY(RTTRFuncName="TextureUUID", RTTRFuncType="getter")
        const UUID &getTextureUUID() const { return mTextureUUID; }

        TPROPERTY(RTTRFuncName="TextureUUID", RTTRFuncType="setter")
        void setTextureUUID(const UUID &uuid) { mTextureUUID = uuid; }

        Texture *getTexture() const { return mTexture; }
        
    protected:
        ShaderSamplerValue(const String &name);

        ShaderSamplerValue() = default;

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }
        
        /// 采样器名称
        String mName {};
        /// 纹理的 UUID
        UUID mTextureUUID {UUID::INVALID};
        /// 纹理对象
        TexturePtr mTexture {nullptr};
    };
}


#endif    /*__T3D_SHADER_SAMPLER_PARAM_H__*/
