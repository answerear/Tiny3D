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

#ifndef __T3D_ANIMATION_TRACK_H__
#define __T3D_ANIMATION_TRACK_H__


#include "Animation/T3DKeyframe.h"


namespace Tiny3D
{
    /**
     * \brief 单根骨骼的动画轨道，聚合平移、旋转、缩放三条关键帧序列
     */
    TCLASS()
    class T3D_ENGINE_API AnimationTrack : public Object
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建动画轨道
         * \param [in] trackT : 平移关键帧序列
         * \param [in] trackO : 旋转关键帧序列
         * \param [in] trackS : 缩放关键帧序列
         * \return 新建的 AnimationTrack 智能指针
         */
        static AnimationTrackPtr create(const TranslationTrack &trackT,
            const OrientationTrack &trackO, const ScalingTrack &trackS);

        /// 获取平移关键帧序列
        TPROPERTY(RTTRFuncName="TranslationTrack", RTTRFuncType="getter")
        const TranslationTrack &getTranslationTrack() const { return mTranslationTrack; }

        /// 获取旋转关键帧序列
        TPROPERTY(RTTRFuncName="OrientationTrack", RTTRFuncType="getter")
        const OrientationTrack &getOrientationTrack() const { return mOrientationTrack; }

        /// 获取缩放关键帧序列
        TPROPERTY(RTTRFuncName="ScalingTrack", RTTRFuncType="getter")
        const ScalingTrack &getScalingTrack() const { return mScalingTrack; }

    private:
        /// 设置平移轨道，供 RTTR 反序列化使用
        TPROPERTY(RTTRFuncName="TranslationTrack", RTTRFuncType="setter")
        void setTranslationTrack(const TranslationTrack &track) { mTranslationTrack = track; }

        /// 设置旋转轨道，供 RTTR 反序列化使用
        TPROPERTY(RTTRFuncName="OrientationTrack", RTTRFuncType="setter")
        void setOrientationTrack(const OrientationTrack &track) { mOrientationTrack = track; }

        /// 设置缩放轨道，供 RTTR 反序列化使用
        TPROPERTY(RTTRFuncName="ScalingTrack", RTTRFuncType="setter")
        void setScalingTrack(const ScalingTrack &track) { mScalingTrack = track; }
        
    protected:
        AnimationTrack() = default;

        /**
         * \brief 构造动画轨道
         * \param [in] trackT : 平移关键帧序列
         * \param [in] trackO : 旋转关键帧序列
         * \param [in] trackS : 缩放关键帧序列
         */
        AnimationTrack(const TranslationTrack &trackT,
            const OrientationTrack &trackO, const ScalingTrack &trackS);
        
        /// 平移关键帧序列
        TranslationTrack mTranslationTrack {};
        /// 旋转关键帧序列
        OrientationTrack mOrientationTrack {};
        /// 缩放关键帧序列
        ScalingTrack mScalingTrack {};
    };
}


#endif    /*__T3D_ANIMATION_TRACK_H__*/
