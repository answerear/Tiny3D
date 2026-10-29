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
     * \brief 单根骨骼的局部空间姿势（供动画采样与混合树节点求值使用）
     */
    struct T3D_ENGINE_API BonePose
    {
        static constexpr uint8_t VALID_TRANSLATION = 0x01;
        static constexpr uint8_t VALID_ORIENTATION = 0x02;
        static constexpr uint8_t VALID_SCALING     = 0x04;

        Vector3     translation {Vector3::ZERO};
        Quaternion  orientation {Quaternion::IDENTITY};
        Vector3     scaling     {Vector3::UNIT_SCALE};
        uint8_t     validMask   {0}; ///< bit0: T, bit1: O, bit2: S（标记该骨骼在 Clip 中是否有对应有效轨道）
    };

    /**
     * \brief 整套骨骼的局部姿势缓冲（混合树节点求值的标准 I/O 载体）
     */
    struct T3D_ENGINE_API SkeletonPose
    {
        BonePose    bones[T3D_MAX_SKIN_BONES];
        size_t      boneCount {0};
    };

    /**
     * \brief 绑定 SkinnedGeometry 的骨骼动画播放器，负责插值更新骨骼变换与 CPU/GPU 蒙皮
     */
    class T3D_ENGINE_API AnimationPlayer : public Object
    {
    public:
        /// 无效播放实例 ID，与 T3D_INVALID_ID 相同
        static const ID INVALID_ID;

        /// 默认 CrossFade 过渡时长（毫秒）
        static constexpr uint32_t DEFAULT_BLEND_DURATION_MS = 250;

        /**
         * \brief 单条动画片段轨道的运行时状态（对应未来混合树叶子节点 ClipNode 的状态）
         */
        struct T3D_ENGINE_API ClipState
        {
            String          clipName {};
            AnimationClip  *clip {nullptr};
            Real            elapsedMS {REAL_ZERO};  ///< 已累积播放时长（毫秒），支持非整数倍速率缩放
            Real            speed {REAL_ONE};       ///< 播放速率倍率，供未来 BlendTree 同步步频使用
            Real            weight {REAL_ONE};      ///< 混合权重
            bool            isLoop {false};
            uint32_t        frameT[T3D_MAX_SKIN_BONES] {0};
            uint32_t        frameO[T3D_MAX_SKIN_BONES] {0};
            uint32_t        frameS[T3D_MAX_SKIN_BONES] {0};

            /// 重置各骨骼的关键帧游标
            void resetFrames();

            /// 获取归一化播放进度 [0, 1]
            Real getNormalizedTime() const;

            /// 按归一化进度 [0, 1] 设置播放时间
            void setNormalizedTime(Real normalizedTime);
        };
        
        /**
         * \brief 创建动画播放器
         * \param [in] geometry : 目标蒙皮几何体，不可为 nullptr
         * \return 新建的 AnimationPlayer 智能指针
         */
        static AnimationPlayerPtr create(SkinnedGeometry *geometry);
        
        /**
         * \brief 启动单片段播放（或从当前播放片段平滑 CrossFade 过渡到新片段）并注册到 AnimationPlayerMgr
         * \param [in] clipName : 动画片段名称，在 SkeletalAnimation 中按名查找（若为 "default" 且未命中则回退首个片段）
         * \param [in] isBlending : 若当前正在播放其他片段，是否启用平滑交叉淡入淡出过渡
         * \param [in] isLoop : 片段播完后是否循环
         * \param [in] isGPUSkinning : 是否切换材质 shader 关键字以启用 GPU 蒙皮
         * \return 成功返回递增生成的播放实例 ID；参数校验失败或未找到片段时返回 INVALID_ID
         * \note geometry 为 nullptr、clipName 为空或未找到对应 AnimationClip 时返回 INVALID_ID
         */
        ID playClip(const String &clipName, bool isBlending, bool isLoop = false, bool isGPUSkinning = false);

        /**
         * \brief 两片段按指定起始时刻交叉混合过渡播放
         * \param [in] fromClipName : 起始片段名称
         * \param [in] toClipName : 目标片段名称
         * \param [in] toClipStartedMS : 当起始片段播放到该时刻（毫秒）时触发向目标片段混合过渡
         * \param [in] isLoop : 目标片段是否循环
         * \param [in] isGPUSkinning : 是否使用 GPU 蒙皮
         * \return 成功返回递增生成的播放实例 ID；校验失败返回 INVALID_ID
         */
        ID playClip(const String &fromClipName, const String &toClipName, uint32_t toClipStartedMS, bool isLoop = false, bool isGPUSkinning = false);
        
        /**
         * \brief 多片段并行混合/分层叠加播放
         * \param [in] clipNames : 待混合的片段名称列表
         * \param [in] isGPUSkinning : 是否使用 GPU 蒙皮
         * \return 成功返回递增生成的播放实例 ID；校验失败返回 INVALID_ID
         */
        ID playClips(const StringArray &clipNames, bool isGPUSkinning = false);

        /**
         * \brief 停止播放并从 AnimationPlayerMgr 移除
         * \param [in] playbackID : 播放实例 ID（传入 INVALID_ID 表示无条件停止当前播放）
         * \return 调用成功返回 T3D_OK；playbackID 不匹配返回 T3D_ERR_INVALID_PARAM
         */
        TResult stopPlayback(ID playbackID);

        /**
         * \brief 暂停当前播放，冻结已累积的动画时间与骨骼姿态
         * \param [in] playbackID : 播放实例 ID（传入 INVALID_ID 表示暂停当前播放）
         * \return 成功返回 T3D_OK；未在播放或 playbackID 不匹配返回 T3D_ERR_INVALID_PARAM
         */
        TResult pausePlayback(ID playbackID);

        /**
         * \brief 从暂停帧平滑恢复播放
         * \param [in] playbackID : 播放实例 ID（传入 INVALID_ID 表示恢复当前播放）
         * \return 成功返回 T3D_OK；未在播放或 playbackID 不匹配返回 T3D_ERR_INVALID_PARAM
         */
        TResult resumePlayback(ID playbackID);

        /**
         * \brief 驱动骨骼插值更新；仅在 isPlaying() 为 true 且未暂停时执行
         */
        void updateAnimation();

        /**
         * \brief 根据 mIsGPUSkinning 执行 CPU 或 GPU 蒙皮
         */
        void skinning();

        /// 是否处于播放状态
        bool isPlaying() const { return mIsPlaying; }

        /// 是否处于暂停状态
        bool isPaused() const { return mIsPaused; }

        /// 是否处于双片段过渡态（包含等待触发或正在插值过渡）
        bool isCrossFading() const { return mTargetClip.clip != nullptr; }

        /// 获取当前播放实例 ID
        ID getCurrentPlaybackID() const { return mCurrentPlaybackID; }

        /// 获取当前主播放片段名称
        const String &getCurrentClipName() const { return mPrimaryClip.clipName; }

        /// 设置默认 CrossFade 过渡时长（毫秒）
        void setBlendDuration(uint32_t durationMS) { mBlendDurationMS = (durationMS > 0) ? durationMS : 1; }

        /// 获取当前 CrossFade 过渡时长（毫秒）
        uint32_t getBlendDuration() const { return mBlendDurationMS; }

        /// 获取当前 CrossFade 混合权重因子 alpha [0, 1]（非过渡态返回 0）
        Real getBlendAlpha() const
        {
            if (mTargetClip.clip == nullptr || mBlendDurationMS == 0)
            {
                return REAL_ZERO;
            }
            return std::clamp(mBlendElapsedMS / static_cast<Real>(mBlendDurationMS), REAL_ZERO, REAL_ONE);
        }

        /// 获取主轨道只读状态
        const ClipState &getPrimaryClipState() const { return mPrimaryClip; }

        /// 获取过渡目标轨道只读状态
        const ClipState &getTargetClipState() const { return mTargetClip; }

        /**
         * \brief 双姿态线性/球面插值算子（用于 CrossFade 过渡与未来 1D BlendTree 相邻区间插值）
         */
        static void blendPoses(const SkeletonPose &poseA, const SkeletonPose &poseB,
            Real alpha, SkeletonPose &outPose);

        /**
         * \brief 多姿态加权混合算子（用于 playClips 同骨骼多轨道混合与未来 2D BlendTree）
         * \note 四元数采用半球符号对齐（dot < 0 时取反）后的加权累加并归一化（Weighted Nlerp），保证顺序无关性
         */
        static void blendPosesWeighted(const SkeletonPose *poses, const Real *weights,
            size_t count, SkeletonPose &outPose);

        /**
         * \brief 分层/局部轨道覆盖混合算子（基于 BonePose::validMask 实现上下半身分层叠加）
         */
        static void blendPosesLayered(const SkeletonPose &basePose, const SkeletonPose &layerPose,
            Real layerWeight, SkeletonPose &outPose);
        
    protected:        
        /**
         * \brief 构造并绑定蒙皮几何体
         * \param [in] geometry : 蒙皮几何体，断言非 nullptr
         */
        AnimationPlayer(SkinnedGeometry *geometry);

        /// 递增生成播放实例 ID
        ID generateID() const { return ++msGeneratedID; }

        /**
         * \brief 在 SkeletalAnimation 中按名查找 AnimationClip（"default" 未命中时回退首个片段）
         */
        AnimationClip *findClip(const String &clipName) const;

        /**
         * \brief 切换 CPU/GPU 蒙皮模式并同步材质关键字
         */
        void applyGPUSkinningMode(bool isGPUSkinning);

        /**
         * \brief 推进单条 ClipState 的播放时间并处理循环取模
         * \return 非循环片段是否已到达末尾
         */
        bool advanceClipState(ClipState &state, Real dtMS);

        /**
         * \brief 单片段整姿采样函数（未来可直接作为混合树叶子节点 ClipNode::evaluate 的实现）
         */
        void sampleClipPose(ClipState &state, const BoneGameObjects &bones, SkeletonPose &outPose);

        /**
         * \brief 将计算完成的整姿缓冲写回各骨骼的 Transform3D 节点
         */
        void applySkeletonPose(const SkeletonPose &pose, const BoneGameObjects &bones);

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
         * \brief 执行「采样 → 混合 → 写回」三步骨骼姿势求值管线；播完非循环片段时自动停止
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

        /// 当前播放实例 ID
        ID mCurrentPlaybackID {INVALID_ID};

        /// 当前主轨道状态（或 CrossFade 的起始片段 fromClip）
        ClipState mPrimaryClip {};
        /// 正在淡入的目标轨道状态（clip == nullptr 表示非 CrossFade 态）
        ClipState mTargetClip {};
        /// 多片段并行混合/分层叠加轨道列表（供 playClips 使用）
        TArray<ClipState> mLayeredClips {};

        /// 触发向 mTargetClip 过渡的主轨道时刻（毫秒，0 表示立即过渡）
        uint32_t mToClipStartedMS {0};
        /// CrossFade 交叉混合总时长（毫秒）
        uint32_t mBlendDurationMS {DEFAULT_BLEND_DURATION_MS};
        /// 当前 CrossFade 已进行时长（毫秒）
        Real mBlendElapsedMS {REAL_ZERO};

        /// 是否处于暂停状态
        bool mIsPaused {false};
        /// 上一帧更新时的墙钟时间戳（毫秒）
        int64_t mLastUpdateTimestamp {0};
        /// 本次播放起始时间戳（毫秒）
        int64_t mStartTimestamp {0};

        /// 是否正在播放
        bool mIsPlaying {false};
        /// 是否使用 GPU 蒙皮路径
        bool mIsGPUSkinning {false};
    };
}


#endif    /*__T3D_ANIMATION_PLAYER_H__*/
