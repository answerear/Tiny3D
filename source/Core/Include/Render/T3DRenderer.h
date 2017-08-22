/***************************************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************************************/

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
            E_PT_POINT_LIST,        // �������б�ͼԪ
            E_PT_LINE_LIST,         // �������б�ͼԪ
            E_PT_LINE_STRIP,        // �ߴ�ͼԪ
            E_PT_TRIANGLE_LIST,     // �����б�ͼԪ
            E_PT_TRIANGLE_STRIP,    // ���Ǵ�ͼԪ
            E_PT_TRIANGLE_FAN,      // ��������ͼԪ
        };

        enum CullingMode
        {
            E_CULL_NONE = 1,        // �����������޳�
            E_CULL_CLOCKWISE,       // ���ն���˳ʱ��˳����������޳�
            E_CULL_ANTICLOCKWISE,   // ���ն�����ʱ��˳����������޳�
        };

        enum RenderMode
        {
            E_RM_POINT,         // ����Ⱦ��
            E_RM_WIREFRAME,     // ��Ⱦ�߿�
            E_RM_SOLID,         // ��Ⱦʵ�壬���������ʵ�
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

        virtual void makeViewportMatrix(ViewportPtr viewport, Matrix4 &mat) = 0;

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
