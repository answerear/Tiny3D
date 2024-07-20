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


#include "GameWindow.h"
#include "EditorScene.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    ImGuiWindowFlags GameWindow::flags() const
    {
        return ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    }

    //--------------------------------------------------------------------------

    void GameWindow::onGUI()
    {
        auto region = ImGui::GetContentRegionAvail();
        EDITOR_SCENE.refreshGameRenderTarget(region);

        RenderTarget *target = EDITOR_SCENE.getGameRenderTarget();
        
        // float rtWidth = static_cast<float>(target->getRenderTexture()->getWidth());
        // float rtHeight = static_cast<float>(target->getRenderTexture()->getHeight());

        float w = 16.0f;
        float h = 9.0f;
        float ratio = w / h;

        float targetRatio = region.x / region.y;

        float x = 0.0f;
        float y = 0.0f;
        ImVec2 size;
        
        if (targetRatio > ratio)
        {
            // 大于 16:9，则屏幕宽度更大，高度更小
            size.y = region.y;
            size.x = size.y * ratio;
            x = (region.x - size.x) * 0.5f;
        }
        else
        {
            // 小于 16:9，如 4:3，则屏幕宽度更小，高度更大
            size.x = region.x;
            size.y = size.x / ratio;
            y = (region.y - size.y) * 0.5f;
        }

        T3D_ASSERT(size.x <= region.x && size.y <= region.y, "Image size must be smaller than region size !");
        
        float u0 = x / region.x;
        float v0 = y / region.y;
        float u1 = (u0 + size.x) / region.x;
        float v1 = (v0 + size.y) / region.y;
        ImVec2 uv0(u0, v0);
        ImVec2 uv1(u1, v1);
        x = ImGui::GetCursorPosX() + x;
        y = ImGui::GetCursorPosY() + y;
        ImGui::SetCursorPosX(x);
        ImGui::SetCursorPosY(y);
        ImGui::Image(EDITOR_SCENE.getGameRT(), size, uv0, uv1);
    }

    //--------------------------------------------------------------------------

    NS_END
}
