/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
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

#ifndef __MCONV_BONE_H__
#define __MCONV_BONE_H__


#include "mconv_node.h"


namespace mconv
{
    class Bone : public Node
    {
    public:
        Bone(const String &ID)
            : Node(ID)
            , mBoneIndex(0xFFFF)
        {

        }

        virtual ~Bone()
        {

        }

        void setID(const String &ID)
        {
            mID = ID;
        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_BONE;
        }

        Matrix4     mLocalTransform;
        Matrix4     mOffsetMatrix;
        uint16_t    mBoneIndex;

        Matrix4     mKeyframeMatrix;    /// 专门给ogre骨骼动画计算关键帧绝对本地变换用的
    };
}


#endif
