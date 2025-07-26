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


#include "Component/T3DSkinnedGeometry.h"
#include "Resource/T3DSkeletalAnimation.h"
#include "Resource/T3DSkinnedMesh.h"
#include "Animation/T3DAnimationClip.h"
#include "Animation/T3DAnimationPlayer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SkinnedGeometry::SkinnedGeometry(const UUID &uuid)
        : Geometry(uuid)
    {
        mAnimationPlayer = AnimationPlayer::create(this);
    }

    //--------------------------------------------------------------------------

    SkinnedGeometry::~SkinnedGeometry()
    {
        mAnimationPlayer = nullptr;
    }

    //--------------------------------------------------------------------------

    void SkinnedGeometry::onStart()
    {
        Geometry::onStart();

        mStartTimestamp = DateTime::currentMSecsSinceEpoch();
    }

    //--------------------------------------------------------------------------

    void SkinnedGeometry::onUpdate()
    {
        SkinnedMesh *skinnedMesh = smart_pointer_cast<SkinnedMesh>(mMesh);
        
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
                uint32_t elapsed = static_cast<uint32_t>((currentTS - mStartTimestamp) % clip->getDuration());
                
                for (const auto &it : tracks)
                {
                    AnimationTrack *track = it.second;
                    const TranslationTrack &trackT = track->getTranslationTrack();
                    const OrientationTrack &trackO = track->getOrientationTrack();
                    const ScalingTrack &trackS = track->getScalingTrack();
                }
            }
        }

        // CPU 蒙皮
    }

    //--------------------------------------------------------------------------

    uint32_t SkinnedGeometry::interpolateTranslation(uint32_t time, const TranslationTrack &track, Vector3 &translation)
    {
        return 0;
    }

    //--------------------------------------------------------------------------

    uint32_t SkinnedGeometry::interpolateOrientation(uint32_t time, const OrientationTrack &track, Quaternion &orientation)
    {
        return 0;
    }
    
    //--------------------------------------------------------------------------

    uint32_t SkinnedGeometry::interpolateScaling(uint32_t time, const ScalingTrack &track, Vector3 &scaling)
    {
        return 0;
    }

    //--------------------------------------------------------------------------

    void SkinnedGeometry::onDestroy()
    {
        Geometry::onDestroy();
    }

    //--------------------------------------------------------------------------

    ComponentPtr SkinnedGeometry::clone() const
    {
        SkinnedGeometryPtr geometry = new SkinnedGeometry();
        TResult ret = geometry->cloneProperties(this);
        if (T3D_FAILED(ret))
        {
            geometry = nullptr;
        }
        return geometry;
    }

    //--------------------------------------------------------------------------

    TResult SkinnedGeometry::cloneProperties(const Component *const src)
    {
        TResult ret;

        do
        {
            ret = Geometry::cloneProperties(src);
            if (T3D_FAILED(ret))
            {
                break;
            }

            const SkinnedGeometry * const other = static_cast<const SkinnedGeometry * const>(src);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void SkinnedGeometry::onPostLoad()
    {
        Geometry::onPostLoad();
    }

    //--------------------------------------------------------------------------

    void SkinnedGeometry::onLoadResource(Archive *archive)
    {
        Geometry::onLoadResource(archive);
    }

    //--------------------------------------------------------------------------

    void SkinnedGeometry::generateRenderMaterial()
    {
        Geometry::generateRenderMaterial();
    }

    //--------------------------------------------------------------------------
}
