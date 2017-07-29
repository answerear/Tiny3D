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

#ifndef __T3D_SG_SPHERE_H__
#define __T3D_SG_SPHERE_H__


#include "SceneGraph/T3DSGGeometry.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGSphere : public SGGeometry
    {
    public:
        static SGSpherePtr create(const String &materialName, uint32_t unID = E_NID_AUTOMATIC);

        virtual ~SGSphere();

        virtual Type getNodeType() const override;
        virtual NodePtr clone() const override;

        void setRadius(Real radius);
        Real getRadius() const  { return mRadius; }

    protected:
        SGSphere(uint32_t uID = E_NID_AUTOMATIC);

        bool init(const String &materialName);

        virtual void cloneProperties(const NodePtr &node) const override;

        virtual MaterialPtr getMaterial() const override;
        virtual Renderer::PrimitiveType getPrimitiveType() const override;
        virtual VertexDataPtr getVertexData() const override;
        virtual IndexDataPtr getIndexData() const override;
        virtual bool isIndicesUsed() const override;

        void loadVertices(Vector3 *vertices, size_t vertexCount);
        void loadIndices(uint16_t *indices, size_t indexCount);

        enum
        {
            MAX_STACKS = 18,
            MAX_SLICES = 18,
            MAX_VERTICES = (MAX_STACKS + 1) * (MAX_SLICES + 1),
            MAX_TRIANGLES = MAX_STACKS * MAX_SLICES * 2,
        };

        Real    mRadius;

        MaterialPtr     mMaterial;
        VertexDataPtr   mVertexData;
        IndexDataPtr    mIndexData;
    };
}



#endif  /*__T3D_SG_SPHERE_H__*/
