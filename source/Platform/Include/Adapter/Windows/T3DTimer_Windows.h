
#ifndef __T3D_TIMER_WINDOWS_H__
#define __T3D_TIMER_WINDOWS_H__


#include "T3DTimerInterface.h"
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
