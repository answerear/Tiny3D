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
#include "EditorWidgetID.h"
#include "ImErrors.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    void GameView::onGUI()
    {
        auto region = ImGui::GetContentRegionAvail();
        EDITOR_SCENE.refreshGameRenderTarget(region);

        RenderTarget *target = EDITOR_SCENE.getGameRenderTarget();
        
        float rtWidth = static_cast<float>(target->getRenderTexture()->getWidth());
        float rtHeight = static_cast<float>(target->getRenderTexture()->getHeight());

        float ratio = rtWidth / rtHeight;

        float targetRatio = region.x / region.y;

        float x = 0.0f;
        float y = 0.0f;
        ImVec2 size;
        
        if (targetRatio > ratio)
        {
            // 大于 16:9，则屏幕宽度更大，高度更小
            if (rtHeight > region.y)
            {
                size.y = region.y;
            }
            else
            {
                size.y = rtHeight;
            }
            size.x = size.y * ratio;
            x = Math::abs(region.x - size.x) * 0.5f;
        }
        else
        {
            // 小于 16:9，如 4:3，则屏幕宽度更小，高度更大
            if (rtWidth > region.x)
            {
                size.x = region.x;
            }
            else
            {
                size.x = rtWidth;
            }
            size.y = size.x / ratio;
            y = Math::abs(region.y - size.y) * 0.5f;
        }

        T3D_ASSERT(size.x <= region.x && size.y <= region.y);
        
        // float u0 = x / region.x;
        // float v0 = y / region.y;
        // float u1 = (x + size.x) / region.x;
        // float v1 = (y + size.y) / region.y;
        // ImVec2 uv0(u0, v0);
        // ImVec2 uv1(u1, v1);
        x = ImGui::GetCursorPosX() + x;
        y = ImGui::GetCursorPosY() + y;
        ImGui::SetCursorPosX(x);
        ImGui::SetCursorPosY(y);
        // ImGui::Image(EDITOR_SCENE.getGameRT(), size, uv0, uv1);
        ImGui::Image(EDITOR_SCENE.getGameRT(), size);
    }

    //--------------------------------------------------------------------------

    ImGuiWindowFlags GameWindow::flags() const
    {
        return ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    }

    //--------------------------------------------------------------------------
    
    TResult GameWindow::createToolBar()
    {
        TResult ret = T3D_OK;

        do
        {
            mToolBar = new ImToolBar();
            ret = mToolBar->create(ID_GAME_WINDOW_TOOLBAR, "GameToolBar", nullptr);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create game tool bar failed ! ERROR [%d]", ret)
                break;
            }
            mToolBar->setAlignment(ImToolBar::Alignment::kMiddle);
            mToolBar->setButtonSize(ImVec2(16.0f, 16.0f));
            
            auto queryEnableDefault = [](const ImWidget *widget) { return true; };
            auto queryDisableDefault = [](const ImWidget *widget) { return false; };
            auto clickedDefault = [](ImWidget *widget) {};

            ret = mToolBar->addPushImageButtonEx(ID_GAME_WIDNOW_TOOL_BTN_PLAY, "Editor/icons/d_PlayButton On@2x.png", queryDisableDefault, nullptr, clickedDefault, "Run Game", "");
            if (T3D_FAILED(ret))
            {
                break;
            }

            ret = mToolBar->addPushImageButtonEx(ID_GAME_WINDOW_TOOL_BTN_PAUSE, "Editor/icons/d_PauseButton On@2x.png", queryDisableDefault, nullptr, clickedDefault, "Pause Game", "");
            if (T3D_FAILED(ret))
            {
                break;
            }

            ret = mToolBar->addPushImageButtonEx(ID_GAME_WINDOW_TOOL_BTN_NEXT, "Editor/icons/d_StepButton On@2x.png", queryDisableDefault, nullptr, clickedDefault, "Next Frame", "");
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GameWindow::createGameView()
    {
        TResult ret = T3D_OK;

        do
        {
            mGameView = new GameView();
            ret = mGameView->create(ID_GAME_VIEW, "GameView", nullptr);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create game view failed ! ERROR [%d]", ret)
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GameWindow::onCreate()
    {
        TResult ret = ImWidget::onCreate();

        do
        {
            // 工具栏
            ret = createToolBar();
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create game window toolbar failed ! ERROR [%d]", ret)
                break;
            }

            // 游戏视图
            ret = createGameView();
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create game view in game window failed ! ERROR [%d]", ret)
                break;
            }

            // 创建自动布局，上下布局
            ImLayout *layout = new ImLayout();
            ret = layout->create(ID_GAME_WINDOW_LAYOUT, "GameWindowLayout", this);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create game window layout failed ! ERROR [%d]", ret)
                break;
            }

            ImLayout::Items items;
            ImLayout::Item item;
            // 工具栏
            item.size = mToolBar->getSize();
            item.childView = mToolBar;
            items.emplace_back(item);
            // 分行
            item.childView = ImLayout::NEWLINE;
            items.emplace_back(item);
            // 游戏视图
            item.size = mGameView->getSize();
            item.childView = mGameView;
            items.emplace_back(item);
            layout->addWidgets(items);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void GameWindow::onGUI()
    {
        
    }

    //--------------------------------------------------------------------------

    NS_END
}
