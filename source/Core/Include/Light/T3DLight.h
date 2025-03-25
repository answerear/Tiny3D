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
    TCLASS()
    class T3D_ENGINE_API Light : public Component
    {
        TRTTI_ENABLE(Component)
        TRTTI_FRIEND
        
    public:        
        ~Light() override = default;

        virtual LightType getLightType() const = 0;

        /**
         * 获取光照颜色
         */
        TPROPERTY(RTTRFuncName="Color", RTTRFuncType="getter")
        const ColorRGBA& getColor() const { return mColor; }

        /**
         * 设置光照颜色
         */
        TPROPERTY(RTTRFuncName="Color", RTTRFuncType="setter")
        void setColor(const ColorRGBA &color) { mColor = color; }
        
    protected:
        Light() = default;

        Light(const UUID &uuid);

        TResult cloneProperties(const Component * const src) override;

    protected:
        /// 光照颜色
        ColorRGBA mColor { ColorRGBA::WHITE };
    };
}


#endif  /*__T3D_LIGHT_H__*/


