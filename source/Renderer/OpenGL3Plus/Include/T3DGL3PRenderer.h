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

#ifndef __T3D_GL3P_RENDERER_H__
#define __T3D_GL3P_RENDERER_H__


#include "Render/T3DRenderer.h"
#include "Render/T3DHardwareBufferManager.h"

namespace Tiny3D
{
    class GL3PRenderer
        : public Renderer
        , public Singleton<GL3PRenderer>
    {
    public:
        GL3PRenderer();
        virtual ~GL3PRenderer();

        virtual String getName() const override;

        virtual RenderWindow *createRenderWindow(
            const RenderWindowCreateParam &rkCreateParam,
            const RenderWindowCreateParamEx &rkCreateParamEx) override;

        virtual bool initialize() override;
        virtual void uninitialize() override;

        virtual bool beginRender(const Color4 &bkgndColor) override;
        virtual bool endRender() override;

        virtual bool queryCapability(Capability cap) override;
        virtual void enableCapability(Capability cap, bool enabled = true) override;

        virtual void setTransform(TransformState state, const Matrix4 &mat) override;

        virtual const Matrix4 &getTransform(TransformState state) const override;

        virtual void setLightEnabled(bool enable) override;
        virtual void setAmbientLight(const Color4 &ambient) override;
        virtual void addDynamicLight(size_t index, const SGLightPtr light) override;
        virtual void removeDynamicLight(size_t index) override;
        virtual void removeAllDynamicLights() override;

        virtual void setMaterial(const MaterialPtr &material) override;

        virtual void setCullingMode(CullingMode mode) override;
        virtual void setRenderMode(RenderMode mode) override;

        virtual void setViewport(const ViewportPtr &viewport) override;

        virtual void drawVertexList(PrimitiveType primitiveType,
            const VertexDataPtr &vertexData, uint32_t startIdx,
            uint32_t primitiveCount) override;

        virtual void drawIndexList(PrimitiveType primitiveType,
            const VertexDataPtr &vertexData, const IndexDataPtr &indexData,
            uint32_t startIdx, uint32_t pritimitiveCount) override;

    protected:
        virtual void makeProjectionMatrix(const Radian &rkFovY, Real aspect,
            Real nearDist, Real farDist, bool ortho, Matrix4 &mat) override;

        virtual void makeViewportMatrix(ViewportPtr viewport, Matrix4 &mat) override;

        virtual void updateFrustum(const Matrix4 &m, Plane *plane, size_t planeCount) override;


    protected:
    };
}


#endif  /*__T3D_GL3P_RENDERER_H__*/