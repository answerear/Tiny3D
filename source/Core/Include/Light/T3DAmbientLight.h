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

#ifndef __T3D_AMBIENT_LIGHT_H__
#define __T3D_AMBIENT_LIGHT_H__


#include "Light/T3DIndirectLight.h"

namespace Tiny3D
{
    /**
     * \brief 全局环境光组件，向材质提供均匀底色
     * \remarks 渲染管线将 getColor() 与 getIntensity() 写入 tiny3d_AmbientLight 着色器 uniform
     */
    TCLASS()
    class T3D_ENGINE_API AmbientLight : public IndirectLight
    {
        TRTTI_ENABLE(IndirectLight)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 创建带新生成 UUID 的环境光实例
         * \return 新建的环境光智能指针
         */
        static AmbientLightPtr create();
        
        ~AmbientLight() override = default;

        /**
         * \brief 深拷贝本环境光组件（含属性，UUID 独立）
         * \return 拷贝成功返回新组件；cloneProperties 失败时返回 nullptr
         */
        ComponentPtr clone() const override;

        /**
         * \brief 返回环境光类型标识
         * \return LightType::kAmbient
         */
        LightType getLightType() const override { return LightType::kAmbient; }

        /// 返回环境光颜色
        TPROPERTY(RTTRFuncName="Color", RTTRFuncType="getter")
        const ColorRGB& getColor() const { return mColor; }

        /**
         * \brief 设置环境光颜色
         * \param [in] color : 新的 RGB 颜色
         */
        TPROPERTY(RTTRFuncName="Color", RTTRFuncType="setter")
        void setColor(const ColorRGB &color) { mColor = color; }
        
        /// 返回环境光强度
        TPROPERTY(RTTRFuncName="Intensity", RTTRFuncType="getter")
        float getIntensity() const { return mIntensity; }

        /**
         * \brief 设置环境光强度
         * \param [in] intensity : 强度系数
         */
        TPROPERTY(RTTRFuncName="Intensity", RTTRFuncType="setter")
        void setIntensity(float intensity) { mIntensity = intensity; }
        
    protected:
        /// 默认构造，供 RTTR 使用
        AmbientLight() = default;

        /**
         * \brief 以指定 UUID 构造环境光
         * \param [in] uuid : 组件唯一标识
         */
        AmbientLight(const UUID &uuid);

        /**
         * \brief 从源组件拷贝 AmbientLight 属性（强度）
         * \param [in] src : 源组件，须为 AmbientLight
         * \return 调用成功返回 T3D_OK；基类 cloneProperties 失败时原样返回其错误码
         * \note 当前实现未拷贝 mColor
         */
        TResult cloneProperties(const Component * const src) override;
        
    protected:
        /// 环境光颜色，默认白色
        ColorRGB mColor { ColorRGB::WHITE };
        /// 环境光强度，默认 0.5
        float mIntensity {0.5f};
    };
}


#endif  /*__T3D_AMBIENT_LIGHT_H__*/
