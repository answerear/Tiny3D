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

#ifndef __T3D_POINT_LIGHT_H__
#define __T3D_POINT_LIGHT_H__


#include "Light/T3DLocalLight.h"


namespace Tiny3D
{
    /**
     * \brief 点光源组件，自 Transform 位置向四周发射并按距离衰减
     * \remarks 衰减系数 (constant, linear, quadratic) 写入 tiny3d_PointLightAttenuation 着色器 uniform
     */
    TCLASS()
    class T3D_ENGINE_API PointLight : public LocalLight
    {
        TRTTI_ENABLE(LocalLight)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 创建带新生成 UUID 的点光源实例
         * \return 新建的点光源智能指针
         */
        static PointLightPtr create();
        
        ~PointLight() override = default;

        /**
         * \brief 深拷贝本点光源组件（含 LocalLight 与衰减属性，UUID 独立）
         * \return 拷贝成功返回新组件；cloneProperties 失败时返回 nullptr
         */
        ComponentPtr clone() const override;

        /**
         * \brief 返回点光源类型标识
         * \return LightType::kPoint
         */
        LightType getLightType() const override { return LightType::kPoint; }

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
        
    protected:
        /// 默认构造，供 RTTR 使用
        PointLight() = default;

        /**
         * \brief 以指定 UUID 构造点光源
         * \param [in] uuid : 组件唯一标识
         */
        PointLight(const UUID &uuid);

        /**
         * \brief 从源组件拷贝 PointLight 属性（衰减向量）
         * \param [in] src : 源组件，须为 PointLight
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
    };
}


#endif  /*__T3D_POINT_LIGHT_H__*/
