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
#include "ImDialog.h"


namespace Tiny3D
{
#if !defined (IM_USE_GENERAL_MUTLTI_TREE)
    //--------------------------------------------------------------------------

    ImWidget::WaitingRemoveWidgets ImWidget::msWaitingRemoveWidgets;
    ImWidget::WaitingDestroyWidgets ImWidget::msWaitingDestroyWidgets;

    bool ImWidget::msInUpdate = false;
    
    //--------------------------------------------------------------------------

    void ImWidget::GC()
    {
        // 移除子节点
        for (auto item : msWaitingRemoveWidgets)
        {
            item.first->removeChild(item.second.itr, item.second.destroy);
        }

        msWaitingRemoveWidgets.clear();

        // 删除节点
        for (auto widget : msWaitingDestroyWidgets)
        {
            widget->onDestroy();
            if (widget->getParent() != nullptr)
            {
                widget->getParent()->removeChild(widget);
            }
        }

        msWaitingDestroyWidgets.clear();
    }
    
    //--------------------------------------------------------------------------

    void ImWidget::beginUpdate()
    {
        msInUpdate = true;
    }

    //--------------------------------------------------------------------------

    void ImWidget::endUpdate()
    {
        msInUpdate = false;

        ImDialog::poll();
        GC();
    }
    
    //--------------------------------------------------------------------------

