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

#ifndef __T3D_LOCAL_LIGHT_H__
#define __T3D_LOCAL_LIGHT_H__


#include "Light/T3DLight.h"


namespace Tiny3D
{
    /**
     * \brief 具有颜色与漫/镜面强度的局部光源基类
     * \remarks 世界空间位置与照射方向由挂载 GameObject 的 Transform 决定
     */
    TCLASS()
    class T3D_ENGINE_API LocalLight : public Light
    {
        TRTTI_ENABLE(Light)
        TRTTI_FRIEND
        
    public:        
        ~LocalLight() override = default;

        /// 返回漫反射光照颜色
        TPROPERTY(RTTRFuncName="Color", RTTRFuncType="getter")
        const ColorRGB& getColor() const { return mColor; }

        /**
         * \brief 设置漫反射光照颜色
         * \param [in] color : 新的 RGB 颜色
         */
        TPROPERTY(RTTRFuncName="Color", RTTRFuncType="setter")
        void setColor(const ColorRGB &color) { mColor = color; }
        
        /// 返回漫反射强度
        TPROPERTY(RTTRFuncName="DiffuseIntensity", RTTRFuncType="getter")
        float getDiffuseIntensity() const { return mDiffuseIntensity; }

        /**
         * \brief 设置漫反射强度
         * \param [in] intensity : 强度系数
         */
        TPROPERTY(RTTRFuncName="DiffuseIntensity", RTTRFuncType="setter")
        void setDiffuseIntensity(float intensity) { mDiffuseIntensity = intensity; }

        /// 返回镜面反射强度
        TPROPERTY(RTTRFuncName="SpecularIntensity", RTTRFuncType="getter")
        float getSpecularIntensity() const { return mSpecularIntensity; }

        /**
         * \brief 设置镜面反射强度
         * \param [in] intensity : 强度系数
         */
        TPROPERTY(RTTRFuncName="SpecularIntensity", RTTRFuncType="setter")
        void setSpecularIntensity(float intensity) { mSpecularIntensity = intensity; }
        
    protected:
        /// 默认构造，供子类与 RTTR 使用
        LocalLight() = default;

        /**
         * \brief 以指定 UUID 构造局部光源
         * \param [in] uuid : 组件唯一标识
         */
        LocalLight(const UUID &uuid);

        /**
         * \brief 从源组件拷贝 LocalLight 属性（漫反射与镜面反射强度）
         * \param [in] src : 源组件，须为 LocalLight 或其派生类
         * \return 调用成功返回 T3D_OK；基类 cloneProperties 失败时原样返回其错误码
         * \note 当前实现未拷贝 mColor
         */
        TResult cloneProperties(const Component * const src) override;

    protected:
        /// 漫反射光照颜色，默认白色
        ColorRGB mColor { ColorRGB::WHITE };
        /// 漫反射强度，默认 1.0
        float mDiffuseIntensity { 1.0f };
        /// 镜面反射强度，默认 1.0
        float mSpecularIntensity { 1.0f };
    };
}


#endif  /*__T3D_LOCAL_LIGHT_H__*/

