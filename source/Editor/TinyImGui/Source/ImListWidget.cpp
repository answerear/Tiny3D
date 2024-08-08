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


#include "ImListWidget.h"
#include "ImTextureManager.h"
#include "ImErrors.h"
#include "ImEventDefine.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    #define IMAGE_DEFAULT_WIDTH         (64)
    #define IMAGE_DEFAULT_HEIGHT        (64)
    
    //--------------------------------------------------------------------------

    ImListItem::ImListItem(ImListWidget *owner)
        : mListWidget(owner)
    {
        
    }
    
    //--------------------------------------------------------------------------

    ImListItem::~ImListItem()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImListItem::create(const String &title, ImWidget *parent)
    {
        uint32_t id = generateItemID();
        return ImWidget::createInternal(id, title, parent, 3, nullptr, nullptr, nullptr);
    }

    //--------------------------------------------------------------------------

    TResult ImListItem::createByTexture(ImTextureID texID, const String &title, ImWidget *parent)
    {
        uint32_t id = generateItemID();
        return ImWidget::createInternal(id, title, parent, 3, texID, nullptr, nullptr);
    }

    //--------------------------------------------------------------------------

    TResult ImListItem::createByTexture(ImTextureID texID, const String &title, const ImListItemClickedCallback &clicked, ImWidget *parent)
    {
        uint32_t id = generateItemID();
        return ImWidget::createInternal(id, title, parent, 3, texID, nullptr, &clicked);
    }
    
    //--------------------------------------------------------------------------

    TResult ImListItem::createByPath(const String &imageName, const String &title, ImWidget *parent)
    {
        uint32_t id = generateItemID();
        return ImWidget::createInternal(id, title, parent, 3, nullptr, &imageName, nullptr);
    }

    //--------------------------------------------------------------------------

    TResult ImListItem::createByPath(const String &imageName, const String &title, const ImListItemClickedCallback &clicked, ImWidget *parent)
    {
        uint32_t id = generateItemID();
        return ImWidget::createInternal(id, title, parent, 3, nullptr, &imageName, &clicked);
    }
    
    //--------------------------------------------------------------------------

    // argc : 2
    // args :
    //  ImTexture : 图标纹理对象
    //  String* : 图标图像路径
    //  ImListItemClickedCallback* : 点击回调
    TResult ImListItem::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        TResult ret = IM_OK;

        do
        {
            // icon texture
            mIconID = va_arg(args, ImTextureID);
            // icon path
            String *imageName = va_arg(args, String*);
            if (mIconID == nullptr && imageName != nullptr)
            {
                mIconID = IM_TEXTURE_MGR.loadTexture(*imageName);
                if (mIconID == nullptr)
                {
                    IMGUI_LOG_ERROR("Load texture %s for ListItem icon failed ! ERROR [%d]", imageName->c_str(), ret)
                    ret = T3D_ERR_FAIL;
                    break;
                }
                mIsInternalLoaded = true;
            }

            // clicked callback
            ImListItemClickedCallback *callback = va_arg(args, ImListItemClickedCallback*);
            if (callback != nullptr)
            {
                mClickedCallback = *callback;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    uint32_t ImListItem::generateItemID()
    {
        return mListWidget->generateID();
    }

    //--------------------------------------------------------------------------

    void ImListItem::updateSizeOnIconMode()
    {
        if (mIsSizeDirty)
        {
            ImGuiStyle &style = ImGui::GetStyle();
            ImVec2 textSize = ImGui::CalcTextSize(getName().c_str());
            ImVec2 imageSize = ImVec2(IMAGE_DEFAULT_WIDTH, IMAGE_DEFAULT_HEIGHT);
            float itemWidth = imageSize.x + style.ItemSpacing.x;
            float itemHeight = imageSize.y + style.ItemSpacing.y + textSize.y;
            itemWidth += style.ItemSpacing.x;
            setSize(ImVec2(itemWidth, itemHeight));

            mIsSizeDirty = false;
        }
    }

    //--------------------------------------------------------------------------

    void ImListItem::updateSizeOnListMode()
    {
        if (mIsSizeDirty)
        {

            mIsSizeDirty = false;
        }
    }

    //--------------------------------------------------------------------------

    void ImListItem::onDestroy()
    {
        if (mIsInternalLoaded)
        {
            if (mIconID != nullptr)
            {
                IM_TEXTURE_MGR.unloadTexture(mIconID);
                mIconID = nullptr;
            }
        }
    }

    //--------------------------------------------------------------------------

    bool ImListItem::onGUIBegin()
    {
        return ImWidget::onGUIBegin();
    }
    
    //--------------------------------------------------------------------------

    bool ImListItem::onGUIBegin(const ImVec2 &size)
    {
        return onGUIBegin();
    }
    
    //--------------------------------------------------------------------------

    String ImListItem::getUniqueName() const
    {
        std::stringstream ss;
        ss << "##" << getUUID().values.high << getUUID().values.low;
        return ss.str();
    }

    //--------------------------------------------------------------------------

    void ImListItem::onGUI()
    {
        if (mListWidget == nullptr)
        {
            return;
        }

        switch (mListWidget->getListMode())
        {
        case ImListWidget::ListMode::kIcon:
            {
                updateSizeOnIconMode();
                drawIconMode();
            }
            break;
        case ImListWidget::ListMode::kList:
            {
                updateSizeOnListMode();
                drawListMode();
            }
            break;
        }
    }

    //--------------------------------------------------------------------------

    void ImListItem::drawTruncateText(const String &text, float maxWidth)
    {
        ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
        ImVec2 ellipsisSize = ImGui::CalcTextSize("...");

        // 检查文本是否超过最大宽度
        if (textSize.x > maxWidth)
        {
            std::string clippedText;
            ImVec2 currentTextSize(0.0f, 0.0f);

            // 遍历文本的每个字符，逐个添加字符，直到达到限制宽度
            for (const char& c : text)
            {
                ImVec2 charSize = ImGui::CalcTextSize(&c, &c + 1);
                if (currentTextSize.x + charSize.x + ellipsisSize.x > maxWidth)
                {
                    break;
                }

                clippedText += c;
                currentTextSize.x += charSize.x;
            }

            ImGui::Text("%s", clippedText.c_str());

            // 在文本末尾添加省略号
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text("...");
        }
        else
        {
            ImGui::Text("%s", text.c_str());
        }
    }

    //--------------------------------------------------------------------------

    void ImListItem::drawIconMode()
    {
        String strID = getUniqueName();
        
        if (mIconID == nullptr)
        {
            bool selected = mListWidget->getSelectedName() == strID;
            ImVec2 itemSize = ImGui::CalcTextSize(getName().c_str());
            if (ImGui::Selectable(strID.c_str(), selected, ImGuiSelectableFlags_None, itemSize))
            {
                mListWidget->getSelectedName() = strID;
            }
        }
        else
        {
            ImGui::BeginGroup();
            
            ImVec2 startPos = ImGui::GetCursorPos();
            ImVec2 textSize = ImGui::CalcTextSize(getName().c_str());
            ImGuiStyle &style = ImGui::GetStyle();
            ImVec2 imageSize = ImVec2(IMAGE_DEFAULT_WIDTH, IMAGE_DEFAULT_HEIGHT);
            float itemWidth = imageSize.x + style.ItemSpacing.x;
            float itemHeight = imageSize.y + style.ItemSpacing.y + textSize.y;
            itemWidth += style.ItemSpacing.x;
            ImVec2 pos = startPos;
            pos.x = startPos.x + style.ItemSpacing.x;
            ImGui::SetCursorPos(pos);
            ImGui::Image(mIconID, imageSize);
            
            pos.y = startPos.y + imageSize.y + style.ItemSpacing.y;
            pos.x = startPos.x;
            ImGui::SetCursorPos(pos);
            drawTruncateText(getName(), itemWidth);
            // ImGui::PushTextWrapPos(pos.x + itemWidth);
            // ImVec2 clipRectMin = ImGui::GetCursorScreenPos();
            // ImVec2 clipRectMax = ImVec2(clipRectMin.x + itemWidth - ellipsisSize.x, clipRectMin.y + textSize.y);
            // ImGui::PushClipRect(clipRectMin, clipRectMax, true);
            // ImGui::Text(getName().c_str());
            // ImGui::PopClipRect();
            // ImGui::PopTextWrapPos();

            bool selected = mListWidget->getSelectedName() == strID;
            ImVec2 itemSize(itemWidth, itemHeight);
            ImGui::SetCursorPos(startPos);
            if (ImGui::Selectable(strID.c_str(), selected, ImGuiSelectableFlags_None, itemSize))
            {
                mListWidget->getSelectedName() = strID;
            }
            
            ImGui::EndGroup();

            if (mListWidget != nullptr && ImGui::IsItemClicked())
            {
                fireClickedEvent();
            }
        }
    }

    //--------------------------------------------------------------------------

    void ImListItem::drawListMode()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImListItem::fireClickedEvent()
    {
        if (mClickedCallback != nullptr)
        {
            mClickedCallback(this);
        }
        else if (mListWidget != nullptr)
        {
            EventParamListItemClicked param(this);
            mListWidget->sendClickedEvent(kEvtTreeNodeClicked, &param);
        }
    }

    //--------------------------------------------------------------------------

    void ImListItem::onGUIEnd()
    {
        ImWidget::onGUIEnd();
    }

    //--------------------------------------------------------------------------

    ImListWidget::~ImListWidget()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImListWidget::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        return ImWidget::createInternal(id, name, parent, argc, args);
    }

    //--------------------------------------------------------------------------

    bool ImListWidget::onGUIBegin()
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool ImListWidget::onGUIBegin(const ImVec2 &size)
    {
        return onGUIBegin();
    }

    //--------------------------------------------------------------------------

    void ImListWidget::onGUI()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImListWidget::onGUIEnd()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImListWidget::update()
    {
        if (isVisible() && onGUIBegin())
        {
            onGUI();

            switch (mListMode)
            {
            case ListMode::kIcon:
                {
                    updateChildrenOnIconMode();
                }
                break;
            case ListMode::kList:
                {
                    updateChildrenOnListMode();
                }
                break;
            }

            onGUIEnd();
        }
    }

    //--------------------------------------------------------------------------

    void ImListWidget::updateChildrenOnIconMode()
    {
        ImGuiStyle &style = ImGui::GetStyle();
        auto region = ImGui::GetContentRegionAvail();
        auto startPos = ImGui::GetCursorPos();
        float x = startPos.x + style.ItemSpacing.x;
        startPos.x = x;
        ImGui::SetCursorPos(startPos);
        float y = startPos.y;

        bool isFirst = true;
        
        for (auto child : getChildren())
        {
            T3D_ASSERT(child->getWidgetType() == WidgetType::kListItem);

            ImListItem *item = static_cast<ImListItem *>(child);
            item->updateSizeOnIconMode();
            
            if (x + child->getSize().x >= region.x)
            {
                // 新的 item 放不下在这一行，只能另起一行
                ImGui::NewLine();
                x = startPos.x;
                ImGui::SetCursorPosX(x);
                isFirst = true;
            }
            else if (!isFirst)
            {
                // 可以放得下，继续同一行
                ImGui::SameLine();
            }
            
            child->update();
            x += child->getSize().x + style.ItemSpacing.x;
            isFirst = false;
        }
    }

    //--------------------------------------------------------------------------

    void ImListWidget::updateChildrenOnListMode()
    {
        for (auto child : getChildren())
        {
            T3D_ASSERT(child->getWidgetType() == WidgetType::kListItem);

            child->update();
            ImGui::NewLine();
        }
    }

    //--------------------------------------------------------------------------

    void ImListWidget::setListMode(ListMode mode)
    {
        if (mode != mListMode)
        {
            mListMode = mode;
            for (auto child : getChildren())
            {
                ImListItem *item = static_cast<ImListItem *>(child);
                item->markSizeDirty();
            }
        }
    }

    //--------------------------------------------------------------------------

    void ImListWidget::sendClickedEvent(uint32_t evt, EventParamListItemClicked *param)
    {
        sendEvent(evt, param);
    }

    //--------------------------------------------------------------------------
}
