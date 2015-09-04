

#ifndef __T3D_FACTORY_WINDOWS_H__
#define __T3D_FACTORY_WINDOWS_H__


#include "T3DFactoryInterface.h"


namespace Tiny3D
{
	class TimerContainer;

	class Factory_Windows : public FactoryInterface
	{
		T3D_DISABLE_COPY(Factory_Windows);

	public:
		Factory_Windows();
		virtual ~Factory_Windows();

	protected:
		virtual ConsoleInterface *createConsoleAdapter();
		virtual TimerInterface *createTimerAdapter();
		virtual DirInterface *createDirAdapter();
		virtual DeviceInfoInterface *createDeviceInfoAdapter();

		virtual EPlatform getPlatform();

	protected:
		TimerContainer	*m_pTimerContainer;
	};
}


#endif	/*__T3D_FACTORY_WINDOWS_H__*/
