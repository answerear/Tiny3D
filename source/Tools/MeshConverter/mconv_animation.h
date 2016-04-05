

#ifndef __MCONV_ANIMATION_H__
#define __MCONV_ANIMATION_H__


#include "mconv_node.h"


namespace mconv
{
    class Keyframe
    {
    public:
        Keyframe(uint32_t uID)
            : mID(uID)
        {

        }

        uint32_t    mID;
        double      mTimestamp;
    };

    class KeyframeT : public Keyframe
    {
    public:
        KeyframeT(uint32_t uID)
            : Keyframe(uID)
        {

        }

        float   x, y, z;
    };

    class KeyframeR : public Keyframe
    {
    public:
        KeyframeR(uint32_t uID)
            : Keyframe(uID)
        {

        }

        float   x, y, z, w;
    };

    class KeyframeS : public Keyframe
    {
    public:
        KeyframeS(uint32_t uID)
            : Keyframe(uID)
        {

        }

        float   x, y, z;
    };

    typedef std::list<Keyframe*>            Keyframes;
    typedef Keyframes::iterator             KeyframesItr;
    typedef Keyframes::const_iterator       KeyframesConstItr;

    typedef std::map<String, Keyframes>     Bones;
    typedef Bones::iterator                 BonesItr;
    typedef Bones::const_iterator           BonesConstItr;
    typedef std::pair<String, Keyframes>    BonesValue;

    class Action : public Node
    {
    public:
        Action(const String &ID)
            : Node(ID)
        {

        }

        virtual ~Action()
        {
            destroyAllKeyframes();
        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_ACTION;
        }

        void destroyKeyframes(Keyframes &keyframes)
        {
            auto i = keyframes.begin();
            while (i != keyframes.end())
            {
                delete *i;
                ++i;
            }
        }

        void destroyAllKeyframes()
        {
            auto itr = mTKeyframes.begin();
            while (itr != mTKeyframes.end())
            {
                Keyframes &keyframes = itr->second;
                destroyKeyframes(keyframes);
                ++itr;
            }

            mTKeyframes.clear();

            itr = mRKeyframes.begin();
            while (itr != mRKeyframes.end())
            {
                Keyframes &keyframes = itr->second;
                destroyKeyframes(keyframes);
                ++itr;
            }

            mRKeyframes.clear();

            itr = mSKeyframes.begin();
            while (itr != mSKeyframes.end())
            {
                Keyframes &keyframes = itr->second;
                destroyKeyframes(keyframes);
                ++itr;
            }

            mSKeyframes.clear();
        }

        bool addKeyframe(Keyframe *pFrame, const String &name, Bones &bones)
        {
            auto itr = bones.find(name);

            if (itr != bones.end())
            {
                Keyframes &keyframes = itr->second;
                keyframes.push_back(pFrame);
            }
            else
            {
                BonesValue value;
                value.first = name;
                value.second.push_back(pFrame);
                bones.insert(value);
            }

            return true;
        }

        Bones      mTKeyframes;
        Bones      mRKeyframes;
        Bones      mSKeyframes;
    };

    class Animation : public Node
    {
    public:
        Animation(const String &ID)
            : Node(ID)
        {

        }

        virtual ~Animation()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_ANIMATION;
        }
    };
}


#endif  /*__MCONV_ANIMATION_H__*/
