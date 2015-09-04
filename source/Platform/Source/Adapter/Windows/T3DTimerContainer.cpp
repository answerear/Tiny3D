

#include "T3DTimerContainer.h"
#include "T3DTimer_Windows.h"


T3D_INIT_SINGLETON(Tiny3D::TimerContainer);


namespace Tiny3D
{
	TimerContainer::TimerContainer()
	{

	}

	TimerContainer::~TimerContainer()
	{

	}

	bool TimerContainer::insertTimer(uint32_t unTimerID, Timer_Windows *pTimer)
	{
		std::pair<T3DTimerMap::iterator, bool> p = m_TimerMap.insert(T3DTimerPair(unTimerID, pTimer));
		return p.second;
	}

	void TimerContainer::removeTimer(uint32_t unTimerID)
	{
		T3DTimerMap::iterator itr = m_TimerMap.find(unTimerID);
		if (itr != m_TimerMap.end())
		{
			m_TimerMap.erase(itr);
		}
	}

	Timer_Windows *TimerContainer::getTimerAdapter(uint32_t unTimerID)
	{
		Timer_Windows *pTimer = nullptr;
		T3DTimerMap::iterator itr = m_TimerMap.find(unTimerID);

		if (itr != m_TimerMap.end())
		{
			pTimer = itr->second;
		}

		return pTimer;
	}
}
