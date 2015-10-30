

#ifndef __T3D_RENDERER_H__
#define __T3D_RENDERER_H__


#include "T3DPrerequisites.h"
#include "Misc/T3DCommon.h"
#include "Listener/T3DFrameListener.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Renderer
    {
    public:
        static const char * const DIRECT3D9;
        static const char * const DIRECT3D11;
        static const char * const OPENGL3PLUS;
        static const char * const OPENGLES2;
        static const char * const OPENGLES3;

        Renderer();
        virtual ~Renderer();

        virtual String getName() const = 0;

        virtual RenderWindow *createRenderWindow(
            const RenderWindowCreateParam &rkCreateParam,
            const RenderWindowCreateParamEx &rkCreateParamEx) = 0;

        virtual void startRendering() = 0;

        virtual bool attachRenderTarget(RenderTarget *target);
        virtual void detachRenderTarget(const String &name);
        RenderTarget *getRenderTarget(const String &name);

        void addFrameListener(FrameListener *listener);
        void removeFrameListener(FrameListener *listener);

    protected:
        bool fireFrameStarted();
        bool fireFrameEnded();

        bool fireFrameStarted(const FrameEvent &evt);
        bool fireFrameEnded(const FrameEvent &evt);

    protected:
        typedef std::map<String, RenderTarget*>     RenderTargetList;
        typedef RenderTargetList::iterator          RenderTargetListItr;
        typedef RenderTargetList::const_iterator    RenderTargetListConstItr;
        typedef RenderTargetList::value_type        RenderTargetListValue;

        typedef std::set<FrameListener*>            FrameListenerList;
        typedef FrameListenerList::iterator         FrameListenerListItr;
        typedef FrameListenerList::const_iterator   FrameListenerListConstItr;

        RenderTargetList    mRenderTargets;
        FrameListenerList   mFrameListeners;

        uint64_t            mLastStartTime;
        uint64_t            mLastEndTime;
    };
}


#endif    /*__T3D_RENDERER_H__*/
