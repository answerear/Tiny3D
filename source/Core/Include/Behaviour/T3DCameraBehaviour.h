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

#ifndef __T3D_CAMERA_BEHAVIOUR_H__
#define __T3D_CAMERA_BEHAVIOUR_H__


#include "Component/T3DBehaviour.h"


namespace Tiny3D
{
    /**
     * \brief 挂在 Camera 所在 GameObject 上的渲染回调基类
     * \remarks 只由管线遍历相机自己的 GameObject，不扫场景树。不要在回调里
     *          setRenderTarget 或改场景图。
     */
    TCLASS()
    class T3D_ENGINE_API CameraBehaviour : public Behaviour
    {
        friend class ForwardRenderPipeline;

        TRTTI_ENABLE(Behaviour)
        TRTTI_FRIEND

    public:
        ~CameraBehaviour() override;

    protected:
        CameraBehaviour() = default;
        explicit CameraBehaviour(const UUID &uuid);

        /**
         * \brief 本相机开始渲染前（清屏之前）
         * \param [in] ctx : RHI 上下文；进入时管线尚未 setRenderTarget
         * \remarks 可以改本相机 viewport / clear 参数，不要自己绑 RT。
         *          要换 RT 请在 onLateUpdate 里 Camera::setRenderTarget，下一帧生效。
         */
        virtual void onPreRender(RHIContext *ctx);

        /**
         * \brief 本相机场景绘制完成后、效果链之前
         * \param [in] ctx : RHI 上下文；进入时本相机 pass 已 endPass
         * \remarks 可以 beginPass / 画 / endPass，用完自己 reset。不要改场景图。
         */
        virtual void onPostRender(RHIContext *ctx);
    };
}


#endif  /*__T3D_CAMERA_BEHAVIOUR_H__*/
