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


#pragma once


#include "ImChildView.h"


namespace Tiny3D
{
    using ButtonClickedCallback = TFunction<void(uint32_t id)>;
    using ButtonQueryCallback = TFunction<bool(uint32_t id)>;

    using EventParamToolButton = EventParamT1<uint32_t>;
    
    class TINYIMGUI_API ImToolBar : public ImChildView, public EventHandler
    {
    public:
        enum class Alignment : uint32_t
        {
            kLeft = 0,
            kMiddle,
            kRight,
        };
        
        ~ImToolBar() override;

        WidgetType getWidgetType() const override { return WidgetType::kToolBar; }

        void setButtonSize(const ImVec2 &size) { mButtonSize = size; mSize.y = size.y + mHorzSpacing * 2.0f; }

        const ImVec2 &getButtonSize() const { return mButtonSize; }
        
        void setAlignment(Alignment alignment) { mAlignment = alignment; }

        TResult addButton(uint32_t id, ImTextureID texID, const String &shortcut, const String &tips, const ButtonQueryCallback &query, const ButtonClickedCallback &clicked);

        TResult addButton(uint32_t id, ImTextureID texID, const String &shortcut, const String &tips, const ButtonQueryCallback &query, uint32_t clickedEvtID);

        TResult addButton(uint32_t id, ImTextureID texID, const String &shortcut, const String &tips, const ButtonQueryCallback &query, const ButtonQueryCallback &check, const ButtonClickedCallback &clicked);
        
        TResult addButton(uint32_t id, ImTextureID texID, const String &shortcut, const String &tips, const ButtonQueryCallback &query, const ButtonQueryCallback &check, uint32_t clickedEvtID);

        TResult removeButton(uint32_t id);
        
    protected:
        TResult onCreate() override;
        bool onGUIBegin() override;
        void onGUI() override;
        void onGUIEnd() override;

        ImGuiChildFlags onGetChildFlags() override;
        ImGuiWindowFlags onGetWindowFlags() override;
        
        String makeName(uint32_t id);
        
    protected:
        struct ToolButton
        {
            ToolButton() = default;
            
            ToolButton(const String &_name, uint32_t _id, uint32_t _clickedEvtID, ImTextureID _texID, const String &_shortcut, const String &_tips, const ButtonQueryCallback &_query, const ButtonQueryCallback &_check, const ButtonClickedCallback &_clicked)
                : id(_id)
                , clickedEvtID(_clickedEvtID)
                , texID(_texID)
                , name(_name)
                , shortcut(_shortcut)
                , tips(_tips)
                , query(_query)
                , check(_check)
                , clicked(_clicked)
            {
            }
            
            /// 用户赋值的 ID
            uint32_t    id {0xFFFFFFFF};
            /// 点击事件 ID
            uint32_t    clickedEvtID {0};
            /// 纹理对象
            ImTextureID texID {nullptr};
            /// 名字
            String      name {};
            /// 快捷键
            String      shortcut {};
            /// 提示语
            String      tips {};
            /// 查询可用状态的 Lambda
            ButtonQueryCallback query {nullptr};
            /// 查询选中状态的 Lambda
            ButtonQueryCallback check {nullptr};
            /// 点击的 Lambda
            ButtonClickedCallback clicked{nullptr};
        };

        using ToolButtons = TList<ToolButton>;

        /// 所有按钮
        ToolButtons mButtons {};
        /// 按钮大小
        ImVec2  mButtonSize {20, 20};
        /// toolbar 上下 space
        float   mHorzSpacing {5.0f};
        /// 按钮对齐方式
        Alignment mAlignment {Alignment::kLeft};
    };
}
