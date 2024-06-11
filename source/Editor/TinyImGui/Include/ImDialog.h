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


#include "ImWidget.h"


namespace Tiny3D
{
    class TINYIMGUI_API ImDialog : public ImWidget
    {
    public:
        enum class ShowType : uint32_t
        {
            /// 排队显示
            kEnqueueBack = 0,
            /// 如果当前有一个正在显示，则当前这个关闭后马上显示；如果没有，那就马上显示
            kEnqueueFront,
            /// 覆盖显示
            kOverlay,
            /// 挤掉当前正在显示的，马上显示
            kImmediately,
        };

        static void poll();

        ~ImDialog() override;

        WidgetType getWidgetType() const override { return WidgetType::kDialog; }

        TResult create(const String &name, ImWidget *parent, bool hasCloseBtn = true);

        TResult show(ShowType type);

        void close(bool destroy = false);
        
    protected:
        virtual void onShow();

        virtual void onClose();

        TResult createInternal(const String &name, ImWidget *parent, int32_t argc, va_list args) override;
        
        void update() override;
        
        bool onGUIBegin() override;
        void onGUI() override;
        void onGUIEnd() override;

        TResult enqueueBack();
        TResult enqueueFront();
        TResult showOverlay();
        TResult showImmediately();

        void appear(bool overlay);
        
    protected:
        /// 将要显示标记
        bool mWillAppear {false};
        /// 是否在 update 中调用
        bool mInUpdate {false};
        /// 是否关闭对话框
        bool mShouldClose {false};
        /// 是否有关闭按钮
        bool mHasCloseBtn {true};

        using DialogStack = TStack<ImDialog*>;

        /// 当前正在显示的 dialog
        static DialogStack msDialogStack;

        using DialogQueue = TList<ImDialog*>;

        /// 排队等待显示的对话框
        static DialogQueue msDialogQueue;
    };

    using OnMessageBoxCB = TFunction<void()>;
    
    class TINYIMGUI_API ImMessageBox : public ImDialog, public Singleton<ImMessageBox>
    {
    public:
        struct Button
        {
            Button() = default;

            Button(const Button &other)
                : name(other.name)
                , callback(other.callback)
            {
            }
            
            Button(Button &&other) noexcept
            {
                name = std::move(other.name);
                callback = std::move(other.callback);
            }
            
            String          name {};
            OnMessageBoxCB  callback {nullptr};
        };
        
        using Buttons = TArray<Button>;
        
        ~ImMessageBox() override = default;

        static TResult show(const String &title, const String &message, ShowType type, Buttons &&buttons, const ImVec4 &txtColor = ImVec4(0, 0, 0, 0));

    protected:
        TResult appear(const String &title, const String &message, ShowType type, Buttons &&buttons, const ImVec4 &txtColor);

        bool onGUIBegin() override;
        
        void onGUI() override;
        
        String  mMessage {};
        Buttons mButtons {};
        ImVec4  mMessageColor {};
    };
}
