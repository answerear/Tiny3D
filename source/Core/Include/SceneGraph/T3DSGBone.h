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

#ifndef __T3D_SG_BONE_H__
#define __T3D_SG_BONE_H__


#include "SceneGraph/T3DSGTransformNode.h"


namespace Tiny3D
{
    class SGBone : public SGTransformNode
    {
    public:
        virtual ~SGBone();

        static SGBonePtr create(ObjectPtr data = nullptr, uint32_t unID = E_NID_AUTOMATIC);

        uint16_t getParentBone() const;
        ObjectPtr getBoneData() const { return mBoneData; }

        virtual void updateTransform() override;

        virtual Type getNodeType() const override;

        virtual NodePtr clone() const override;

        const Matrix4 &getFinalMatrix() const { return mFinalMatrix; }

    protected:
        SGBone(uint32_t unID = E_NID_AUTOMATIC);

        virtual bool init(ObjectPtr data);

    protected:
        ObjectPtr   mBoneData;
        Matrix4     mFinalMatrix;
    };
}


#endif  /*__T3D_SG_BONE_H__*/
