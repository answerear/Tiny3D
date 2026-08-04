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
     * \brief 着色器纹理采样器的反射描述，记录名称、纹理类型与纹理/采样器绑定点
     */
    TCLASS()
    class T3D_ENGINE_API ShaderSamplerParam
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 创建采样器参数对象
         * \param [in] name : 采样器名称
         * \return 新建的 ShaderSamplerParam 智能指针
         */
        static ShaderSamplerParamPtr create(const String &name);

        /// 析构函数
        ~ShaderSamplerParam() override;

        /**
         * \brief 复制全部字段创建新对象
         * \return 新建的 ShaderSamplerParam 副本
         */
        ShaderSamplerParamPtr clone() const;

        /// 获取采样器名称
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }
        
        /// 获取纹理类型
        TPROPERTY(RTTRFuncName="TextureState", RTTRFuncType="getter")
        TEXTURE_TYPE getTextureType() const { return mTexType; }

        /// 设置纹理类型
        TPROPERTY(RTTRFuncName="TextureState", RTTRFuncType="setter")
        void setTextureType(TEXTURE_TYPE texType) { mTexType = texType; }
        
        /// 获取采样器绑定点
        TPROPERTY(RTTRFuncName="SamplerBinding", RTTRFuncType="getter")
        uint32_t getSamplerBinding() const { return mSamplerBinding; }

        /// 设置采样器绑定点
        TPROPERTY(RTTRFuncName="SamplerBinding", RTTRFuncType="setter")
        void setSamplerBinding(uint32_t binding) { mSamplerBinding = binding; }

        /// 获取纹理绑定点
        TPROPERTY(RTTRFuncName="TexBinding", RTTRFuncType="getter")
        uint32_t getTexBinding() const { return mTexBinding; }

        /// 设置纹理绑定点
        TPROPERTY(RTTRFuncName="TexBinding", RTTRFuncType="setter")
        void setTexBinding(uint32_t binding) { mTexBinding = binding; }
        
    private:
        /// 默认构造，供 clone 与 RTTR 使用
        ShaderSamplerParam() = default;

        /// 设置采样器名称（RTTR setter）
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }
        
    protected:
        /**
         * \brief 构造采样器参数对象
         * \param [in] name : 采样器名称
         */
        ShaderSamplerParam(const String &name);

        /// 反序列化后回调（当前无额外逻辑）
        void onPostLoad() override;

        /// 采样器名称
        String          mName {};
        /// 纹理类型
        TEXTURE_TYPE    mTexType {TEXTURE_TYPE::TT_2D};
        /// 纹理绑定点
        uint32_t        mTexBinding {0};
        /// 采样器绑定点
        uint32_t        mSamplerBinding {0};
    };

    /**
     * \brief 着色器采样器的运行时值，持有纹理 UUID 与 Texture 对象引用
     */
    TCLASS()
    class T3D_ENGINE_API ShaderSamplerValue
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建采样器值对象
         * \param [in] name : 采样器名称
         * \return 新建的 ShaderSamplerValue 智能指针
         */
        static ShaderSamplerValuePtr create(const String &name);

        /// 析构函数
        ~ShaderSamplerValue() override;

        /**
         * \brief 复制名称、UUID 与 Texture 引用创建新对象
         * \return 新建的 ShaderSamplerValue 副本
         */
        ShaderSamplerValuePtr clone() const;

        /// 获取采样器名称
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        /// 获取纹理 UUID
        TPROPERTY(RTTRFuncName="TextureUUID", RTTRFuncType="getter")
        const UUID &getTextureUUID() const { return mTextureUUID; }

        /// 设置纹理 UUID
        TPROPERTY(RTTRFuncName="TextureUUID", RTTRFuncType="setter")
        void setTextureUUID(const UUID &uuid) { mTextureUUID = uuid; }

        /// 获取缓存的 Texture 对象指针
        Texture *getTexture() const { return mTexture; }
        
    protected:
        /**
         * \brief 构造采样器值对象
         * \param [in] name : 采样器名称
         */
        ShaderSamplerValue(const String &name);

        /// 默认构造，供 clone 与 RTTR 使用
        ShaderSamplerValue() = default;

        /// 设置采样器名称（RTTR setter）
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }
        
        /// 采样器名称
        String mName {};
        /// 纹理 UUID
        UUID mTextureUUID {UUID::INVALID};
        /// 纹理对象缓存
        TexturePtr mTexture {nullptr};
    };
}


#endif    /*__T3D_SHADER_SAMPLER_PARAM_H__*/
