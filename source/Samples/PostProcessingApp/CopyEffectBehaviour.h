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

#ifndef __COPY_EFFECT_BEHAVIOUR_H__
#define __COPY_EFFECT_BEHAVIOUR_H__


#include "Behaviour/T3DCameraEffectBehaviour.h"


namespace Tiny3D
{
    /**
     * \brief 把上一环节的结果原样拷到下一张 RT，用于验证效果链与临时 RT 池
     */
    TCLASS()
    class CopyEffectBehaviour : public CameraEffectBehaviour
    {
        TRTTI_ENABLE(CameraEffectBehaviour)
        TRTTI_FRIEND

    public:
        ~CopyEffectBehaviour() override = default;

        void onRenderImage(RHIContext *ctx,
                           RenderTexture *src,
                           RenderTexture *dst,
                           RenderTexture *depth) override;

    protected:
        CopyEffectBehaviour() = default;
        explicit CopyEffectBehaviour(const UUID &uuid);
    };
}


#endif  /*__COPY_EFFECT_BEHAVIOUR_H__*/
