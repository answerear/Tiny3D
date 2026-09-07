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

#ifndef __T3D_CAMERA_EFFECT_BEHAVIOUR_H__
#define __T3D_CAMERA_EFFECT_BEHAVIOUR_H__


#include "Component/T3DBehaviour.h"


namespace Tiny3D
{
    /**
     * \brief 相机图像后处理组件，挂在 Camera 所在的 GameObject 上
     * \remarks 与 CameraBehaviour 是兄弟类。链内顺序靠 getEffectOrder，不依赖
     *          组件容器插入顺序。实现必须写满整个 dst。不要在 onRenderImage 里 map。
     */
    TCLASS()
    class T3D_ENGINE_API CameraEffectBehaviour : public Behaviour
    {
        TRTTI_ENABLE(Behaviour)
        TRTTI_FRIEND

    public:
        ~CameraEffectBehaviour() override;

        CameraEffectBehaviour *asCameraEffectBehaviour() override { return this; }

        /**
         * \brief 链内执行顺序，小的先执行
         * \return 当前顺序值，默认 0
         */
        TPROPERTY(RTTRFuncName="EffectOrder", RTTRFuncType="getter", "TOOLTIP"="Smaller values run first in the camera effect chain.")
        virtual int32_t getEffectOrder() const { return mEffectOrder; }

        /**
         * \brief 设置链内执行顺序
         * \param [in] order : 新的顺序值
         */
        TPROPERTY(RTTRFuncName="EffectOrder", RTTRFuncType="setter")
        void setEffectOrder(int32_t order) { mEffectOrder = order; }

        /**
         * \brief 把 src 处理后写入 dst
         * \param [in] ctx : RHI 上下文，进入时未绑定任何 RT
         * \param [in] src : 上一环节的结果（只读，已 resolve 成非 MSAA）
         * \param [in] dst : 本环节输出目标，尺寸格式与 src 一致
         * \param [in] depth : 相机深度纹理，可为 nullptr
         * \remarks 必须写满整个 dst。进入时不要假设当前 RT / viewport / shader
         *          还在。自己绑，用完 reset。src 只读。
         */
        virtual void onRenderImage(RHIContext *ctx,
                                   RenderTexture *src,
                                   RenderTexture *dst,
                                   RenderTexture *depth) = 0;

    protected:
        CameraEffectBehaviour() = default;
        explicit CameraEffectBehaviour(const UUID &uuid);

        /**
         * \brief 把 src 整张 blit 到 dst，供拷贝类效果复用
         * \param [in] ctx : RHI 上下文
         * \param [in] src : 源纹理，不可为 nullptr
         * \param [in] dst : 目标纹理，不可为 nullptr
         */
        void blitCopy(RHIContext *ctx, RenderTexture *src, RenderTexture *dst);

        /// 链内顺序，小的先执行
        int32_t mEffectOrder {0};
    };
}


#endif  /*__T3D_CAMERA_EFFECT_BEHAVIOUR_H__*/
