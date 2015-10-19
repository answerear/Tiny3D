

#include "T3DTimer_Windows.h"
#include "Time/T3DTimerObserver.h"
#include "T3DTimerContainer.h"


namespace Tiny3D
{
    Timer_Windows::Timer_Windows()
        : m_pObserver(nullptr)
        , m_unTimerID(T3D_INVALID_TIMER_ID)
    {

    }

    Timer_Windows::~Timer_Windows()
    {
        stop();
    }

    uint32_t Timer_Windows::start(uint32_t unInterval)
    {
        m_unTimerID = ::SetTimer(nullptr, 1000, unInterval, Timer_Windows::onTimer);

        if (TimerContainer::getInstancePtr() != nullptr)
        {
            TimerContainer::getInstancePtr()->insertTimer(m_unTimerID, this);
        }

        return m_unTimerID;
    }

    void Timer_Windows::stop()
    {
        if (T3D_INVALID_TIMER_ID != m_unTimerID)
        {
            if (TimerContainer::getInstancePtr() != nullptr)
            {
                TimerContainer::getInstancePtr()->removeTimer(m_unTimerID);
            }

            ::KillTimer(nullptr, m_unTimerID);
            m_unTimerID = T3D_INVALID_TIMER_ID;
        }
    }

    void Timer_Windows::setObserver(TimerObserver *pObserver)
    {
        m_pObserver = pObserver;
    }

    uint32_t Timer_Windows::getTimerID() const
    {
        return m_unTimerID;
    }

    void Timer_Windows::onTimer(HWND hWnd, UINT uMsg, UINT_PTR uIDEvent, DWORD dwTime)
    {
        if (TimerContainer::getInstancePtr() != nullptr)
        {
            Timer_Windows *pTimer = TimerContainer::getInstancePtr()->getTimerAdapter(uIDEvent);
            if (pTimer != nullptr)
            {
                if (pTimer->m_pObserver != nullptr)
                {
                    pTimer->m_pObserver->onTimer(pTimer->m_unTimerID);
                }
            }
        }
    }
}
