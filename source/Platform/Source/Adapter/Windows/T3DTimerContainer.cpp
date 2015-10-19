

#include "T3DTimerContainer.h"
#include "T3DTimer_Windows.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(TimerContainer);

    TimerContainer::TimerContainer()
    {

    }

    TimerContainer::~TimerContainer()
    {

    }

    bool TimerContainer::insertTimer(uint32_t unTimerID, Timer_Windows *pTimer)
    {
        std::pair<TimerMap::iterator, bool> p = m_TimerMap.insert(T3DTimerPair(unTimerID, pTimer));
        return p.second;
    }

    void TimerContainer::removeTimer(uint32_t unTimerID)
    {
        TimerMap::iterator itr = m_TimerMap.find(unTimerID);
        if (itr != m_TimerMap.end())
        {
            m_TimerMap.erase(itr);
        }
    }

    Timer_Windows *TimerContainer::getTimerAdapter(uint32_t unTimerID)
    {
        Timer_Windows *pTimer = nullptr;
        TimerMap::iterator itr = m_TimerMap.find(unTimerID);

        if (itr != m_TimerMap.end())
        {
            pTimer = itr->second;
        }

        return pTimer;
    }
}
