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
#include "Render/T3DVertexBuffer.h"
#include "T3DConfig.h"
#include "Animation/T3DAnimationPlayerMgr.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    template <typename keyframes_t>
    bool getKeyframe(uint32_t startFrame, uint32_t time, const keyframes_t &keyframes, Keyframe *&frame0, Keyframe *&frame1, uint32_t &frame)
    {
        bool found = false;
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
                frame1 = nullptr;
                found = true;
                break;
            }
            
            i++;
        }

        return found;
    }
    
    //--------------------------------------------------------------------------

    const ID AnimationPlayer::INVALID_ID = T3D_INVALID_ID;

    //--------------------------------------------------------------------------
    
    ID AnimationPlayer::msGeneratedID = INVALID_ID;
    
    //--------------------------------------------------------------------------

    AnimationPlayerPtr AnimationPlayer::create(SkinnedGeometry *geometry)
    {
        return new AnimationPlayer(geometry);
    }
    
    //--------------------------------------------------------------------------

    AnimationPlayer::AnimationPlayer(SkinnedGeometry *geometry)
        : mSkinnedGeometry(geometry)
    {
        
    }

    //--------------------------------------------------------------------------

    ID AnimationPlayer::playClip(const String &clipName, bool isBlending, bool isLoop)
    {
        TResult ret = INVALID_ID;

        do
        {
            if (mSkinnedGeometry == nullptr)
            {
                ret = T3D_ERR_SYS_NOT_INIT;
                T3D_LOG_ERROR(LOG_TAG_ANIMATION, "SkinnedGeometry is nullptr.");
                break;
            }

            if (clipName.empty())
            {
                ret = T3D_ERR_INVALID_PARAM;
                T3D_LOG_ERROR(LOG_TAG_ANIMATION, "clipName is empty.");
                break;
            }

            mStartTimestamp = DateTime::currentMSecsSinceEpoch();
            mCurrentFrameT = 0;
            mCurrentFrameO = 0;
            mCurrentFrameS = 0;
            mIsPlaying = true;

            T3D_ANIMATION_PLAYER_MGR.addPlayer(this);
        } while (false);
        
        return ret;
    }   

    //--------------------------------------------------------------------------

    ID AnimationPlayer::playClip(const String &fromClipName, const String &toClipName, uint32_t toClipStartedMS, bool isLoop)
    {
        return INVALID_ID;
    }

    //--------------------------------------------------------------------------

    ID AnimationPlayer::playClips(const StringArray &clipNames)
    {
        return INVALID_ID;
    }

    //--------------------------------------------------------------------------

    TResult AnimationPlayer::stopPlayback(ID playbackID)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult AnimationPlayer::pausePlayback(ID playbackID)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult AnimationPlayer::resumePlayback(ID playbackID)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void AnimationPlayer::updateAnimation()
    {
        do
        {
            if (!isPlaying())
            {
                // 没有播放动画
                break;
            }

            updateBones();
        } while (false);
    }
    
    //--------------------------------------------------------------------------

    void AnimationPlayer::updateBones()
    {
        const BoneGameObjectsMap &bones = mSkinnedGeometry->getAllBones();
            
        SkinnedMesh *skinnedMesh = (SkinnedMesh *)(mSkinnedGeometry->getMeshObject());
    
        // 更新动画
        SkeletalAnimation *skeletalAni = skinnedMesh->getSkeletalAnimation();
        const AnimationClips &clips = skeletalAni->getAnimationClips();
        if (clips.size() > 0)
        {
            AnimationClip *clip = clips.begin()->second;
            if (clip != nullptr)
            {
                const AnimationTracks &tracks = clip->getTracks();

                int64_t currentTS = DateTime::currentMSecsSinceEpoch();
                uint32_t elapsed = static_cast<uint32_t>(currentTS - mStartTimestamp);

                if (elapsed >= clip->getDuration())
                {
                    elapsed = clip->getDuration();
                    T3D_ANIMATION_PLAYER_MGR.removePlayer(this);
                    mIsPlaying = false;
                }

                T3D_LOG_DEBUG(LOG_TAG_ANIMATION, "Elapsed Time : %u", elapsed);
            
                for (const auto &it : tracks)
                {
                    AnimationTrack *track = it.second;

                    // 差值计算平移
                    const TranslationTrack &trackT = track->getTranslationTrack();
                    Vector3 translation;
                    mCurrentFrameT = interpolateTranslation(mCurrentFrameT, elapsed, trackT, translation);

                    // 差值计算旋转
                    const OrientationTrack &trackO = track->getOrientationTrack();
                    Quaternion orientation;
                    mCurrentFrameO = interpolateOrientation(mCurrentFrameO, elapsed, trackO, orientation);

                    // 差值计算缩放
                    const ScalingTrack &trackS = track->getScalingTrack();
                    Vector3 scaling;
                    mCurrentFrameS = interpolateScaling(mCurrentFrameS, elapsed, trackS, scaling);

                    // 更新对应骨骼的 RTS
                    const auto itr = bones.find(it.first);
                    if (itr == bones.end())
                    {
                        // 没有对应的骨骼，这里要报错了
                        T3D_LOG_ERROR(LOG_TAG_ANIMATION,
                            "Could not find the corresponding bone [%s] in skinned geometry hierarchy !",
                            it.first.c_str());
                        continue;
                    }

                    Transform3D *xform = static_cast<Transform3D *>(itr->second->getTransformNode());
                    if (mCurrentFrameT != static_cast<uint32_t>(-1))
                    {
                        xform->setPosition(translation);
                    }
                    if (mCurrentFrameO != static_cast<uint32_t>(-1))
                    {
                        xform->setOrientation(orientation);
                    }
                    if (mCurrentFrameS != static_cast<uint32_t>(-1))
                    {
                        xform->setScaling(scaling);
                    }

                    Matrix3 matR;
                    xform->getOrientation().toRotationMatrix(matR);
                    Radian xAngle, yAngle, zAngle;
                    matR.toEulerAnglesZXY(zAngle, xAngle, yAngle);
                    T3D_LOG_DEBUG(LOG_TAG_ANIMATION, "Bone %s, Translation : (%f, %f, %f), Euler Angle : (%f, %f, %f), Scaling : (%f, %f, %f)",
                        it.first.c_str(),
                        xform->getPosition().x(), xform->getPosition().y(), xform->getPosition().z(),
                        xAngle.valueDegrees(), yAngle.valueDegrees(), zAngle.valueDegrees(),
                        xform->getScaling().x(), xform->getScaling().y(), xform->getScaling().z());
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    void AnimationPlayer::skinning()
    {
        CPUSkinning();
    }
    
    //--------------------------------------------------------------------------

    void AnimationPlayer::CPUSkinning()
    {
        // CPU 蒙皮
        SkinnedMesh *skinnedMesh = (SkinnedMesh *)(mSkinnedGeometry->getMeshObject());

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

        const auto getBoneMatrix = [](const Bones &bones, const BoneGameObjects &boneGameObjects, uint8_t boneIndex)
        {
            if (boneIndex == 0xFF)
            {
                return Matrix4::ZERO;
            }
            
            const BoneNodePtr &bone = bones[boneIndex];
            const Matrix4 &matOffset = bone->getOffsetMatrix();
            Transform3D *xform = static_cast<Transform3D *>(boneGameObjects[boneIndex]->getTransformNode());
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
            uint8_t *srcIndex = (uint8_t *)(srcBIVerts.Data + biOffset);

            for (uint32_t idx = 0; idx < T3D_MAX_BLEND_BONES; idx++)
            {
                float blendWeight = srcWeight[idx];
                uint8_t blendIdx = srcIndex[idx];
                
                // 混合计算位置
                pos += getBoneMatrix(skinnedMesh->getBones(), mSkinnedGeometry->getBoneGameObjects(), blendIdx) * srcPos1 * blendWeight;

                // 混合计算法线
                normal += getBoneMatrix(skinnedMesh->getBones(), mSkinnedGeometry->getBoneGameObjects(), blendIdx) * (srcNormal1) * blendWeight;
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

            posOffset += posStride;
            normalOffset += normalStride;
        }

        if (posSlot == normalSlot)
        {
            vbos[posSlot]->writeData(0, dstPosVerts, true);
        }
        else
        {
            vbos[posSlot]->writeData(0, dstPosVerts, true);
            vbos[normalSlot]->writeData(0, dstNormalVerts, true);
        }
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
        
        return currentFrame;
    }

    //--------------------------------------------------------------------------
}
