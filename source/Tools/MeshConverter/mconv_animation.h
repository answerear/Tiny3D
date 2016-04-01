

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
        int64_t     mTimestamp;
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

        void destroyAllKeyframes()
        {
            auto itr = mTKeyframes.begin();
            while (itr != mTKeyframes.end())
            {
                delete *itr;
                ++itr;
            }

            mTKeyframes.clear();

            itr = mRKeyframes.begin();
            while (itr != mRKeyframes.end())
            {
                delete *itr;
                ++itr;
            }

            mRKeyframes.clear();

            itr = mSKeyframes.begin();
            while (itr != mSKeyframes.end())
            {
                delete *itr;
                ++itr;
            }

            mSKeyframes.clear();
        }

        typedef std::list<Keyframe*>        Keyframes;
        typedef Keyframes::iterator         KeyframesItr;
        typedef Keyframes::const_iterator   KeyframesConstItr;

        Keyframes   mTKeyframes;
        Keyframes   mRKeyframes;
        Keyframes   mSKeyframes;
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
