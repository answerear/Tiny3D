

#ifndef __T3D_TIMER_CONTAINER_H__
#define __T3D_TIMER_CONTAINER_H__


#include "T3DSingleton.h"
#include <map>


namespace Tiny3D
{
	class Timer_Windows;

	class TimerContainer : public Singleton<TimerContainer>
	{
		T3D_DISABLE_COPY(TimerContainer);

	public:
		TimerContainer();
		virtual ~TimerContainer();

		bool insertTimer(uint32_t unTimerID, Timer_Windows *pTimer);
		void removeTimer(uint32_t unTimerID);
		Timer_Windows *getTimerAdapter(uint32_t unTimerID);

	protected:
		typedef std::pair<uint32_t, Timer_Windows*>	T3DTimerPair;
		typedef std::map<uint32_t, Timer_Windows*>	TimerMap;

		TimerMap	m_TimerMap;
	};
}


#endif	/*__T3D_TIMER_CONTAINER_H__*/
