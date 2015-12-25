

#ifndef __T3D_RENDERER_H__
#define __T3D_RENDERER_H__


#include "T3DPrerequisites.h"
#include "Misc/T3DCommon.h"
#include "Listener/T3DFrameListener.h"
#include "Render/T3DRenderTarget.h"


namespace Tiny3D
{
    class VertexList;
    class IndexList;

    class T3D_ENGINE_API Renderer
    {
    public:
        static const char * const DIRECT3D9;
        static const char * const DIRECT3D11;
        static const char * const OPENGL3PLUS;
        static const char * const OPENGLES2;
        static const char * const OPENGLES3;

        enum Capability
        {

        };

        enum TransformState
        {
            E_TS_VIEW,
            E_TS_WORLD,
            E_TS_PROJECTION,
            E_TS_TEXTURE_0,
            E_TS_MAX
        };

        enum PrimitiveType
        {
            E_PT_POINT,
            E_PT_LINE,
            E_PT_TRIANGLE_LIST,
            E_PT_TRIANGLE_STRIP,
            E_PT_TRIANGLE_FAN,
        };

        Renderer();
        virtual ~Renderer();

        virtual bool initialize() = 0;
        virtual void uninitialize() = 0;

        virtual String getName() const = 0;

        virtual RenderWindow *createRenderWindow(
            const RenderWindowCreateParam &rkCreateParam,
            const RenderWindowCreateParamEx &rkCreateParamEx) = 0;

        virtual void startRendering() = 0;

        virtual bool attachRenderTarget(RenderTarget *target);
        virtual void detachRenderTarget(const String &name);
        RenderTargetPtr getRenderTarget(const String &name);

        void addFrameListener(FrameListener *listener);
        void removeFrameListener(FrameListener *listener);

        virtual bool beginRender() = 0;
        virtual bool endRender() = 0;

        virtual bool queryCapability(Capability cap) = 0;
        virtual void enableCapability(Capability cap, bool enabled = true) = 0;

        virtual void setTransform(TransformState state, const Matrix4 &mat) = 0;
        
        void setViewTransform(const Matrix4 &mat);
        void setWorldTransform(const Matrix4 &mat);
        void setProjectionTransform(const Matrix4 &mat);
        void setTextureTransform(int32_t textureIdx, const Matrix4 &mat);

        virtual void makeProjectionMatrix(const Radian &rkFovY, Real aspect, Real nearDist, Real farDist, bool ortho, Matrix4 &mat) = 0;

        virtual const Matrix4 &getTransform(TransformState state) const = 0;

        virtual void setMaterial() = 0;

        virtual void drawVertexList(PrimitiveType primitiveType, 
            const VertexList &vertices, uint32_t startIdx, 
            uint32_t primitiveCount) = 0;

        virtual void drawIndexList(PrimitiveType primitiveType, 
            const VertexList &vertices, const IndexList *indicies, 
            uint32_t startIdx, uint32_t pritimitiveCount) = 0;

    protected:
        bool fireFrameStarted();
        bool fireFrameEnded();

        bool fireFrameStarted(const FrameEvent &evt);
        bool fireFrameEnded(const FrameEvent &evt);

    protected:
        typedef std::map<String, RenderTargetPtr>   RenderTargetList;
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


#include "T3DRenderer.inl"


#endif    /*__T3D_RENDERER_H__*/
