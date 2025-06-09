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
         * \brief 场景物体遍历和剔除
         * \param [in] scene : 场景对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult cull(Scene *scene) = 0;

        /**
         * \brief 执行渲染
         * \param [in] ctx : RHI 渲染上下文
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult render(RHIContext *ctx) = 0;

        virtual TResult addRenderable(Camera *camera, Renderable *renderable) = 0;

        virtual TResult removeRenderable(Renderable *renderable) = 0;

        virtual TResult addLight(Light *light) = 0;

        virtual TResult removeLight(Light *light) = 0;

        /**
         * 开启或关闭动态合批
         * @param enabled 开关
         */
        void enableDynamicBatch(bool enabled) { mDynamicBatchEnabled = enabled; }

        bool isDynamicBatchEnabled() const { return mDynamicBatchEnabled; }

        void setMaxNumOfVertexInMethForDynamicBatch(uint32_t num) { mMaxNumOfVertexInMethForDynamicBatch = num; }

        uint32_t getMaxNumOfVertexInMethForDynamicBatch() const { return mMaxNumOfVertexInMethForDynamicBatch; }

    protected:
        bool mDynamicBatchEnabled {false};
        uint32_t mMaxNumOfVertexInMethForDynamicBatch {0};
    };

    #define T3D_RENDER_PIPELINE     (RenderPipeline::getInstance()) 
}


#endif  /*__T3D_RENDER_PIPELINE_H__*/
