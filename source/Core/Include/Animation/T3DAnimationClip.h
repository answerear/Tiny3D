/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#ifndef __T3D_ANIMATION_CLIP_H__
#define __T3D_ANIMATION_CLIP_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * @brief 骨骼动画片段
     */
    TCLASS()
    class T3D_ENGINE_API AnimationClip : public Object
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        static AnimationClipPtr create(const String &name, uint32_t duration, AnimationTracks &&tracks);

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        TPROPERTY(RTTRFuncName="Duration", RTTRFuncType="getter")
        uint32_t getDuration() const { return mDuration; }

        TPROPERTY(RTTRFuncName="Tracks", RTTRFuncType="getter")
        const AnimationTracks &getTracks() const { return mTracks; }

    private:
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }

        TPROPERTY(RTTRFuncName="Duration", RTTRFuncType="setter")
        void setDuration(uint32_t duration) { mDuration = duration; }

        TPROPERTY(RTTRFuncName="Tracks", RTTRFuncType="setter")
        void setTracks(const AnimationTracks &tracks) { mTracks = tracks; }

    protected:
        AnimationClip() = default;

        AnimationClip(const String &name, uint32_t duration, AnimationTracks &&tracks);
        
        /// 动画片段名称
        String mName {};

        /// 动画时长
        uint32_t mDuration {0};

        /// 骨骼动画轨道数据
        AnimationTracks mTracks {};
    };
}


#endif    /*__T3D_ANIMATION_CLIP_H__*/
