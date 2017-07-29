/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
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
 ******************************************************************************/

#ifndef __T3D_SG_AXIS_H__
#define __T3D_SG_AXIS_H__


#include "SceneGraph/T3DSGGeometry.h"


namespace Tiny3D
{
    enum Axis
    {
        AXIS_X,
        AXIS_Y,
        AXIS_Z
    };

    class T3D_ENGINE_API SGAxis : public SGRenderable
    {
    public:
        static SGAxisPtr create(Real fAxisX, Real fAxisY, Real fAxisZ, uint32_t uID = E_NID_AUTOMATIC);

        virtual ~SGAxis();
        virtual Type getNodeType() const override;
        virtual NodePtr clone() const override;

        Real getAxisLength(Axis axis) const;
        void setAxisLength(Axis axis, Real fLength);

    protected:
        SGAxis(uint32_t uID = E_NID_AUTOMATIC);

        virtual bool init(Real fAxisX, Real fAxisY, Real fAxisZ);
        
        virtual void cloneProperties(const NodePtr &node) const override;

        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue) override;

        virtual MaterialPtr getMaterial() const override;
        virtual Renderer::PrimitiveType getPrimitiveType() const override;
        virtual VertexDataPtr getVertexData() const override;
        virtual IndexDataPtr getIndexData() const override;
        virtual bool isIndicesUsed() const override;

        bool updateVertices();

    private:
        struct Vertex
        {
            Vertex() 
                : position(Vector3::ZERO)
                , color(Color4::WHITE)
            {
            }

            Vector3 position;
            Color4  color;
        };

        Vertex  mVertices[6];
        bool    mIsDataDirty;

        VertexDataPtr   mVertexData;
    };
}

#endif  /*__T3D_SG_AXIS_H__*/

