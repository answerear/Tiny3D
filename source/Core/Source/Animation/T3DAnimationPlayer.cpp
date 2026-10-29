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


#include "Animation/T3DAnimationPlayer.h"
#include "Animation/T3DAnimationClip.h"
#include "Component/T3DSkinnedGeometry.h"
#include "Resource/T3DSkinnedMesh.h"
#include "Resource/T3DSkeletalAnimation.h"
#include "Component/T3DTransform3D.h"
#include "Component/T3DBone.h"
#include "Kernel/T3DGameObject.h"
#include "Render/T3DVertexBuffer.h"
#include "T3DConfig.h"
#include "Animation/T3DAnimationPlayerMgr.h"
#include "Resource/T3DMaterial.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

#if defined (T3D_DEBUG)
    static void debugBoneHierarchy(TransformNode *node)
    {
        Transform3D *xform = static_cast<Transform3D *>(node);
        const Vector3 &pos = xform->getLocalToWorldTransform().getTranslation();
        const Quaternion &ori = xform->getLocalToWorldTransform().getOrientation();
        const Vector3 &scaling = xform->getLocalToWorldTransform().getScaling();

        // Matrix3 matR;
        // ori.toRotationMatrix(matR);
        // Radian xAngle, yAngle, zAngle;
        // matR.toEulerAnglesYXZ(yAngle, xAngle, zAngle);
        // const Quaternion &localOri = xform->getLocalTransform().getOrientation();
        // localOri.toRotationMatrix(matR);
        // Radian xLocal, yLocal, zLocal;
        // matR.toEulerAnglesYXZ(yLocal, xLocal, zLocal);
        Radian xAngle, yAngle, zAngle;
        ori.toEulerAnglesYXZ(yAngle, xAngle, zAngle);
        Radian xLocal, yLocal, zLocal;
        xform->getLocalTransform().getOrientation().toEulerAnglesYXZ(yLocal, xLocal, zLocal);
        T3D_LOG_DEBUG(LOG_TAG_ANIMATION, "Bone %s - World Translation : %s, Euler Angle : (%f, %f, %f) - Local Translation : %s, Euler Angle : (%f, %f, %f)",
            xform->getGameObject()->getName().c_str(),
            pos.getDebugString().c_str(),
            xAngle.valueDegrees(), yAngle.valueDegrees(), zAngle.valueDegrees(),
            xform->getLocalTransform().getTranslation().getDebugString().c_str(),
            xLocal.valueDegrees(), yLocal.valueDegrees(), zLocal.valueDegrees());

        for (const auto child : xform->getChildren())
        {
            debugBoneHierarchy(child.get());
        }
    };
