

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

    ActionDataPtr ActionData::create(const String &name)
    {
        ActionDataPtr action = new ActionData(name);

        if (action != nullptr)
        {
            action->release();
        }

        return action;
    }

    ActionData::ActionData(const String &name)
        : mName(name)
    {

    }

    ActionData::~ActionData()
    {

    }


}
