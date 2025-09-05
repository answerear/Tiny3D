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


#include "Animation/T3DKeyframe.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Keyframe::Keyframe(uint32_t time)
        : mTime(time)
    {
        
    }

    //--------------------------------------------------------------------------

    KfTranslationPtr KfTranslation::create(uint32_t time, const Vector3 &translation)
    {
        return T3D_NEW KfTranslation(time, translation);
    }
    
    //--------------------------------------------------------------------------

    KfTranslation::KfTranslation(uint32_t time, const Vector3 &translation)
        : Keyframe(time)
        , mTranslation(translation)
    {
        
    }
    
    //--------------------------------------------------------------------------

    KfOrientationPtr KfOrientation::create(uint32_t time, const Quaternion &orientation)
    {
        return T3D_NEW KfOrientation(time, orientation);
    }
    
    //--------------------------------------------------------------------------

    KfOrientation::KfOrientation(uint32_t time, const Quaternion &orientation)
        : Keyframe(time)
        , mOrientation(orientation)
    {
        
    }
    //--------------------------------------------------------------------------

    KfScalingPtr KfScaling::create(uint32_t time, const Vector3 &scaling)
    {
        return T3D_NEW KfScaling(time, scaling);
    }

    //--------------------------------------------------------------------------

    KfScaling::KfScaling(uint32_t time, const Vector3 &scaling)
        : Keyframe(time)
        , mScaling(scaling)
    {
        
    }

    //--------------------------------------------------------------------------
}

