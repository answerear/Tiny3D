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


#include "Component/T3DBone.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const uint16_t Bone::kInvalidIndex = 0xFFFF;
    
    //--------------------------------------------------------------------------

    BonePtr Bone::create()
    {
        return T3D_NEW Bone(UUID::generate());
    }

    //--------------------------------------------------------------------------

    Bone::Bone(const UUID &uuid)
        : Component(uuid)
    {
        
    }
    //--------------------------------------------------------------------------
    
    ComponentPtr Bone::clone() const
    {
        BonePtr bone = Bone::create();
        TResult ret = bone->cloneProperties(this);
        if (T3D_FAILED(ret))
        {
            bone = nullptr;
        }
        
        return bone;
    }
    
    //--------------------------------------------------------------------------
    
    TResult Bone::cloneProperties(const Component *const src)
    {
        TResult ret;

        do
        {
            ret = Component::cloneProperties(src);
            if (T3D_FAILED(ret))
            {
                break;
            }

            const Bone * other = static_cast<const Bone * const>(src);
            mOffsetMatrix = other->mOffsetMatrix;
            mParentIndex = other->mParentIndex;
        } while (false);
        
        return ret;
    }
    
    //--------------------------------------------------------------------------
}
