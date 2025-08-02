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

#ifndef __T3D_ANIMATION_PLAYER_H__
#define __T3D_ANIMATION_PLAYER_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * @brief 骨骼动画播放器，同一时间，只能播放一个动画
     */
    class T3D_ENGINE_API AnimationPlayer : public Object
    {
    public:
        static const ID INVALID_ID;
        
        static AnimationPlayerPtr create(SkinnedGeometry *geometry);
        
        /**
         * @brief 播放动画片段
         * @param clipName 动画片段名称
         * @param isBlending 是否启用混合。 启用后，会把当前正在播放的动画和将要播放的动画进行混合以作过渡
         * @param isLoop 是否循环播放，默认为false
         * @return 调用失败返回 AnimationPlayer::INVALID_ID. 成功返回一个实例ID，该ID可用于停止播放
         */
        ID playClip(const String &clipName, bool isBlending, bool isLoop = false);

        /**
         * @brief 混合播放两个动画片段
         * @param fromClipName 起始动画片段名称
         * @param toClipName 目标动画片段名称
         * @param toClipStartedMS 目标动画开始播放的时间点，单位为毫秒
         * @param isLoop 目标动画是否循环播放，默认为false
         * @return 调用失败返回 AnimationPlayer::INVALID_ID. 成功返回一个实例ID，该ID可用于停止播放
         */
        ID playClip(const String &fromClipName, const String &toClipName, uint32_t toClipStartedMS, bool isLoop = false);
        
        /**
         * @brief 同时混合播放多个动画片段，只播放一次。 当最长时间的 clip 播放完成后，混合播放结束。
         * @param clipNames 动画片段名称数组
         * @return 调用失败返回 AnimationPlayer::INVALID_ID. 成功返回一个实例ID，该ID可用于停止播放
         */
        ID playClips(const StringArray &clipNames);

        /**
         * @brief 停止播放
         * @param playbackID 播放实例ID
         * @return 成功返回 T3D_OK, 否则返回错误码
         */
        TResult stopPlayback(ID playbackID);

        /**
         * @brief 暂停播放
         * @param playbackID 播放实例ID
         * @return 成功返回 T3D_OK, 否则返回错误码
         */
        TResult pausePlayback(ID playbackID);

        /**
         * @brief 恢复播放
         * @param playbackID 播放实例ID
         * @return 成功返回 T3D_OK, 否则返回错误码
         */
        TResult resumePlayback(ID playbackID);

        /**
         * @brief 驱动动画更新
         */
        void update();

        /**
         * @brief 是否在播放中
         * @return 播放中返回 true，否则返回 false 
         */
        bool isPlaying() const { return mIsPlaying; }
        
    protected:        
        AnimationPlayer(SkinnedGeometry *geometry);

        ID generateID() const { return ++msGeneratedID; }

        // 插值函数，返回当前帧号
        uint32_t interpolateTranslation(uint32_t startFrame, uint32_t time, const TranslationTrack &track, Vector3 &translation);

        // 插值函数，返回当前帧号
        uint32_t interpolateOrientation(uint32_t startFrame, uint32_t time, const OrientationTrack &track, Quaternion &orientation);

        // 插值函数，返回当前帧号
        uint32_t interpolateScaling(uint32_t startFrame, uint32_t time, const ScalingTrack &track, Vector3 &scaling);

        Real getInterplationTime(Keyframe *kf0, Keyframe *kf1, uint32_t time) const;

        void updateBones();

    private:
        static ID msGeneratedID;
        
    protected:
        /// 蒙皮几何体
        SkinnedGeometry *mSkinnedGeometry {nullptr};

        /// 当前播放实例ID
        ID mCurrentPlaybackID {INVALID_ID};

        /// 动画开始时间戳
        int64_t mStartTimestamp {0};

        /// 平移轨道当前帧号
        uint32_t mCurrentFrameT {0};
        /// 旋转轨道当前帧号
        uint32_t mCurrentFrameO {0};
        /// 缩放轨道当前帧号
        uint32_t mCurrentFrameS {0};

        /// 是否在播放中
        bool mIsPlaying {false};
    };
}


#endif    /*__T3D_ANIMATION_PLAYER_H__*/
