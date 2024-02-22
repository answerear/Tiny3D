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

#include "T3DEventHandler.h"
#include "T3DEventMacro.h"
#include "T3DEventErrorDef.h"
#include "T3DEventManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    EventHandler::EventHandler(bool canAutoRegister /* = true */)
        : mInstance(T3D_INVALID_INSTANCE)
    {
        if (canAutoRegister)
        {
            mInstance = T3D_EVENT_MGR.registerHandler(this);
        }
    }

    //--------------------------------------------------------------------------

    EventHandler::~EventHandler()
    {
        unregisterAllEvent();
        T3D_EVENT_MGR.unregisterHandler(mInstance);
    }

    //--------------------------------------------------------------------------

    TResult EventHandler::sendEvent(EventID evid, EventParam *param,
        TINSTANCE receiver)
    {
        return T3D_EVENT_MGR.sendEvent(evid, param, receiver, mInstance);
    }

    //--------------------------------------------------------------------------

    TResult EventHandler::postEvent(EventID evid, EventParam *param,
        TINSTANCE receiver)
    {
        return T3D_EVENT_MGR.postEvent(evid, param, receiver, mInstance);
    }

    //--------------------------------------------------------------------------

    TResult EventHandler::sendEvent(EventID evid, EventParam *param)
    {
        return T3D_EVENT_MGR.sendEvent(evid, param, 
            T3D_MULTICAST_INSTANCE, mInstance);
    }

    //--------------------------------------------------------------------------

    TResult EventHandler::postEvent(EventID evid, EventParam *param)
    {
        return T3D_EVENT_MGR.postEvent(evid, param, 
            T3D_MULTICAST_INSTANCE, mInstance);
    }

    //--------------------------------------------------------------------------

    TINSTANCE EventHandler::registerHandler()
    {
        if (mInstance == T3D_INVALID_INSTANCE)
        {
            mInstance = T3D_EVENT_MGR.registerHandler(this);
        }

        return mInstance;
    }

    //--------------------------------------------------------------------------

    TResult EventHandler::unregisterHandler()
    {
        TResult ret = T3D_EVENT_MGR.unregisterHandler(mInstance);

        if (T3D_FAILED(ret))
        {
            mInstance = T3D_INVALID_INSTANCE;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult EventHandler::setupEventFilter()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult EventHandler::registerEvent(EventID evid)
    {
        TResult ret = T3D_EVENT_MGR.registerEvent(evid, mInstance);

        if (T3D_OK == ret)
        {
            mEventList.push_back(evid);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult EventHandler::unregisterEvent(EventID evid)
    {
        TResult ret = T3D_EVENT_MGR.unregisterEvent(evid, mInstance);

        if (T3D_OK == ret)
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

    //--------------------------------------------------------------------------

    void EventHandler::unregisterAllEvent()
    {
        while (!mEventList.empty())
        {
            auto eventID = mEventList.back();
            T3D_EVENT_MGR.unregisterEvent(eventID, mInstance);
            mEventList.pop_back();
        }

        mEventList.clear();
    }

    //--------------------------------------------------------------------------

    TResult EventHandler::processEvent(EventID evid, EventParam *param,
        TINSTANCE sender)
    {
        // 都跑到基类了，还没有人处理过这个事件，那只能不处理了
        return T3D_ERR_FWK_NONE_HANDLER;
    }
}
