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


#include "UISceneWindow.h"
#include "EditorSceneImpl.h"
#include "UIEditorWidgetID.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------
    
    void UISceneWindow::onGUI()
    {
        EDITOR_SCENE.refreshSceneRenderTarget();

        const char *items[] = { "Shaded", "Wireframe", "Shaded Wireframe" };
        int current = static_cast<int>(EDITOR_SCENE.getDrawMode());
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Shading Mode");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(180.0f);
        ImGui::PushID(ID_SCENE_DRAW_MODE_COMBO);
        if (ImGui::Combo("##SceneDrawMode", &current, items, IM_ARRAYSIZE(items)))
        {
            EDITOR_SCENE.setDrawMode(static_cast<SceneDrawMode>(current));
        }
        ImGui::PopID();

        RenderTarget *target = EDITOR_SCENE.getSceneRenderTarget();

        float rtWidth = static_cast<float>(target->getRenderTexture()->getWidth());
        float rtHeight = static_cast<float>(target->getRenderTexture()->getHeight());

        auto region = ImGui::GetContentRegionAvail();
        float x = (rtWidth - region.x) * 0.5f;
        float y = (rtHeight - region.y) * 0.5f;
        float u0 = x / rtWidth;
        float v0 = y / rtHeight;
        float u1 = (x + region.x) / rtWidth;
        float v1 = (y + region.y) / rtHeight;
        ImVec2 uv0(u0, v0);
        ImVec2 uv1(u1, v1);
        ImGui::Image(EDITOR_SCENE.getSceneRT(), region, uv0, uv1);
    }
    
    //--------------------------------------------------------------------------
    
    NS_END
}
