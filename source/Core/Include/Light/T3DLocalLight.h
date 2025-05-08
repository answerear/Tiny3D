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
    TCLASS()
    class T3D_ENGINE_API LocalLight : public Light
    {
        TRTTI_ENABLE(Light)
        TRTTI_FRIEND
        
    public:        
        ~LocalLight() override = default;

        /**
         * 获取漫反射光照颜色
         */
        TPROPERTY(RTTRFuncName="Color", RTTRFuncType="getter")
        const ColorRGB& getColor() const { return mColor; }

        /**
         * 设置漫反射光照颜色
         */
        TPROPERTY(RTTRFuncName="Color", RTTRFuncType="setter")
        void setColor(const ColorRGB &color) { mColor = color; }
        
        /**
         * 获取漫反射强度
         */
        TPROPERTY(RTTRFuncName="DiffuseIntensity", RTTRFuncType="getter")
        float getDiffuseIntensity() const { return mDiffuseIntensity; }

        /**
         * 设置漫反射强度
         */
        TPROPERTY(RTTRFuncName="DiffuseIntensity", RTTRFuncType="setter")
        void setDiffuseIntensity(float intensity) { mDiffuseIntensity = intensity; }

        /**
         * 获取镜面反射强度
         */
        TPROPERTY(RTTRFuncName="SpecularIntensity", RTTRFuncType="getter")
        float getSpecularIntensity() const { return mSpecularIntensity; }

        /**
         * 设置镜面反射强度
         */
        TPROPERTY(RTTRFuncName="SpecularIntensity", RTTRFuncType="setter")
        void setSpecularIntensity(float intensity) { mSpecularIntensity = intensity; }

        /**
         * 获取镜面反射发光值 (shininess)
         */
        TPROPERTY(RTTRFuncName="SpecularShininess", RTTRFuncType="getter")
        float getSpecularShininess() const { return mSpecularShininess; }

        /**
         * 设置镜面反射发光值 (shininess)
         */
        TPROPERTY(RTTRFuncName="SpecularShininess", RTTRFuncType="setter")
        void setSpecularShininess(float shininess) { mSpecularShininess = shininess; }
        
    protected:
        LocalLight() = default;

        LocalLight(const UUID &uuid);

        TResult cloneProperties(const Component * const src) override;

    protected:
        /// 光照颜色
        ColorRGB mColor { ColorRGB::WHITE };
        /// 漫反射强度
        float mDiffuseIntensity { 1.0f };
        /// 镜面反射强度
        float mSpecularIntensity { 1.0f };
        /// 镜面反射发光值 (shininess)
        float mSpecularShininess { 0.0f };
    };
}


#endif  /*__T3D_LOCAL_LIGHT_H__*/


