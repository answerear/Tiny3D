

#include "T3DTimer.h"
#include "T3DSystem.h"
#include "T3DFactoryInterface.h"
#include "T3DTimerInterface.h"


namespace Tiny3D
{
    Timer::Timer()
        : m_pAdapter(nullptr)
    {
        m_pAdapter = T3D_ADAPTER_FACTORY.createTimerAdapter();
    }

    Timer::~Timer()
    {
        T3D_SAFE_DELETE(m_pAdapter);
    }

    uint32_t Timer::start(uint32_t unInterval)
    {
        if (nullptr != m_pAdapter)
        {
            m_pAdapter->stop();
            return m_pAdapter->start(unInterval);
        }

        return T3D_INVALID_TIMER_ID;
    }

    void Timer::stop()
    {
        if (nullptr != m_pAdapter)
        {
            return m_pAdapter->stop();
        }
    }

    void Timer::setObserver(TimerObserver *pObserver)
    {
        if (nullptr != m_pAdapter)
        {
            m_pAdapter->setObserver(pObserver);
        }
    }

    uint32_t Timer::getTimerID() const
    {
        if (nullptr != m_pAdapter)
        {
            return m_pAdapter->getTimerID();
        }

        return T3D_INVALID_TIMER_ID;
    }
}
