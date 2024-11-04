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
#include "ImEventDefine.h"


namespace Tiny3D
{
    /**
     * 列表项
     */
    class TINYIMGUI_API ImListItem : public ImWidget
    {
        friend class ImListWidget;
        
    public:
        ImListItem(ImListWidget *owner);
        
        ~ImListItem() override;

        WidgetType getWidgetType() const override { return WidgetType::kListItem; }

        TResult create(const String &title, ImWidget *parent);

        TResult createByTexture(ImTextureID texID, const String &title, ImWidget *parent);
        
        TResult createByTexture(ImTextureID texID, const String &title, const ImListItemClickedCallback &clicked, ImWidget *parent);

        TResult createByPath(const String &imageName, const String &title, ImWidget *parent);
        
        TResult createByPath(const String &imageName, const String &title, const ImListItemClickedCallback &clicked, ImWidget *parent);

    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args) override;

        bool onGUIBegin() override;
        bool onGUIBegin(const ImVec2 &size) override;
        void onGUI() override;
        void onGUIEnd() override;

        void onDestroy() override;
        
        uint32_t generateItemID();

        void drawIconMode();

        void drawListMode();

        String getUniqueName() const;

        void drawTruncateText(const String &text, const ImVec2 &pos, float maxWidth);

        void updateSizeOnIconMode();

        void updateSizeOnListMode();

        void markSizeDirty() { mIsSizeDirty = true; }

        void fireClickedEvent();
        
    protected:
        /// 列表项绑定的列表控件
        ImListWidget    *mListWidget {nullptr};
        /// 图标纹理对象
        ImTextureID     mIconID {nullptr};
        /// 单击回调
        ImListItemClickedCallback mClickedCallback {nullptr};
        /// 是否内部加载图标
        bool mIsInternalLoaded {false};
        /// 是否更新大小尺寸
        bool mIsSizeDirty {true};
    };

    /**
     * 列表控件
     */
    class TINYIMGUI_API ImListWidget
        : public ImWidget
        , public EventHandler
    {
        friend class ImListItem;
        
    public:
        /**
         * 列表模式
         */
        enum class ListMode : uint32_t
        {
            /// 图标模式
            kIcon,
            /// 列表模式
            kList,
        };
        
        ~ImListWidget() override;

        WidgetType getWidgetType() const override { return WidgetType::kListWidget; }

        /**
         * 获取当前列表模式
         * @return 返回当前列表模式
         */
        ListMode getListMode() const { return mListMode; }

        /**
         * 设置当前列表模式
         * @param [in] mode : 新的列表模式
         */
        void setListMode(ListMode mode);

    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args) override;
        
        bool onGUIBegin() override;
        bool onGUIBegin(const ImVec2 &size) override;
        void onGUI() override;
        void onGUIEnd() override;

        void update() override;

        void updateChildrenOnIconMode();

        void updateChildrenOnListMode();

        uint32_t generateID() { return mItemID++; }

        String &getSelectedName() { return mSelectedName; }

        void sendClickedEvent(uint32_t evt, EventParamListItemClicked *param);
        
    protected:
        String  mSelectedName {};
        /// 列表模式
        ListMode    mListMode {ListMode::kIcon};
        /// 列表项 ID
        uint32_t    mItemID {0};
    };
}
