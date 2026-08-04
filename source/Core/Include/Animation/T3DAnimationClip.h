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


#include "Animation/T3DAnimationTrack.h"


namespace Tiny3D
{
    /**
     * \brief 骨骼动画片段，按骨骼名称索引各 AnimationTrack
     */
    TCLASS()
    class T3D_ENGINE_API AnimationClip : public Object
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建动画片段
         * \param [in] name : 片段名称
         * \param [in] duration : 片段时长（毫秒）
         * \param [in] tracks : 骨骼名到 AnimationTrack 的映射，右值移入
         * \return 新建的 AnimationClip 智能指针
         */
        static AnimationClipPtr create(const String &name, uint32_t duration, AnimationTracks &&tracks);

        /// 获取片段名称
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        /// 获取片段时长（毫秒）
        TPROPERTY(RTTRFuncName="Duration", RTTRFuncType="getter")
        uint32_t getDuration() const { return mDuration; }

        /// 获取全部骨骼动画轨道
        TPROPERTY(RTTRFuncName="Tracks", RTTRFuncType="getter")
        const AnimationTracks &getTracks() const { return mTracks; }

    private:
        /// 设置片段名称，供 RTTR 反序列化使用
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }

        /// 设置片段时长，供 RTTR 反序列化使用
        TPROPERTY(RTTRFuncName="Duration", RTTRFuncType="setter")
        void setDuration(uint32_t duration) { mDuration = duration; }

        /// 设置骨骼轨道表，供 RTTR 反序列化使用
        TPROPERTY(RTTRFuncName="Tracks", RTTRFuncType="setter")
        void setTracks(const AnimationTracks &tracks) { mTracks = tracks; }

    protected:
        AnimationClip() = default;

        /**
         * \brief 构造动画片段
         * \param [in] name : 片段名称
         * \param [in] duration : 片段时长（毫秒）
         * \param [in] tracks : 骨骼轨道映射，右值移入
         */
        AnimationClip(const String &name, uint32_t duration, AnimationTracks &&tracks);
        
        /// 片段名称
        String mName {};

        /// 片段时长（毫秒）
        uint32_t mDuration {0};

        /// 骨骼名 → AnimationTrack 映射
        AnimationTracks mTracks {};
    };
}


#endif    /*__T3D_ANIMATION_CLIP_H__*/
