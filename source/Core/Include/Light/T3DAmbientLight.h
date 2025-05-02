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
    TCLASS()
    class T3D_ENGINE_API AmbientLight : public IndirectLight
    {
        TRTTI_ENABLE(IndirectLight)
        TRTTI_FRIEND
        
    public:
        static AmbientLightPtr create();
        
        ~AmbientLight() override = default;

        ComponentPtr clone() const override;

        LightType getLightType() const override { return LightType::kAmbient; }

        /**
         * 获取光照颜色
         */
        TPROPERTY(RTTRFuncName="Color", RTTRFuncType="getter")
        const ColorRGB& getColor() const { return mColor; }

        /**
         * 设置光照颜色
         */
        TPROPERTY(RTTRFuncName="Color", RTTRFuncType="setter")
        void setColor(const ColorRGB &color) { mColor = color; }
        
        /**
         * @brief 获取光照强度
         */
        TPROPERTY(RTTRFuncTpe="Intensity", RTTRFuncType="getter")
        float getIntensity() const { return mIntensity; }

        /**
         * @brief 设置光照强度
         */
        TPROPERTY(RTTRFuncTpe="Intensity", RTTRFuncType="setter")
        void setIntensity(float intensity) { mIntensity = intensity; }
        
    protected:
        AmbientLight() = default;

        AmbientLight(const UUID &uuid);

        TResult cloneProperties(const Component * const src) override;
        
    protected:
        /// 光照颜色
        ColorRGB mColor { ColorRGB::WHITE };
        /// 光照强度
        float mIntensity {0.5f};
    };
}


#endif  /*__T3D_AMBIENT_LIGHT_H__*/
