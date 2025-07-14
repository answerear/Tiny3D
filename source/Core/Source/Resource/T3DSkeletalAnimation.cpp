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

#include "Resource/T3DSkeletalAnimation.h"
#include "Animation/T3DKeyframe.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SkeletalAnimationPtr SkeletalAnimation::create(const String &name)
    {
        return new SkeletalAnimation(name);
    }

    //--------------------------------------------------------------------------

    SkeletalAnimationPtr SkeletalAnimation::create(const String &name, const AnimationClips &clips)
    {
        return new SkeletalAnimation(name, clips);
    }
    
    //--------------------------------------------------------------------------

    SkeletalAnimation::~SkeletalAnimation()
    {

    }
    
    //--------------------------------------------------------------------------

    SkeletalAnimation::SkeletalAnimation(const String &name)
        : Resource(name)
    {
        
    }

    //--------------------------------------------------------------------------

    SkeletalAnimation::SkeletalAnimation(const String &name, const AnimationClips &clips)
        : Resource(name)
        , mAnimationClips(clips)
    {
        
    }
    
    //--------------------------------------------------------------------------

    Resource::Type SkeletalAnimation::getType() const
    {
        return Type::kSkeletalAnimation;
    }

    //--------------------------------------------------------------------------
    
    ResourcePtr SkeletalAnimation::clone() const
    {
        SkeletalAnimationPtr ani = create(getName());
        ani->cloneProperties(this);
        return ani;
    }
    
    //--------------------------------------------------------------------------

    void SkeletalAnimation::cloneProperties(const Resource *const src)
    {
        const SkeletalAnimation *mesh = static_cast<const SkeletalAnimation*>(src);
    }

    //--------------------------------------------------------------------------

    TResult SkeletalAnimation::onCreate()
    {
        TResult ret = Resource::onCreate();
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult SkeletalAnimation::onLoad(Archive *archive)
    {
        TResult ret;

        do
        {
            ret = Resource::onLoad(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            ret = generateRenderResource(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);
        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult SkeletalAnimation::generateRenderResource(Archive *archive)
    {
        TResult ret = T3D_OK;

        do
        {
            
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------
}
