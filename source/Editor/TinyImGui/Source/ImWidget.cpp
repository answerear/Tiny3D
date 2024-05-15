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


#include "ImWidget.h"
#include "ImErrors.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImWidget::WaitingDestroyWidgets ImWidget::msWaitingDestroyWidgets;
    
    //--------------------------------------------------------------------------

    void ImWidget::GC()
    {
        for (auto widget : msWaitingDestroyWidgets)
        {
            widget->onDestroy();
            if (widget->getParent() != nullptr)
            {
                widget->getParent()->removeWidget(widget);
            }
        }

        msWaitingDestroyWidgets.clear();
    }
    
    //--------------------------------------------------------------------------

    ImWidget::~ImWidget()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::create(const String &name, ImWidget *parent)
    {
        return create(name, parent, 0);
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::create(const String &name, ImWidget *parent, int32_t argc, ...)
    {
        TResult ret = IM_OK;

        do
        {
            mName = name;
            
            if (parent != nullptr)
            {
                parent->addWidget(this);
            }

            mUUID = UUID::generate();
            
            va_list args;
            va_start(args, argc);
            ret = create(name, parent, argc, args);
            va_end(args);

            if (T3D_FAILED(ret))
            {
                break;
            }
            
            ret = onCreate();
            
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);

        if (T3D_FAILED(ret))
        {
            mUUID = UUID::INVALID;
            mParent->removeWidget(this, false);
            mParent = nullptr;
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::create(const String &name, ImWidget *parent, int32_t argc, va_list args)
    {
        return IM_OK;
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::destroy()
    {
        for (auto widget : mChildren)
        {
            destroyWidget(widget);
        }

        msWaitingDestroyWidgets.emplace_back(this);
        
        return IM_OK;
    }

    //--------------------------------------------------------------------------

    void ImWidget::destroyWidget(ImWidget *child)
    {
        for (auto widget : child->getChildren())
        {
            destroyWidget(widget);
        }

        msWaitingDestroyWidgets.emplace_back(child);
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::addWidget(ImWidget *widget)
    {
        TResult ret = IM_OK;

        do
        {
            if (widget == nullptr)
            {
                ret = IM_ERR_INVALID_PARAM;
                break;
            }

            if (widget->getParent() != nullptr)
            {
                ret = IM_ERR_INVALID_PARENT;
                T3D_LOG_ERROR(LOG_TAG_TINYIMGUI, "The parent of widget [%s] is not nullptr when addWidget !", widget->getName().c_str());
                break;
            }

            if (widget != nullptr)
            {
                widget->mParent = this;
                mChildren.emplace_back(widget);
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::insertAfterWidget(const String &prevName, ImWidget *widget)
    {
        TResult ret = IM_OK;

        do
        {
            if (widget == nullptr)
            {
                // 空 widget
                ret = IM_ERR_INVALID_PARAM;
                break;
            }

            if (widget->getParent() != nullptr)
            {
                // 已经有父节点，直接报错
                ret = IM_ERR_INVALID_PARENT;
                T3D_LOG_ERROR(LOG_TAG_TINYIMGUI, "The parent of widget [%s] is not nullptr when insertAfterWidget !", widget->getName().c_str());
                break;
            }

            if (prevName.empty())
            {
                // 插入最前面
                mChildren.emplace_front(widget);
            }
            else
            {
                for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
                {
                    if ((*itr)->getName() == prevName)
                    {
                        ++itr;
                        mChildren.emplace(itr, widget);
                        break;
                    }
                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::insertAfterWidget(const UUID &prevUUID, ImWidget *widget)
    {
        TResult ret = IM_OK;

        do
        {
            if (widget == nullptr)
            {
                // 空 widget
                ret = IM_ERR_INVALID_PARAM;
                break;
            }

            if (widget->getParent() != nullptr)
            {
                // 已经有父节点，直接报错
                ret = IM_ERR_INVALID_PARENT;
                T3D_LOG_ERROR(LOG_TAG_TINYIMGUI, "The parent of widget [%s] is not nullptr when insertAfterWidget !", widget->getName().c_str());
                break;
            }

            if (prevUUID == UUID::INVALID)
            {
                // 插入最前面
                mChildren.emplace_front(widget);
            }
            else
            {
                for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
                {
                    if ((*itr)->getUUID() == prevUUID)
                    {
                        ++itr;
                        mChildren.emplace(itr, widget);
                        break;
                    }
                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::insertAfterWidget(ImWidget *prevWidget, ImWidget *widget)
    {
        TResult ret = IM_OK;

        do
        {
            if (widget == nullptr)
            {
                // 空 widget
                ret = IM_ERR_INVALID_PARAM;
                break;
            }

            if (widget->getParent() != nullptr)
            {
                // 已经有父节点，直接报错
                ret = IM_ERR_INVALID_PARENT;
                T3D_LOG_ERROR(LOG_TAG_TINYIMGUI, "The parent of widget [%s] is not nullptr when insertAfterWidget !", widget->getName().c_str());
                break;
            }

            if (prevWidget == nullptr)
            {
                // 插入最前面
                mChildren.emplace_front(widget);
            }
            else
            {
                for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
                {
                    if ((*itr) == prevWidget)
                    {
                        ++itr;
                        mChildren.emplace(itr, widget);
                        break;
                    }
                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::removeWidget(ImWidget *widget, bool destroy)
    {
        TResult ret = IM_OK;

        do
        {
            if (widget == nullptr)
            {
                ret = IM_ERR_INVALID_PARAM;
                break;
            }

            for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
            {
                if (*itr == widget)
                {
                    widget->mParent = nullptr;
                    mChildren.erase(itr);
                    if (destroy)
                    {
                        widget->destroy();
                    }
                    break;
                }
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::removeWidget(const String &name, bool destroy)
    {
        ImWidget *widget = nullptr;

        for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
        {
            if ((*itr)->getName() == name)
            {
                widget = *itr;
                widget->mParent = nullptr;
                mChildren.erase(itr);
                if (destroy)
                {
                    widget->destroy();
                    widget = nullptr;
                }
                break;
            }
        }
        
        return widget;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::removeWidget(const UUID &uuid, bool destroy)
    {
        ImWidget *widget = nullptr;

        for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
        {
            if ((*itr)->getUUID() == uuid)
            {
                widget = *itr;
                widget->mParent = nullptr;
                mChildren.erase(itr);
                if (destroy)
                {
                    widget->destroy();
                    widget = nullptr;
                }
                break;
            }
        }
        
        return widget;
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::removeAllWidgets()
    {
        for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
        {
            ImWidget *widget = *itr;
            widget->destroy();
        }
        
        return IM_OK;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::getWidget(const String &name) const
    {
        StringList names = StringUtil::split2(name, "/");
        return getWidgetRecursively(names);
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::getWidgetRecursively(StringList &names) const
    {
        ImWidget *widget = nullptr;

        const String &name = names.front();
        
        for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
        {
            if ((*itr)->getName() == name)
            {
                // 找到，返回
                widget = *itr;
                break;
            }
        }

        if (widget == nullptr)
        {
            // 没有找到，遍历找子节点
            names.pop_front();
            
            for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
            {
                widget = (*itr)->getWidgetRecursively(names);
                if (widget != nullptr)
                {
                    break;
                }
            }
        }

        return widget;
    }
    
    //--------------------------------------------------------------------------

    ImWidget *ImWidget::getWidget(const UUID &uuid) const
    {
        ImWidget *widget = nullptr;

        for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
        {
            if (uuid == (*itr)->getUUID())
            {
                widget = *itr;
                break;
            }

            widget = widget->getWidget(uuid);
            if (widget != nullptr)
            {
                break;
            }
        }

        return widget;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::getChild(const String &name) const
    {
        ImWidget *widget = nullptr;

        for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
        {
            if (name == (*itr)->getName())
            {
                widget = *itr;
                break;
            }
        }

        return widget;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::getChild(const UUID &uuid) const
    {
        ImWidget *widget = nullptr;

        for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
        {
            if (uuid == (*itr)->getUUID())
            {
                widget = *itr;
                break;
            }
        }
        
        return widget;
    }

    //--------------------------------------------------------------------------

    void ImWidget::update()
    {
        if (mVisible && onGUIBegin())
        {
            onGUI();

            for (auto child : mChildren)
            {
                child->update();
            }
            
            onGUIEnd();
        }
    }

    //--------------------------------------------------------------------------

    void ImWidget::setVisible(bool visible)
    {
        if (mVisible != visible)
        {
            mVisible = visible;

            if (visible)
            {
                onVisible();
            }
            else
            {
                onInvisible();
            }
        }
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::onCreate()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool ImWidget::onGUIBegin()
    {
        ImGui::PushID(this);
        return true;
    }

    //--------------------------------------------------------------------------

    void ImWidget::onGUI()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImWidget::onGUIEnd()
    {
        ImGui::PopID();
    }

    //--------------------------------------------------------------------------

    void ImWidget::onDestroy()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImWidget::onVisible()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImWidget::onInvisible()
    {
        
    }

    //--------------------------------------------------------------------------
}
