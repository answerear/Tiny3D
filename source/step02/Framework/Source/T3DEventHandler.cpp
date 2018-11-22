/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "T3DEventHandler.h"
#include "T3DEventMacro.h"
#include "T3DEventErrorDef.h"
#include "T3DEventManager.h"


namespace Tiny3D
{
    EventHandler::EventHandler(bool canAutoRegister /* = true */)
        : mInstance(T3D_INVALID_INSTANCE)
    {
        if (canAutoRegister)
        {
            mInstance = T3D_EVENT_MGR.registerHandler(this);
        }
    }

    EventHandler::~EventHandler()
    {
        unregisterAllEvent();
        T3D_EVENT_MGR.unregisterHandler(mInstance);
    }

    int32_t EventHandler::sendEvent(uint32_t evid, EventParam *param,
        TINSTANCE receiver)
    {
        return T3D_EVENT_MGR.sendEvent(evid, param, receiver, mInstance);
    }

    int32_t EventHandler::postEvent(uint32_t evid, EventParam *param,
        TINSTANCE receiver)
    {
        return T3D_EVENT_MGR.postEvent(evid, param, receiver, mInstance);
    }

    int32_t EventHandler::sendEvent(uint32_t evid, EventParam *param)
    {
        return T3D_EVENT_MGR.sendEvent(evid, param, 
            T3D_MULTICAST_INSTANCE, mInstance);
    }

    int32_t EventHandler::postEvent(uint32_t evid, EventParam *param)
    {
        return T3D_EVENT_MGR.postEvent(evid, param, 
            T3D_MULTICAST_INSTANCE, mInstance);
    }

    TINSTANCE EventHandler::registerHandler()
    {
        if (mInstance == T3D_INVALID_INSTANCE)
        {
            mInstance = T3D_EVENT_MGR.registerHandler(this);
        }

        return mInstance;
    }

    int32_t EventHandler::unregisterHandler()
    {
        bool ret = T3D_EVENT_MGR.unregisterHandler(mInstance);

        if (ret)
        {
            mInstance = T3D_INVALID_INSTANCE;
        }

        return ret;
    }

    int32_t EventHandler::setupEventFilter()
    {
        return T3D_ERR_OK;
    }

    int32_t EventHandler::registerEvent(uint32_t evid)
    {
        int32_t ret = T3D_EVENT_MGR.registerEvent(evid, mInstance);

        if (T3D_ERR_OK == ret)
        {
            mEventList.push_back(evid);
        }

        return ret;
    }

    int32_t EventHandler::unregisterEvent(uint32_t evid)
    {
        int32_t ret = T3D_EVENT_MGR.unregisterEvent(evid, mInstance);

        if (T3D_ERR_OK == ret)
        {
            auto itr = mEventList.begin();

            while (itr != mEventList.end())
            {
                if (evid == *itr)
                {
                    mEventList.erase(itr);
                    break;
                }

                ++itr;
            }
        }

        return ret;
    }

    void EventHandler::unregisterAllEvent()
    {
        auto itr = mEventList.begin();

        while (itr != mEventList.end())
        {
            T3D_EVENT_MGR.unregisterEvent(*itr, mInstance);
            ++itr;
        }

        mEventList.clear();
    }

    int32_t EventHandler::processEvent(uint32_t evid, EventParam *param, 
        TINSTANCE sender)
    {
        // 都跑到基类了，还没有人处理过这个事件，那只能不处理了
        return T3D_ERR_FWK_NONE_HANDLER;
    }
}
