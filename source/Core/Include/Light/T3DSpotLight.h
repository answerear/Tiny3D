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
    /**
     * \brief 聚光灯组件，自 Transform 世界位置沿局部 Transform 矩阵 Z 列方向锥形照射并按距离衰减
     * \remarks 内外锥角与衰减系数写入 tiny3d_SpotLightDir / tiny3d_SpotLightAttenuation 着色器 uniform
     */
    TCLASS()
    class T3D_ENGINE_API SpotLight : public LocalLight
    {
        TRTTI_ENABLE(Light)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 创建带新生成 UUID 的聚光灯实例
         * \return 新建的聚光灯智能指针
         */
        static SpotLightPtr create();
        
        ~SpotLight() override = default;

        /**
         * \brief 深拷贝本聚光灯组件（含 LocalLight、衰减与锥角属性，UUID 独立）
         * \return 拷贝成功返回新组件；cloneProperties 失败时返回 nullptr
         */
        ComponentPtr clone() const override;

        /**
         * \brief 返回聚光灯类型标识
         * \return LightType::kSpot
         */
        LightType getLightType() const override { return LightType::kSpot; }

        /// 返回距离衰减常数项系数（mAttenuation.x）
        float getAttenuationConstant() const { return mAttenuation.x(); }

        /**
         * \brief 设置距离衰减常数项系数
         * \param [in] value : 常数项系数
         */
        void setAttenuationConstant(float value) { mAttenuation.x() = value; }

        /// 返回距离衰减一次项系数（mAttenuation.y）
        float getAttenuationLinear() const { return mAttenuation.y(); }

        /**
         * \brief 设置距离衰减一次项系数
         * \param [in] value : 一次项系数
         */
        void setAttenuationLinear(float value) { mAttenuation.y() = value; }

        /// 返回距离衰减二次项系数（mAttenuation.z）
        float getAttenuationQuadratic() const { return mAttenuation.z(); }

        /**
         * \brief 设置距离衰减二次项系数
         * \param [in] value : 二次项系数
         */
        void setAttenuationQuadratic(float value) { mAttenuation.z() = value; }

        /// 返回外锥半角（光锥边缘）
        TPROPERTY(RTTRFuncName="CutoffAngle", RTTRFuncType="getter");
        const Degree &getCutoffAngle() const { return mCutoffAngle; }

        /**
         * \brief 设置外锥半角
         * \param [in] value : 外锥半角
         */
        TPROPERTY(RTTRFuncName="CutoffAngle", RTTRFuncType="setter");
        void setCutoffAngle(const Degree &value) { mCutoffAngle = value; }

        /// 返回内锥半角（全强度区域边缘）
        TPROPERTY(RTTRFuncName="InnerCutoffAngle", RTTRFuncType="getter");
        const Degree &getInnerCutoffAngle() const { return mInnerCutoffAngle; }

        /**
         * \brief 设置内锥半角
         * \param [in] value : 内锥半角
         */
        TPROPERTY(RTTRFuncName="InnerCutoffAngle", RTTRFuncType="setter");
        void setInnerCutoffAngle(const Degree &value) { mInnerCutoffAngle = value; }
        
    protected:
        /// 默认构造，供 RTTR 使用
        SpotLight() = default;

        /**
         * \brief 以指定 UUID 构造聚光灯
         * \param [in] uuid : 组件唯一标识
         */
        SpotLight(const UUID &uuid);

        /**
         * \brief 从源组件拷贝 SpotLight 属性（衰减向量与内外锥角）
         * \param [in] src : 源组件，须为 SpotLight
         * \return 调用成功返回 T3D_OK；基类 cloneProperties 失败时原样返回其错误码
         */
        TResult cloneProperties(const Component * const src) override;

        /// 返回完整衰减向量 (constant, linear, quadratic)，供 RTTR 序列化
        TPROPERTY(RTTRFuncName="Attenuation", RTTRFuncType="getter");
        const Vector3 &getAttenuation() const { return mAttenuation; }

        /**
         * \brief 一次性设置完整衰减向量
         * \param [in] value : (constant, linear, quadratic) 三元组
         */
        TPROPERTY(RTTRFuncName="Attenuation", RTTRFuncType="setter");
        void setAttenuation(const Vector3 &value) { mAttenuation = value; }

    protected:
        /// 距离衰减系数 (constant, linear, quadratic)，默认全零
        Vector3 mAttenuation { Vector3::ZERO };

        /// 内锥半角，默认 0°
        Degree mInnerCutoffAngle { 0.0f };
        /// 外锥半角，默认 0°
        Degree mCutoffAngle { 0.0f };
    };
}


#endif  /*__T3D_SPOT_LIGHT_H__*/
