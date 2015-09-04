

#ifndef __T3D_TIMER_INTERFACE_H__
#define __T3D_TIMER_INTERFACE_H__


#include "T3DType.h"
#include "T3DMacro.h"


namespace Tiny3D
{
	class TimerObserver;

	const uint32_t T3D_INVALID_TIMER_ID = 0;		/**< 无效定时器ID */

	/**
	 * @class ITimerAdapter
	 * @brief 操作系统平台相关的定时器接口类.
	 * @note 相关操作系统平台需要继承该接口类实现具体平台相关功能.
	 */
	class TimerInterface
	{
		T3D_DECLARE_INTERFACE(TimerInterface);

	public:
		/**
		 * @brief 启动定时器，定时器是循环定时.
		 * @param [in] unInterval : 定时器时间间隔毫秒数
		 * @return 返回定时器对应ID.  
		 */
		virtual uint32_t start(uint32_t unInterval) = 0;

		/**
		 * @brief 关闭定时器.
		 * @return void  
		 */
		virtual void stop() = 0;

		/**
		 * @brief 设置定时器回调观察者对象.
		 * @param [in] pObserver : 观察者对象
		 * @return void
		 * @see class ITimerObserver
		 */
		virtual void setObserver(TimerObserver *pObserver) = 0;

		/**
		 * @brief 获取定时器对象对应的定时器ID 
		 * @note 当前定时器正在计时，则返回合法的对应的定时器ID，否则返回无效定时器ID.
		 * @return 返回定时器ID  
		 */
		virtual uint32_t getTimerID() const = 0;
	};
}



#endif	/*__T3D_TIMER_ADPATER_INTERFACE_H__*/
