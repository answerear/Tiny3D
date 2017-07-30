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

#include "Time/T3DTimer.h"
#include "T3DSystem.h"
#include "Adapter/T3DFactoryInterface.h"
#include "Adapter/T3DTimerInterface.h"


namespace Tiny3D
{
    Timer::Timer()
        : m_pAdapter(nullptr)
    {
        m_pAdapter = T3D_ADAPTER_FACTORY.createTimerAdapter();
    }

    Timer::~Timer()
    {
        T3D_SAFE_DELETE(m_pAdapter);
    }

    uint32_t Timer::start(uint32_t unInterval)
    {
        if (nullptr != m_pAdapter)
        {
            m_pAdapter->stop();
            return m_pAdapter->start(unInterval);
        }

        return T3D_INVALID_TIMER_ID;
    }

    void Timer::stop()
    {
        if (nullptr != m_pAdapter)
        {
            return m_pAdapter->stop();
        }
    }

    void Timer::setObserver(TimerObserver *pObserver)
    {
        if (nullptr != m_pAdapter)
        {
            m_pAdapter->setObserver(pObserver);
        }
    }

    uint32_t Timer::getTimerID() const
    {
        if (nullptr != m_pAdapter)
        {
            return m_pAdapter->getTimerID();
        }

        return T3D_INVALID_TIMER_ID;
    }
}
