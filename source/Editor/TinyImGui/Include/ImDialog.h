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

        TResult create(uint32_t id, const String &name, ImWidget *parent, bool hasCloseBtn = true);

        TResult show(ShowType type);

        void close(bool destroy = false);
        
    protected:
        virtual void onShow();

        virtual void onClose();

        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args) override;
        
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

    class ImDialogButton
    {
    public:
        ImDialogButton() = default;

        ImDialogButton(const ImDialogButton &other)
            : name(other.name)
            , callback(other.callback)
        {
        }
            
        ImDialogButton(ImDialogButton &&other) noexcept
        {
            name = std::move(other.name);
            callback = std::move(other.callback);
        }

        ImDialogButton &operator =(const ImDialogButton &other)
        {
            name = other.name;
            callback = other.callback;
            return *this;
        }
            
        String          name {};
        OnMessageBoxCB  callback {nullptr};
    };

    using ImDialogButtons = TArray<ImDialogButton>;
    
    class TINYIMGUI_API ImMessageBox : public ImDialog, public Singleton<ImMessageBox>
    {
    public:
        ~ImMessageBox() override = default;

        static TResult show(const String &title, const String &message, ShowType type, ImDialogButtons &&buttons, const ImVec4 &txtColor = ImVec4(0, 0, 0, 0));

        static TResult show(const String &title, const String &message, const String &btnText, ShowType type, const ImVec4 &txtColor = ImVec4(0, 0, 0, 0));

    protected:
        TResult appear(const String &title, const String &message, ShowType type, ImDialogButtons &&buttons, const ImVec4 &txtColor);

        bool onGUIBegin() override;
        
        void onGUI() override;
        
        String          mMessage {};
        ImDialogButtons mButtons {};
        ImVec4          mMessageColor {};
    };

    class TINYIMGUI_API ImProgressDialog : public ImDialog
    {
    public:
        /**
         * 进度条形式
         */
        enum class Style : uint32_t
        {
            /// 不确定进度的
            Indeterminate = 0,
            /// 明确知道进度的
            Determinate,
        };
        
        ~ImProgressDialog() override = default;

        /**
         * @brief 创建进度对话框
         * @param [in] id : 控件 ID
         * @param [in] title : 对话框标题
         * @param [in] style : 样式，see enum class Style
         * @param [in] button : 按钮
         * @param [in] parent : 父节点 
         * @param [in] hasCloseBtn : 是否有关闭按钮，默认有
         * @return 调用成功返回 IM_OK
         */
        TResult create(uint32_t id, const String &title, Style style, const ImDialogButton &button, ImWidget *parent = nullptr, bool hasCloseBtn = true);

        /**
         * 设置显示在 ProgressBar 上方或者里面的文本，inBar 只有在 Style::Indeterminate 有效。 Style::Determinate 的时候只显示在 ProgressBar 上方
         * @param [in] text : 文本
         * @param [in] inBar : 是否显示在 ProgressBar 里面
         */
        void setText(const String &text, bool inBar = false) { mText = text; mIsInBar = inBar; }
        
        /**
         * 设置百分比进度，Style::Determinate 下有效
         * @param [in] fraction : 进度百分比. 此时不用设置总进度，因为总进度就是 1.0
         */
        void setProgress(float fraction) { mFraction = fraction; }

        /**
         * 设置整数总进度，Style::Determinate 下有效。 一般用于知道总数的，例如文件数量等
         * @param [in] total : 总数量
         */
        void setTotal(uint32_t total) { mTotal = total; }

        /**
         * 设置整数进度，Style::Determinate 下有效。 一般用于知道总数，当前进度到哪个步骤
         * @param [in] step : 步骤
         */
        void setProgress(uint32_t step) { step < mTotal ? mFraction = static_cast<float>(step) / static_cast<float>(mTotal) : mFraction = 1.0f; }

    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args) override;
        
        bool onGUIBegin() override;
        bool onGUIBegin(const ImVec2 &size) override;
        void onGUI() override;
        void onGUIEnd() override;

    protected:
        /// 进度条样式
        Style mStyle {Style::Indeterminate};
        /// 总数量
        uint32_t mTotal {0};
        /// 进度百分比
        float mFraction {0.0f};
        /// 对话框按钮
        ImDialogButton mButton {};
        /// 显示的文本
        String mText {};
        /// 文本是否显示在 ProgressBar 里还是显示在 ProgressBar 上方
        bool mIsInBar {true};
    };
}
