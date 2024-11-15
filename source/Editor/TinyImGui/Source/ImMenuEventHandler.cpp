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


#include "ImMenuEventHandler.h"
#include "ImEventDefine.h"
#include "ImMenu.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImMenuEventHandler::~ImMenuEventHandler()
    {
        unregisterAllMenuEvents();
    }
    
    //--------------------------------------------------------------------------

    TResult ImMenuEventHandler::registerMenuEvent(uint32_t menuID, const ImMenuEventHandle &handle)
    {
        return ImMenuEventManager::getInstance().registerMenuEvent(menuID, this, handle);
    }
    
    //--------------------------------------------------------------------------

    TResult ImMenuEventHandler::unregisterMenuEvent(uint32_t menuID)
    {
        return ImMenuEventManager::getInstance().unregisterMenuEvent(menuID, this);
    }

    //--------------------------------------------------------------------------

    void ImMenuEventHandler::unregisterAllMenuEvents()
    {
        ImMenuEventManager::getInstance().unregisterMenuEventHandler(this);
    }

    //--------------------------------------------------------------------------

    TResult ImMenuEventHandler::setMenuItemQueryEnabledHandle(uint32_t menuID, const ImMenuQueryHandle &handle)
    {
        return ImMenuEventManager::getInstance().setMenuItemQueryEnabledHandle(menuID, handle);
    }
    
    //--------------------------------------------------------------------------

    TResult ImMenuEventHandler::setMenuItemQueryCheckedHandle(uint32_t menuID, const ImMenuQueryHandle &handle)
    {
        return ImMenuEventManager::getInstance().setMenuItemQueryCheckedHandle(menuID, handle);
    }

    //--------------------------------------------------------------------------

    ImMenuEventManager::ImMenuEventManager()
    {
        ON_MEMBER(kEvtMenuItemClicked, ImMenuEventManager::onHandleMenuItem);
    }

    //--------------------------------------------------------------------------

    ImMenuEventManager::~ImMenuEventManager()
    {
        unregisterAllEvent();
    }

    //--------------------------------------------------------------------------

    TResult ImMenuEventManager::setMenuItemQueryEnabledHandle(uint32_t menuID, const ImMenuQueryHandle &handle)
    {
        mMenuItemQueryEnabledHandles.emplace(menuID, handle);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ImMenuEventManager::setMenuItemQueryCheckedHandle(uint32_t menuID, const ImMenuQueryHandle &handle)
    {
        mMenuItemQueryCheckedHandles.emplace(menuID, handle);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool ImMenuEventManager::onQueryEnabled(ImWidget *menuItem)
    {
        bool enabled = true;
        
        const auto itr = mMenuItemQueryEnabledHandles.find(menuItem->getID());
        if (itr != mMenuItemQueryEnabledHandles.end())
        {
            enabled = itr->second(menuItem->getID(), menuItem);
        }
        
        return enabled;
    }

    //--------------------------------------------------------------------------

    bool ImMenuEventManager::onQueryChecked(ImWidget *menuItem)
    {
        bool checked = false;

        const auto itr = mMenuItemQueryCheckedHandles.find(menuItem->getID());
        if (itr != mMenuItemQueryCheckedHandles.end())
        {
            checked = itr->second(menuItem->getID(), menuItem);
        }
        
        return checked;
    }
    
    //--------------------------------------------------------------------------

    TResult ImMenuEventManager::registerMenuEvent(uint32_t menuID, ImMenuEventHandler *handler, const ImMenuEventHandle &handle)
    {
        TResult ret = T3D_OK;

        do
        {
            const auto itr = mMenuItemHandlers.find(menuID);
            if (itr == mMenuItemHandlers.end())
            {
                // 从来没加过菜单项，则新生成一个菜单项处理对象和函数列表
                auto it = mMenuItemHandlers.emplace(menuID, ImMenuItemHandles());
                it->second.emplace(handler, handle);
            }
            else
            {
                itr->second.emplace(handler, handle);
            }
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult ImMenuEventManager::unregisterMenuEvent(uint32_t menuID, ImMenuEventHandler *handler)
    {
        TResult ret = T3D_OK;

        do
        {
            const auto itr = mMenuItemHandlers.find(menuID);
            if (itr != mMenuItemHandlers.end())
            {
                itr->second.erase(handler);
            }
            else
            {
                ret = T3D_ERR_NOT_FOUND;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImMenuEventManager::unregisterMenuEventHandler(ImMenuEventHandler *handler)
    {
        TResult ret = T3D_OK;

        do
        {
            for (auto itr = mMenuItemHandlers.begin(); itr != mMenuItemHandlers.end(); ++itr)
            {
                itr->second.erase(handler);
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ImMenuEventManager::onHandleMenuItem(EventParam *param, TINSTANCE sender)
    {
        bool ret = true;
        EventParamMenuItem *p = static_cast<EventParamMenuItem *>(param);

        uint32_t menuID = p->arg1->getID();
        const auto itr = mMenuItemHandlers.find(menuID);
        if (itr != mMenuItemHandlers.end())
        {
            if (!itr->second.empty())
            {
                for (auto it = itr->second.begin(); it != itr->second.end(); ++it)
                {
                    EventHandler *obj = nullptr;
                    EventManager::getInstance().getEventHandler(sender, obj);
                    ret = ret && it->second(menuID, (ImWidget *)obj);
                }
            }
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------
}
