
#include "Time/T3DRunLoop.h"
#include "Time/T3DDateTime.h"
#include "Time/T3DRunLoopObserver.h"


namespace Tiny3D
{
    uint32_t RunLoop::m_unLoopID = 0;

    RunLoop::RunLoop()
    {

    }

    RunLoop::~RunLoop()
    {

    }

    uint32_t RunLoop::start(uint32_t unInterval, bool bRepeat, RunLoopObserver *pObserver)
    {
        if (pObserver == nullptr)
            return T3D_INVALID_LOOP_ID;

        uint32_t unLoopID = m_unLoopID + 1;
        uint64_t unTimestamp = DateTime::currentMSecsSinceEpoch();
        RunLoopInfo info = {unTimestamp, unInterval, pObserver, bRepeat};
        std::pair<T3DRunLoopInfosItr, bool> r = m_infos.insert(T3DRunLoopValue(unLoopID, info));

        if (r.second)
        {
            m_unLoopID++;
        }

        return unLoopID;
    }

    uint32_t RunLoop::start(RunLoopObserver *pObserver)
    {
        if (pObserver == nullptr)
            return T3D_INVALID_LOOP_ID;

        uint32_t unLoopID = m_unLoopID + 1;
        uint64_t unTimestamp = DateTime::currentMSecsSinceEpoch();
        RunLoopInfo info = {unTimestamp, 0, pObserver, true};
        std::pair<T3DRunLoopInfosItr, bool> r = m_infos.insert(T3DRunLoopValue(unLoopID, info));

        if (r.second)
        {
            m_unLoopID++;
        }

        return unLoopID;
    }

    bool RunLoop::stop(uint32_t unLoopID)
    {
        bool bRet = false;

        T3DRunLoopInfosItr itr = m_infos.find(unLoopID);

        if (itr != m_infos.end())
        {
            m_infos.erase(itr);
        }

        return bRet;
    }

    void RunLoop::execute()
    {
        T3DRunLoopInfosItr itr = m_infos.begin();
        uint64_t ullTimestamp = DateTime::currentMSecsSinceEpoch();

        while (itr != m_infos.end())
        {
            uint32_t unLoopID = itr->first;
            RunLoopInfo &info = itr->second;
            uint64_t dt = ullTimestamp - info.ullLastTimestamp;

            if (dt >= info.ullInterval)
            {
                /// 回调
                info.pObserver->onExecute(unLoopID, dt);
                info.ullLastTimestamp = ullTimestamp;
            }

            if (!info.bRepeat)
            {
                /// 不循环，那么就删掉咯
                m_infos.erase(itr++);
            }
            else
            {
                ++itr;
            }
        }
    }
}
