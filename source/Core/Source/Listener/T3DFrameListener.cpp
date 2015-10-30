

#include "Listener/T3DFrameListener.h"


namespace Tiny3D
{
    FrameListener::FrameListener()
    {

    }

    FrameListener::~FrameListener()
    {

    }

    bool FrameListener::onFrameStarted(const FrameEvent &evt)
    {
        return true;
    }

    bool FrameListener::onFrameEnded(const FrameEvent &evt)
    {
        return true;
    }
}

