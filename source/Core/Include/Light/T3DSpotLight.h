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

#ifndef __T3D_SPOT_LIGHT_H__
#define __T3D_SPOT_LIGHT_H__


#include "Light/T3DLocalLight.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API SpotLight : public LocalLight
    {
        TRTTI_ENABLE(Light)
        TRTTI_FRIEND
        
    public:
        static SpotLightPtr create();
        
        ~SpotLight() override = default;

        ComponentPtr clone() const override;

        LightType getLightType() const override { return LightType::kSpot; }

        /**
         * 获取衰减常量系数
         */
        float getAttenuationConstant() const { return mAttenuation.x(); }

        /**
         * 设置衰减常量系数
         */
        void setAttenuationConstant(float value) { mAttenuation.x() = value; }

        /**
         * 获取衰减一次系数
         */
        float getAttenuationLinear() const { return mAttenuation.y(); }

        /**
         * 设置衰减一次系数
         */
        void setAttenuationLinear(float value) { mAttenuation.y() = value; }

        /**
         * 获取衰减二次系数
         */
        float getAttenuationQuadratic() const { return mAttenuation.z(); }

        /**
         * 设置衰减二次系数
         */
        void setAttenuationQuadratic(float value) { mAttenuation.z() = value; }

        TPROPERTY(RTTRFuncName="CutoffAngle", RTTRFuncType="getter");
        const Degree &getCutoffAngle() const { return mCutoffAngle; }

        TPROPERTY(RTTRFuncName="CutoffAngle", RTTRFuncType="setter");
        void setCutoffAngle(const Degree &value) { mCutoffAngle = value; }

        TPROPERTY(RTTRFuncName="InnerCutoffAngle", RTTRFuncType="getter");
        const Degree &getInnerCutoffAngle() const { return mInnerCutoffAngle; }

        TPROPERTY(RTTRFuncName="InnerCutoffAngle", RTTRFuncType="setter");
        void setInnerCutoffAngle(const Degree &value) { mInnerCutoffAngle = value; }
        
    protected:
        SpotLight() = default;

        SpotLight(const UUID &uuid);

        TResult cloneProperties(const Component * const src) override;

        TPROPERTY(RTTRFuncName="Attenuation", RTTRFuncType="getter");
        const Vector3 &getAttenuation() const { return mAttenuation; }

        TPROPERTY(RTTRFuncName="Attenuation", RTTRFuncType="setter");
        void setAttenuation(const Vector3 &value) { mAttenuation = value; }

    protected:
        Vector3 mAttenuation { Vector3::ZERO };

        Degree mInnerCutoffAngle { 0.0f };
        Degree mCutoffAngle { 0.0f };
    };
}


#endif  /*__T3D_SPOT_LIGHT_H__*/
