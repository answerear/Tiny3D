
#include "T3DFactory_Windows.h"
#include "T3DConsole_Windows.h"
#include "T3DDir_Windows.h"
#include "T3DDeviceInfo_Windows.h"
#include "T3DTimer_Windows.h"
#include "T3DTimerContainer.h"


namespace Tiny3D
{
	FactoryInterface *createAdapterFactory()
	{
		return new Factory_Windows();
	}

	Factory_Windows::Factory_Windows()
		: m_pTimerContainer(new TimerContainer())
	{

	}

	Factory_Windows::~Factory_Windows()
	{
		T3D_SAFE_DELETE(m_pTimerContainer);
	}

	ConsoleInterface *Factory_Windows::createConsoleAdapter()
	{
		return new Console_Windows();
	}

	TimerInterface *Factory_Windows::createTimerAdapter()
	{
		return new Timer_Windows();
	}

	DirInterface *Factory_Windows::createDirAdapter()
	{
		return new Dir_Windows();
	}

	DeviceInfoInterface *Factory_Windows::createDeviceInfoAdapter()
	{
		return new DeviceInfo_Windows();
	}

	EPlatform Factory_Windows::getPlatform()
	{
		return E_PLATFORM_WIN32;
	}
}