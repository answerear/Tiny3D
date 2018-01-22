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


#include "T3DEventManager.h"
#include "T3DEventMacro.h"
#include "T3DEventInstance.h"

namespace Tiny3D
{
    T3D_INIT_SINGLETON(EventManager);

    const TINSTANCE EventManager::INVALID_INSTANCE = nullptr;
    const TINSTANCE EventManager::BROADCAST_INSTANCE = (const TINSTANCE)-1;
    const TINSTANCE EventManager::MULTICAST_INSTANCE = (const TINSTANCE)1;

    EventManager::EventManager(uint32_t maxEvents, int32_t maxHandlingDuration,
        int32_t maxCallStacks)
    {

    }

    EventManager::~EventManager()
    {

    }

    bool EventManager::sendEvent(uint32_t evid, EventParam *param, 
        TINSTANCE receiver, TINSTANCE sender)
    {
        return true;
    }

    bool EventManager::postEvent(uint32_t evid, EventParam *param, 
        TINSTANCE receiver, TINSTANCE sender)
    {
        return true;
    }

    bool EventManager::getEventHandler(TINSTANCE instance, 
        EventHandler *&handler)
    {
        return true;
    }

    bool EventManager::isValidHandler(EventHandler *handler)
    {
        return false;
    }

    bool EventManager::dispatchEvent()
    {
        return false;
    }

    void EventManager::pauseDispatching()
    {

    }

    bool EventManager::resumeDispatching(bool dispatchImmdiately)
    {
        return false;
    }

    TINSTANCE EventManager::registerHandler(EventHandler *handler)
    {
        TINSTANCE instance = T3D_INVALID_INSTANCE;

        return instance;
    }

    bool EventManager::unregisterHandler(TINSTANCE instance)
    {
        return false;
    }

    bool EventManager::registerEvent(uint32_t evid, TINSTANCE instance)
    {
        return false;
    }

    bool EventManager::unregisterEvent(uint32_t evid, TINSTANCE instance)
    {
        return false;
    }

    bool EventManager::peekEvent(EventItem &Item, bool bRemovable /* = true */)
    {
        return false;
    }

    void EventManager::clearEventQueue()
    {

    }
}
