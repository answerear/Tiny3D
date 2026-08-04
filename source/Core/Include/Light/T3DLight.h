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

#ifndef __T3D_LIGHT_H__
#define __T3D_LIGHT_H__


#include "Component/T3DComponent.h"
#include "Light/T3DLightType.h"


namespace Tiny3D
{
    /**
     * \brief 场景光源 Component 的抽象基类
     * \remarks 具体光照参数由 IndirectLight / LocalLight 及其子类实现；渲染管线通过 getLightType() 区分类型
     */
    TCLASS()
    class T3D_ENGINE_API Light : public Component
    {
        TRTTI_ENABLE(Component)
        TRTTI_FRIEND
        
    public:        
        ~Light() override = default;

        /// 本组件支持 enabled 开关；禁用后不再参与照明
        bool supportsEnabled() const override { return true; }

        /**
         * \brief 返回本光源的具体类型
         * \return 对应 LightType 枚举值
         */
        virtual LightType getLightType() const = 0;
        
    protected:
        /// 默认构造，供子类与 RTTR 使用
        Light() = default;

        /**
         * \brief 以指定 UUID 构造光源组件
         * \param [in] uuid : 组件唯一标识
         */
        Light(const UUID &uuid);

        /**
         * \brief 从源组件拷贝 Light 层属性（当前仅委托 Component 拷贝 enabled）
         * \param [in] src : 源组件，须为 Light 或其派生类
         * \return 调用成功返回 T3D_OK；基类 cloneProperties 失败时原样返回其错误码
         */
        TResult cloneProperties(const Component * const src) override;
    };
}


#endif  /*__T3D_LIGHT_H__*/

