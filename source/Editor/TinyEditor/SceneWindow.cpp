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


#include "SceneWindow.h"
#include "EditorScene.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------
    
    void SceneWindow::onGUI()
    {
        EDITOR_SCENE.refreshSceneRenderTarget();

        RenderTarget *target = EDITOR_SCENE.getSceneRenderTarget();
        
        float rtWidth = static_cast<float>(target->getRenderTexture()->getWidth());
        float rtHeight = static_cast<float>(target->getRenderTexture()->getHeight());

        auto region = ImGui::GetContentRegionAvail();
        float u0 = (rtWidth - region.x) * 0.5f;
        float v0 = (rtHeight - region.y) * 0.5f;
        float u1 = u0 + region.x;
        float v1 = v0 + region.y;
        ImVec2 uv0(u0, v0);
        ImVec2 uv1(u1, v1);
        ImGui::Image(EDITOR_SCENE.getSceneRT(), region, uv0, uv1);
    }
    
    //--------------------------------------------------------------------------
    
    NS_END
}
