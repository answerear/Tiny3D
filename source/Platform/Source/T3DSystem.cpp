

#include "T3DSystem.h"
#include "Adapter/T3DFactoryInterface.h"
#include "IO/T3DDir.h"
#include "Codec/T3DTextCodec.h"
#include "Console/T3DConsole.h"
#include "Device/T3DDeviceInfo.h"
#include "Time/T3DRunLoop.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(System);

    System::System()
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

    System::~System()
    {
        T3D_SAFE_DELETE(m_pMainRunLoop);
        T3D_SAFE_DELETE(m_pConsole);
        T3D_SAFE_DELETE(m_pDeviceInfo);
        T3D_SAFE_DELETE(m_pAdapterFactory);
    }

    void System::process()
    {
        if (m_pMainRunLoop != nullptr)
        {
            m_pMainRunLoop->execute();
        }
    }

    RunLoop &System::getMainRunLoop()
    {
        return *m_pMainRunLoop;
    }
}
