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


#include "T3DTimerManager.h"
#include "Adapter/Common/T3DTimerService.h"
#include "Adapter/T3DFactoryInterface.h"
#include "T3DPlatform.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const ID TimerManager::INVALID_TIMER_ID = ITimerService::INVALID_TIMER_ID;

    //--------------------------------------------------------------------------


    TimerManager::TimerManager()
        : mTimerService(nullptr)
    {
        mTimerService = T3D_PLATFORM_FACTORY.createPlatformTimerService();
    }

    //--------------------------------------------------------------------------

    TimerManager::~TimerManager()
    {
        T3D_SAFE_DELETE(mTimerService);
    }

    //--------------------------------------------------------------------------

    ID TimerManager::startTimer(uint32_t interval, bool repeat, const TimerCallback &callback)
    {
        if (mTimerService != nullptr)
        {
            return mTimerService->startTimer(interval, repeat, callback);
        }

        return INVALID_TIMER_ID;
    }

    //--------------------------------------------------------------------------

    TResult TimerManager::stopTimer(ID timerID)
    {
        if (mTimerService != nullptr)
        {
            return mTimerService->stopTimer(timerID);
        }

        return T3D_ERR_INVALID_POINTER;
    }

    //--------------------------------------------------------------------------

    TResult TimerManager::init()
    {
        if (mTimerService != nullptr)
        {
            return mTimerService->init();
        }

        return T3D_ERR_INVALID_POINTER;
    }

    //--------------------------------------------------------------------------

    TResult TimerManager::pollEvents()
    {
        if (mTimerService != nullptr)
        {
            return mTimerService->pollEvents();
        }

        return T3D_ERR_INVALID_POINTER;
    }
}

