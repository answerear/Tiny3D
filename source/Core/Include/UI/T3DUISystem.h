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


#ifndef __T3D_UI_SYSTEM_H__
#define __T3D_UI_SYSTEM_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief 运行时 UI 的绘制阶段
     */
    enum class UIRenderPhase : uint32_t
    {
        /// 场景绘制之后、相机效果链之前。UI 会参与后处理
        kBeforePostProcess = 0,
        /// 相机效果链与 blit 之后。UI 不受后处理影响
        kOverlay,
    };

    /**
     * \brief 运行时 UI 系统接口
     * \remarks 由 UI 实现模块实现，并在插件 install 时通过 Agent::setUISystem 注册。
     *          所有方法都在主线程调用。未加载实现时 Agent::getUISystem() 为 nullptr。
     */
    class T3D_ENGINE_API UISystem : public Object
    {
    public:
        ~UISystem() override;

        /// 实现名称，用于日志
        virtual const String &getName() const = 0;

        /**
         * \brief 启动
         * \remarks 调用时 RHI、渲染资源管理器、AssetManager 均已就绪。
         *          失败时 Agent 记录错误并注销该实现，引擎继续运行。
         */
        virtual TResult startup() = 0;

        /**
         * \brief 关闭，释放全部 GPU 资源与全局缓存
         * \remarks 在 ~Agent 最开始调用，此时 RHI 与资源管理器仍然有效、场景尚未卸载。
         *          调用之后不会再收到 update / render。
         */
        virtual void shutdown() = 0;

        /**
         * \brief 每帧更新
         * \remarks 在 Agent::update 中 Scene::update 之后调用。场景不存在时同样调用。
         */
        virtual void update() = 0;

        /**
         * \brief 绘制某台相机上的 UI
         * \param [in] ctx : RHI 上下文；进入时上一个 pass 已结束
         * \param [in] camera : 当前相机；该相机没有 UI 时实现应立即返回
         * \param [in] target : 本阶段的绘制目标
         * \param [in] phase : 绘制阶段
         * \remarks 需自行 setRenderTarget + beginPass / endPass，不要清屏。
         *          不得在这里执行任何 UI 回调或改动布局。
         */
        virtual void render(RHIContext *ctx, Camera *camera, RenderTarget *target, UIRenderPhase phase) = 0;

        /// 指针是否落在可交互的 UI 上（截至最近一次 update）
        virtual bool isPointerOverUI() const = 0;

        /// 是否有 UI 输入框占用键盘（截至最近一次 update）
        virtual bool wantsKeyboard() const = 0;

        /**
         * \brief 释放所有 UI 实例，以及可能引用业务代码的缓存
         * \remarks 编辑器卸载业务插件 DLL 之前调用。调用后 UI 系统仍可继续使用。
         */
        virtual void releaseAll() = 0;
    };
}


#endif  /*__T3D_UI_SYSTEM_H__*/
