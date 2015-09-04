
#ifndef __T3D_TIMER_H__
#define __T3D_TIMER_H__


#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DPlatformMacro.h"


namespace Tiny3D
{
	class TimerObserver;
	class TimerInterface;

	/**
	 * @class VTimer
	 * @brief 定时器类.
	 * @note 该类是操作系统平台无关定时器，对平台适配层的进一步封装.
	 */
	class T3D_PLATFORM_API Timer
	{
		T3D_DISABLE_COPY(Timer);

	public:
		/**
		 * @brief Constructor for VTimer.
		 */
		Timer();

		/**
		 * @brief Destructor for VTimer.
		 */
		~Timer();

		/**
		 * @brief 启动定时器.
		 * @param [in] unInterval : 定时毫秒数
		 * @return 定时器ID  
		 */
		uint32_t start(uint32_t unInterval);

		/**
		 * @brief 关闭定时器.
		 * @return void  
		 */
		void stop();

		/**
		 * @brief 设置定时器观察者对象.
		 * @param [in] pObserver : 观察者对象
		 * @return void  
		 */
		void setObserver(TimerObserver *pObserver);

		/**
		 * @brief 获取定时器ID.
		 * @return 返回定时器ID  
		 */
		uint32_t getTimerID() const;

	protected:
		TimerInterface *m_pAdapter;
	};
}

#endif	/*__T3D_TIMER_H__*/
