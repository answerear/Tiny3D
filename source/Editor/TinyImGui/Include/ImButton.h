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
    /**
     * 普通按钮
     */
    class TINYIMGUI_API ImButton : public ImWidget, public EventHandler
    {
    public:
        ~ImButton() override;

        WidgetType getWidgetType() const override { return WidgetType::kButton; }

        TResult create(uint32_t id, const String &title, ImWidget *parent, const String &tips = "", const String &shortcut = "");
        
        TResult create(uint32_t id, const String &title, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips = "", const String &shortcut = "");
        
        TResult create(uint32_t id, const String &title, const ImButtonQueryCallback &queryEnabled, ImWidget *parent, const String &tips = "", const String &shortcut = "");
        
        TResult create(uint32_t id, const String &title, const ImButtonQueryCallback &queryEnabled, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips = "", const String &shortcut = "");

    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args) override;
        
        void onGUI() override;

    protected:
        /// 查询是否可用的 Lambda 对象
        ImButtonQueryCallback   mQueryEnabled {nullptr};
        /// 点击响应的 Lambda 对象
        ImButtonClickedCallback mClicked {nullptr};
        /// 提示语
        String      mTips {};
    };

    /**
     * 有按下状态的按钮
     */
    class TINYIMGUI_API ImPushButton : public ImButton
    {
    public:
        ~ImPushButton() override;

        WidgetType getWidgetType() const override { return WidgetType::kPushButton; }

        TResult create(uint32_t id, const String &title, ImButtonQueryCallback &queryChecked, ImWidget *parent, const String &tips = "", const String &shortcut = "");

        TResult create(uint32_t id, const String &title, ImButtonQueryCallback &queryChecked, ImButtonClickedCallback clicked, ImWidget *parent, const String &tips = "", const String &shortcut = "");

        TResult create(uint32_t id, const String &title, ImButtonQueryCallback &queryEnabled, ImButtonQueryCallback &queryChecked, ImWidget *parent, const String &tips = "", const String &shortcut = "");

        TResult create(uint32_t id, const String &title, ImButtonQueryCallback &queryEnabled, ImButtonQueryCallback &queryChecked, ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips = "", const String &shortcut = "");

        bool isPushed() const { return mQueryChecked ? mQueryChecked(this) : false; }
        
    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list&args) override;

        void onGUI() override;

    protected:
        /// 查询是否按下状态的 Lambda 对象
        ImButtonQueryCallback   mQueryChecked {nullptr};
    };

    /**
     * 纯图片按钮
     */
    class TINYIMGUI_API ImImageButton : public ImButton
    {
    public:
        ~ImImageButton() override;

        WidgetType getWidgetType() const override { return WidgetType::kImageButton; }

        TResult create(uint32_t id, ImTextureID texID, ImWidget *parent, const String &tips = "", const String &shortcut = "");
        
        TResult createEx(uint32_t id, const String &imageName, ImWidget *parent, const String &tips = "", const String &shortcut = "");
        
        TResult create(uint32_t id, ImTextureID texID, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips = "", const String &shortcut = "");
        
        TResult createEx(uint32_t id, const String &imageName, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips = "", const String &shortcut = "");
        
        TResult create(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, ImWidget *parent, const String &tips = "", const String &shortcut = "");
        
        TResult createEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, ImWidget *parent, const String &tips = "", const String &shortcut = "");
        
        TResult create(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips = "", const String &shortcut = "");
        
        TResult createEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips = "", const String &shortcut = "");

    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list&args) override;

        void onGUI() override;

        void onDestroy() override;

        String makeName(uint32_t id);
        
    protected:
        /// 图片 ID
        ImTextureID mTexID {nullptr};
        /// 是否内部加载纹理对象，内部加载的，需要自己卸载
        bool    mIsInternalLoaded {false};
    };

    /**
     * 带按下状态的纯图片按钮
     */
    class TINYIMGUI_API ImPushImageButton : public ImImageButton
    {
    public:
        ~ImPushImageButton() override;

        WidgetType getWidgetType() const override { return WidgetType::kPushImageButton; }

        TResult create(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryChecked, ImWidget *parent, const String &tips = "", const String &shortcut = "");
        
        TResult createEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryChecked, ImWidget *parent, const String &tips = "", const String &shortcut = "");

        TResult create(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryChecked, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips = "", const String &shortcut = "");
        
        TResult createEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryChecked, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips = "", const String &shortcut = "");

        TResult create(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, ImWidget *parent, const String &tips = "", const String &shortcut = "");
        
        TResult createEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, ImWidget *parent, const String &tips = "", const String &shortcut = "");

        TResult create(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips = "", const String &shortcut = "");

        TResult createEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips = "", const String &shortcut = "");

        bool isPushed() const { return mQueryChecked ? mQueryChecked(this) : false; }
        
    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list&args) override;

        void onGUI() override;

    protected:
        /// 查询是否按下状态的 Lambda 对象
        ImButtonQueryCallback   mQueryChecked {nullptr};
    };
}
