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

#include "ImErrors.h"


namespace Tiny3D
{
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
        return ImWidget::createInternal(id, title, parent, 2, nullptr, nullptr);
    }

    //--------------------------------------------------------------------------

    TResult ImListItem::createByTexture(ImTextureID texID, const String &title, ImWidget *parent)
    {
        uint32_t id = generateItemID();
        return ImWidget::createInternal(id, title, parent, 2, texID, nullptr);
    }

    //--------------------------------------------------------------------------

    TResult ImListItem::createByPath(const String &imageName, const String &title, ImWidget *parent)
    {
        uint32_t id = generateItemID();
        return ImWidget::createInternal(id, title, parent, 2, nullptr, nullptr);
    }

    //--------------------------------------------------------------------------

    // argc : 2
    // args :
    //  ImTexture : 图标纹理对象
    //  String* : 图标图像路径
    TResult ImListItem::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        TResult ret = IM_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    uint32_t ImListItem::generateItemID()
    {
        return mListWidget->generateID();
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

    void ImListItem::onGUI()
    {
        
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
        TResult ret = IM_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ImListWidget::onGUIBegin()
    {
        return ImWidget::onGUIBegin();
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
        ImWidget::onGUIEnd();
    }

    //--------------------------------------------------------------------------
}