    ImWidget::~ImWidget()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::create(uint32_t id, const String &name, ImWidget *parent)
    {
        return createInternal(id, name, parent, 0);
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, ...)
    {
        TResult ret = IM_OK;

        do
        {
            mID = id;
            mName = name;
            mUUID = UUID::generate();
            
            if (parent != nullptr)
            {
                parent->addChild(this);
            }
            
            va_list args;
            va_start(args, argc);
            ret = createInternal(id, name, parent, argc, args);
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
            mID = 0;
            mName = "";
            mUUID = UUID::INVALID;
            mParent->removeChild(this, false);
            mParent = nullptr;
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        // TResult ret = IM_OK;
        //
        // setName(name);
        //
        // if (parent != nullptr)
        // {
        //     ret = parent->addChild(this);
        // }
        //
        // return ret;
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

    TResult ImWidget::addWidget(const String &parentName, ImWidget *widget)
    {
        TResult ret = T3D_ERR_NOT_FOUND;

        do
        {
            if (parentName == getName())
            {
                ret = addChild(widget);
                break;
            }

            for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
            {
                auto child = *itr;
                ret = child->addWidget(parentName, widget);
                if (T3D_SUCCEEDED(ret))
                {
                    break;
                }
            }
        } while (false);
        

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::addWidget(const UUID &parentUUID, ImWidget *widget)
    {
        TResult ret = T3D_ERR_NOT_FOUND;

        do
        {
            if (parentUUID == getUUID())
            {
                ret = addChild(widget);
                break;
            }

            for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
            {
                auto child = *itr;
                ret = child->addWidget(parentUUID, widget);
                if (T3D_SUCCEEDED(ret))
                {
                    break;
                }
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::addWidget(uint32_t parentID, ImWidget *widget)
    {
        TResult ret = IM_OK;

        do
        {
            if (parentID == getID())
            {
                ret = addChild(widget);
                break;
            }

            for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
            {
                auto child = *itr;
                ret = child->addWidget(parentID, widget);
                if (T3D_SUCCEEDED(ret))
                {
                    break;
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
            for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
            {
                auto child = *itr;
                if (widget == child)
                {
                    ret = removeChild(child, destroy);
                    break;
                }
            }

            if (T3D_SUCCEEDED(ret))
            {
                break;
            }

            for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
            {
                auto child = *itr;
                ret = child->removeWidget(widget, destroy);
                if (T3D_SUCCEEDED(ret))
                {
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

        do
        {
            for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
            {
                auto child = *itr;
                if (name == child->getName())
                {
                    if (T3D_SUCCEEDED(removeChild(child, destroy)))
                    {
                        widget = child;
                        break;
                    }
                }
            }

            if (widget != nullptr)
            {
                break;
            }

            for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
            {
                auto child = *itr;
                widget = child->removeWidget(name, destroy);
                if (widget != nullptr)
                {
                    break;
                }
            }
        } while (false);
        
        return widget;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::removeWidget(const UUID &uuid, bool destroy)
    {
        ImWidget *widget = nullptr;

        do
        {
            for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
            {
                auto child = *itr;
                if (uuid == child->getUUID())
                {
                    if (T3D_SUCCEEDED(removeChild(child, destroy)))
                    {
                        widget = child;
                        break;
                    }
                }
            }

            if (widget != nullptr)
            {
                break;
            }

            for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
            {
                auto child = *itr;
                widget = child->removeWidget(uuid, destroy);
                if (widget != nullptr)
                {
                    break;
                }
            }
        } while (false);

        return widget;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::removeWidget(uint32_t id, bool destroy)
    {
        ImWidget *widget = nullptr;

        do
        {
            for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
            {
                auto child = *itr;
                if (id == child->getID())
                {
                    if (T3D_SUCCEEDED(removeChild(child, destroy)))
                    {
                        widget = child;
                        break;
                    }
                }
            }

            if (widget != nullptr)
            {
                break;
            }

            for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
            {
                auto child = *itr;
                widget = child->removeWidget(id, destroy);
                if (widget != nullptr)
                {
                    break;
                }
            }
        } while (false);
        
        return widget;
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::addChild(ImWidget *widget)
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
                T3D_LOG_ERROR(LOG_TAG_TINYIMGUI, "The parent of widget [%s] is not nullptr when addChild !", widget->getName().c_str());
                break;
            }

            if (widget != nullptr)
            {
                widget->mParent = this;
                mChildren.emplace_back(widget);

                widget->mSelfItr = mChildren.end()--;

                // 回调通知加入子 widget
                onChildAdded(widget);
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::insertAfterChild(const String &prevName, ImWidget *widget)
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
                T3D_LOG_ERROR(LOG_TAG_TINYIMGUI, "The parent of widget [%s] is not nullptr when insertAfterChild !", widget->getName().c_str());
                break;
            }

            if (prevName.empty())
            {
                // 插入最前面
                mChildren.emplace_front(widget);
                onChildAdded(widget);
            }
            else
            {
                for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
                {
                    if ((*itr)->getName() == prevName)
                    {
                        ++itr;
                        mChildren.emplace(itr, widget);
                        onChildAdded(widget);
                        break;
                    }
                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::insertAfterChild(const UUID &prevUUID, ImWidget *widget)
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
                T3D_LOG_ERROR(LOG_TAG_TINYIMGUI, "The parent of widget [%s] is not nullptr when insertAfterChild !", widget->getName().c_str());
                break;
            }

            if (prevUUID == UUID::INVALID)
            {
                // 插入最前面
                mChildren.emplace_front(widget);
                onChildAdded(widget);
            }
            else
            {
                for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
                {
                    if ((*itr)->getUUID() == prevUUID)
                    {
                        ++itr;
                        mChildren.emplace(itr, widget);
                        onChildAdded(widget);
                        break;
                    }
                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::insertAfterChild(ImWidget *prevWidget, ImWidget *widget)
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
                T3D_LOG_ERROR(LOG_TAG_TINYIMGUI, "The parent of widget [%s] is not nullptr when insertAfterChild !", widget->getName().c_str());
                break;
            }

            if (prevWidget == nullptr)
            {
                // 插入最前面
                mChildren.emplace_front(widget);
                onChildAdded(widget);
            }
            else
            {
                for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
                {
                    if ((*itr) == prevWidget)
                    {
                        ++itr;
                        mChildren.emplace(itr, widget);
                        onChildAdded(widget);
                        break;
                    }
                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::insertAfterChild(uint32_t prevID, ImWidget *widget)
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
                T3D_LOG_ERROR(LOG_TAG_TINYIMGUI, "The parent of widget [%s] is not nullptr when insertAfterChild !", widget->getName().c_str());
                break;
            }

            if (prevID == T3D_INVALID_ID)
            {
                // 插入最前面
                mChildren.emplace_front(widget);
                onChildAdded(widget);
            }
            else
            {
                for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
                {
                    if ((*itr)->getID() == prevID)
                    {
                        ++itr;
                        mChildren.emplace(itr, widget);
                        onChildAdded(widget);
                        break;
                    }
                }
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void ImWidget::removeChild(Children::iterator itr, bool destroy)
    {
        ImWidget *widget = *itr;
        widget->mParent = nullptr;
        mChildren.erase(itr);
        widget->mSelfItr = mChildren.end();
        onChildRemoved(widget);
        if (destroy)
        {
            widget->destroy();
        }
    }
    
    //--------------------------------------------------------------------------

    TResult ImWidget::removeChild(ImWidget *widget, bool destroy)
    {
        TResult ret = IM_OK;

        do
        {
            if (widget == nullptr)
            {
                ret = IM_ERR_INVALID_PARAM;
                break;
            }

            auto itr = std::find_if(
                mChildren.begin(),
                mChildren.end(),
                [widget](ImWidget *w)
                {
                    return widget == w;
                });

            if (itr != mChildren.end())
            {
                widget = *itr;
                if (msInUpdate)
                {
                    // 在遍历调用中，不能直接移除
                    RemoveWidgetInfo info;
                    info.itr = itr;
                    info.destroy = destroy;
                    msWaitingRemoveWidgets.emplace(this, info);
                }
                else
                {
                    // 没在遍历调用中，直接移除
                    removeChild(itr, destroy);
                }
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::removeChild(const String &name, bool destroy)
    {
        ImWidget *widget = nullptr;

        auto itr = std::find_if(
            mChildren.begin(),
            mChildren.end(),
            [&name](ImWidget *widget)
            {
                return widget->getName() == name;
            });

        if (itr != mChildren.end())
        {
            widget = *itr;
            if (msInUpdate)
            {
                // 在遍历调用中，不能直接移除
                RemoveWidgetInfo info;
                info.itr = itr;
                info.destroy = destroy;
                msWaitingRemoveWidgets.emplace(this, info);
            }
            else
            {
                // 没在遍历调用中，直接移除
                removeChild(itr, destroy);
            }
        }
        return widget;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::removeChild(const UUID &uuid, bool destroy)
    {
        ImWidget *widget = nullptr;

        auto itr = std::find_if(
            mChildren.begin(),
            mChildren.end(),
            [&uuid](ImWidget *widget)
            {
                return widget->getUUID() == uuid;
            });

        if (itr != mChildren.end())
        {
            widget = *itr;
            if (msInUpdate)
            {
                // 在遍历调用中，不能直接移除
                RemoveWidgetInfo info;
                info.itr = itr;
                info.destroy = destroy;
                msWaitingRemoveWidgets.emplace(this, info);
            }
            else
            {
                // 没在遍历调用中，直接移除
                removeChild(itr, destroy);
            }
        }
        
        return widget;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::removeChild(uint32_t id, bool destroy)
    {
        ImWidget *widget = nullptr;

        auto itr = std::find_if(
            mChildren.begin(),
            mChildren.end(),
            [id](ImWidget *widget)
            {
                return widget->getID() == id;
            });

        if (itr != mChildren.end())
        {
            widget = *itr;
            if (msInUpdate)
            {
                // 在遍历调用中，不能直接移除
                RemoveWidgetInfo info;
                info.itr = itr;
                info.destroy = destroy;
                msWaitingRemoveWidgets.emplace(this, info);
            }
            else
            {
                // 没在遍历调用中，直接移除
                removeChild(itr, destroy);
            }
        }
        
        return widget;
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::removeAllChildren()
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

    ImWidget *ImWidget::getWidget(uint32_t id) const
    {
        ImWidget *widget = nullptr;

        for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr)
        {
            if (id == (*itr)->getID())
            {
                widget = *itr;
                break;
            }

            widget = widget->getWidget(id);
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
        auto itr = std::find_if(
            mChildren.begin(),
            mChildren.end(),
            [&name](ImWidget *widget)
            {
                return name == widget->getName();
            });

        return itr != mChildren.end() ? *itr : nullptr;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::getChild(const UUID &uuid) const
    {
        auto itr = std::find_if(
            mChildren.begin(),
            mChildren.end(),
            [&uuid](ImWidget *widget)
            {
                return uuid == widget->getUUID();
            });

        return itr != mChildren.end() ? *itr : nullptr;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::getChild(uint32_t id) const
    {
        auto itr = std::find_if(
            mChildren.begin(),
            mChildren.end(),
            [id](ImWidget *widget)
            {
                return id == widget->getID();
            });

        return itr != mChildren.end() ? *itr : nullptr;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::getPrevSibling() const
    {
        if (getParent() == nullptr || mSelfItr == getParent()->getChildren().begin())
        {
            return nullptr;
        }

        auto itr = mSelfItr;
        itr--;
        return *itr;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::getNextSibling() const
    {
        if (getParent() == nullptr)
        {
            return nullptr;
        }

        auto itr = mSelfItr;
        itr++;

        if (itr == getParent()->getChildren().end())
        {
            return nullptr;
        }

        return *itr;
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

        if (getParent() == nullptr)
        {
            if (mDebugEnabled && (mDebugFrame == 0xFFFFFFFF || mDebugFrame == mFrameCount))
            {
                T3D_LOG_INFO(LOG_TAG_TINYIMGUI, "Widget hierarchy : ");
                traverseHierarchyPreOrder(
                    [](ImWidget *widget, int32_t depth)
                    {
                        std::stringstream ss;
                        for (int32_t i = 0; i < depth; ++i)
                        {
                            ss << "\t";
                        }
                        ss << widget->getName();
                        T3D_LOG_INFO(LOG_TAG_TINYIMGUI, "%s", ss.str().c_str());
                    });
            }
            
            mFrameCount++;
        }
    }

    //--------------------------------------------------------------------------

    void ImWidget::update(const ImVec2 &size)
    {
        if (mVisible && onGUIBegin(size))
        {
            onGUI();

            for (auto child : mChildren)
            {
                child->update(size);
            }
            
            onGUIEnd();
        }

        if (getParent() == nullptr && mDebugEnabled && (mDebugFrame == 0xFFFFFFFF || mDebugFrame == mFrameCount))
        {
            T3D_LOG_INFO(LOG_TAG_TINYIMGUI, "Widget hierarchy : ");
            traverseHierarchyPreOrder(
                [](ImWidget *widget, int32_t depth)
                {
                    std::stringstream ss;
                    for (int32_t i = 0; i < depth; ++i)
                    {
                        ss << "\t";
                    }
                    ss << widget->getName();
                    T3D_LOG_INFO(LOG_TAG_TINYIMGUI, "%s", ss.str().c_str());
                });
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

    void ImWidget::setEnable(bool enable)
    {
        if (mEnable != enable)
        {
            mEnable = enable;

            if (enable)
            {
                onEnable();
            }
            else
            {
                onDisable();
            }
        }
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::onCreate()
    {
        return IM_OK;
    }

    //--------------------------------------------------------------------------

    bool ImWidget::onGUIBegin()
    {
        ImGui::PushID(this);
        return true;
    }

    //--------------------------------------------------------------------------

    bool ImWidget::onGUIBegin(const ImVec2 &size)
    {
        return onGUIBegin();
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

    void ImWidget::onEnable()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImWidget::onDisable()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImWidget::onChildAdded(ImWidget *widget)
    {
        
    }

    //--------------------------------------------------------------------------

    void ImWidget::onChildRemoved(ImWidget *widget)
    {
        
    }

    //--------------------------------------------------------------------------

    bool *ImWidget::onGetVisible()
    {
        return &mVisible;
    }

    //--------------------------------------------------------------------------

    void ImWidget::setDebugHierarchy(bool enable, uint32_t frame)
    {
        mDebugEnabled = enable;
        mDebugFrame = frame;
    }

    //--------------------------------------------------------------------------

    void ImWidget::traverseHierarchyPreOrder(const TraverseHierarchyCallback &callback, int32_t depth)
    {
        if (callback == nullptr)
        {
            return;
        }
        
        callback(this, depth);

        depth++;
        for (auto widget : getChildren())
        {
            widget->traverseHierarchyPreOrder(callback, depth);
        }
        depth--;
    }

    //--------------------------------------------------------------------------

    void ImWidget::traverseHierarchyPostOrder(const TraverseHierarchyCallback &callback, int32_t depth)
    {
        if (callback == nullptr)
        {
            return;
        }

        depth++;
        for (auto widget : getChildren())
        {
            widget->traverseHierarchyPostOrder(callback);
        }
        depth--;

        callback(this, depth);
    }

    //--------------------------------------------------------------------------

#else

    //--------------------------------------------------------------------------

    ImWidget::WaitingRemoveWidgets ImWidget::msWaitingRemoveWidgets;
    ImWidget::WaitingDestroyWidgets ImWidget::msWaitingDestroyWidgets;

    bool ImWidget::msInUpdate = false;
    
    //--------------------------------------------------------------------------

    void ImWidget::GC()
    {
        // 移除子节点
        for (auto item : msWaitingRemoveWidgets)
        {
            item.first->_removeChild(item.second.widget, item.second.destroy);
        }

        msWaitingRemoveWidgets.clear();

        // 删除节点
        for (auto widget : msWaitingDestroyWidgets)
        {
            widget->onDestroy();
            if (widget->getParent() != nullptr)
            {
                widget->getParent()->removeChild(widget);
            }
        }

        msWaitingDestroyWidgets.clear();
    }
    
    //--------------------------------------------------------------------------

    void ImWidget::beginUpdate()
    {
        msInUpdate = true;
    }

    //--------------------------------------------------------------------------

    void ImWidget::endUpdate()
    {
        msInUpdate = false;

        ImDialog::poll();
        GC();
    }
    
    //--------------------------------------------------------------------------

    ImWidget::~ImWidget()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::create(uint32_t id, const String &name, ImWidget *parent)
    {
        return createInternal(id, name, parent, 0);
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, ...)
    {
        TResult ret = IM_OK;

        do
        {
            mID = id;
            mName = name;
            mUUID = UUID::generate();
            
            if (parent != nullptr)
            {
                parent->addChild(this);
            }
            
            va_list args;
            va_start(args, argc);
            ret = createInternal(id, name, parent, argc, args);
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
            mID = 0;
            mName = "";
            mUUID = UUID::INVALID;
            mParent->removeChild(this, false);
            mParent = nullptr;
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        // TResult ret = IM_OK;
        //
        // setName(name);
        //
        // if (parent != nullptr)
        // {
        //     ret = parent->addChild(this);
        // }
        //
        // return ret;
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

    bool ImWidget::addWidget(const String &parentName, ImWidget *widget)
    {
        bool ret = false;

        do
        {
            if (parentName == getName())
            {
                ret = addChild(widget);
                break;
            }

            for (auto child : getChildren())
            {
                ret = child->addWidget(parentName, widget);
                if (ret)
                {
                    break;
                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ImWidget::addWidget(const UUID &parentUUID, ImWidget *widget)
    {
        bool ret = false;

        do
        {
            if (parentUUID == getUUID())
            {
                ret = addChild(widget);
                break;
            }

            for (auto child : getChildren())
            {
                ret = child->addWidget(parentUUID, widget);
                if (ret)
                {
                    break;
                }
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    bool ImWidget::addWidget(uint32_t parentID, ImWidget *widget)
    {
        bool ret = false;

        do
        {
            if (parentID == getID())
            {
                ret = addChild(widget);
                break;
            }

            for (auto child : getChildren())
            {
                ret = child->addWidget(parentID, widget);
                if (ret)
                {
                    break;
                }
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    bool ImWidget::removeWidget(ImWidget *widget, bool destroy)
    {
        TResult ret = IM_OK;

        do
        {
            // 先搜自己的子节点，有符合条件的，则直接删掉
            for (auto child : getChildren())
            {
                if (widget == child)
                {
                    ret = removeChild(child, destroy);
                    break;
                }
            }

            if (ret)
            {
                break;
            }

            // 子节点没有搜到，递归搜索子节点，让其删除其子节点
            for (auto child : getChildren())
            {
                ret = child->removeWidget(widget, destroy);
                if (ret)
                {
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

        do
        {
            for (auto child : getChildren())
            {
                if (name == child->getName())
                {
                    if (removeChild(child, destroy))
                    {
                        widget = child;
                        break;
                    }
                }
            }

            if (widget != nullptr)
            {
                break;
            }

            for (auto child : getChildren())
            {
                widget = child->removeWidget(name, destroy);
                if (widget != nullptr)
                {
                    break;
                }
            }
        } while (false);
        
        return widget;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::removeWidget(const UUID &uuid, bool destroy)
    {
        ImWidget *widget = nullptr;

        do
        {
            for (auto child : getChildren())
            {
                if (uuid == child->getUUID())
                {
                    if (removeChild(child, destroy))
                    {
                        widget = child;
                        break;
                    }
                }
            }

            if (widget != nullptr)
            {
                break;
            }

            for (auto child : getChildren())
            {
                widget = child->removeWidget(uuid, destroy);
                if (widget != nullptr)
                {
                    break;
                }
            }
        } while (false);

        return widget;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::removeWidget(uint32_t id, bool destroy)
    {
        ImWidget *widget = nullptr;

        do
        {
            for (auto child : getChildren())
            {
                if (id == child->getID())
                {
                    if (T3D_SUCCEEDED(removeChild(child, destroy)))
                    {
                        widget = child;
                        break;
                    }
                }
            }

            if (widget != nullptr)
            {
                break;
            }

            for (auto child : getChildren())
            {
                widget = child->removeWidget(id, destroy);
                if (widget != nullptr)
                {
                    break;
                }
            }
        } while (false);
        
        return widget;
    }

    //--------------------------------------------------------------------------

    bool ImWidget::addChild(ImWidget *widget)
    {
        return TreeNode::addChild(widget,
            [this](ImWidget *widget)
            {
                onChildAdded(widget);
            });
    }

    //--------------------------------------------------------------------------

    bool ImWidget::insertAfterChild(const String &prevName, ImWidget *widget)
    {
        bool ret = false;

        if (prevName.empty())
        {
            // 插入最前面
            ret = insertAfterChild(nullptr, widget);
        }
        else
        {
            for (auto child : getChildren())
            {
                if (child->getName() == prevName)
                {
                    ret = insertAfterChild(child, widget);
                    break;
                }
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ImWidget::insertAfterChild(const UUID &prevUUID, ImWidget *widget)
    {
        bool ret = false;

        if (prevUUID == UUID::INVALID)
        {
            // 插入最前面
            ret = insertAfterChild(nullptr, widget);
        }
        else
        {
            for (auto child : getChildren())
            {
                if (child->getUUID() == prevUUID)
                {
                    ret = insertAfterChild(child, widget);
                    break;
                }
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ImWidget::insertAfterChild(ImWidget *prevWidget, ImWidget *widget)
    {
        return TreeNode::insertAfterChild(prevWidget, widget,
            [this](ImWidget *widget)
            {
                onChildAdded(widget);
            });
    }

    //--------------------------------------------------------------------------

    bool ImWidget::insertAfterChild(uint32_t prevID, ImWidget *widget)
    {
        bool ret = false;

        if (prevID == T3D_INVALID_ID)
        {
            // 插入最前面
            ret = insertAfterChild(nullptr, widget);
        }
        else
        {
            for (auto child : getChildren())
            {
                if (child->getID() == prevID)
                {
                    ret = insertAfterChild(child, widget);
                    break;
                }
            }
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    bool ImWidget::_removeChild(ImWidget *widget, bool destroy)
    {
        bool ret = false;
        
        if (msInUpdate)
        {
            // 在遍历调用中，不能直接移除
            RemoveWidgetInfo info;
            info.widget = widget;
            info.destroy = destroy;
            msWaitingRemoveWidgets.emplace(this, info);
            ret = true;
        }
        else
        {
            // 没在遍历调用中，直接移除
            ret = TreeNode::removeChild(widget,
                [destroy](ImWidget *widget)
                {
                    if (destroy)
                    {
                        widget->destroy();
                    }
                });
        }
        return ret;
    }
    
    //--------------------------------------------------------------------------

    bool ImWidget::removeChild(ImWidget *widget, bool destroy)
    {
        bool ret = false;

        auto itr = std::find(child_begin(), child_end(), widget);
        
        if (itr != child_end())
        {
            ImWidget *child = *itr;
            _removeChild(child, destroy);
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::removeChild(const String &name, bool destroy)
    {
        ImWidget *widget = nullptr;

        auto itr = std::find_if(
            child_begin(),
            child_end(),
            [&name](ImWidget *widget)
            {
                return widget->getName() == name;
            });

        if (itr != child_end())
        {
            widget = *itr;
            _removeChild(widget, destroy);
        }
        return widget;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::removeChild(const UUID &uuid, bool destroy)
    {
        ImWidget *widget = nullptr;

        auto itr = std::find_if(
            child_begin(),
            child_end(),
            [&uuid](ImWidget *widget)
            {
                return widget->getUUID() == uuid;
            });

        if (itr != child_end())
        {
            widget = *itr;
            _removeChild(widget, destroy);
        }
        
        return widget;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::removeChild(uint32_t id, bool destroy)
    {
        ImWidget *widget = nullptr;

        auto itr = std::find_if(
            child_begin(),
            child_end(),
            [id](ImWidget *widget)
            {
                return widget->getID() == id;
            });

        if (itr != child_end())
        {
            widget = *itr;
            _removeChild(widget, destroy);
        }
        
        return widget;
    }

    //--------------------------------------------------------------------------

    void ImWidget::removeAllChildren()
    {
        return TreeNode::removeAllChildren(nullptr, [](ImWidget *widget) { widget->destroy(); });
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
        
        for (auto child : getChildren())
        {
            if (child->getName() == name)
            {
                // 找到，返回
                widget = child;
                break;
            }
        }

        if (widget == nullptr)
        {
            // 没有找到，遍历找子节点
            names.pop_front();
            
            for (auto child : getChildren())
            {
                widget = child->getWidgetRecursively(names);
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

        for (auto child : getChildren())
        {
            if (uuid == child->getUUID())
            {
                widget = child;
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

    ImWidget *ImWidget::getWidget(uint32_t id) const
    {
        ImWidget *widget = nullptr;

        for (auto child : getChildren())
        {
            if (id == child->getID())
            {
                widget = child;
                break;
            }

            widget = widget->getWidget(id);
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
        auto itr = std::find_if(
            child_begin(),
            child_end(),
            [&name](ImWidget *widget)
            {
                return name == widget->getName();
            });

        return itr != mChildren.end() ? *itr : nullptr;
    }

    //--------------------------------------------------------------------------
    
    ImWidget *ImWidget::getChild(uint32_t id) const
    {
        auto itr = std::find_if(
            mChildren.begin(),
            mChildren.end(),
            [id](ImWidget *widget)
            {
                return id == widget->getID();
            });

        return itr != mChildren.end() ? *itr : nullptr;
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::getPrevSibling() const
    {
        if (getParent() == nullptr || mSelfItr == getParent()->getChildren().begin())
        {
            return nullptr;
        }

        return *std::prev(mSelfItr);
    }

    //--------------------------------------------------------------------------

    ImWidget *ImWidget::getNextSibling() const
    {
        if (getParent() == nullptr)
        {
            return nullptr;
        }

        auto itr = std::next(mSelfItr);

        if (itr == getParent()->getChildren().end())
        {
            return nullptr;
        }

        return *itr;
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

        if (getParent() == nullptr)
        {
            if (mDebugEnabled && (mDebugFrame == 0xFFFFFFFF || mDebugFrame == mFrameCount))
            {
                T3D_LOG_INFO(LOG_TAG_TINYIMGUI, "Widget hierarchy : ");
                traverseHierarchyPreOrder(
                    [](ImWidget *widget, int32_t depth)
                    {
                        std::stringstream ss;
                        for (int32_t i = 0; i < depth; ++i)
                        {
                            ss << "\t";
                        }
                        ss << widget->getName();
                        T3D_LOG_INFO(LOG_TAG_TINYIMGUI, "%s", ss.str().c_str());
                    });
            }
            
            mFrameCount++;
        }
    }

    //--------------------------------------------------------------------------

    void ImWidget::update(const ImVec2 &size)
    {
        if (mVisible && onGUIBegin(size))
        {
            onGUI();

            for (auto child : mChildren)
            {
                child->update(size);
            }
            
            onGUIEnd();
        }

        if (getParent() == nullptr && mDebugEnabled && (mDebugFrame == 0xFFFFFFFF || mDebugFrame == mFrameCount))
        {
            T3D_LOG_INFO(LOG_TAG_TINYIMGUI, "Widget hierarchy : ");
            traverseHierarchyPreOrder(
                [](ImWidget *widget, int32_t depth)
                {
                    std::stringstream ss;
                    for (int32_t i = 0; i < depth; ++i)
                    {
                        ss << "\t";
                    }
                    ss << widget->getName();
                    T3D_LOG_INFO(LOG_TAG_TINYIMGUI, "%s", ss.str().c_str());
                });
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

    void ImWidget::setEnable(bool enable)
    {
        if (mEnable != enable)
        {
            mEnable = enable;

            if (enable)
            {
                onEnable();
            }
            else
            {
                onDisable();
            }
        }
    }

    //--------------------------------------------------------------------------

    TResult ImWidget::onCreate()
    {
        return IM_OK;
    }

    //--------------------------------------------------------------------------

    bool ImWidget::onGUIBegin()
    {
        ImGui::PushID(this);
        return true;
    }

    //--------------------------------------------------------------------------

    bool ImWidget::onGUIBegin(const ImVec2 &size)
    {
        return onGUIBegin();
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

    void ImWidget::onEnable()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImWidget::onDisable()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImWidget::onChildAdded(ImWidget *widget)
    {
        
    }

    //--------------------------------------------------------------------------

    void ImWidget::onChildRemoved(ImWidget *widget)
    {
        
    }

    //--------------------------------------------------------------------------

    bool *ImWidget::onGetVisible()
    {
        return &mVisible;
    }

    //--------------------------------------------------------------------------

    void ImWidget::setDebugHierarchy(bool enable, uint32_t frame)
    {
        mDebugEnabled = enable;
        mDebugFrame = frame;
    }

    //--------------------------------------------------------------------------

    void ImWidget::traverseHierarchyPreOrder(const TraverseHierarchyCallback &callback, int32_t depth)
    {
        if (callback == nullptr)
        {
            return;
        }
        
        callback(this, depth);

        depth++;
        for (auto widget : getChildren())
        {
            widget->traverseHierarchyPreOrder(callback, depth);
        }
        depth--;
    }

    //--------------------------------------------------------------------------

    void ImWidget::traverseHierarchyPostOrder(const TraverseHierarchyCallback &callback, int32_t depth)
    {
        if (callback == nullptr)
        {
            return;
        }

        depth++;
        for (auto widget : getChildren())
        {
            widget->traverseHierarchyPostOrder(callback);
        }
        depth--;

        callback(this, depth);
    }

    //--------------------------------------------------------------------------
    
#endif
}
