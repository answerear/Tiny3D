/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
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
            , mDuration(0.0f)
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

        size_t getKeyFramesSize(const String &name, const Bones &bones)
        {
            size_t s = 0;

            auto itr = bones.find(name);

            if (itr != bones.end())
            {
                auto keyframes = itr->second;
                s = keyframes.size();
            }

            return s;
        }

        Bones       mTKeyframes;
        Bones       mRKeyframes;
        Bones       mSKeyframes;

        float       mDuration;
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
