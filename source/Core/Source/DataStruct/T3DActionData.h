

#ifndef __T3D_ACTION_DATA_H__
#define __T3D_ACTION_DATA_H__


#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"
#include "Misc/T3DObject.h"
#include "Math/T3DVector3.h"
#include "Math/T3DQuaternion.h"


namespace Tiny3D
{
    class KeyFrameData : public Object
    {
    protected:
        KeyFrameData();

    private:
        KeyFrameData(const KeyFrameData &);
        KeyFrameData &operator =(const KeyFrameData &);

    public:
        int64_t     mTimestamp;
    };

    class KeyFrameDataT : public KeyFrameData
    {
    public:
        static KeyFrameDataTPtr create();

    protected:
        KeyFrameDataT();

    private:
        KeyFrameDataT(const KeyFrameDataT &);
        KeyFrameDataT &operator =(const KeyFrameDataT &);

    public:
        Vector3     mTranslation;
    };

    class KeyFrameDataR : public KeyFrameData
    {
    public:
        static KeyFrameDataRPtr create();

    protected:
        KeyFrameDataR();

    private:
        KeyFrameDataR(const KeyFrameDataR &);
        KeyFrameDataR &operator =(const KeyFrameDataR &);

    public:
        Quaternion  mOrientation;
    };

    class KeyFrameDataS : public KeyFrameData
    {
    public:
        static KeyFrameDataSPtr create();

    protected:
        KeyFrameDataS();

    private:
        KeyFrameDataS(const KeyFrameDataS &);
        KeyFrameDataS &operator =(const KeyFrameDataS &);

    public:
        Vector3     mScaling;
    };

    class ActionData : public Object
    {
    public:
        static ActionDataPtr create(const String &name);

        virtual ~ActionData();

    protected:
        ActionData(const String &name);

    public:
        typedef std::list<KeyFrameDataPtr>  KeyFrames;
        typedef KeyFrames::iterator         KeyFramesItr;
        typedef KeyFrames::const_iterator   KeyFramesConstItr;

        typedef std::map<String, KeyFrames> Bones;
        typedef Bones::iterator             BonesItr;
        typedef Bones::const_iterator       BonesConstItr;

        String      mName;
        Bones       mBonesTranslation;
        Bones       mBonesRotation;
        Bones       mBonesScaling;
    };
}


#endif  /*__T3D_ACTION_DATA_H__*/
