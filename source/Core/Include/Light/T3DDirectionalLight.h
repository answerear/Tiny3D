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

#ifndef __T3D_DIRECTIONAL_LIGHT_H__
#define __T3D_DIRECTIONAL_LIGHT_H__


#include "T3DTypedef.h"
#include "Light/T3DLocalLight.h"

namespace Tiny3D
{
    /**
     * \brief 平行光组件，光照方向取自挂载 GameObject Transform 的 local-to-world 矩阵 Z 列
     * \remarks 继承 LocalLight 的颜色与漫/镜面强度属性，无额外专有字段
     */
    TCLASS()
    class T3D_ENGINE_API DirectionalLight : public LocalLight
    {
        TRTTI_ENABLE(LocalLight)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 创建带新生成 UUID 的平行光实例
         * \return 新建的平行光智能指针
         */
        static DirectionalLightPtr create();
        
        ~DirectionalLight() override = default;

        /**
         * \brief 深拷贝本平行光组件（含 LocalLight 属性，UUID 独立）
         * \return 拷贝成功返回新组件；cloneProperties 失败时返回 nullptr
         */
        ComponentPtr clone() const override;

        /**
         * \brief 返回平行光类型标识
         * \return LightType::kDirectional
         */
        LightType getLightType() const override { return LightType::kDirectional; }
        
    protected:
        /// 默认构造，供 RTTR 使用
        DirectionalLight() = default;

        /**
         * \brief 以指定 UUID 构造平行光
         * \param [in] uuid : 组件唯一标识
         */
        DirectionalLight(const UUID &uuid);
    };
}


#endif  /*__T3D_DIRECTIONAL_LIGHT_H__*/