#endif
    
    //--------------------------------------------------------------------------

    template <typename keyframes_t>
    bool getKeyframe(uint32_t startFrame, uint32_t time, const keyframes_t &keyframes, Keyframe *&frame0, Keyframe *&frame1, uint32_t &frame)
    {
        bool found = false;
        if (!keyframes.empty())
        {
            uint32_t checkIdx = (startFrame < keyframes.size())
                ? startFrame
                : static_cast<uint32_t>(keyframes.size() - 1);
            if (time < keyframes[checkIdx]->getTime())
            {
                // 关键帧游标回退保护：当播放速率变化或归一化时间回退时重置搜索起点
                startFrame = 0;
            }
        }

        uint32_t i = startFrame;
        
        while (i < keyframes.size())
        {
            Keyframe *current = keyframes[i];
            Keyframe *next = nullptr;
            if (i + 1 < keyframes.size())
            {
                next = keyframes[i+1];

                if (time >= current->getTime() && time < next->getTime())
                {
                    // 位于两个关键帧之间
                    frame = i;
                    frame0 = current;
                    frame1 = next;
                    found = true;
                    break;
                }
            }
            else
            {
                // 最后一帧，直接用最后一帧的数据
                frame = i;
                frame0 = current;
                frame1 = next;
                found = true;
                break;
            }
            
            i++;
        }

        return found;
    }
    
    //--------------------------------------------------------------------------

    void AnimationPlayer::ClipState::resetFrames()
    {
        memset(frameT, 0, sizeof(frameT));
        memset(frameO, 0, sizeof(frameO));
        memset(frameS, 0, sizeof(frameS));
    }

    //--------------------------------------------------------------------------

    Real AnimationPlayer::ClipState::getNormalizedTime() const
    {
        if (clip == nullptr || clip->getDuration() == 0)
        {
            return REAL_ZERO;
        }
        return std::clamp(elapsedMS / static_cast<Real>(clip->getDuration()), REAL_ZERO, REAL_ONE);
    }

    //--------------------------------------------------------------------------

    void AnimationPlayer::ClipState::setNormalizedTime(Real normalizedTime)
    {
        if (clip != nullptr)
        {
            Real clamped = std::clamp(normalizedTime, REAL_ZERO, REAL_ONE);
            Real newElapsed = clamped * static_cast<Real>(clip->getDuration());
            if (newElapsed < elapsedMS)
            {
                resetFrames();
            }
            elapsedMS = newElapsed;
        }
    }

    //--------------------------------------------------------------------------

    const ID AnimationPlayer::INVALID_ID = T3D_INVALID_ID;

    //--------------------------------------------------------------------------
    
    ID AnimationPlayer::msGeneratedID = INVALID_ID;
    
    //--------------------------------------------------------------------------

    AnimationPlayerPtr AnimationPlayer::create(SkinnedGeometry *geometry)
    {
        return T3D_NEW AnimationPlayer(geometry);
    }
    
    //--------------------------------------------------------------------------

    AnimationPlayer::AnimationPlayer(SkinnedGeometry *geometry)
        : mSkinnedGeometry(geometry)
    {
        T3D_ASSERT(mSkinnedGeometry != nullptr);
    }

    //--------------------------------------------------------------------------

    AnimationClip *AnimationPlayer::findClip(const String &clipName) const
    {
        if (mSkinnedGeometry == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_ANIMATION, "SkinnedGeometry is nullptr.");
            return nullptr;
        }

        if (clipName.empty())
        {
            T3D_LOG_ERROR(LOG_TAG_ANIMATION, "clipName is empty.");
            return nullptr;
        }

        SkinnedMesh *skinnedMesh = static_cast<SkinnedMesh *>(mSkinnedGeometry->getMeshObject());
        if (skinnedMesh == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_ANIMATION, "SkinnedMesh is nullptr.");
            return nullptr;
        }

        SkeletalAnimation *skeletalAni = skinnedMesh->getSkeletalAnimation();
        if (skeletalAni == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_ANIMATION, "SkeletalAnimation is nullptr.");
            return nullptr;
        }

        const AnimationClips &clips = skeletalAni->getAnimationClips();
        AnimationClip *clip = nullptr;
        const auto clipIt = clips.find(clipName);
        if (clipIt != clips.end())
        {
            clip = clipIt->second;
        }
        else if (clipName == "default" && !clips.empty())
        {
            clip = clips.begin()->second;
            T3D_LOG_WARNING(LOG_TAG_ANIMATION,
                "Animation clip [%s] not found, fallback to first clip [%s].",
                clipName.c_str(), clips.begin()->first.c_str());
        }
        else
        {
            T3D_LOG_ERROR(LOG_TAG_ANIMATION, "Animation clip [%s] not found !", clipName.c_str());
            return nullptr;
        }

        if (clip == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_ANIMATION, "Animation clip [%s] is nullptr !", clipName.c_str());
        }

        return clip;
    }

    //--------------------------------------------------------------------------

    void AnimationPlayer::applyGPUSkinningMode(bool isGPUSkinning)
    {
        if (mIsGPUSkinning != isGPUSkinning)
        {
            // 切换 GPU 还是 CPU 蒙皮
            mIsGPUSkinning = isGPUSkinning;

            // 通过 shader 变体来控制使用 GPU 蒙皮还是 CPU 蒙皮
            const String kwGPUSkin = "T3D_GPU_SKIN";
            Material *material = (mSkinnedGeometry != nullptr) ? mSkinnedGeometry->getMaterial() : nullptr;
            if (material != nullptr)
            {
                StringArray enabledKeywrods = material->getEnabledKeywords();
                StringArray disabledKeywords = material->getDisabledKeywords();
                if (mIsGPUSkinning)
                {
                    enabledKeywrods.emplace_back(kwGPUSkin);
                    const auto it = std::find(disabledKeywords.begin(), disabledKeywords.end(), kwGPUSkin);
                    if (it != disabledKeywords.end())
                    {
                        disabledKeywords.erase(it);
                    }
                }
                else
                {
                    disabledKeywords.emplace_back(kwGPUSkin);
                    const auto it = std::find(enabledKeywrods.begin(), enabledKeywrods.end(), kwGPUSkin);
                    if (it != enabledKeywrods.end())
                    {
                        enabledKeywrods.erase(it);
                    }
                }
            
                material->switchKeywords(enabledKeywrods, disabledKeywords);
            }
        }
    }

    //--------------------------------------------------------------------------

    ID AnimationPlayer::playClip(const String &clipName, bool isBlending, bool isLoop, bool isGPUSkinning)
    {
        AnimationClip *clip = findClip(clipName);
        if (clip == nullptr)
        {
            return INVALID_ID;
        }

        int64_t now = DateTime::currentMSecsSinceEpoch();
        mLayeredClips.clear();

        if (mIsPlaying && isBlending && mPrimaryClip.clip != nullptr && clipName != mPrimaryClip.clipName)
        {
            // 若在未暂停播放中触发平滑过渡，先把当前帧已流逝时间累积到主轨道
            if (!mIsPaused && now > mLastUpdateTimestamp)
            {
                advanceClipState(mPrimaryClip, static_cast<Real>(now - mLastUpdateTimestamp));
            }

            mTargetClip = ClipState{};
            mTargetClip.clipName = clipName;
            mTargetClip.clip = clip;
            mTargetClip.elapsedMS = REAL_ZERO;
            mTargetClip.speed = REAL_ONE;
            mTargetClip.weight = REAL_ONE;
            mTargetClip.isLoop = isLoop;
            mTargetClip.resetFrames();

            mToClipStartedMS = 0;
            if (mBlendDurationMS == 0)
            {
                mBlendDurationMS = DEFAULT_BLEND_DURATION_MS;
            }
            mBlendElapsedMS = REAL_ZERO;
        }
        else
        {
            mPrimaryClip = ClipState{};
            mPrimaryClip.clipName = clipName;
            mPrimaryClip.clip = clip;
            mPrimaryClip.elapsedMS = REAL_ZERO;
            mPrimaryClip.speed = REAL_ONE;
            mPrimaryClip.weight = REAL_ONE;
            mPrimaryClip.isLoop = isLoop;
            mPrimaryClip.resetFrames();

            mTargetClip = ClipState{};
            mToClipStartedMS = 0;
            mBlendElapsedMS = REAL_ZERO;
        }

        mStartTimestamp = now;
        mLastUpdateTimestamp = now;
        mIsPlaying = true;
        mIsPaused = false;
        mCurrentPlaybackID = generateID();

        applyGPUSkinningMode(isGPUSkinning);
        T3D_ANIMATION_PLAYER_MGR.addPlayer(this);

        return mCurrentPlaybackID;
    }   

    //--------------------------------------------------------------------------

    ID AnimationPlayer::playClip(const String &fromClipName, const String &toClipName, uint32_t toClipStartedMS, bool isLoop, bool isGPUSkinning)
    {
        AnimationClip *fromClip = findClip(fromClipName);
        AnimationClip *toClip = findClip(toClipName);
        if (fromClip == nullptr || toClip == nullptr)
        {
            return INVALID_ID;
        }

        mLayeredClips.clear();

        mPrimaryClip = ClipState{};
        mPrimaryClip.clipName = fromClipName;
        mPrimaryClip.clip = fromClip;
        mPrimaryClip.elapsedMS = REAL_ZERO;
        mPrimaryClip.speed = REAL_ONE;
        mPrimaryClip.weight = REAL_ONE;
        mPrimaryClip.isLoop = false;
        mPrimaryClip.resetFrames();

        mTargetClip = ClipState{};
        mTargetClip.clipName = toClipName;
        mTargetClip.clip = toClip;
        mTargetClip.elapsedMS = REAL_ZERO;
        mTargetClip.speed = REAL_ONE;
        mTargetClip.weight = REAL_ONE;
        mTargetClip.isLoop = isLoop;
        mTargetClip.resetFrames();

        mToClipStartedMS = toClipStartedMS;
        mBlendElapsedMS = REAL_ZERO;
        if (toClipStartedMS == 0)
        {
            mBlendDurationMS = DEFAULT_BLEND_DURATION_MS;
        }
        else
        {
            uint32_t fromDuration = fromClip->getDuration();
            uint32_t fromRemain = (fromDuration > toClipStartedMS)
                ? (fromDuration - toClipStartedMS)
                : DEFAULT_BLEND_DURATION_MS;
            uint32_t toDuration = (toClip->getDuration() > 0)
                ? toClip->getDuration()
                : DEFAULT_BLEND_DURATION_MS;
            uint32_t computedDuration = std::min(fromRemain, toDuration);
            mBlendDurationMS = (computedDuration > 0) ? computedDuration : DEFAULT_BLEND_DURATION_MS;
        }

        int64_t now = DateTime::currentMSecsSinceEpoch();
        mStartTimestamp = now;
        mLastUpdateTimestamp = now;
        mIsPlaying = true;
        mIsPaused = false;
        mCurrentPlaybackID = generateID();

        applyGPUSkinningMode(isGPUSkinning);
        T3D_ANIMATION_PLAYER_MGR.addPlayer(this);

        return mCurrentPlaybackID;
    }

    //--------------------------------------------------------------------------

    ID AnimationPlayer::playClips(const StringArray &clipNames, bool isGPUSkinning)
    {
        if (clipNames.empty())
        {
            T3D_LOG_ERROR(LOG_TAG_ANIMATION, "clipNames is empty.");
            return INVALID_ID;
        }

        TArray<ClipState> layeredStates;
        layeredStates.reserve(clipNames.size());
        const Real uniformWeight = REAL_ONE / static_cast<Real>(clipNames.size());

        for (const String &name : clipNames)
        {
            AnimationClip *clip = findClip(name);
            if (clip == nullptr)
            {
                return INVALID_ID;
            }

            ClipState state{};
            state.clipName = name;
            state.clip = clip;
            state.elapsedMS = REAL_ZERO;
            state.speed = REAL_ONE;
            state.weight = uniformWeight;
            state.isLoop = true;
            state.resetFrames();
            layeredStates.push_back(state);
        }

        mLayeredClips = std::move(layeredStates);
        mPrimaryClip = ClipState{};
        mPrimaryClip.clipName = clipNames.front();
        mTargetClip = ClipState{};
        mToClipStartedMS = 0;
        mBlendElapsedMS = REAL_ZERO;

        int64_t now = DateTime::currentMSecsSinceEpoch();
        mStartTimestamp = now;
        mLastUpdateTimestamp = now;
        mIsPlaying = true;
        mIsPaused = false;
        mCurrentPlaybackID = generateID();

        applyGPUSkinningMode(isGPUSkinning);
        T3D_ANIMATION_PLAYER_MGR.addPlayer(this);

        return mCurrentPlaybackID;
    }

    //--------------------------------------------------------------------------

    TResult AnimationPlayer::stopPlayback(ID playbackID)
    {
        if (playbackID != INVALID_ID && mIsPlaying && playbackID != mCurrentPlaybackID)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        T3D_ANIMATION_PLAYER_MGR.removePlayer(this);
        mIsPlaying = false;
        mIsPaused = false;
        mCurrentPlaybackID = INVALID_ID;
        mPrimaryClip = ClipState{};
        mTargetClip = ClipState{};
        mLayeredClips.clear();
        mToClipStartedMS = 0;
        mBlendElapsedMS = REAL_ZERO;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult AnimationPlayer::pausePlayback(ID playbackID)
    {
        if (!mIsPlaying || (playbackID != INVALID_ID && playbackID != mCurrentPlaybackID))
        {
            return T3D_ERR_INVALID_PARAM;
        }

        if (!mIsPaused)
        {
            int64_t now = DateTime::currentMSecsSinceEpoch();
            if (now > mLastUpdateTimestamp)
            {
                Real dtMS = static_cast<Real>(now - mLastUpdateTimestamp);
                if (!mLayeredClips.empty())
                {
                    for (ClipState &layer : mLayeredClips)
                    {
                        advanceClipState(layer, dtMS);
                    }
                }
                else
                {
                    advanceClipState(mPrimaryClip, dtMS);
                    if (mTargetClip.clip != nullptr
                        && mPrimaryClip.elapsedMS >= static_cast<Real>(mToClipStartedMS))
                    {
                        advanceClipState(mTargetClip, dtMS);
                        mBlendElapsedMS += dtMS;
                    }
                }
            }
            mLastUpdateTimestamp = now;
            mIsPaused = true;
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult AnimationPlayer::resumePlayback(ID playbackID)
    {
        if (!mIsPlaying || (playbackID != INVALID_ID && playbackID != mCurrentPlaybackID))
        {
            return T3D_ERR_INVALID_PARAM;
        }

        if (mIsPaused)
        {
            mLastUpdateTimestamp = DateTime::currentMSecsSinceEpoch();
            mIsPaused = false;
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void AnimationPlayer::updateAnimation()
    {
        do
        {
            if (!isPlaying() || mIsPaused)
            {
                // 没有播放动画或处于暂停态
                break;
            }

            updateBones();
        } while (false);
    }

    //--------------------------------------------------------------------------

    bool AnimationPlayer::advanceClipState(ClipState &state, Real dtMS)
    {
        if (state.clip == nullptr)
        {
            return true;
        }

        state.elapsedMS += dtMS * state.speed;
        const uint32_t duration = state.clip->getDuration();
        const Real durationReal = static_cast<Real>(std::max(1u, duration));

        if (state.elapsedMS >= static_cast<Real>(duration))
        {
            if (state.isLoop)
            {
                state.elapsedMS = std::fmod(state.elapsedMS, durationReal);
                state.resetFrames();
                return false;
            }
            else
            {
                state.elapsedMS = static_cast<Real>(duration);
                return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------

    void AnimationPlayer::sampleClipPose(ClipState &state, const BoneGameObjects &bones, SkeletonPose &outPose)
    {
        const size_t boneCount = std::min(bones.size(), static_cast<size_t>(T3D_MAX_SKIN_BONES));
        outPose.boneCount = boneCount;

        if (state.clip == nullptr && !state.clipName.empty())
        {
            state.clip = findClip(state.clipName);
        }

        if (state.clip == nullptr)
        {
            for (size_t i = 0; i < boneCount; ++i)
            {
                outPose.bones[i] = BonePose{};
            }
            return;
        }

        const AnimationTracks &tracks = state.clip->getTracks();
        const uint32_t duration = state.clip->getDuration();
        const uint32_t elapsed = std::min(
            static_cast<uint32_t>( std::max(REAL_ZERO, state.elapsedMS) ),
            duration);

        for (size_t i = 0; i < boneCount; ++i)
        {
            BonePose &bonePose = outPose.bones[i];
            bonePose = BonePose{};

            GameObject *boneGO = bones[i];
            if (boneGO == nullptr)
            {
                continue;
            }

            const String &boneName = boneGO->getName();
            const auto trackIt = tracks.find(boneName);
            if (trackIt == tracks.end() || trackIt->second == nullptr)
            {
                continue;
            }

            AnimationTrack *track = trackIt->second;

            // 插值计算平移
            const TranslationTrack &trackT = track->getTranslationTrack();
            Vector3 translation;
            state.frameT[i] = interpolateTranslation(state.frameT[i], elapsed, trackT, translation);
            if (state.frameT[i] != static_cast<uint32_t>(-1))
            {
                bonePose.translation = translation;
                bonePose.validMask |= BonePose::VALID_TRANSLATION;
            }

            // 插值计算旋转
            const OrientationTrack &trackO = track->getOrientationTrack();
            Quaternion orientation;
            state.frameO[i] = interpolateOrientation(state.frameO[i], elapsed, trackO, orientation);
            if (state.frameO[i] != static_cast<uint32_t>(-1))
            {
                bonePose.orientation = orientation;
                bonePose.validMask |= BonePose::VALID_ORIENTATION;
            }

            // 插值计算缩放
            const ScalingTrack &trackS = track->getScalingTrack();
            Vector3 scaling;
            state.frameS[i] = interpolateScaling(state.frameS[i], elapsed, trackS, scaling);
            if (state.frameS[i] != static_cast<uint32_t>(-1))
            {
                bonePose.scaling = scaling;
                bonePose.validMask |= BonePose::VALID_SCALING;
            }
        }
    }

    //--------------------------------------------------------------------------

    void AnimationPlayer::blendPoses(const SkeletonPose &poseA, const SkeletonPose &poseB,
        Real alpha, SkeletonPose &outPose)
    {
        const Real t = std::clamp(alpha, REAL_ZERO, REAL_ONE);
        const size_t boneCount = std::min(
            std::max(poseA.boneCount, poseB.boneCount),
            static_cast<size_t>(T3D_MAX_SKIN_BONES));
        outPose.boneCount = boneCount;

        for (size_t i = 0; i < boneCount; ++i)
        {
            const BonePose &a = poseA.bones[i];
            const BonePose &b = poseB.bones[i];
            BonePose dst{};
            dst.validMask = a.validMask | b.validMask;

            // Translation
            const bool hasTA = (a.validMask & BonePose::VALID_TRANSLATION) != 0;
            const bool hasTB = (b.validMask & BonePose::VALID_TRANSLATION) != 0;
            if (hasTA && hasTB)
            {
                dst.translation.lerp(a.translation, b.translation, t);
            }
            else if (hasTA)
            {
                dst.translation = a.translation;
            }
            else if (hasTB)
            {
                dst.translation = b.translation;
            }

            // Orientation（Quaternion::slerp 默认 shortestPath = true，自动处理点积 < 0 最短路径反转）
            const bool hasOA = (a.validMask & BonePose::VALID_ORIENTATION) != 0;
            const bool hasOB = (b.validMask & BonePose::VALID_ORIENTATION) != 0;
            if (hasOA && hasOB)
            {
                dst.orientation.slerp(a.orientation, b.orientation, t, true);
            }
            else if (hasOA)
            {
                dst.orientation = a.orientation;
            }
            else if (hasOB)
            {
                dst.orientation = b.orientation;
            }

            // Scaling
            const bool hasSA = (a.validMask & BonePose::VALID_SCALING) != 0;
            const bool hasSB = (b.validMask & BonePose::VALID_SCALING) != 0;
            if (hasSA && hasSB)
            {
                dst.scaling.lerp(a.scaling, b.scaling, t);
            }
            else if (hasSA)
            {
                dst.scaling = a.scaling;
            }
            else if (hasSB)
            {
                dst.scaling = b.scaling;
            }

            outPose.bones[i] = dst;
        }
    }

    //--------------------------------------------------------------------------

    void AnimationPlayer::blendPosesWeighted(const SkeletonPose *poses, const Real *weights,
        size_t count, SkeletonPose &outPose)
    {
        if (poses == nullptr || weights == nullptr || count == 0)
        {
            outPose.boneCount = 0;
            return;
        }

        size_t maxBoneCount = 0;
        for (size_t c = 0; c < count; ++c)
        {
            maxBoneCount = std::max(maxBoneCount, poses[c].boneCount);
        }
        const size_t boneCount = std::min(maxBoneCount, static_cast<size_t>(T3D_MAX_SKIN_BONES));
        outPose.boneCount = boneCount;

        for (size_t i = 0; i < boneCount; ++i)
        {
            BonePose dst{};

            // 1. Translation 加权混合
            Vector3 accumT = Vector3::ZERO;
            Real sumWeightT = REAL_ZERO;
            for (size_t c = 0; c < count; ++c)
            {
                const BonePose &bp = poses[c].bones[i];
                if ((bp.validMask & BonePose::VALID_TRANSLATION) != 0 && weights[c] > REAL_ZERO)
                {
                    accumT += bp.translation * weights[c];
                    sumWeightT += weights[c];
                    dst.validMask |= BonePose::VALID_TRANSLATION;
                }
            }
            if (sumWeightT > REAL_ZERO)
            {
                dst.translation = accumT / sumWeightT;
            }

            // 2. Orientation 加权混合（半球对齐 Weighted Nlerp，若仅两个有效轨道则精确 slerp）
            size_t validOCount = 0;
            size_t firstOIdx = 0;
            size_t secondOIdx = 0;
            Quaternion accumO = Quaternion::ZERO;
            Quaternion refO = Quaternion::IDENTITY;
            Real sumWeightO = REAL_ZERO;

            for (size_t c = 0; c < count; ++c)
            {
                const BonePose &bp = poses[c].bones[i];
                if ((bp.validMask & BonePose::VALID_ORIENTATION) != 0 && weights[c] > REAL_ZERO)
                {
                    if (validOCount == 0)
                    {
                        firstOIdx = c;
                        refO = bp.orientation;
                    }
                    else if (validOCount == 1)
                    {
                        secondOIdx = c;
                    }
                    validOCount++;

                    Quaternion aligned = (refO.dot(bp.orientation) < REAL_ZERO)
                        ? (-bp.orientation)
                        : bp.orientation;
                    accumO += aligned * weights[c];
                    sumWeightO += weights[c];
                    dst.validMask |= BonePose::VALID_ORIENTATION;
                }
            }

            if (validOCount == 1)
            {
                dst.orientation = poses[firstOIdx].bones[i].orientation;
            }
            else if (validOCount == 2 && sumWeightO > REAL_ZERO)
            {
                Real t = weights[secondOIdx] / sumWeightO;
                dst.orientation.slerp(
                    poses[firstOIdx].bones[i].orientation,
                    poses[secondOIdx].bones[i].orientation,
                    t, true);
            }
            else if (validOCount > 2 && sumWeightO > REAL_ZERO)
            {
                accumO.normalize();
                dst.orientation = accumO;
            }

            // 3. Scaling 加权混合
            Vector3 accumS = Vector3::ZERO;
            Real sumWeightS = REAL_ZERO;
            for (size_t c = 0; c < count; ++c)
            {
                const BonePose &bp = poses[c].bones[i];
                if ((bp.validMask & BonePose::VALID_SCALING) != 0 && weights[c] > REAL_ZERO)
                {
                    accumS += bp.scaling * weights[c];
                    sumWeightS += weights[c];
                    dst.validMask |= BonePose::VALID_SCALING;
                }
            }
            if (sumWeightS > REAL_ZERO)
            {
                dst.scaling = accumS / sumWeightS;
            }

            outPose.bones[i] = dst;
        }
    }

    //--------------------------------------------------------------------------

    void AnimationPlayer::blendPosesLayered(const SkeletonPose &basePose, const SkeletonPose &layerPose,
        Real layerWeight, SkeletonPose &outPose)
    {
        blendPoses(basePose, layerPose, layerWeight, outPose);
    }

    //--------------------------------------------------------------------------

    void AnimationPlayer::applySkeletonPose(const SkeletonPose &pose, const BoneGameObjects &bones)
    {
        // 按照骨骼层次结构顺序（BoneGameObjects 已按父子 DFS 顺序存储）写回 Transform3D
        const size_t count = std::min(pose.boneCount, bones.size());
        for (size_t i = 0; i < count; ++i)
        {
            const BonePose &bp = pose.bones[i];
            if (bp.validMask == 0 || bones[i] == nullptr)
            {
                continue;
            }

            Transform3D *xform = static_cast<Transform3D *>(bones[i]->getTransformNode());
            if (xform == nullptr)
            {
                continue;
            }

            if ((bp.validMask & BonePose::VALID_TRANSLATION) != 0)
            {
                xform->setPosition(bp.translation);
            }
            if ((bp.validMask & BonePose::VALID_ORIENTATION) != 0)
            {
                xform->setOrientation(bp.orientation);
            }
            if ((bp.validMask & BonePose::VALID_SCALING) != 0)
            {
                xform->setScaling(bp.scaling);
            }
        }
    }
    
    //--------------------------------------------------------------------------

    void AnimationPlayer::updateBones()
    {
        if (mSkinnedGeometry == nullptr)
        {
            return;
        }

        int64_t now = DateTime::currentMSecsSinceEpoch();
        Real dtMS = REAL_ZERO;
        if (now > mLastUpdateTimestamp)
        {
            dtMS = static_cast<Real>(now - mLastUpdateTimestamp);
        }
        mLastUpdateTimestamp = now;

        const BoneGameObjects &boneGameObjects = mSkinnedGeometry->getBoneGameObjects();
        SkeletonPose finalPose{};

        // 1. 多片段并行混合/分层叠加模式（playClips）
        if (!mLayeredClips.empty())
        {
            const size_t clipCount = mLayeredClips.size();
            TArray<SkeletonPose> sampledPoses(clipCount);
            TArray<Real> weights(clipCount);

            for (size_t c = 0; c < clipCount; ++c)
            {
                advanceClipState(mLayeredClips[c], dtMS);
                sampleClipPose(mLayeredClips[c], boneGameObjects, sampledPoses[c]);
                weights[c] = mLayeredClips[c].weight;
            }

            blendPosesWeighted(sampledPoses.data(), weights.data(), clipCount, finalPose);
            applySkeletonPose(finalPose, boneGameObjects);
            return;
        }

        // 2. 双片段 CrossFade 过渡模式或单片段播放模式
        if (mPrimaryClip.clip == nullptr && !mPrimaryClip.clipName.empty())
        {
            mPrimaryClip.clip = findClip(mPrimaryClip.clipName);
        }

        if (mPrimaryClip.clip == nullptr)
        {
            return;
        }

        const bool primaryFinished = advanceClipState(mPrimaryClip, dtMS);

        if (mTargetClip.clip != nullptr)
        {
            // 检查是否已达到过渡触发时刻 mToClipStartedMS
            if (mPrimaryClip.elapsedMS >= static_cast<Real>(mToClipStartedMS))
            {
                advanceClipState(mTargetClip, dtMS);
                mBlendElapsedMS += dtMS;

                const Real blendDuration = static_cast<Real>(std::max(1u, mBlendDurationMS));
                const Real alpha = std::clamp(mBlendElapsedMS / blendDuration, REAL_ZERO, REAL_ONE);

                SkeletonPose poseA{};
                SkeletonPose poseB{};
                sampleClipPose(mPrimaryClip, boneGameObjects, poseA);
                sampleClipPose(mTargetClip, boneGameObjects, poseB);
                blendPoses(poseA, poseB, alpha, finalPose);
                applySkeletonPose(finalPose, boneGameObjects);

                if (alpha >= REAL_ONE)
                {
                    // 过渡完成：无缝切入目标片段独立播放
                    mPrimaryClip = mTargetClip;
                    mTargetClip = ClipState{};
                    mToClipStartedMS = 0;
                    mBlendElapsedMS = REAL_ZERO;
                }
            }
            else
            {
                // 尚未到达 toClipStartedMS，仅采样主片段
                sampleClipPose(mPrimaryClip, boneGameObjects, finalPose);
                applySkeletonPose(finalPose, boneGameObjects);
            }
        }
        else
        {
            // 单片段播放
            sampleClipPose(mPrimaryClip, boneGameObjects, finalPose);
            applySkeletonPose(finalPose, boneGameObjects);

            if (primaryFinished && !mPrimaryClip.isLoop)
            {
                stopPlayback(mCurrentPlaybackID);
            }
        }
    }

    //--------------------------------------------------------------------------

    void AnimationPlayer::skinning()
    {
        if (mIsGPUSkinning)
        {
            GPUSkinning();
        }
        else
        {
            CPUSkinning();
        }
    }
    
    //--------------------------------------------------------------------------

    void AnimationPlayer::CPUSkinning()
    {
        static uint32_t frameCount = 0;
// #if defined (T3D_DEBUG)
//         int64_t currentTS = DateTime::currentMSecsSinceEpoch();
//         uint32_t elapsed = static_cast<uint32_t>(currentTS - mStartTimestamp);
//         static uint32_t frameCount = 0;
//         constexpr uint32_t divisor = 1000;
//         uint32_t t = elapsed / divisor;
//         bool isDebugOutput = false;
//         if (t > frameCount)
//         {
//             GameObject *go = mSkinnedGeometry->getGameObject();
//             T3D_LOG_DEBUG(LOG_TAG_ANIMATION, "Frame : %d", frameCount);
//             debugBoneHierarchy(go->getTransformNode());
//             isDebugOutput = true;
//             frameCount = t;
//         }
// #endif
        
        frameCount++;
        if (frameCount > 1)
        {
            // return;
        }
        
        // CPU 蒙皮
        SkinnedMesh *skinnedMesh = (SkinnedMesh *)(mSkinnedGeometry->getMeshObject());
        GameObjectPtr go = mSkinnedGeometry->getGameObject();
        Transform3D *xform = static_cast<Transform3D *>(go->getTransformNode());

        const VertexBuffers &vbos = skinnedMesh->getVertexBuffers();
        const Vertices &vertices = skinnedMesh->getVertices();
        T3D_ASSERT(vertices.size() == vbos.size());
        
        // 位置属性
        const VertexAttribute *posAttrib = skinnedMesh->findVertexAttributeBySemantic(VertexAttribute::Semantic::E_VAS_POSITION, 0);
        uint32_t posSlot = posAttrib->getSlot();
        T3D_ASSERT(posSlot < vbos.size());
        uint32_t posStride = skinnedMesh->getVertexStride(posSlot);
        
        // 法线属性
        const VertexAttribute *normalAttrib = skinnedMesh->findVertexAttributeBySemantic(VertexAttribute::Semantic::E_VAS_NORMAL, 0);
        uint32_t normalSlot = normalAttrib->getSlot();
        T3D_ASSERT(normalSlot < vbos.size());
        uint32_t normalStride = skinnedMesh->getVertexStride(normalSlot);

        // 骨骼混合权重属性
        const VertexAttribute *bwAttrib = skinnedMesh->findVertexAttributeBySemantic(VertexAttribute::Semantic::E_VAS_BLENDWEIGHT, 0);
        T3D_ASSERT(bwAttrib != nullptr);
        uint32_t bwSlot = bwAttrib->getSlot();
        T3D_ASSERT(bwSlot < vbos.size());
        uint32_t bwStride = skinnedMesh->getVertexStride(bwSlot);
        uint32_t bwOffset = bwAttrib->getOffset();
        
            
        // 骨骼混合索引属性
        const VertexAttribute *biAttrib = skinnedMesh->findVertexAttributeBySemantic(VertexAttribute::Semantic::E_VAS_BLENDINDICES, 0);
        uint32_t biSlot = biAttrib->getSlot();
        T3D_ASSERT(biSlot < vbos.size());
        uint32_t biStride = skinnedMesh->getVertexStride(biSlot);
        uint32_t biOffset = biAttrib->getOffset();

        // 从 vertices 获取原始顶点数据，然后经过蒙皮，写到 vbo 里面
        const Buffer &srcPosVerts = vertices[posSlot];
        const Buffer &srcNormalVerts = vertices[normalSlot];
        const Buffer &srcBWVerts = vertices[bwSlot];
        const Buffer &srcBIVerts = vertices[biSlot];
        
        const Buffer &dstPosVerts = vbos[posSlot]->getBuffer();
        const Buffer &dstNormalVerts = vbos[normalSlot]->getBuffer();

        T3D_ASSERT(srcPosVerts.Data != dstPosVerts.Data);
        T3D_ASSERT(srcPosVerts.DataSize == dstPosVerts.DataSize);

        const auto getBoneMatrix = [](const BoneGameObjects &boneGameObjects, uint8_t boneIndex)
        {
            if (boneIndex == 0xFF)
            {
                return Matrix4::IDENTITY;
            }
            
            GameObject *boneGO = boneGameObjects[boneIndex];
            Bone *bone = boneGO->getComponent<Bone>();
            const Matrix4 &matOffset = (bone != nullptr) ? bone->getOffsetMatrix() : Matrix4::IDENTITY;
            Transform3D *xform = static_cast<Transform3D *>(boneGO->getTransformNode());
            const Matrix4 &matBoneMatrix = xform->getLocalToWorldTransform().getAffineMatrix();
            Matrix4 mat = matBoneMatrix * matOffset;
            return mat;
        };
        
        size_t vertexCount = vertices[0].DataSize / skinnedMesh->getVertexStride(0);
        size_t posOffset = posAttrib->getOffset(), normalOffset = normalAttrib->getOffset();
        for (size_t i = 0; i < vertexCount; i++)
        {
            // 位置
            T3D_ASSERT(posOffset < srcPosVerts.DataSize);
            Vector3 *srcPos = (Vector3 *)(srcPosVerts.Data + posOffset);
            
            Vector4 srcPos1(srcPos->x(), srcPos->y(), srcPos->z(), 1);
            Vector4 pos;

            // 法线
            T3D_ASSERT(normalOffset < srcNormalVerts.DataSize);
            Vector3 *srcNormal = (Vector3 *)(srcNormalVerts.Data + normalOffset);
            
            Vector4 srcNormal1(srcNormal->x(), srcNormal->y(), srcNormal->z(), 0);
            Vector4 normal;

            // 混合权重
            T3D_ASSERT(bwOffset < srcBWVerts.DataSize);
            float *srcWeight = (float *)(srcBWVerts.Data + bwOffset);

            // 混合索引
            T3D_ASSERT(biOffset < srcBIVerts.DataSize);
            uint8_t *srcIndex = srcBIVerts.Data + biOffset;

            for (uint32_t idx = 0; idx < T3D_MAX_BLEND_BONES; idx++)
            {
                float blendWeight = srcWeight[idx];
                uint8_t blendIdx = srcIndex[idx];
                
                // 混合计算位置
                pos += getBoneMatrix(mSkinnedGeometry->getBoneGameObjects(), blendIdx) * srcPos1 * blendWeight;

                // 混合计算法线
                normal += getBoneMatrix(mSkinnedGeometry->getBoneGameObjects(), blendIdx) * (srcNormal1) * blendWeight;
            }

            T3D_ASSERT(posOffset < dstPosVerts.DataSize);
            Vector3 *dstPos = (Vector3 *)(dstPosVerts.Data + posOffset);
            dstPos->x() = pos.x();
            dstPos->y() = pos.y();
            dstPos->z() = pos.z();

            T3D_ASSERT(normalOffset < dstNormalVerts.DataSize);
            Vector3 *dstNormal = (Vector3 *)(dstNormalVerts.Data + normalOffset);
            dstNormal->x() = normal.x();
            dstNormal->y() = normal.y();
            dstNormal->z() = normal.z();

// #if defined (T3D_DEBUG)
//             if (isDebugOutput)
//             {
//                 T3D_LOG_DEBUG(LOG_TAG_ANIMATION, "Vertex - Index %u, pos (%s), dst pos (%s), weight (%f, %f, %f, %f), index (%d, %d, %d, %d)", i,
//                     srcPos->getDebugString().c_str(),
//                     dstPos->getDebugString().c_str(),
//                     srcWeight[0], srcWeight[1], srcWeight[2], srcWeight[3],
//                     srcIndex[0], srcIndex[1], srcIndex[2], srcIndex[3]);
//             }
// #endif

            posOffset += posStride;
            normalOffset += normalStride;
            bwOffset += bwStride;
            biOffset += biStride;
        }

        if (posSlot == normalSlot)
        {
            // 顶点和法线是同一个 VBO
            vbos[posSlot]->writeData(0, dstPosVerts, true);
        }
        else
        {
            // 顶点和法线不是同一个 VBO
            vbos[posSlot]->writeData(0, dstPosVerts, true);
            vbos[normalSlot]->writeData(0, dstNormalVerts, true);
        }
    }

    //--------------------------------------------------------------------------

    void AnimationPlayer::GPUSkinning()
    {
        T3D_ASSERT(mSkinnedGeometry != nullptr);
        Material *material = mSkinnedGeometry->getMaterial();
        T3D_ASSERT(material != nullptr);
        const auto &boneGameObjects = mSkinnedGeometry->getBoneGameObjects();
        Matrix4 matrices[T3D_MAX_SKIN_BONES] = { Matrix4::IDENTITY };
        for (int32_t i = 0; i < static_cast<int32_t>(boneGameObjects.size()); i++)
        {
            GameObject *boneGO = boneGameObjects[i];
            Bone *bone = boneGO->getComponent<Bone>();
            const Matrix4 &matOffset = (bone != nullptr) ? bone->getOffsetMatrix() : Matrix4::IDENTITY;
            const Matrix4 &mat = static_cast<Transform3D *>(boneGO->getTransformNode())->getLocalToWorldTransform().getAffineMatrix();
            matrices[i] = mat * matOffset;
        }
        Matrix4Array matrixArray(matrices, matrices+T3D_MAX_SKIN_BONES);
        material->setMatrixArray("tiny3d_BoneMatrices", matrixArray);
    }

    //--------------------------------------------------------------------------

    Real AnimationPlayer::getInterplationTime(Keyframe *kf0, Keyframe *kf1, uint32_t time) const
    {
        const Real t0 = static_cast<Real>(kf0->getTime());
        const Real t1 = static_cast<Real>(kf1->getTime());
        const Real t = (static_cast<Real>(time) - t0) / (t1 - t0);
        return t;
    }
    
    //--------------------------------------------------------------------------

    uint32_t AnimationPlayer::interpolateTranslation(uint32_t startFrame, uint32_t time, const TranslationTrack &track, Vector3 &translation)
    {
        uint32_t currentFrame = static_cast<uint32_t>(-1);
        Keyframe *kf0 = nullptr, *kf1 = nullptr;
        
        if (getKeyframe(startFrame, time, track, kf0, kf1, currentFrame))
        {
            // T3D_LOG_DEBUG(LOG_TAG_ANIMATION, 
            //     "interpolateTranslation: time in range, startFrame %u, time %u, currentFrame %u",
            //     startFrame, time, currentFrame)
            
            if (kf1 != nullptr)
            {
                // 位于两帧之间，插值
                const Real t = getInterplationTime(kf0, kf1, time);  
                KfTranslation *keyframe0 = static_cast<KfTranslation *>(kf0);
                KfTranslation *keyframe1 = static_cast<KfTranslation *>(kf1);
                translation.lerp(keyframe0->getTranslation(), keyframe1->getTranslation(), t);
            }
            else
            {
                // 直接最后一帧，不用插值
                KfTranslation *keyframe = static_cast<KfTranslation *>(kf0);
                translation = keyframe->getTranslation();
            }
        }
        else if (!track.empty())
        {
            // 找不到关键帧（可能时间超出范围），使用最后一帧的数据
            currentFrame = static_cast<uint32_t>(track.size() - 1);
            KfTranslation *keyframe = static_cast<KfTranslation *>(track[currentFrame]);
            translation = keyframe->getTranslation();
        }
        
        return currentFrame;
    }

    //--------------------------------------------------------------------------

    uint32_t AnimationPlayer::interpolateOrientation(uint32_t startFrame, uint32_t time, const OrientationTrack &track, Quaternion &orientation)
    {
        uint32_t currentFrame = static_cast<uint32_t>(-1);
        Keyframe *kf0 = nullptr, *kf1 = nullptr;

        if (getKeyframe(startFrame, time, track, kf0, kf1, currentFrame))
        {
            if (kf1 != nullptr)
            {
                // 位于两帧之间，插值
                const Real t = getInterplationTime(kf0, kf1, time);
                KfOrientation *keyframe0 = static_cast<KfOrientation *>(kf0);
                KfOrientation *keyframe1 = static_cast<KfOrientation *>(kf1);
                orientation.slerp(keyframe0->getOrientation(), keyframe1->getOrientation(), t);
            }
            else
            {
                // 最后一帧，不用插值
                KfOrientation *keyframe = static_cast<KfOrientation *>(kf0);
                orientation = keyframe->getOrientation();
            }
        }
        else if (!track.empty())
        {
            // 找不到关键帧（可能时间超出范围），使用最后一帧的数据
            currentFrame = static_cast<uint32_t>(track.size() - 1);
            KfOrientation *keyframe = static_cast<KfOrientation *>(track[currentFrame]);
            orientation = keyframe->getOrientation();
            T3D_LOG_WARNING(LOG_TAG_ANIMATION, "interpolateOrientation: time out of range, using last frame")
        }
        
        return currentFrame;
    }
    
    //--------------------------------------------------------------------------

    uint32_t AnimationPlayer::interpolateScaling(uint32_t startFrame, uint32_t time, const ScalingTrack &track, Vector3 &scaling)
    {
        uint32_t currentFrame = static_cast<uint32_t>(-1);
        Keyframe *kf0 = nullptr, *kf1 = nullptr;

        if (getKeyframe(startFrame, time, track, kf0, kf1, currentFrame))
        {
            if (kf1 != nullptr)
            {
                // 位于两帧之间，插值
                const Real t = getInterplationTime(kf0, kf1, time);
                KfScaling *keyframe0 = static_cast<KfScaling *>(kf0);
                KfScaling *keyframe1 = static_cast<KfScaling *>(kf1);
                scaling.lerp(keyframe0->getScaling(), keyframe1->getScaling(), t);
            }
            else
            {
                // 最后一帧，不用插值
                KfScaling *keyframe = static_cast<KfScaling *>(kf0);
                scaling = keyframe->getScaling();
            }
        }
        else if (!track.empty())
        {
            // 找不到关键帧（可能时间超出范围），使用最后一帧的数据
            currentFrame = static_cast<uint32_t>(track.size() - 1);
            KfScaling *keyframe = static_cast<KfScaling *>(track[currentFrame]);
            scaling = keyframe->getScaling();
        }
        
        return currentFrame;
    }

    //--------------------------------------------------------------------------
}
