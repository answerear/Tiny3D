

#ifndef __T3D_TIMER_OBSERVER_H__
#define __T3D_TIMER_OBSERVER_H__


#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DPlatformMacro.h"


namespace Tiny3D
{
	/**
	 * @class ITimerObserver
	 * @brief 定时器观察者接口类.
	 * @note 继承该类实现接口以对定时器事件监听.
	 */
	class T3D_PLATFORM_API TimerObserver
	{
		T3D_DECLARE_INTERFACE(TimerObserver);

	public:
		/**
		 * @brief 定时器时间到达回调.
		 * @param [in] unTimerID : 到达时间的定时器ID
		 * @return void
		 */
		virtual void onTimer(uint32_t unTimerID) = 0;
	};
}


#endif	/*__T3D_TIMER_OBSERVER_H__*/
