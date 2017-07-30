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

#include "T3DActionData.h"


namespace Tiny3D
{
    KeyFrameData::KeyFrameData(int64_t timestamp)
        : mTimestamp(timestamp)
    {

    }

    ////////////////////////////////////////////////////////////////////////////

    KeyFrameDataTPtr KeyFrameDataT::create(int64_t timestamp, const Vector3 &translation)
    {
        KeyFrameDataTPtr keyframe = new KeyFrameDataT(timestamp, translation);

        if (keyframe != nullptr)
        {
            keyframe->release();
        }

        return keyframe;
    }

    KeyFrameDataT::KeyFrameDataT(int64_t timestamp, const Vector3 &translation)
        : KeyFrameData(timestamp)
        , mTranslation(translation)
    {

    }

    KeyFrameData::Type KeyFrameDataT::getType() const
    {
        return E_TYPE_TRANSLATION;
    }

    ////////////////////////////////////////////////////////////////////////////

    KeyFrameDataRPtr KeyFrameDataR::create(int64_t timestamp, const Quaternion &orientation)
    {
        KeyFrameDataRPtr keyframe = new KeyFrameDataR(timestamp, orientation);

        if (keyframe != nullptr)
        {
            keyframe->release();
        }

        return keyframe;
    }

    KeyFrameDataR::KeyFrameDataR(int64_t timestamp, const Quaternion &orientation)
        : KeyFrameData(timestamp)
        , mOrientation(orientation)
    {

    }

    KeyFrameData::Type KeyFrameDataR::getType() const
    {
        return E_TYPE_ROTATION;
    }

    ////////////////////////////////////////////////////////////////////////////

    KeyFrameDataSPtr KeyFrameDataS::create(int64_t timestamp, const Vector3 &scaling)
    {
        KeyFrameDataSPtr keyframe = new KeyFrameDataS(timestamp, scaling);

        if (keyframe != nullptr)
        {
            keyframe->release();
        }

        return keyframe;
    }

    KeyFrameDataS::KeyFrameDataS(int64_t timestamp, const Vector3 &scaling)
        : KeyFrameData(timestamp)
        , mScaling(scaling)
    {

    }

    KeyFrameData::Type KeyFrameDataS::getType() const
    {
        return E_TYPE_SCALING;
    }

    ////////////////////////////////////////////////////////////////////////////

    ActionDataPtr ActionData::create(const String &name, int32_t duration)
    {
        ActionDataPtr action = new ActionData(name, duration);

        if (action != nullptr)
        {
            action->release();
        }

        return action;
    }

    ActionData::ActionData(const String &name, int32_t duration)
        : mName(name)
        , mDuration(duration)
    {

    }

    ActionData::~ActionData()
    {

    }


}
