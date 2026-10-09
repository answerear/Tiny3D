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


#ifndef __T3D_RMLUI_SYSTEM_H__
#define __T3D_RMLUI_SYSTEM_H__


#include "T3DRmlUiPrerequisites.h"


namespace Tiny3D
{
    /**
     * \brief RmlUi 对 UISystem 的实现
     * \remarks Phase 0 只完成 Initialise / Shutdown。update、render 与查询接口为空。
     */
    class T3D_RMLUI_API RmlUiSystem : public UISystem
    {
    public:
        static RmlUiSystemPtr create();

        const String &getName() const override;

        TResult startup() override;
        void shutdown() override;
        void update() override;
        void render(RHIContext *ctx, Camera *camera, RenderTarget *target, UIRenderPhase phase) override;
        bool isPointerOverUI() const override;
        bool wantsKeyboard() const override;
        void releaseAll() override;

    private:
        RmlUiSystem() = default;

        String  mName {"RmlUi"};
        bool    mInitialised {false};
    };
}


#endif  /*__T3D_RMLUI_SYSTEM_H__*/
