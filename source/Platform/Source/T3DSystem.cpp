

#include "T3DSystem.h"
#include "T3DFactoryInterface.h"
#include "T3DDir.h"
#include "T3DTextCodec.h"
#include "T3DConsole.h"
#include "T3DDeviceInfo.h"
#include "T3DRunLoop.h"


T3D_INIT_SINGLETON(Tiny3D::T3DSystem);


namespace Tiny3D
{
	T3DSystem::T3DSystem()
		: m_pAdapterFactory(nullptr)
		, m_pTextCodec(nullptr)
		, m_pConsole(nullptr)
		, m_pDeviceInfo(nullptr)
		, m_pMainRunLoop(nullptr)
	{
		m_pAdapterFactory = createAdapterFactory();

		Dir::getNativeSeparator();

		m_pTextCodec = new TextCodec("GB2312.dat");
		m_pConsole = new Console(m_pAdapterFactory);
		m_pDeviceInfo = new DeviceInfo(m_pAdapterFactory);
		m_pMainRunLoop = new RunLoop();
	}

	T3DSystem::~T3DSystem()
	{
		T3D_SAFE_DELETE(m_pMainRunLoop);
		T3D_SAFE_DELETE(m_pConsole);
		T3D_SAFE_DELETE(m_pDeviceInfo);
		T3D_SAFE_DELETE(m_pAdapterFactory);
	}

	void T3DSystem::process()
	{
		if (m_pMainRunLoop != nullptr)
		{
			m_pMainRunLoop->execute();
		}
	}

	RunLoop &T3DSystem::getMainRunLoop()
	{
		return *m_pMainRunLoop;
	}
}
