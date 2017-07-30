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

namespace Tiny3D
{
    inline uint32_t Bound::getID() const
    {
        return mID;
    }

    inline void Bound::setGroupID(uint32_t groupID)
    {
        mGroupID = groupID;
    }

    inline uint32_t Bound::getGroupID() const
    {
        return mGroupID;
    }

    inline const Sphere &Bound::getSphere() const
    {
        return mSphere;
    }

    inline SGNodePtr Bound::getNode() const
    {
        return mNode;
    }

    inline SGNodePtr Bound::getNode()
    {
        return mNode;
    }

    inline void Bound::setCollisionSource(bool isSource)
    {
        mCollisionSource = isSource;
    }

    inline bool Bound::isCollisionSource() const
    {
        return mCollisionSource;
    }

    inline void Bound::setEnable(bool enable)
    {
        mEnable = enable;
    }

    inline bool Bound::isEnable() const
    {
        return mEnable;
    }
}
