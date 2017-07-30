/*******************************************************************************
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
 ******************************************************************************/

#ifndef __T3D_SG_BOX_H__
#define __T3D_SG_BOX_H__


#include "SceneGraph/T3DSGGeometry.h"
#include "T3DAabb.h"
#include "T3DVector2.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGBox : public SGGeometry
    {
    public:
        static SGBoxPtr create(const String &materialName, uint32_t unID = E_NID_AUTOMATIC);

        virtual ~SGBox();

        virtual Type getNodeType() const override;
        virtual NodePtr clone() const override;

        void setMinX(Real x);
        void setMaxX(Real x);
        void setMinY(Real y);
        void setMaxY(Real y);
        void setMinZ(Real z);
        void setMaxZ(Real z);

        Real getMinX() const;
        Real getMaxX() const;
        Real getMinY() const;
        Real getMaxY() const;
        Real getMinZ() const;
        Real getMaxZ() const;

    protected:
        SGBox(uint32_t unID = E_NID_AUTOMATIC);
        virtual bool init(const String &materialName);

        virtual void cloneProperties(const NodePtr &node) const override;

        virtual MaterialPtr getMaterial() const override;
        virtual Renderer::PrimitiveType getPrimitiveType() const override;
        virtual VertexDataPtr getVertexData() const override;
        virtual IndexDataPtr getIndexData() const override;
        virtual bool isIndicesUsed() const override;

        void updateVertices();

    protected:
        enum
        {
            MAX_FACE_NUM = 6,
            MAX_VERTICES_NUM = 23,//24,
            INDICES_PER_FACE = 6
        };

        struct Vertex
        {
            Vector3 position;
//             Vector2 texcoord;
//             Vector4 diffuse;
            uint32_t diffuse;
        };

        Vertex mVertices[MAX_VERTICES_NUM];

        MaterialPtr     mMaterial;
        VertexDataPtr   mVertexData;
        IndexDataPtr    mIndexData;
    };
}


#endif  /*__T3D_SG_BOX_H__*/
