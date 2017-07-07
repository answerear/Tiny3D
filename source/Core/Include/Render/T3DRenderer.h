

#ifndef __T3D_RENDERER_H__
#define __T3D_RENDERER_H__


#include "Misc/T3DObject.h"
#include "Misc/T3DCommon.h"
#include "T3DTypedef.h"
#include "Listener/T3DFrameListener.h"
#include "Render/T3DRenderTarget.h"


namespace Tiny3D
{
    class VertexList;
    class IndexList;

    class T3D_ENGINE_API Renderer : public Object
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
            E_PT_POINT_LIST,        // 单独点列表图元
            E_PT_LINE_LIST,         // 单独线列表图元
            E_PT_LINE_STRIP,        // 线带图元
            E_PT_TRIANGLE_LIST,     // 三角列表图元
            E_PT_TRIANGLE_STRIP,    // 三角带图元
            E_PT_TRIANGLE_FAN,      // 三角扇形图元
        };

        enum CullingMode
        {
            E_CULL_NONE = 1,        // 不做消隐面剔除
            E_CULL_CLOCKWISE,       // 按照顶点顺时针顺序的消隐面剔除
            E_CULL_ANTICLOCKWISE,   // 按照顶点逆时针顺序的消隐面剔除
        };

        enum RenderMode
        {
            E_RM_POINT,         // 仅渲染点
            E_RM_WIREFRAME,     // 渲染线框
            E_RM_SOLID,         // 渲染实体，带纹理、材质的
        };

        Renderer();
        virtual ~Renderer();

        virtual bool initialize() = 0;
        virtual void uninitialize() = 0;

        virtual String getName() const = 0;

        virtual RenderWindow *createRenderWindow(
            const RenderWindowCreateParam &rkCreateParam,
            const RenderWindowCreateParamEx &rkCreateParamEx) = 0;

        virtual bool renderOneFrame();

        virtual bool attachRenderTarget(RenderTarget *target);
        virtual void detachRenderTarget(const String &name);
        RenderTargetPtr getRenderTarget(const String &name);

        void addFrameListener(FrameListener *listener);
        void removeFrameListener(FrameListener *listener);

        virtual bool beginRender(const Color4 &bkgndColor) = 0;
        virtual bool endRender() = 0;

        virtual bool queryCapability(Capability cap) = 0;
        virtual void enableCapability(Capability cap, bool enabled = true) = 0;

        virtual void setTransform(TransformState state, const Matrix4 &mat) = 0;
        
        void setViewTransform(const Matrix4 &mat);
        void setWorldTransform(const Matrix4 &mat);
        void setProjectionTransform(const Matrix4 &mat);
        void setTextureTransform(int32_t textureIdx, const Matrix4 &mat);

        virtual void makeProjectionMatrix(const Radian &rkFovY, Real aspect, Real nearDist, Real farDist, bool ortho, Matrix4 &mat) = 0;

        virtual void updateFrustum(const Matrix4 &m, Plane *plane, size_t planeCount) = 0;

        virtual const Matrix4 &getTransform(TransformState state) const = 0;

        virtual void setCullingMode(CullingMode mode) = 0;
        virtual CullingMode getCullingMode() const;

        virtual void setRenderMode(RenderMode mode) = 0;
        virtual RenderMode getRenderMode() const;

        virtual void setLightEnabled(bool enable) = 0;
        virtual void setAmbientLight(const Color4 &ambient) = 0;

        virtual void addDynamicLight(size_t index, const SGLightPtr light) = 0;
        virtual void removeDynamicLight(size_t index) = 0;
        virtual void removeAllDynamicLights() = 0;

        virtual void setMaterial(const MaterialPtr &material) = 0;

        virtual void setViewport(const ViewportPtr &viewport) = 0;
        virtual ViewportPtr getViewport();

        virtual void drawVertexList(PrimitiveType primitiveType, 
            const VertexDataPtr &vertices, uint32_t startIdx, 
            uint32_t primitiveCount) = 0;

        virtual void drawIndexList(PrimitiveType primitiveType, 
            const VertexDataPtr &vertices, const IndexDataPtr &indicies, 
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

        ViewportPtr         mViewport;

        uint64_t            mLastStartTime;
        uint64_t            mLastEndTime;

        CullingMode         mCullingMode;
        RenderMode          mRenderMode;

        Material            *mMaterial;
    };
}


#include "T3DRenderer.inl"


#endif    /*__T3D_RENDERER_H__*/
