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

#include "Resource/T3DSkeleton.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SkeletonPtr Skeleton::create(const String &name)
    {
        return T3D_NEW Skeleton(name);
    }

    //--------------------------------------------------------------------------

    SkeletonPtr Skeleton::create(const String &name, Bones &&bones)
    {
        return T3D_NEW Skeleton(name, std::move(bones));
    }

    //--------------------------------------------------------------------------

    Skeleton::~Skeleton()
    {

    }
    
    //--------------------------------------------------------------------------

    Skeleton::Skeleton(const String &name)
        : Resource(name)
    {
        
    }

    //--------------------------------------------------------------------------

    Skeleton::Skeleton(const String &name, Bones &&bones)
        : Resource(name)
        , mBones(std::move(bones))
    {
        
    }
    
    //--------------------------------------------------------------------------

    Resource::Type Skeleton::getType() const
    {
        return Type::kSkeleton;
    }

    //--------------------------------------------------------------------------
    
    ResourcePtr Skeleton::clone() const
    {
        SkeletonPtr skel = create(getName());
        skel->cloneProperties(this);
        return skel;
    }
    
    //--------------------------------------------------------------------------

    void Skeleton::cloneProperties(const Resource *const src)
    {
        Resource::cloneProperties(src);
        
        const Skeleton *skel = static_cast<const Skeleton*>(src);
    }

    //--------------------------------------------------------------------------

    TResult Skeleton::onCreate()
    {
        TResult ret = Resource::onCreate();
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Skeleton::onLoad(Archive *archive)
    {
        TResult ret;

        do
        {
            ret = Resource::onLoad(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);
        return ret;
    }

    //--------------------------------------------------------------------------
}
