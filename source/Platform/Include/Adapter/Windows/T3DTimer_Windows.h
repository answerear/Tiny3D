/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
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

#ifndef __T3D_TIMER_WINDOWS_H__
#define __T3D_TIMER_WINDOWS_H__


#include "Adapter/T3DTimerInterface.h"
#include <windows.h>


namespace Tiny3D
{
    class Timer_Windows : public TimerInterface
    {
        T3D_DISABLE_COPY(Timer_Windows);

    public:
        Timer_Windows();
        virtual ~Timer_Windows();

    protected:
        virtual uint32_t start(uint32_t unInterval);
        virtual void stop();
        virtual void setObserver(TimerObserver *pObserver);
        virtual uint32_t getTimerID() const;

    private:
        static void CALLBACK onTimer(HWND hWnd, UINT uMsg, UINT_PTR uIDEvent, DWORD dwTime);

    private:
        TimerObserver   *m_pObserver;
        uint32_t            m_unTimerID;
    };
}


#endif  /*__T3D_TIMER_WINDOWS_H__*/
