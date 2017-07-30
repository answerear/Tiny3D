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

#ifndef __T3D_SG_SKELETON_H__
#define __T3D_SG_SKELETON_H__


#include "SceneGraph/T3DSGGeometry.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGSkeleton : public SGGeometry
    {
    public:
        static SGSkeletonPtr create(SGBonePtr bone, uint32_t uID = E_NID_AUTOMATIC);

        virtual ~SGSkeleton();

        virtual Type getNodeType() const override;
        virtual NodePtr clone() const override;

    protected:
        struct BoneVertex
        {
            BoneVertex() : position(), color(Color4::WHITE) {}
            Vector3     position;
            Color4      color;
        };

        SGSkeleton(uint32_t uID = E_NID_AUTOMATIC);

        virtual bool init(SGBonePtr bone);
        bool buildSkeletonVertices(SGBonePtr bone, std::vector<BoneVertex> &vertices);

        void updateVertices();

        virtual void updateTransform() override;

        virtual void cloneProperties(const NodePtr &node) const override;

        virtual MaterialPtr getMaterial() const override;
        virtual Renderer::PrimitiveType getPrimitiveType() const override;
        virtual VertexDataPtr getVertexData() const override;
        virtual IndexDataPtr getIndexData() const override;
        virtual bool isIndicesUsed() const override;

        bool searchSkinData(const String &name, ObjectPtr &skin);

    protected:
        SGBonePtr       mSkeleton;
        VertexDataPtr   mVertexData;
    };
}


#endif  /*__T3D_SG_SKELETON_H__*/
