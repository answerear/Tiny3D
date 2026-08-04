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
#include "T3DConfig.h"


namespace Tiny3D
{
    /**
     * \brief 绑定 SkinnedGeometry 的骨骼动画播放器，负责插值更新骨骼变换与 CPU/GPU 蒙皮
     */
    class T3D_ENGINE_API AnimationPlayer : public Object
    {
    public:
        /// 无效播放实例 ID，与 T3D_INVALID_ID 相同
        static const ID INVALID_ID;
        
        /**
         * \brief 创建动画播放器
         * \param [in] geometry : 目标蒙皮几何体，不可为 nullptr
         * \return 新建的 AnimationPlayer 智能指针
         */
        static AnimationPlayerPtr create(SkinnedGeometry *geometry);
        
        /**
         * \brief 启动单片段播放并注册到 AnimationPlayerMgr
         * \param [in] clipName : 动画片段名称（当前实现未按名称查找片段）
         * \param [in] isBlending : 是否混合过渡（当前实现未使用）
         * \param [in] isLoop : 片段播完后是否循环
         * \param [in] isGPUSkinning : 是否切换材质 shader 关键字以启用 GPU 蒙皮
         * \return 当前实现始终返回 INVALID_ID；参数校验失败时内部记录错误码
         * \note geometry 为 nullptr 或 clipName 为空时提前失败
         */
        ID playClip(const String &clipName, bool isBlending, bool isLoop = false, bool isGPUSkinning = false);

        /**
         * \brief 两片段混合播放（未实现）
         * \param [in] fromClipName : 起始片段名称
         * \param [in] toClipName : 目标片段名称
         * \param [in] toClipStartedMS : 目标片段起始时间（毫秒）
         * \param [in] isLoop : 目标片段是否循环
         * \param [in] isGPUSkinning : 是否使用 GPU 蒙皮
         * \return 当前实现始终返回 INVALID_ID
         */
        ID playClip(const String &fromClipName, const String &toClipName, uint32_t toClipStartedMS, bool isLoop = false, bool isGPUSkinning = false);
        
        /**
         * \brief 多片段混合播放（未实现）
         * \param [in] clipNames : 待混合的片段名称列表
         * \param [in] isGPUSkinning : 是否使用 GPU 蒙皮
         * \return 当前实现始终返回 INVALID_ID
         */
        ID playClips(const StringArray &clipNames, bool isGPUSkinning = false);

        /**
         * \brief 停止播放并从 AnimationPlayerMgr 移除
         * \param [in] playbackID : 播放实例 ID（当前实现未校验）
         * \return 调用成功返回 T3D_OK
         */
        TResult stopPlayback(ID playbackID);

        /**
         * \brief 暂停播放（当前实现为空操作）
         * \param [in] playbackID : 播放实例 ID（当前实现未使用）
         * \return 始终返回 T3D_OK
         */
        TResult pausePlayback(ID playbackID);

        /**
         * \brief 恢复播放（当前实现为空操作）
         * \param [in] playbackID : 播放实例 ID（当前实现未使用）
         * \return 始终返回 T3D_OK
         */
        TResult resumePlayback(ID playbackID);

        /**
         * \brief 驱动骨骼插值更新；仅在 isPlaying() 为 true 时执行
         */
        void updateAnimation();

        /**
         * \brief 根据 mIsGPUSkinning 执行 CPU 或 GPU 蒙皮
         */
        void skinning();

        /// 是否处于播放状态
        bool isPlaying() const { return mIsPlaying; }
        
    protected:        
        /**
         * \brief 构造并绑定蒙皮几何体
         * \param [in] geometry : 蒙皮几何体，断言非 nullptr
         */
        AnimationPlayer(SkinnedGeometry *geometry);

        /// 递增生成播放实例 ID（当前 playClip 未调用）
        ID generateID() const { return ++msGeneratedID; }

        /**
         * \brief 在平移轨道上按时间插值
         * \param [in] startFrame : 搜索起始帧索引
         * \param [in] time : 当前播放时间（毫秒）
         * \param [in] track : 平移关键帧序列
         * \param [out] translation : 插值结果
         * \return 命中的关键帧索引；未命中且轨道非空时返回最后一帧索引；否则为 uint32_t(-1)
         */
        uint32_t interpolateTranslation(uint32_t startFrame, uint32_t time, const TranslationTrack &track, Vector3 &translation);

        /**
         * \brief 在旋转轨道上按时间 slerp 插值
         * \param [in] startFrame : 搜索起始帧索引
         * \param [in] time : 当前播放时间（毫秒）
         * \param [in] track : 旋转关键帧序列
         * \param [out] orientation : 插值结果
         * \return 命中的关键帧索引；未命中且轨道非空时返回最后一帧索引；否则为 uint32_t(-1)
         */
        uint32_t interpolateOrientation(uint32_t startFrame, uint32_t time, const OrientationTrack &track, Quaternion &orientation);

        /**
         * \brief 在缩放轨道上按时间插值
         * \param [in] startFrame : 搜索起始帧索引
         * \param [in] time : 当前播放时间（毫秒）
         * \param [in] track : 缩放关键帧序列
         * \param [out] scaling : 插值结果
         * \return 命中的关键帧索引；未命中且轨道非空时返回最后一帧索引；否则为 uint32_t(-1)
         */
        uint32_t interpolateScaling(uint32_t startFrame, uint32_t time, const ScalingTrack &track, Vector3 &scaling);

        /**
         * \brief 计算两关键帧之间的归一化插值系数
         * \param [in] kf0 : 区间左端关键帧
         * \param [in] kf1 : 区间右端关键帧
         * \param [in] time : 当前时间（毫秒）
         * \return (time - t0) / (t1 - t0)
         */
        Real getInterplationTime(Keyframe *kf0, Keyframe *kf1, uint32_t time) const;

        /**
         * \brief 按骨骼层次顺序插值并写回 Transform3D；播完非循环片段时自动停止
         * \note 当前取 SkeletalAnimation 中 clips 映射的首个片段，不按 clipName 选择
         */
        void updateBones();

        /**
         * \brief CPU 蒙皮：按 blend weight/index 混合骨骼矩阵，写回顶点与法线 VBO
         */
        void CPUSkinning();

        /**
         * \brief GPU 蒙皮：收集骨骼矩阵并写入材质 uniform tiny3d_BoneMatrices
         */
        void GPUSkinning();

    private:
        /// 全局播放 ID 递增计数器
        static ID msGeneratedID;
        
    protected:
        /// 绑定的蒙皮几何体
        SkinnedGeometry *mSkinnedGeometry {nullptr};

        /// 当前播放实例 ID（当前 playClip 未赋值）
        ID mCurrentPlaybackID {INVALID_ID};

        /// 本次播放起始时间戳（毫秒）
        int64_t mStartTimestamp {0};

        /// 各骨骼平移轨道当前关键帧索引
        uint32_t mCurrentFrameT[T3D_MAX_SKIN_BONES] {0};
        /// 各骨骼旋转轨道当前关键帧索引
        uint32_t mCurrentFrameO[T3D_MAX_SKIN_BONES] {0};
        /// 各骨骼缩放轨道当前关键帧索引
        uint32_t mCurrentFrameS[T3D_MAX_SKIN_BONES] {0};

        /// 是否正在播放
        bool mIsPlaying {false};
        /// 播完后是否循环
        bool mIsLoop {false};
        /// 是否使用 GPU 蒙皮路径
        bool mIsGPUSkinning {false};
    };
}


#endif    /*__T3D_ANIMATION_PLAYER_H__*/
