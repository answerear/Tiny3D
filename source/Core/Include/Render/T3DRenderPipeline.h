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


#ifndef __T3D_RENDER_PIPELINE_H__
#define __T3D_RENDER_PIPELINE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief 渲染管线抽象基类与全局单例，定义场景剔除与绘制流程
     */
    class T3D_ENGINE_API RenderPipeline
        : public Object
        , public Singleton<RenderPipeline>
    {
    public:
        enum : uint32_t
        {
            DEFAULT_GROUP = 0,
            NONE_GROUP = 0xFFFFFFFF,
            DEFAULT_ORDER = 0xFFFFFFFF,
        };

        /**
         * \brief 初始化渲染管线（创建阴影贴图等内部资源）
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult init() = 0;

        /**
         * \brief 遍历场景、收集相机/光源并对可见 Renderable 做视锥剔除
         * \param [in] scene : 待渲染场景；为 nullptr 时返回 T3D_ERR_INVALID_PARAM
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult cull(Scene *scene) = 0;

        /**
         * \brief 对所有已收集相机执行渲染（阴影 pass + 前向 pass 等）
         * \param [in] ctx : RHI 渲染上下文
         * \return 最后一台相机的渲染结果；单台失败时跳过并继续下一台
         */
        virtual TResult render(RHIContext *ctx) = 0;

        /**
         * \brief 将 Renderable 按相机、渲染队列和材质归入内部渲染队列
         * \param [in] camera : 目标相机
         * \param [in] renderable : 待加入的可渲染对象
         * \return 调用成功返回 T3D_OK；材质或 Technique 为空时静默跳过仍返回 T3D_OK
         */
        virtual TResult addRenderable(Camera *camera, Renderable *renderable) = 0;

        /**
         * \brief 从渲染队列移除 Renderable
         * \param [in] renderable : 待移除的可渲染对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult removeRenderable(Renderable *renderable) = 0;

        /**
         * \brief 登记场景光源；首个方向光会被选为阴影投射光源
         * \param [in] light : 待登记光源；为 nullptr 时返回 T3D_ERR_INVALID_PARAM
         * \return 调用成功返回 T3D_OK；UUID 重复时返回 T3D_ERR_DUPLICATED_ITEM
         */
        virtual TResult addLight(Light *light) = 0;

        /**
         * \brief 从光源表移除光源
         * \param [in] light : 待移除光源；为 nullptr 时返回 T3D_ERR_INVALID_PARAM
         * \return 调用成功返回 T3D_OK；未找到时返回 T3D_ERR_NOT_FOUND
         */
        virtual TResult removeLight(Light *light) = 0;

        /**
         * \brief 释放管线持有的内部资源（阴影 RT、天空盒 VB 等）
         */
        virtual void destroy() = 0;

        /// 开启或关闭动态合批
        void enableDynamicBatch(bool enabled) { mDynamicBatchEnabled = enabled; }

        /// 动态合批是否启用
        bool isDynamicBatchEnabled() const { return mDynamicBatchEnabled; }

        /// 设置参与动态合批的 mesh 最大顶点数
        void setMaxNumOfVertexInMethForDynamicBatch(uint32_t num) { mMaxNumOfVertexInMethForDynamicBatch = num; }

        /// 参与动态合批的 mesh 最大顶点数
        uint32_t getMaxNumOfVertexInMethForDynamicBatch() const { return mMaxNumOfVertexInMethForDynamicBatch; }
        

    protected:
        bool mDynamicBatchEnabled {false};
        uint32_t mMaxNumOfVertexInMethForDynamicBatch {0};
    };

    #define T3D_RENDER_PIPELINE     (RenderPipeline::getInstance()) 

    T3D_EXTERN_SINGLETON(RenderPipeline)
}


#endif  /*__T3D_RENDER_PIPELINE_H__*/
